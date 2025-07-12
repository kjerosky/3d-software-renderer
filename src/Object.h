#ifndef OBJECT_H
#define OBJECT_H

#include <SDL3/SDL.h>
#include <vector>
#include <glm/glm.hpp>

#include "TriangleRasterizer.h"

struct WorldVertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 tex_coord;
};

struct WorldTriangle {
    WorldVertex p0;
    WorldVertex p1;
    WorldVertex p2;
};

class Object {

public:

    Object();
    ~Object();

    void add_triangle(WorldTriangle& triangle);
    void rasterize(TriangleRasterizer& triangle_rasterizer,
                   SDL_Renderer* renderer,
                   SDL_Surface* texture_surface,
                   glm::mat4& projection,
                   glm::mat4& view,
                   glm::mat4& model);

private:

    std::vector<WorldTriangle> triangles;
};

#endif
