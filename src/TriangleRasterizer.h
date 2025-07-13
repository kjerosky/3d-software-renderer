#ifndef TRIANGLE_RASTERIZER_H
#define TRIANGLE_RASTERIZER_H

#include <SDL3/SDL.h>
#include <glm/glm.hpp>
#include <vector>

#include "texture.h"

struct Vertex {
    glm::vec2 screen_coord;
    glm::vec3 color;
    glm::vec2 tex_coord;
    float ndc_z;
    float view_z;
};

struct Triangle {
    Vertex v0;
    Vertex v1;
    Vertex v2;
};

class TriangleRasterizer {

public:

    TriangleRasterizer(int depth_buffer_width, int depth_buffer_height);
    ~TriangleRasterizer();

    void rasterize(SDL_Renderer* renderer, const Triangle& triangle, SDL_Surface* texture);
    void clear_depth_buffer();
    void resize_depth_buffer(int new_width, int new_height);
    void set_texture_filter(const texture::TextureFilter texture_filter);
    void set_texture_wrap(const texture::TextureWrap texture_wrap);

private:

    int depth_buffer_width;
    int depth_buffer_height;
    std::vector<float> depth_buffer;

    texture::TextureFilter texture_filter;
    texture::TextureWrap texture_wrap;

    float edge(const glm::vec2& a, const glm::vec2& b, const glm::vec2& p);
};

#endif