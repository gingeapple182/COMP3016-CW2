#include "terrain.h"
#include "FastNoiseLite.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <learnOpenGL/shader_m.h>

#include <vector>
#include <cmath>

#define TERRAIN_RENDER_DIST 256
#define TERRAIN_MAP_SIZE (TERRAIN_RENDER_DIST * TERRAIN_RENDER_DIST)
constexpr float TERRAIN_SPACING = 1.0f;

static GLfloat terrainVertices[TERRAIN_MAP_SIZE][6];
static GLuint terrainIndices[(TERRAIN_MAP_SIZE - 1) * (TERRAIN_RENDER_DIST - 1) * 2][3];

constexpr glm::vec2 CAVE_CENTER = glm::vec2(0.0f, 0.0f);

constexpr float BOWL_RADIUS = 300.0f;
constexpr float BOWL_DEPTH = -18.0f;
constexpr float BOWL_HEIGHT = 25.0f;


void InitialiseTerrain(TerrainInstance& terrain, bool inverted)
{
    const int mapSize = terrain.renderDist * terrain.renderDist;

    std::vector<GLfloat> vertices(mapSize * 6);
    std::vector<GLuint> indices((terrain.renderDist - 1) *
        (terrain.renderDist - 1) * 6);

    float xOffset = 0.0f;
    float zOffset = 0.0f;
    int rowIndex = 0;

    FastNoiseLite noise;
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(0.25f);   // large, soft dunes
    noise.SetSeed(1337);         // fixed seed = stable terrain


    for (int i = 0; i < mapSize; i++)
    {
        int v = i * 6;

        vertices[v + 0] = xOffset;
        vertices[v + 2] = zOffset;

        float distance = glm::length(glm::vec2(xOffset, zOffset) - terrain.center);
        float t = glm::clamp(distance / terrain.bowlRadius, 0.0f, 1.0f);
        float smoothT = t * t * (3.0f - 2.0f * t);

        float height = inverted
            ? glm::mix(terrain.bowlHeight, terrain.bowlDepth, smoothT) // cap
            : glm::mix(terrain.bowlDepth, terrain.bowlHeight, smoothT); // bowl

        // --- Perlin noise (sand roughness) ---
        float noiseValue = noise.GetNoise(
            xOffset * 0.2f,
            zOffset * 0.2f
        );

        // Strength of sand variation
        constexpr float NOISE_STRENGTH = 4.5f;

        // Fade noise near the cave centre so it stays readable
        float noiseFalloff = smoothT; // 0 at centre, 1 at edges

        height += noiseValue * NOISE_STRENGTH * noiseFalloff;

        vertices[v + 1] = height;

        // Sand colour with noise variation
        float sandShade = 0.8f + noiseValue * 0.05f;

        // Slope-based tint
        float slopeTint = 1.0f - smoothT * 0.15f;
        slopeTint = glm::clamp(slopeTint, 0.7f, 1.0f);

        vertices[v + 3] = sandShade * slopeTint;
        vertices[v + 4] = sandShade * 0.95f * slopeTint;
        vertices[v + 5] = sandShade * 0.7f * slopeTint;



        xOffset += terrain.spacing;
        rowIndex++;

        if (rowIndex == terrain.renderDist)
        {
            rowIndex = 0;
            xOffset = 0.0f;
            zOffset += terrain.spacing;
        }
    }

    // indices (same logic you already have)
    int idx = 0;
    for (int z = 0; z < terrain.renderDist - 1; z++)
    {
        for (int x = 0; x < terrain.renderDist - 1; x++)
        {
            int topLeft = z * terrain.renderDist + x;
            int topRight = topLeft + 1;
            int bottomLeft = topLeft + terrain.renderDist;
            int bottomRight = bottomLeft + 1;

            indices[idx++] = topLeft;
            indices[idx++] = bottomLeft;
            indices[idx++] = topRight;

            indices[idx++] = topRight;
            indices[idx++] = bottomLeft;
            indices[idx++] = bottomRight;
        }
    }

    // Upload
    glGenVertexArrays(1, &terrain.VAO);
    glGenBuffers(1, &terrain.VBO);
    glGenBuffers(1, &terrain.EBO);

    glBindVertexArray(terrain.VAO);

    glBindBuffer(GL_ARRAY_BUFFER, terrain.VBO);
    glBufferData(GL_ARRAY_BUFFER,
        vertices.size() * sizeof(GLfloat),
        vertices.data(),
        GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}


void DrawTerrain(const TerrainInstance& terrain)
{
    glBindVertexArray(terrain.VAO);

    int indexCount =
        (terrain.renderDist - 1) *
        (terrain.renderDist - 1) * 6;

    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



float TerrainHalfSize() {
	return (TERRAIN_RENDER_DIST * TERRAIN_SPACING) / 2.0f; 
}