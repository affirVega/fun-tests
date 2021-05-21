TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        # long-polling.cpp
        polling.cpp

QMAKE_CXXFLAGS += -std=c++17 -pthread
LIBS += -pthread
