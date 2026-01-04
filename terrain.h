#pragma once

#include <GLAD/glad.h>
#include <glm/glm.hpp>
#include <learnOpenGL/shader_m.h>

struct TerrainInstance
{
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;

    int renderDist;        // grid resolution
    float spacing;         // vertex spacing

    float bowlRadius;
    float bowlDepth;
    float bowlHeight;

    glm::vec2 center;      // centre of bowl in grid space
};

void InitialiseTerrain(TerrainInstance& terrain, bool inverted);

void DrawTerrain(const TerrainInstance& terrain);

void CleanupTerrain();

float TerrainHalfSize();