#include "terrain.h"

#include <GLAD/glad.h>
#include <glm/glm.hpp>
#include <learnOpenGL/shader_m.h>


#define TERRAIN_RENDER_DIST 256
#define TERRAIN_MAP_SIZE (TERRAIN_RENDER_DIST * TERRAIN_RENDER_DIST)
constexpr float TERRAIN_SPACING = 1.0f;

static GLuint terrainVAO = 0;
static GLuint terrainVBO = 0;
static GLuint terrainEBO = 0;

static GLfloat terrainVertices[TERRAIN_MAP_SIZE][6];
static GLuint terrainIndices[(TERRAIN_MAP_SIZE - 1) * (TERRAIN_RENDER_DIST - 1) * 2][3];

constexpr glm::vec2 CAVE_CENTER = glm::vec2(0.0f, 0.0f);

constexpr float BOWL_RADIUS = 300.0f;
constexpr float BOWL_DEPTH = -18.0f;
constexpr float BOWL_HEIGHT = 25.0f;



void InitialiseTerrain()
{
	float start = 0.0f;
	float xOffset = start;
	float yOffset = start;

	int rowIndex = 0;

	for (int i = 0; i < TERRAIN_MAP_SIZE; i++)
	{
		// Position (XZ)
		terrainVertices[i][0] = xOffset;
		terrainVertices[i][2] = yOffset;

		// --- BOWL HEIGHT (PER VERTEX) ---
		glm::vec2 caveCenter(TerrainHalfSize(), TerrainHalfSize());
		glm::vec2 worldPos(xOffset, yOffset);

		float distance = glm::length(worldPos - caveCenter);
		float t = glm::clamp(distance / (BOWL_RADIUS * 1.0f), 0.0f, 1.0f);
		float smoothT = pow(t, 1.5f); 
		float height = glm::mix(BOWL_DEPTH, BOWL_HEIGHT, smoothT);
		float edgeT = glm::clamp((distance - BOWL_RADIUS * 1.2f) / 40.0f, 0.0f, 1.0f);
		height += edgeT * edgeT * 30.0f;


		terrainVertices[i][1] = height;

		// Colour
		terrainVertices[i][3] = 0.85f;
		terrainVertices[i][4] = 0.8f;
		terrainVertices[i][5] = 0.55f;

		// Advance grid
		xOffset += TERRAIN_SPACING;
		rowIndex++;

		if (rowIndex == TERRAIN_RENDER_DIST)
		{
			rowIndex = 0;
			xOffset = start;
			yOffset += TERRAIN_SPACING;
		}
	}


	// Generate quads
	int squaresPerRow = TERRAIN_RENDER_DIST - 1;
	int colOffset = 0;
	int rowOffset = 0;
	rowIndex = 0;

	int index = 0;
	for (int i = 0; i < squaresPerRow * squaresPerRow; i++) {
		// Triangle 1 (top-left, bottom-left, top-right)
		terrainIndices[index][0] = colOffset + rowOffset;
		terrainIndices[index][1] = colOffset + rowOffset + TERRAIN_RENDER_DIST;
		terrainIndices[index][2] = colOffset + rowOffset + 1;
		index++;

		// Triangle 2 (top-right, bottom-left, bottom-right)
		terrainIndices[index][0] = colOffset + rowOffset + 1;
		terrainIndices[index][1] = colOffset + rowOffset + TERRAIN_RENDER_DIST;
		terrainIndices[index][2] = colOffset + rowOffset + TERRAIN_RENDER_DIST + 1;
		index++;


		colOffset++;
		rowIndex++;

		if (rowIndex == squaresPerRow) {
			rowIndex = 0;
			colOffset = 0;
			rowOffset += TERRAIN_RENDER_DIST;
		}
	}

	// Upload to GPU
	glGenVertexArrays(1, &terrainVAO);
	glGenBuffers(1, &terrainVBO);
	glGenBuffers(1, &terrainEBO);

	glBindVertexArray(terrainVAO);
	 
	glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terrainIndices), terrainIndices, GL_STATIC_DRAW);

	// Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Colours
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void DrawTerrain(const Shader& shader)
{
	glBindVertexArray(terrainVAO);

	int indexCount =
		(TERRAIN_RENDER_DIST - 1) *
		(TERRAIN_RENDER_DIST - 1) * 6;

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}


float TerrainHalfSize() {
	return (TERRAIN_RENDER_DIST * TERRAIN_SPACING) / 2.0f; 
}