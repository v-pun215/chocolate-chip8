#include <SDL.h>
#include <bits/stdc++.h>


using namespace std;

namespace Display {
    static SDL_Window* window = nullptr;
    static SDL_Renderer* renderer = nullptr;
    void init(int scale) {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            std::cerr << "error: sdl could not be initialised.";
        }

        window = SDL_CreateWindow(
            "Chocolate CHIP-8",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            64 * scale,
            32 * scale,
            SDL_WINDOW_SHOWN
        );

        if (!window) {
            cerr << "window couldnt be created";
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    }

    void render(const array<bool, 64 * 32>& display, int scale) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);


        for (int i=0;i<64;i++) {
            for (int j=0;j<32;j++) {
                if (display[j * 64 + i] == 1) {
                    SDL_Rect rect = { i * scale, j * scale, scale, scale };
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
        SDL_RenderPresent(renderer);
    }

    bool should_quit() {
        bool quit = false;
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

        }
        return quit;
    }

    void shutdown() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }
}