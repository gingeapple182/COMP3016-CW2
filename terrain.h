#pragma once

#include <GLAD/glad.h>
#include <glm/glm.hpp>
#include <learnOpenGL/shader_m.h>

void InitialiseTerrain();

void DrawTerrain(const Shader& shader);

void CleanupTerrain();

float TerrainHalfSize();