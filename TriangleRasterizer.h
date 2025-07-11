#ifndef TRIANGLE_RASTERIZER_H
#define TRIANGLE_RASTERIZER_H

#include <SDL3/SDL.h>
#include <algorithm>

struct Color {
    float r;
    float g;
    float b;

    Color operator*(float x) const {
        return { r * x, g * x, b * x };
    }

    Color operator+(const Color& other) const {
        return { r + other.r, g + other.g, b + other.b };
    }

    void clamp() {
        r = std::clamp(r, 0.0f, 1.0f);
        g = std::clamp(g, 0.0f, 1.0f);
        b = std::clamp(b, 0.0f, 1.0f);
    }
};

struct Vertex {
    SDL_FPoint coordinates;
    Color color;
    SDL_FPoint uv_coordinates;
};

struct Triangle {
    Vertex v1;
    Vertex v2;
    Vertex v3;
};

class TriangleRasterizer {

public:

    TriangleRasterizer();
    ~TriangleRasterizer();

    void rasterize(SDL_Renderer* renderer, const Triangle& triangle, SDL_Surface* texture);

private:

    float edge(const SDL_FPoint& a, const SDL_FPoint& b, const SDL_FPoint& p);
    Color sample_locked_surface(SDL_Surface* surface, const SDL_PixelFormatDetails* surface_pixel_format_details, const SDL_FPoint& texture_coordinates);
};

#endif