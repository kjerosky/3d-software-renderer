#include <SDL3/SDL.h>
#include <iostream>

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

// --------------------------------------------------------------------------

void cleanup() {
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }

    SDL_Quit();
}

// --------------------------------------------------------------------------

int main() {
    const int WINDOW_WIDTH = 640;
    const int WINDOW_HEIGHT = 360;

    // --- sdl3 setup ---

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Test", WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    if (!window) {
        std::cerr << "[ERROR] SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "[ERROR] SDL_CreateRenderer error:" << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    // --- main loop ---

    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderLine(renderer, 0, 0, WINDOW_WIDTH - 1, WINDOW_HEIGHT - 1);
        SDL_RenderLine(renderer, 0, WINDOW_HEIGHT - 1, WINDOW_WIDTH - 1, 0);

        SDL_RenderPresent(renderer);
    }

    // --- cleanup and quit ---

    cleanup();
    return 0;
}
