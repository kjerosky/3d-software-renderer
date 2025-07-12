#ifndef TRIANGLE_RASTERIZER_H
#define TRIANGLE_RASTERIZER_H

#include <SDL3/SDL.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec2 screen_coord;
    glm::vec3 color;
    glm::vec2 tex_coord;
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

    float edge(const glm::vec2& a, const glm::vec2& b, const glm::vec2& p);
    glm::vec3 sample_locked_surface(SDL_Surface* surface, const SDL_PixelFormatDetails* surface_pixel_format_details, const glm::vec2& tex_coord);
};

#endif