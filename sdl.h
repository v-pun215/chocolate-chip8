#pragma once
#include <SDL.h>
#include <array>
using namespace std;
namespace Display {
    void init(int scale);
    void render(const array<bool, 64*32>& display, int scale);
    bool should_quit(array<bool, 16>& keys);
    uint16_t map_key(SDL_Keycode key);

    void init_audio();

    void update_audio(uint8_t sound_timer);
    void shutdown();
}