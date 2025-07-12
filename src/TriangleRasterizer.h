#ifndef TRIANGLE_RASTERIZER_H
#define TRIANGLE_RASTERIZER_H

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec2 screen_coord;
    glm::vec3 color;
    glm::vec2 tex_coord;
    float z;
};

struct Triangle {
    Vertex v1;
    Vertex v2;
    Vertex v3;
};

class TriangleRasterizer {

public:

    TriangleRasterizer(int depth_buffer_width, int depth_buffer_height);
    ~TriangleRasterizer();

    void rasterize(SDL_Renderer* renderer, const Triangle& triangle, SDL_Surface* texture);
    void clear_depth_buffer();
    void resize_depth_buffer(int new_width, int new_height);

private:

    int depth_buffer_width;
    int depth_buffer_height;
    std::vector<float> depth_buffer;

    float edge(const glm::vec2& a, const glm::vec2& b, const glm::vec2& p);
    glm::vec3 sample_locked_surface(SDL_Surface* surface, const SDL_PixelFormatDetails* surface_pixel_format_details, const glm::vec2& tex_coord);
};

#endif