#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <algorithm>

#include "TriangleRasterizer.h"

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Surface* texture_surface = nullptr;

// --------------------------------------------------------------------------

// Linearly remap an input x in [a, b] to [u, v].
float linear_remap(float x, float a, float b, float u, float v) {
    return (v - u) / (b - a) * (x - a) + u;
}

// --------------------------------------------------------------------------

void cleanup() {
    if (texture_surface != nullptr) {
        SDL_DestroySurface(texture_surface);
    }

    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
    }

    if (window != nullptr) {
        SDL_DestroyWindow(window);
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

    window = SDL_CreateWindow("Triangle Rasterization Test", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        std::cerr << "[ERROR] SDL_CreateWindow error: " << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        std::cerr << "[ERROR] SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    texture_surface = IMG_Load("test-texture.png");
    if (texture_surface == nullptr) {
        std::cerr << "[ERROR] IMG_Load error: " << SDL_GetError() << std::endl;
        cleanup();
        return 1;
    }

    const SDL_PixelFormatDetails* pixel_format_details = SDL_GetPixelFormatDetails(texture_surface->format);
    if (pixel_format_details->bytes_per_pixel != 4) {
        SDL_Surface* converted_surface = SDL_ConvertSurface(texture_surface, SDL_PIXELFORMAT_RGBA8888);
        if (converted_surface == nullptr) {
            std::cerr << "[ERROR] SDL_ConvertSurface could not convert loaded image surface to RGBA8888 pixel format: " << SDL_GetError() << std::endl;
            cleanup();
            return 1;
        } else {
            SDL_DestroySurface(texture_surface);
            texture_surface = converted_surface;
        }
    }

    SDL_SetRenderVSync(renderer, 1);

    TriangleRasterizer triangle_rasterizer;

    bool is_rasterizing_textures = false;

    const bool* keyboard_state = SDL_GetKeyboardState(nullptr);
    bool previous_render_mode_key_state = false;

    float rotation_degrees_y = 0.0f;
    const float ROTATION_DEGREES_Y_PER_SECOND = 360.0f / 8.0f;

    Uint64 previous_timestamp = SDL_GetTicks();

    float seconds_left_until_fps_report = 1.0f;
    int frame_count = 0;

    // --- main loop ---

    bool running = true;
    SDL_Event event;
    while (running) {
        Uint64 current_timestamp = SDL_GetTicks();
        float delta_time = (current_timestamp - previous_timestamp) / 1000.0f;
        previous_timestamp = current_timestamp;

        frame_count++;
        seconds_left_until_fps_report -= delta_time;
        if (seconds_left_until_fps_report <= 0.0f) {
            SDL_SetWindowTitle(window, (std::string("FPS: ") + std::to_string(frame_count)).c_str());

            seconds_left_until_fps_report = 1.0f;
            frame_count = 0;
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_ESCAPE) {
                running = false;
            }
        }

        const bool current_render_mode_key_state = keyboard_state[SDL_SCANCODE_SPACE];
        if (!previous_render_mode_key_state && current_render_mode_key_state) {
            is_rasterizing_textures = !is_rasterizing_textures;
        }
        previous_render_mode_key_state = current_render_mode_key_state;

        SDL_SetRenderDrawColor(renderer, 32, 32, 32, 255);
        SDL_RenderClear(renderer);

        int render_width, render_height;
        SDL_GetCurrentRenderOutputSize(renderer, &render_width, &render_height);

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(render_width) / render_height,
            0.1f,
            100.0f
        );

        glm::vec3 camera_position = glm::vec3(0.0f, 0.0f, 5.0f);
        glm::mat4 view = glm::lookAt(camera_position, camera_position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        rotation_degrees_y += ROTATION_DEGREES_Y_PER_SECOND * delta_time;
        glm::mat4 model = glm::rotate(glm::mat4(1.0f), glm::radians(rotation_degrees_y), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::vec3 world_p0 = glm::vec3(-1.0f, -1.0f, 0.0f);
        glm::vec3 world_p1 = glm::vec3( 1.0f, -1.0f, 0.0f);
        glm::vec3 world_p2 = glm::vec3( 1.0f,  1.0f, 0.0f);
        glm::vec3 world_p3 = glm::vec3(-1.0f,  1.0f, 0.0f);

        glm::vec4 p0 = projection * view * model * glm::vec4(world_p0, 1.0f);
        glm::vec4 p1 = projection * view * model * glm::vec4(world_p1, 1.0f);
        glm::vec4 p2 = projection * view * model * glm::vec4(world_p2, 1.0f);
        glm::vec4 p3 = projection * view * model * glm::vec4(world_p3, 1.0f);

        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w;
        p3 /= p3.w;

        Triangle tri1 = {
            {{ linear_remap(p0.x, -1.0f, 1.0f, 0, render_width - 1.0f), linear_remap(p0.y, -1.0f, 1.0f, render_height - 1.0f, 0.0f) }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
            {{ linear_remap(p1.x, -1.0f, 1.0f, 0, render_width - 1.0f), linear_remap(p1.y, -1.0f, 1.0f, render_height - 1.0f, 0.0f) }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f }},
            {{ linear_remap(p2.x, -1.0f, 1.0f, 0, render_width - 1.0f), linear_remap(p2.y, -1.0f, 1.0f, render_height - 1.0f, 0.0f) }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }},
        };
        Triangle tri2 = {
            {{ linear_remap(p0.x, -1.0f, 1.0f, 0, render_width - 1.0f), linear_remap(p0.y, -1.0f, 1.0f, render_height - 1.0f, 0.0f) }, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f }},
            {{ linear_remap(p2.x, -1.0f, 1.0f, 0, render_width - 1.0f), linear_remap(p2.y, -1.0f, 1.0f, render_height - 1.0f, 0.0f) }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 1.0f }},
            {{ linear_remap(p3.x, -1.0f, 1.0f, 0, render_width - 1.0f), linear_remap(p3.y, -1.0f, 1.0f, render_height - 1.0f, 0.0f) }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 1.0f }},
        };

        glm::vec3 tri1_normal = glm::normalize(glm::cross(glm::vec3(p1 - p0), glm::vec3(p2 - p0)));
        glm::vec3 tri2_normal = glm::normalize(glm::cross(glm::vec3(p2 - p0), glm::vec3(p3 - p0)));
        bool cull_tri1 = glm::dot(glm::vec3(p0) - camera_position, tri1_normal) >= 0;
        bool cull_tri2 = glm::dot(glm::vec3(p0) - camera_position, tri2_normal) >= 0;

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        if (cull_tri1) {
            SDL_RenderLine(renderer, tri1.v1.coordinates.x, tri1.v1.coordinates.y, tri1.v2.coordinates.x, tri1.v2.coordinates.y);
            SDL_RenderLine(renderer, tri1.v2.coordinates.x, tri1.v2.coordinates.y, tri1.v3.coordinates.x, tri1.v3.coordinates.y);
            SDL_RenderLine(renderer, tri1.v3.coordinates.x, tri1.v3.coordinates.y, tri1.v1.coordinates.x, tri1.v1.coordinates.y);
        }
        if (cull_tri2) {
            SDL_RenderLine(renderer, tri2.v1.coordinates.x, tri2.v1.coordinates.y, tri2.v2.coordinates.x, tri2.v2.coordinates.y);
            SDL_RenderLine(renderer, tri2.v2.coordinates.x, tri2.v2.coordinates.y, tri2.v3.coordinates.x, tri2.v3.coordinates.y);
            SDL_RenderLine(renderer, tri2.v3.coordinates.x, tri2.v3.coordinates.y, tri2.v1.coordinates.x, tri2.v1.coordinates.y);
        }

        SDL_Surface* render_texture_surface = nullptr;
        if (is_rasterizing_textures) {
            render_texture_surface = texture_surface;
        }

        if (!cull_tri1) {
            triangle_rasterizer.rasterize(renderer, tri1, render_texture_surface);
        }
        if (!cull_tri2) {
            triangle_rasterizer.rasterize(renderer, tri2, render_texture_surface);
        }

        SDL_RenderPresent(renderer);
    }

    // --- cleanup and quit ---

    cleanup();
    return 0;
}
