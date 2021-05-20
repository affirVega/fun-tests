// данные
#include <unordered_map>
#include <vector>
#include <iostream>

// потоки
#include <thread>
#include <chrono>
#include <condition_variable>
#include <mutex>

// json и httplib
#include "include/httplib.h"
#include "include/json.hpp"

using namespace std;
using nlohmann::json;
using namespace httplib;

const char *response_type = "text/json; charset=utf-8";

struct Game {
    struct Move {
        int player;
        string move;
    };

    vector<Move> moves;
};

// map[uid] -> Game
unordered_map<string, Game> game_map;
mutex m;

void post_handler(const Request &req, Response &res)
{
    string uid, move;
    int player;
    json j = json::parse(req.body);
    uid = j["uid"].get<string>();
    move = j["move"].get<string>();
    player = j["player"].get<int>();

    {
        unique_lock<mutex> lock(m);

        if (!game_map[uid].moves.empty())
        {
            Game::Move last_move = game_map[uid].moves.back();
            if (last_move.player == player) {
                json to_send {
                    {"status", "not-your-move"}
                };
                res.set_content(to_send.dump(2), response_type);
                return;
            }
        }

        game_map[uid].moves.push_back({player, move});
    }

    json to_send {
        {"status", "success"}
    };
    res.set_content(to_send.dump(2), response_type);
}

void get_handler(const Request &req, Response &res)
{
    string uid;
    int player;
    uid = req.get_param_value("uid");
    player = stoi(req.get_param_value("player"));

    {
        unique_lock<mutex> lock(m);

        if (game_map[uid].moves.empty()) {
            json to_send {
                {"status", "empty"}
            };
            res.set_content(to_send.dump(2), response_type);
            return;
        }

        Game::Move last_move = game_map[uid].moves.back();
        if (last_move.player != player)
        {
            json to_send {
                {"status", "move"},
                {"move", last_move.move},
                {"player", last_move.player}
            };
            res.set_content(to_send.dump(2), response_type);
            return;
        }
        else
        {
            json to_send {
                {"status", "no-move-from-other-player"},
            };
            res.set_content(to_send.dump(2), response_type);
            return;
        }
    }
}

int main()
{
    Server server;
    server.Get("/", get_handler);
    server.Post("/", post_handler);
    server.set_exception_handler([] (const Request &, Response &res, std::exception &e) {
        cout << "Exception: " << e.what() << endl;
        json j {{"status", "server-error"}};
        res.set_content(j.dump(2), response_type);
    });

    string ip("127.0.0.1");
//    cout << "Введите IP сервера: ";
//    cin >> ip;

    int port = 8080;
    cout << "Введите порт сервера: " << flush;
    cin >> port;

    cout << "Запускаю сервер..." << endl;
    server.listen(ip.c_str(), port);
}
