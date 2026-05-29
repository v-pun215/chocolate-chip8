#pragma once
#include <array>
using namespace std;
namespace Display {
    void init(int scale);
    void render(const array<bool, 64*32>& display, int scale);
    bool should_quit();
    void shutdown();
}