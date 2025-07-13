#include "primitives.h"

// --------------------------------------------------------------------------

Object primitives::quad(float length, float height, glm::vec3 color, float max_texture_coord) {
    length = glm::abs(length);
    height = glm::abs(height);

    glm::vec3 p0 = glm::vec3(-length / 2, -height / 2, 0.0f);
    glm::vec3 p1 = glm::vec3( length / 2, -height / 2, 0.0f);
    glm::vec3 p2 = glm::vec3( length / 2,  height / 2, 0.0f);
    glm::vec3 p3 = glm::vec3(-length / 2,  height / 2, 0.0f);

    WorldTriangle tri0 = {
        { p0, color, glm::vec2(0.0f,              0.0f             ) },
        { p1, color, glm::vec2(max_texture_coord, 0.0f             ) },
        { p2, color, glm::vec2(max_texture_coord, max_texture_coord) },
    };
    WorldTriangle tri1 = {
        { p0, color, glm::vec2(0.0f,              0.0f             ) },
        { p2, color, glm::vec2(max_texture_coord, max_texture_coord) },
        { p3, color, glm::vec2(0.0f,              max_texture_coord) },
    };

    Object quad;
    quad.add_triangle(tri0);
    quad.add_triangle(tri1);

    return quad;
}

// --------------------------------------------------------------------------

Object primitives::cuboid(float length, float height, float depth, glm::vec3 color, float max_texture_coord) {
    length = glm::abs(length);
    height = glm::abs(height);
    depth = glm::abs(depth);

    glm::vec3 p0 = glm::vec3(-length / 2, -height / 2,  depth / 2);
    glm::vec3 p1 = glm::vec3( length / 2, -height / 2,  depth / 2);
    glm::vec3 p2 = glm::vec3( length / 2,  height / 2,  depth / 2);
    glm::vec3 p3 = glm::vec3(-length / 2,  height / 2,  depth / 2);
    glm::vec3 p4 = glm::vec3(-length / 2, -height / 2, -depth / 2);
    glm::vec3 p5 = glm::vec3( length / 2, -height / 2, -depth / 2);
    glm::vec3 p6 = glm::vec3( length / 2,  height / 2, -depth / 2);
    glm::vec3 p7 = glm::vec3(-length / 2,  height / 2, -depth / 2);

    WorldTriangle face0_tri0 = {
        { p0, color, glm::vec2(0.0f,              0.0f             ) },
        { p1, color, glm::vec2(max_texture_coord, 0.0f             ) },
        { p2, color, glm::vec2(max_texture_coord, max_texture_coord) },
    };
    WorldTriangle face0_tri1 = {
        { p0, color, glm::vec2(0.0f,              0.0f             ) },
        { p2, color, glm::vec2(max_texture_coord, max_texture_coord) },
        { p3, color, glm::vec2(0.0f,              max_texture_coord) },
    };

    WorldTriangle face1_tri0 = {
        { p1, color, glm::vec2(0.0f,              0.0f             ) },
        { p5, color, glm::vec2(max_texture_coord, 0.0f             ) },
        { p6, color, glm::vec2(max_texture_coord, max_texture_coord) },
    };
    WorldTriangle face1_tri1 = {
        { p1, color, glm::vec2(0.0f,              0.0f             ) },
        { p6, color, glm::vec2(max_texture_coord, max_texture_coord) },
        { p2, color, glm::vec2(0.0f,              max_texture_coord) },
    };

    WorldTriangle face2_tri0 = {
        { p5, color, glm::vec2(0.0f,              0.0f             ) },
        { p4, color, glm::vec2(max_texture_coord, 0.0f             ) },
        { p7, color, glm::vec2(max_texture_coord, max_texture_coord) },
    };
    WorldTriangle face2_tri1 = {
        { p5, color, glm::vec2(0.0f,              0.0f             ) },
        { p7, color, glm::vec2(max_texture_coord, max_texture_coord) },
        { p6, color, glm::vec2(0.0f,              max_texture_coord) },
    };

    WorldTriangle face3_tri0 = {
        { p4, color, glm::vec2(0.0f,              0.0f             ) },
        { p0, color, glm::vec2(max_texture_coord, 0.0f             ) },
        { p3, color, glm::vec2(max_texture_coord, max_texture_coord) },
    };
    WorldTriangle face3_tri1 = {
        { p4, color, glm::vec2(0.0f,              0.0f             ) },
        { p3, color, glm::vec2(max_texture_coord, max_texture_coord) },
        { p7, color, glm::vec2(0.0f,              max_texture_coord) },
    };

    WorldTriangle face4_tri0 = {
        { p3, color, glm::vec2(0.0f,              0.0f             ) },
        { p2, color, glm::vec2(max_texture_coord, 0.0f             ) },
        { p6, color, glm::vec2(max_texture_coord, max_texture_coord) },
    };
    WorldTriangle face4_tri1 = {
        { p3, color, glm::vec2(0.0f,              0.0f             ) },
        { p6, color, glm::vec2(max_texture_coord, max_texture_coord) },
        { p7, color, glm::vec2(0.0f,              max_texture_coord) },
    };

    WorldTriangle face5_tri0 = {
        { p1, color, glm::vec2(0.0f,              0.0f             ) },
        { p0, color, glm::vec2(max_texture_coord, 0.0f             ) },
        { p4, color, glm::vec2(max_texture_coord, max_texture_coord) },
    };
    WorldTriangle face5_tri1 = {
        { p1, color, glm::vec2(0.0f,              0.0f             ) },
        { p4, color, glm::vec2(max_texture_coord, max_texture_coord) },
        { p5, color, glm::vec2(0.0f,              max_texture_coord) },
    };

    Object quad;
    quad.add_triangle(face0_tri0);
    quad.add_triangle(face0_tri1);
    quad.add_triangle(face1_tri0);
    quad.add_triangle(face1_tri1);
    quad.add_triangle(face2_tri0);
    quad.add_triangle(face2_tri1);
    quad.add_triangle(face3_tri0);
    quad.add_triangle(face3_tri1);
    quad.add_triangle(face4_tri0);
    quad.add_triangle(face4_tri1);
    quad.add_triangle(face5_tri0);
    quad.add_triangle(face5_tri1);
    return quad;
}
