#include "Primitives.h"

// --------------------------------------------------------------------------

Object primitives::quad(float length, float height, glm::vec3 color) {
    length = glm::abs(length);
    height = glm::abs(height);

    glm::vec3 p0 = glm::vec3(-length / 2, -height / 2, 0.0f);
    glm::vec3 p1 = glm::vec3( length / 2, -height / 2, 0.0f);
    glm::vec3 p2 = glm::vec3( length / 2,  height / 2, 0.0f);
    glm::vec3 p3 = glm::vec3(-length / 2,  height / 2, 0.0f);

    WorldTriangle tri1 = {
        { p0, color, glm::vec2(0.0f, 0.0f) },
        { p1, color, glm::vec2(1.0f, 0.0f) },
        { p2, color, glm::vec2(1.0f, 1.0f) },
    };
    WorldTriangle tri2 = {
        { p0, color, glm::vec2(0.0f, 0.0f) },
        { p2, color, glm::vec2(1.0f, 1.0f) },
        { p3, color, glm::vec2(0.0f, 1.0f) },
    };

    Object quad;
    quad.add_triangle(tri1);
    quad.add_triangle(tri2);

    return quad;
}

// --------------------------------------------------------------------------

Object primitives::cuboid(float length, float height, float depth, glm::vec3 color) {
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

    WorldTriangle face1_tri1 = {
        { p0, color, glm::vec2(0.0f, 0.0f) },
        { p1, color, glm::vec2(1.0f, 0.0f) },
        { p2, color, glm::vec2(1.0f, 1.0f) },
    };
    WorldTriangle face1_tri2 = {
        { p0, color, glm::vec2(0.0f, 0.0f) },
        { p2, color, glm::vec2(1.0f, 1.0f) },
        { p3, color, glm::vec2(0.0f, 1.0f) },
    };

    WorldTriangle face2_tri1 = {
        { p1, color, glm::vec2(0.0f, 0.0f) },
        { p5, color, glm::vec2(1.0f, 0.0f) },
        { p6, color, glm::vec2(1.0f, 1.0f) },
    };
    WorldTriangle face2_tri2 = {
        { p1, color, glm::vec2(0.0f, 0.0f) },
        { p6, color, glm::vec2(1.0f, 1.0f) },
        { p2, color, glm::vec2(0.0f, 1.0f) },
    };

    WorldTriangle face3_tri1 = {
        { p5, color, glm::vec2(0.0f, 0.0f) },
        { p4, color, glm::vec2(1.0f, 0.0f) },
        { p7, color, glm::vec2(1.0f, 1.0f) },
    };
    WorldTriangle face3_tri2 = {
        { p5, color, glm::vec2(0.0f, 0.0f) },
        { p7, color, glm::vec2(1.0f, 1.0f) },
        { p6, color, glm::vec2(0.0f, 1.0f) },
    };

    WorldTriangle face4_tri1 = {
        { p4, color, glm::vec2(0.0f, 0.0f) },
        { p0, color, glm::vec2(1.0f, 0.0f) },
        { p3, color, glm::vec2(1.0f, 1.0f) },
    };
    WorldTriangle face4_tri2 = {
        { p4, color, glm::vec2(0.0f, 0.0f) },
        { p3, color, glm::vec2(1.0f, 1.0f) },
        { p7, color, glm::vec2(0.0f, 1.0f) },
    };

    WorldTriangle face5_tri1 = {
        { p3, color, glm::vec2(0.0f, 0.0f) },
        { p2, color, glm::vec2(1.0f, 0.0f) },
        { p6, color, glm::vec2(1.0f, 1.0f) },
    };
    WorldTriangle face5_tri2 = {
        { p3, color, glm::vec2(0.0f, 0.0f) },
        { p6, color, glm::vec2(1.0f, 1.0f) },
        { p7, color, glm::vec2(0.0f, 1.0f) },
    };

    WorldTriangle face6_tri1 = {
        { p1, color, glm::vec2(0.0f, 0.0f) },
        { p0, color, glm::vec2(1.0f, 0.0f) },
        { p4, color, glm::vec2(1.0f, 1.0f) },
    };
    WorldTriangle face6_tri2 = {
        { p1, color, glm::vec2(0.0f, 0.0f) },
        { p4, color, glm::vec2(1.0f, 1.0f) },
        { p5, color, glm::vec2(0.0f, 1.0f) },
    };

    Object quad;
    quad.add_triangle(face1_tri1);
    quad.add_triangle(face1_tri2);
    quad.add_triangle(face2_tri1);
    quad.add_triangle(face2_tri2);
    quad.add_triangle(face3_tri1);
    quad.add_triangle(face3_tri2);
    quad.add_triangle(face4_tri1);
    quad.add_triangle(face4_tri2);
    quad.add_triangle(face5_tri1);
    quad.add_triangle(face5_tri2);
    quad.add_triangle(face6_tri1);
    quad.add_triangle(face6_tri2);
    return quad;
}
