#ifndef TRIANGLE_RASTERIZER_H
#define TRIANGLE_RASTERIZER_H

#include <SDL3/SDL.h>

struct Triangle {
    SDL_FPoint p1;
    SDL_FPoint p2;
    SDL_FPoint p3;
};

class TriangleRasterizer {

public:

    TriangleRasterizer();
    ~TriangleRasterizer();

    void rasterize(SDL_Renderer* renderer, const Triangle& triangle);

private:

    float edge(const SDL_FPoint& a, const SDL_FPoint& b, const SDL_FPoint& p);
};

#endif