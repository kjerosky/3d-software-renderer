#include "Object.h"

// --------------------------------------------------------------------------

// Linearly remap an input x in [a, b] to [u, v].
float linear_remap(float x, float a, float b, float u, float v) {
    return (v - u) / (b - a) * (x - a) + u;
}

// --------------------------------------------------------------------------

Object::Object() {
    // nothing to do for now
}

// --------------------------------------------------------------------------

Object::~Object() {
    // nothing to do for now
}

// --------------------------------------------------------------------------

void Object::add_triangle(WorldTriangle& triangle) {
    triangles.push_back(triangle);
}

// --------------------------------------------------------------------------

void Object::rasterize(TriangleRasterizer& triangle_rasterizer, SDL_Renderer* renderer, SDL_Surface* texture_surface, glm::mat4& mvp_matrix) {
    int render_width, render_height;
    SDL_GetCurrentRenderOutputSize(renderer, &render_width, &render_height);

    for (WorldTriangle world_triangle : triangles) {
        glm::vec4 p0 = mvp_matrix * glm::vec4(world_triangle.p0.location, 1.0f);
        glm::vec4 p1 = mvp_matrix * glm::vec4(world_triangle.p1.location, 1.0f);
        glm::vec4 p2 = mvp_matrix * glm::vec4(world_triangle.p2.location, 1.0f);

        p0 /= p0.w;
        p1 /= p1.w;
        p2 /= p2.w;

        glm::vec2 screen_p0 = glm::vec2(linear_remap(p0.x, -1.0f, 1.0f, 0, render_width - 1.0f), linear_remap(p0.y, -1.0f, 1.0f, render_height - 1.0f, 0.0f));
        glm::vec2 screen_p1 = glm::vec2(linear_remap(p1.x, -1.0f, 1.0f, 0, render_width - 1.0f), linear_remap(p1.y, -1.0f, 1.0f, render_height - 1.0f, 0.0f));
        glm::vec2 screen_p2 = glm::vec2(linear_remap(p2.x, -1.0f, 1.0f, 0, render_width - 1.0f), linear_remap(p2.y, -1.0f, 1.0f, render_height - 1.0f, 0.0f));

        Triangle triangle = {
            { screen_p0, world_triangle.p0.color, world_triangle.p0.tex_coord },
            { screen_p1, world_triangle.p1.color, world_triangle.p1.tex_coord },
            { screen_p2, world_triangle.p2.color, world_triangle.p2.tex_coord },
        };

        triangle_rasterizer.rasterize(renderer, triangle, texture_surface);
    }
}
