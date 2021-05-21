#include <iostream>
#include <thread>
#include <mutex>
#include <cstring>
#include <functional>
#include <chrono>

const int thread_count = 8;

double pi_square(unsigned long long res)
{
    unsigned long long in_circle = 0;
    unsigned long long res_sqr = res * res;
    for (unsigned long long x = 0; x < res; ++x)
        for (unsigned long long y = 0; y < res; ++y)
            // если r <= 1
            if ((x*x + y*y) <= res_sqr) in_circle += 1;
    return in_circle * 4 / static_cast<double>(res_sqr);
}

double pi_rand(unsigned long long res)
{
    unsigned long long in_circle = 0;
    unsigned long long total = res;

    while (res--)
    {
        double x = rand() % 1000 / 1000.0;
        double y = rand() % 1000 / 1000.0;

        if (x*x + y*y <= 1)
            in_circle += 1;
    }

    return in_circle * 4 / static_cast<double>(total);
}

double pi_rand_thread(unsigned long long res)
{
    unsigned long long total = res;
    unsigned long long in_circle = 0;
    std::mutex m;

    std::thread *threads[thread_count];
    for (int i = 0; i < thread_count; ++i)
        threads[i] = new std::thread([&m, &in_circle, &res](){
            unsigned long long times = res / thread_count;
            unsigned long long my_in = 0;
            while (times--)
            {
                double x = rand() % 1000 / 1000.0;
                double y = rand() % 1000 / 1000.0;

                if (x*x + y*y <= 1) {
                    my_in += 1;
                }
            }
            m.lock();
            in_circle += my_in;
            m.unlock();
    });

    for (int i = 0; i < thread_count; ++i)
    {
        threads[i]->join();
        delete threads[i];
    }

    return in_circle * 4 / static_cast<double>(total);
}

std::array<std::string, 3> const g_variants {
    "square",
    "rand",
    "rand_thread"
};

std::array<std::function<double(int)>, 3> const g_func_ptr {
    pi_square,
    pi_rand,
    pi_rand_thread
};

void print_usage(const char * name) {
    std::cout << "Usage: " << name << " method resolution\n\nMethods\n";
    for (std::string str : g_variants)
        std::cout << "\t" << str << "\n";
    std::cout << "\nResolution\n\tany integer positive number, that fits in 'unsigned long long' type\n";
}

int main(int const argc, char const** argv)
{
    std::ios::sync_with_stdio(false);

    if (argc < 3 or argc > 3) {
        print_usage(argv[0]);
        return 0;
    }

    int choice = -1;

    for (int i = 0; i < g_variants.size(); ++i) {
        if (argv[1] == g_variants[i]) {
            choice = i;
            break;
        }
    }

    if (choice == -1) {
        std::cout << '\'' << argv[1] << "' is not a valid choice." << std::endl;
        return 0;
    }

    unsigned long long res;

    try {
         res = std::stoll(argv[2]);
    } catch (std::exception &e) {
        std::cout << '\'' << argv[2] << "' is not an integer number" << std::endl;
        return 1;
    }

    auto now = std::chrono::high_resolution_clock::now();
    double pi = g_func_ptr[choice](res);
    auto after = std::chrono::high_resolution_clock::now();

    using namespace std::literals;

    std::cout << pi << std::endl << "Function took " << (after - now) / 1ms << "ms" << std::endl;

    return 0;
}
