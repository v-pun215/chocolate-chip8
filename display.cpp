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
    uint16_t map_key(SDL_Keycode key) {
        switch (key) {
            case SDLK_1:
            return 0x1;
            
            case SDLK_2:
            return 0x2;

            case SDLK_3:
            return 0x3;

            case SDLK_4:
            return 0xC;

            case SDLK_q:
            return 0x4;

            case SDLK_w:
            return 0x5;

            case SDLK_e:
            return 0x6;

            case SDLK_r:
            return 0xD;

            case SDLK_a:
            return 0x7;

            case SDLK_s:
            return 0x8;

            case SDLK_d:
            return 0x9;

            case SDLK_f:
            return 0xE;

            case SDLK_z:
            return 0xA;

            case SDLK_x:
            return 0x0;

            case SDLK_c:
            return 0xB;

            case SDLK_v:
            return 0xF;

            default:
            return 293;
        };
    }
    bool should_quit(array<bool, 16>& keys) {
        bool quit = false;
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            if (e.type == SDL_KEYDOWN) {
                uint16_t key = map_key(e.key.keysym.sym);
                if (key!=293) {
                    keys[key] = true;
                    std::cout << "KEY PRESSED: " << key << '\n';
                }
            }
            if (e.type == SDL_KEYUP) {
                uint16_t key = map_key(e.key.keysym.sym);
                if (key!=293) {
                    keys[key] = false;
                }
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