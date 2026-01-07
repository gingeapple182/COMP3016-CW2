// -----------------------------------------------------------------------------
// INCLUDES
// -----------------------------------------------------------------------------

// STD
#include <iostream>

// GLAD
#include <glad/glad.h>

// GLM
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/type_ptr.hpp>

// ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

// LEARNOPENGL
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

// PROJECT
#include "main.h"
#include "terrain.h"

using namespace std;
using namespace glm;

// -----------------------------------------------------------------------------
// WINDOW & OPENGL STATE
// -----------------------------------------------------------------------------

int windowWidth;
int windowHeight;

// Legacy enums retained for consistency with earlier labs
enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 };
GLuint VAOs[NumVAOs];

enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
GLuint Buffers[NumBuffers];

// -----------------------------------------------------------------------------
// GAME STATE
// -----------------------------------------------------------------------------

int collectedArtifacts = 0;
const int totalArtifacts = 3;
bool gameWon = false;

// -----------------------------------------------------------------------------
// CAMERA STATE
// -----------------------------------------------------------------------------

vec3 cameraPosition = vec3(0.0f, 60.0f, 123.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

// Yaw : left/right rotation - world
// Pitch : up/down rotation - camera
float cameraYaw = -90.0f;
float cameraPitch = 0.0f;

// Mouse tracking
bool  mouseFirstEntry = true;
float cameraLastXPos = 800.0f / 2.0f;
float cameraLastYPos = 600.0f / 2.0f;

// -----------------------------------------------------------------------------
// GODMODE TOGGLES
//
// Enable/disabel these only for testing
// -----------------------------------------------------------------------------

bool ENABLE_COLLISIONS = true;
bool ENABLE_GRAVITY = true;

// -----------------------------------------------------------------------------
// RENDER PIPELINE STATE
// -----------------------------------------------------------------------------

mat4 mvp;
mat4 model;
mat4 view;
mat4 projection;

// -----------------------------------------------------------------------------
// TIME
// -----------------------------------------------------------------------------

float deltaTime = 0.0f;
float lastFrame = 0.0f;

// -----------------------------------------------------------------------------
// WORLD SCALE SYSTEM + VALUES
// -----------------------------------------------------------------------------

constexpr float WORLD_SCALE = 1.0f;

constexpr float CAVE_SCALE = WORLD_SCALE * 0.5f;
constexpr float PLATFORM_SCALE = WORLD_SCALE * 1.5f;
constexpr float PILLAR_SCALE = WORLD_SCALE * 4.5f;
constexpr float RUIN_SCALE = WORLD_SCALE * 1.0f;
constexpr float STATUE_SCALE = WORLD_SCALE * 0.2f;

// Player unable to fall below this level -- acts as floor within cave
constexpr float PLAYER_MIN_HEIGHT = -36.0f * WORLD_SCALE;

// -----------------------------------------------------------------------------
// COLLISION SYSTEM (CPU SIDE)
//
// Uses low-poly collision meshes authored in Blender (_COL).
// Collision meshes are never rendered and exist only on the CPU.
// Current implementation handles WALL collision only (XZ plane).
// Floor / platform collision is intentionally handled separately.
// -----------------------------------------------------------------------------

// Data structures
struct CollisionMesh
{
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
};

struct InstanceTransform
{
    vec3 position;
    float rotationY;
    vec3 scale;
};

struct CollisionGroup
{
    const CollisionMesh* mesh;
    const std::vector<InstanceTransform>* instances;

    CollisionGroup(const CollisionMesh* m, const std::vector<InstanceTransform>* i) : mesh(m), instances(i) {}
};

// Collision loading
CollisionMesh LoadCollisionMesh(const std::string& path)
{
    CollisionMesh result;

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_GenNormals);

    if (!scene || !scene->HasMeshes())
    {
        std::cerr << "[COLLISION] Failed to load: " << path << std::endl;
        return result;
    }

    // For now, assume ONE mesh per collision OBJ (true for your assets)
    aiMesh* mesh = scene->mMeshes[0];

    // Vertices
    result.vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D& v = mesh->mVertices[i];
        result.vertices.emplace_back(v.x, v.y, v.z);
    }

    // Indices (triangles)
    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        if (face.mNumIndices == 3)
        {
            result.indices.push_back(face.mIndices[0]);
            result.indices.push_back(face.mIndices[1]);
            result.indices.push_back(face.mIndices[2]);
        }
    }

    // Debug output
    std::cout << "[COLLISION] Loaded " << path << std::endl;
    std::cout << "  Vertices : " << result.vertices.size() << std::endl;
    std::cout << "  Triangles: " << result.indices.size() / 3 << std::endl;

    return result;
}

// Transform helpers
glm::mat4 BuildModelMatrix(const InstanceTransform& inst, const glm::vec3& levelOffset)
{
    glm::mat4 m(1.0f);

    m = glm::translate(m, levelOffset);
    m = glm::translate(m, inst.position);
    m = glm::rotate(m, glm::radians(inst.rotationY), glm::vec3(0, 1, 0));
    m = glm::scale(m, inst.scale);

    return m;
}

std::vector<glm::vec3> TransformCollisionVertices(const CollisionMesh& mesh, const InstanceTransform& inst, const glm::vec3& levelOffset)
{
    std::vector<glm::vec3> worldVerts;
    worldVerts.reserve(mesh.vertices.size());

    glm::mat4 modelMatrix = BuildModelMatrix(inst, levelOffset);

    for (const glm::vec3& v : mesh.vertices)
    {
        glm::vec4 worldPos = modelMatrix * glm::vec4(v, 1.0f);
        worldVerts.emplace_back(worldPos.x, worldPos.y, worldPos.z);
    }

    return worldVerts;
}

// Geometry helpers
float DistancePointToSegmentXZ(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b)
{
    glm::vec2 ab = b - a;
    float t = glm::dot(p - a, ab) / glm::dot(ab, ab);
    t = glm::clamp(t, 0.0f, 1.0f);
    glm::vec2 closest = a + t * ab;
    return glm::length(p - closest);
}

bool CircleIntersectsTriangleXZ(const glm::vec2& circleCenter, float radius, const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2)
{
    glm::vec2 p(circleCenter);

    glm::vec2 a(v0.x, v0.z);
    glm::vec2 b(v1.x, v1.z);
    glm::vec2 c(v2.x, v2.z);

    // Check distance to triangle edges
    if (DistancePointToSegmentXZ(p, a, b) <= radius) return true;
    if (DistancePointToSegmentXZ(p, b, c) <= radius) return true;
    if (DistancePointToSegmentXZ(p, c, a) <= radius) return true;

    return false;
}

// Collision check
bool CheckWallCollision(const glm::vec3& playerPos, const std::vector<CollisionGroup>& groups, float radius, const glm::vec3& levelOffset)
{
    glm::vec2 playerXZ(playerPos.x, playerPos.z);

    for (const CollisionGroup& group : groups)
    {
        for (const auto& inst : *group.instances)
        {
            std::vector<glm::vec3> verts =
                TransformCollisionVertices(*group.mesh, inst, levelOffset);

            for (size_t i = 0; i + 2 < group.mesh->indices.size(); i += 3)
            {
                const glm::vec3& v0 = verts[group.mesh->indices[i]];
                const glm::vec3& v1 = verts[group.mesh->indices[i + 1]];
                const glm::vec3& v2 = verts[group.mesh->indices[i + 2]];

                if (CircleIntersectsTriangleXZ(playerXZ, radius, v0, v1, v2))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

// -----------------------------------------------------------------------------
// PLAYER STATE
// -----------------------------------------------------------------------------

vec3 playerPosition = cameraPosition;
vec3 playerVelocity = vec3(0.0f);

constexpr float PLAYER_HEIGHT = 6.1f * WORLD_SCALE;
constexpr float PLAYER_RADIUS = 0.35f * WORLD_SCALE;

// -----------------------------------------------------------------------------
// SCENE ANCHOR + HELPERS
// 
// BlenderToOPenGL: Converts Blender world coordinates to OpenGL world coordinates
// Blender: X = left/right, Y = forward, Z = up
// OpenGL:  X = left/right, Y = up,      Z = -forward
//
// -----------------------------------------------------------------------------

const vec3 LEVEL_OFFSET = vec3(0.0f, -30.0f, 100.0f);

vec3 BlenderToOpenGL(float bx, float by, float bz)
{
    return vec3(bx, bz, -by);
}

static float GetTerrainHeightFromInstance(const TerrainInstance& t, float worldX, float worldZ)
{
    glm::vec2 pos(worldX, worldZ);
    float distance = glm::length(pos);

    float t01 = glm::clamp(distance / t.bowlRadius, 0.0f, 1.0f);
    float smoothT = t01 * t01 * (3.0f - 2.0f * t01);

    return glm::mix(t.bowlDepth, t.bowlHeight, smoothT);
}

bool CheckWinCondition(const vec3& playerPos, const InstanceTransform& trigger, float radius)
{
	vec3 triggerWorldPos = trigger.position + LEVEL_OFFSET;
    
    vec2 playerXZ(playerPos.x, playerPos.z);
    vec2 triggerXZ(triggerWorldPos.x, triggerWorldPos.z);

    float distance = length(playerXZ - triggerXZ);
    return distance <= radius;
}

// -----------------------------------------------------------------------------
// RENDERING HELPERS
// -----------------------------------------------------------------------------

void DrawInstances(Shader& shader, Model& modelAsset, const std::vector<InstanceTransform>& instances, const glm::vec3& levelOffset) {
    // -------------------------------------------------------------------------
    // MODEL TRANSFORM REFERENCE
    //
    // model = mat4(1.0f);                      // Reset to WORLD space
    // model = transform(model, LEVEL_OFFSET);  // Level anchor point
    // model = translate(model, position);      // Place object in world
    // model = rotate(model, angle, axis);      // Optional rotation
    // model = scale(model, instance.scale);    // Uniform/non-uniform scale
    // SetMatrices(Shaders);                    // Upload to GPU
    // modelAsset.Draw(Shaders);                // Render object
    // --------------------------------------------------------------------------
    for (const auto& inst : instances)
    {
        model = mat4(1.0f);
        model = translate(model, levelOffset);
        model = translate(model, inst.position);
        model = rotate(model, radians(inst.rotationY), vec3(0, 1, 0));
        model = scale(model, inst.scale);

        SetMatrices(shader);
        modelAsset.Draw(shader);
    }
}

// -----------------------------------------------------------------------------
// Asset locations
// -----------------------------------------------------------------------------

// Walls
vector<InstanceTransform> caveWall1_APositions = {
    // -- //
};
vector<InstanceTransform> caveWall1_BPositions = {
    {
        BlenderToOpenGL(-2.81f, 138.38f, 0.00f), 199.00f, vec3(CAVE_SCALE)
    }
};
vector<InstanceTransform> caveWall1_CPositions = {
    {
        BlenderToOpenGL(3.95f, 66.21f, 0.00f), 60.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-57.57f, 149.02f, 0.00f), 60.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-38.99f, 113.24f, 0.00f), 158.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(80.91f, 149.79f, 0.00f), -13.00f, vec3(CAVE_SCALE)
    }
};
vector<InstanceTransform> caveWall1_DPositions = {
    {
        BlenderToOpenGL(-11.23f, 125.48f, 0.00f), 69.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-19.33f, 104.81f, 0.00f), 159.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(59.19f, 150.46f, 0.00f), 340.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-56.86f, 129.93f, 0.00f), 159.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(80.85f, 77.62f, 0.00f), 249.00f, vec3(CAVE_SCALE)
    }
};

vector<InstanceTransform> caveWall2_APositions = {
    {
        BlenderToOpenGL(0.00f, 0.00f, 0.00f), 0.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(49.74f, 140.21f, 0.00f), 163.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(18.31f, 65.78f, 0.00f), 152.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(73.03f, 86.96f, 0.00f), 248.75f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-2.98f, 119.03f, 0.00f), 248.00f, vec3(CAVE_SCALE)
    }
};
vector<InstanceTransform> caveWall2_BPositions = {
    {
        BlenderToOpenGL(21.24f, -13.29f, 0.00f), 294.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-22.12f, -13.29f, 0.00f), 242.00f, vec3(CAVE_SCALE)
    }
};
vector<InstanceTransform> caveWall2_CPositions = {
    {
        BlenderToOpenGL(50.12f, 66.44f, 0.00f), 250.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(71.21f, 119.85f, 0.00f), 340.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-0.34f, 89.78f, 0.00f), 160.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(19.94f, 137.42f, 0.00f), 70.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(99.52f, 129.61f, 0.00f), 340.00f, vec3(CAVE_SCALE)
    }
};

vector<InstanceTransform> caveWall3Positions = {
    {
        BlenderToOpenGL(-12.00f, 32.00f, 0.00f), 75.00f,  vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(19.00f, 32.00f, 0.00f), 75.00f, vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(24.44f, 169.37f, 0.00f), 354.00f,  vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(100.01f, 96.51f, 0.00f), 67.00f, vec3(CAVE_SCALE)
    }
};

vector<InstanceTransform> caveWall4_APositions = {
    {
        BlenderToOpenGL(-35.79f, 162.31f, 0.00f),  13.00f, vec3(CAVE_SCALE)
    }
};
vector<InstanceTransform> caveWall4_DPositions = {
    {
        BlenderToOpenGL(53.34f, 107.70f, -5.03f), 0.00f,  vec3(CAVE_SCALE)
    }
};

// Platforms -- ceiling
vector<InstanceTransform> cavePlatform2_1Positions = {
    {
        BlenderToOpenGL(4.76f, 28.36f, 14.50f), 279.00f, vec3(PLATFORM_SCALE)
    }
};
vector<InstanceTransform> cavePlatform2_2Positions = {
    {
        BlenderToOpenGL(15.93f, 154.37f, 15.00f), 29.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(-38.04f, 139.17f, 14.00f), 41.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(62.10f, 133.82f, 14.00f), 221.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(86.50f, 126.87f, 15.00f), 128.00f,  vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(60.87f, 84.09f, 14.50f), 242.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(4.83f, 110.31f, 14.50f), 221.00f, vec3(PLATFORM_SCALE)
    }
};
vector<InstanceTransform> cavePlatform2_4Positions = {
    {
        BlenderToOpenGL(22.04f, 77.04f, 14.00f), 40.00f, vec3(PLATFORM_SCALE)
    }
};

// Platforms -- floors
vector<InstanceTransform> cavePlatform2_2FloorPositions = {
    {
        BlenderToOpenGL(15.93f, 154.37f, -14.99f), 29.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(-38.04f, 139.17f, -15.99f), 41.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(62.10f, 133.82f, -15.99f), 221.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(86.50f, 126.87f, -14.99f), 128.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(60.87f, 84.09f, -15.49f), 242.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(4.83f, 110.31f, -15.49f), 221.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(6.18f, 35.50f, -14.99f), 117.00f, vec3(PLATFORM_SCALE)
    }
};
vector<InstanceTransform> cavePlatform2_4FloorPositions = {
    {
        BlenderToOpenGL(22.04f, 77.04f, -14.64f), 40.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(32.27f, 57.62f, -14.14f), 40.00f, vec3(PLATFORM_SCALE)
    },
    {
        BlenderToOpenGL(45.00f, 123.82f, -13.96f), -21.00f, vec3(PLATFORM_SCALE)
    }
};

// Pillars
vector<InstanceTransform> cavePillarPositions = {
    {
        BlenderToOpenGL(16.82f, 96.48f, -7.35f), 0.00f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(42.46f, 86.03f, -7.35f), 0.00f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(27.51f, 120.49f, -7.35f), 0.00f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-40.27f, 119.20f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-45.65f, 121.63f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-35.05f, 116.93f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-29.68f, 114.50f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-50.44f, 124.02f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-55.81f, 126.44f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-61.03f, 128.72f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-58.59f, 134.45f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-27.23f, 120.23f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-22.45f, 131.23f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-53.81f, 145.45f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-56.25f, 139.72f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-24.89f, 125.50f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-51.57f, 150.29f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-20.22f, 136.07f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    },
    {
        BlenderToOpenGL(-36.19f, 143.20f, -8.03f), 66.93f, vec3(PILLAR_SCALE)
    }
};

// Temple
vector<InstanceTransform> templePositions = {
    {
        BlenderToOpenGL(-36.20f, 122.53f, -7.33f), 116.00f, vec3(RUIN_SCALE)
    },
    {
        BlenderToOpenGL(-52.09f, 135.41f, -7.33f), 26.00f, vec3(RUIN_SCALE)
    }
};

// R2D2
vector<InstanceTransform> r2d2Positions = {
	{
		BlenderToOpenGL(-38.35f,  130.43f, -7.77f), 156.00f, vec3(WORLD_SCALE)
	}
};


int main()
{
    // -------------------------------------------------------------------------
    // GLFW INITIALISATION
    // -------------------------------------------------------------------------
    glfwInit();

    windowWidth = 1280;
    windowHeight = 720;
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Oliver Cole", NULL, NULL);

    if (window == NULL)
    {
        cout << "GLFW Window did not instantiate\n";
        glfwTerminate();
        return -1;
    }

    // Lock and hide cursor for FPS-style camera
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Bind OpenGL context
    glfwMakeContextCurrent(window);


    // -------------------------------------------------------------------------
    // GLAD INITIALISATION
    // Must be done AFTER context creation
    // -------------------------------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "GLAD failed to initialise\n";
        return -1;
    }

    // Enable depth testing so closer objects obscure farther ones
    glEnable(GL_DEPTH_TEST);


    // Terrain
    TerrainInstance terrainCap;
    TerrainInstance terrainBowl;

    terrainCap.renderDist = 256;
    terrainCap.spacing = 1.0f;
    terrainCap.bowlRadius = 140.0f;
    terrainCap.bowlDepth = -18.0f;
    terrainCap.bowlHeight = 25.0f;
    terrainCap.center = glm::vec2(128.0f, 128.0f);

    terrainBowl.renderDist = 1024;
    terrainBowl.spacing = 2.0f;
    terrainBowl.bowlRadius = 800.0f;
    terrainBowl.bowlDepth = -40.0f;
    terrainBowl.bowlHeight = 60.0f;
    terrainBowl.center = glm::vec2(1024.0f, 1024.0f);

    InitialiseTerrain(terrainCap, true);   // inverted
    InitialiseTerrain(terrainBowl, false); // normal bowl


    // -------------------------------------------------------------------------
    // SHADERS & MODELS
    //
    // LearnOpenGL handles VAOs/VBOs internally for models.
    // Collision models are loaded the same way, but will NOT be rendered.
    // -----------------------------------------------------------------------------
    Shader Shaders("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    Shader terrainShaders("shaders/terrain.vert", "shaders/terrain.frag");

    // -------------------------------------------------------------------------
    // Cave walls (visual)
    // -------------------------------------------------------------------------
    Model CaveWall1_A("media/cave/CaveWalls1/CaveWalls1_A.obj");
    Model CaveWall1_B("media/cave/CaveWalls1/CaveWalls1_B.obj");
    Model CaveWall1_C("media/cave/CaveWalls1/CaveWalls1_C.obj");
    Model CaveWall1_D("media/cave/CaveWalls1/CaveWalls1_D.obj");

    Model CaveWall2_A("media/cave/CaveWalls2/CaveWalls2_A.obj");
    Model CaveWall2_B("media/cave/CaveWalls2/CaveWalls2_B.obj");
    Model CaveWall2_C("media/cave/CaveWalls2/CaveWalls2_C.obj");

    Model CaveWall3("media/cave/CaveWalls3/CaveWalls3.obj");

    Model CaveWall4_A("media/cave/CaveWalls4/CaveWalls4_A.obj");
    Model CaveWall4_B("media/cave/CaveWalls4/CaveWalls4_B.obj");
    Model CaveWall4_C("media/cave/CaveWalls4/CaveWalls4_C.obj");
    Model CaveWall4_D("media/cave/CaveWalls4/CaveWalls4_D.obj");

    // -------------------------------------------------------------------------
    // Cave walls (collision meshes – CPU only)
    // -------------------------------------------------------------------------
    CollisionMesh CaveWall1_A_Collision = LoadCollisionMesh("media/cave/CaveWalls1/CaveWalls1_A_COL.obj");
    CollisionMesh CaveWall1_B_Collision = LoadCollisionMesh("media/cave/CaveWalls1/CaveWalls1_B_COL.obj");
    CollisionMesh CaveWall1_C_Collision = LoadCollisionMesh("media/cave/CaveWalls1/CaveWalls1_C_COL.obj");
    CollisionMesh CaveWall1_D_Collision = LoadCollisionMesh("media/cave/CaveWalls1/CaveWalls1_D_COL.obj");

    CollisionMesh CaveWall2_A_Collision = LoadCollisionMesh("media/cave/CaveWalls2/CaveWalls2_A_COL.obj");
    CollisionMesh CaveWall2_B_Collision = LoadCollisionMesh("media/cave/CaveWalls2/CaveWalls2_B_COL.obj");
    CollisionMesh CaveWall2_C_Collision = LoadCollisionMesh("media/cave/CaveWalls2/CaveWalls2_C_COL.obj");
    CollisionMesh CaveWall2_D_Collision = LoadCollisionMesh("media/cave/CaveWalls2/CaveWalls2_D_COL.obj");

    CollisionMesh CaveWall3_Collision = LoadCollisionMesh("media/cave/CaveWalls3/CaveWalls3_COL.obj");

    CollisionMesh CaveWall4_A_Collision = LoadCollisionMesh("media/cave/CaveWalls4/CaveWalls4_A_COL.obj");
    CollisionMesh CaveWall4_B_Collision = LoadCollisionMesh("media/cave/CaveWalls4/CaveWalls4_B_COL.obj");
    CollisionMesh CaveWall4_C_Collision = LoadCollisionMesh("media/cave/CaveWalls4/CaveWalls4_C_COL.obj");
    CollisionMesh CaveWall4_D_Collision = LoadCollisionMesh("media/cave/CaveWalls4/CaveWalls4_D_COL.obj");

    // -------------------------------------------------------------------------
    // Cave platforms (visual)
    // -------------------------------------------------------------------------
    Model CavePlatform2_1("media/cave/CavePlatform2/CavePlatform2_1.obj");
    Model CavePlatform2_2("media/cave/CavePlatform2/CavePlatform2_2.obj");
    Model CavePlatform2_3("media/cave/CavePlatform2/CavePlatform2_3.obj");
    Model CavePlatform2_4("media/cave/CavePlatform2/CavePlatform2_4.obj");

    // -------------------------------------------------------------------------
    // Cave platforms (collision meshes – CPU only)
    // -------------------------------------------------------------------------
    CollisionMesh CavePlatform2_1_Collision = LoadCollisionMesh("media/cave/CavePlatform2/CavePlatform2_1_COL.obj");
    CollisionMesh CavePlatform2_2_Collision = LoadCollisionMesh("media/cave/CavePlatform2/CavePlatform2_2_COL.obj");
    CollisionMesh CavePlatform2_3_Collision = LoadCollisionMesh("media/cave/CavePlatform2/CavePlatform2_3_COL.obj");
    CollisionMesh CavePlatform2_4_Collision = LoadCollisionMesh("media/cave/CavePlatform2/CavePlatform2_4_COL.obj");

    Model GenericTexture("media/Ruins/generic.obj");

    // -------------------------------------------------------------------------
    // Cave pillars (visual)
    // -------------------------------------------------------------------------
	Model CavePillar("media/Ruins/Pillar_egyptian/obj/objPillar.obj");

    // -------------------------------------------------------------------------
    // Cave pillars (collision meshes - CPU only)
    // -------------------------------------------------------------------------
	CollisionMesh CavePillar_Collision = LoadCollisionMesh("media/Ruins/Pillar_egyptian/obj/objPillar_COL.obj");

	Model R2D2("media/Statues/Low_Poly_R2D2.obj");


    // -------------------------------------------------------------------------
    // Wall collision groups (explicit opt-in)
    // -------------------------------------------------------------------------
    std::vector<CollisionGroup> wallCollisionGroups = {
        { &CaveWall1_A_Collision, &caveWall1_APositions },
        { &CaveWall1_B_Collision, &caveWall1_BPositions },
        { &CaveWall1_C_Collision, &caveWall1_CPositions },
        { &CaveWall1_D_Collision, &caveWall1_DPositions },

        //{ &CaveWall2_A_Collision, &caveWall2_APositions }, // commented out to avoid awkward collision jank
        { &CaveWall2_B_Collision, &caveWall2_BPositions },
        { &CaveWall2_C_Collision, &caveWall2_CPositions },

        { &CaveWall3_Collision,   &caveWall3Positions },

        { &CaveWall4_A_Collision, &caveWall4_APositions },
        { &CaveWall4_D_Collision, &caveWall4_DPositions },

		{ &CavePillar_Collision,   &cavePillarPositions}
    };

    // -------------------------------------------------------------------------
    // WIN CONDITION SETUP (temporary test)
    // -------------------------------------------------------------------------
    const InstanceTransform& artefactTransform = r2d2Positions[0];
    const float TRIGGER_RADIUS = 10.0f;

    Shaders.use();

    // -------------------------------------------------------------------------
    // VIEWPORT & CALLBACKS
    // -----------------------------------------------------------------------------
    glViewport(0, 0, 1280, 720);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);



    // -------------------------------------------------------------------------
    // PROJECTION MATRIX
    //  FOV
    //  Aspect
    //  Near plane
    //  Far plane
    // -----------------------------------------------------------------------------
    projection = perspective(radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 700.0f);


    {
        const InstanceTransform& testInstance = caveWall2_CPositions[0];

        std::vector<glm::vec3> worldVerts = TransformCollisionVertices(CaveWall2_C_Collision, testInstance, LEVEL_OFFSET);

        std::cout << "[COLLISION DEBUG] First 5 world vertices:\n";
        for (int i = 0; i < 5 && i < worldVerts.size(); ++i)
        {
            std::cout << "  "
                << worldVerts[i].x << ", "
                << worldVerts[i].y << ", "
                << worldVerts[i].z << "\n";
        }
    }
    // -------------------------------------------------------------------------
    // RENDER LOOP
    // -----------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // Time step
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Input
        ProcessUserInput(window);

        // ---------------------------------------------------------------------
        // WIN CONDITION CHECK
        // ---------------------------------------------------------------------
        if (CheckWinCondition(playerPosition, artefactTransform, TRIGGER_RADIUS))
        {
            std::cout << "\n=================================\n";
            std::cout << " YOU FOUND THE ARTEFACT!\n";
            std::cout << "        YOU WIN\n";
            std::cout << "=================================\n";

            glfwSetWindowShouldClose(window, true);
        }

        // ---------------------------------------------------------------------
        // PLAYER PHYSICS (TERRAIN ONLY)
        // ---------------------------------------------------------------------
        if (ENABLE_GRAVITY)
        {
            const float GRAVITY = -25.0f * WORLD_SCALE;
            playerVelocity.y += GRAVITY * deltaTime;
        }

        // Predict next position ONCE
        vec3 nextPosition = playerPosition + playerVelocity * deltaTime;

        // Terrain collision (Y only)
        if (ENABLE_COLLISIONS)
        {
            // Sample height at the *predicted* horizontal position (nextPosition),
            // so it matches what you're about to apply this frame.
            float terrainHeight = GetTerrainHeightFromInstance(terrainBowl, nextPosition.x, nextPosition.z);

            float minY = terrainHeight + PLAYER_HEIGHT;

            if (nextPosition.y < minY)
            {
                nextPosition.y = minY;
                playerVelocity.y = 0.0f;
            }
        }

        // Cave floor height
        {
            float absoluteMinY = PLAYER_MIN_HEIGHT + PLAYER_HEIGHT;

            if (nextPosition.y < absoluteMinY)
            {
                nextPosition.y = absoluteMinY;
                playerVelocity.y = 0.0f;
            }
        }


        // ---------------------------------------------------------------------
        // WALL COLLISION (all registered wall groups)
        // ---------------------------------------------------------------------
        if (ENABLE_COLLISIONS)
        {
            // X axis
            vec3 testX(nextPosition.x, playerPosition.y, playerPosition.z);

            if (CheckWallCollision(testX, wallCollisionGroups, PLAYER_RADIUS, LEVEL_OFFSET))
            {
                nextPosition.x = playerPosition.x;
            }

            // Z axis
            vec3 testZ(nextPosition.x, playerPosition.y, nextPosition.z);

            if (CheckWallCollision(testZ, wallCollisionGroups, PLAYER_RADIUS, LEVEL_OFFSET))
            {
                nextPosition.z = playerPosition.z;
            }
        }




        // Apply
        playerPosition = nextPosition;

        // Camera follows player
        cameraPosition = playerPosition;


        // Clear buffers
        glClearColor(0.25f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Cull back-facing triangles for performance
        glEnable(GL_CULL_FACE);

        // ---------------------------------------------------------------------
        // VIEW MATRIX
        // ---------------------------------------------------------------------
        view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

        // ---------------------------------------------------------------------
        // TERRAIN
        // ---------------------------------------------------------------------
        terrainShaders.use();

        // Large bowl first
        model = mat4(1.0f);
        model = translate(model, vec3(-terrainBowl.center.x, 0.0f, -terrainBowl.center.y));
        SetMatrices(terrainShaders);
        DrawTerrain(terrainBowl);


        // ---------------------------------------------------------------------
        // CAVE WALLS 
        // ---------------------------------------------------------------------
        Shaders.use();
        // Cave walls
        //DrawInstances(Shaders, CaveWall1_A, caveWall1_APositions, LEVEL_OFFSET);
        DrawInstances(Shaders, CaveWall1_B, caveWall1_BPositions, LEVEL_OFFSET);
        DrawInstances(Shaders, CaveWall1_C, caveWall1_CPositions, LEVEL_OFFSET);
        DrawInstances(Shaders, CaveWall1_D, caveWall1_DPositions, LEVEL_OFFSET);

        DrawInstances(Shaders, CaveWall2_A, caveWall2_APositions, LEVEL_OFFSET);
        DrawInstances(Shaders, CaveWall2_B, caveWall2_BPositions, LEVEL_OFFSET);
        DrawInstances(Shaders, CaveWall2_C, caveWall2_CPositions, LEVEL_OFFSET);

        DrawInstances(Shaders, CaveWall3, caveWall3Positions, LEVEL_OFFSET);

        DrawInstances(Shaders, CaveWall4_A, caveWall4_APositions, LEVEL_OFFSET);
        DrawInstances(Shaders, CaveWall4_D, caveWall4_DPositions, LEVEL_OFFSET);

        // Platforms
        DrawInstances(Shaders, CavePlatform2_1, cavePlatform2_1Positions, LEVEL_OFFSET);
        DrawInstances(Shaders, CavePlatform2_2, cavePlatform2_2Positions, LEVEL_OFFSET);
        DrawInstances(Shaders, CavePlatform2_4, cavePlatform2_4Positions, LEVEL_OFFSET);

        DrawInstances(Shaders, CavePlatform2_2, cavePlatform2_2FloorPositions, LEVEL_OFFSET);
        DrawInstances(Shaders, CavePlatform2_4, cavePlatform2_4FloorPositions, LEVEL_OFFSET);

        // Generic fake asset for shader fixing
		DrawInstances(Shaders, GenericTexture, templePositions, LEVEL_OFFSET);

        // Cave pillars
        DrawInstances(Shaders, CavePillar, cavePillarPositions, LEVEL_OFFSET);

		// R2D2
		DrawInstances(Shaders, R2D2, r2d2Positions, LEVEL_OFFSET);


        // Swap buffers & poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// -----------------------------------------------------------------------------
// CALLBACKS & INPUT
// -----------------------------------------------------------------------------

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (mouseFirstEntry)
    {
        cameraLastXPos = (float)xpos;
        cameraLastYPos = (float)ypos;
        mouseFirstEntry = false;
    }

    float xOffset = (float)xpos - cameraLastXPos;
    float yOffset = cameraLastYPos - (float)ypos;

    cameraLastXPos = (float)xpos;
    cameraLastYPos = (float)ypos;

    const float sensitivity = 0.025f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    cameraYaw += xOffset;
    cameraPitch += yOffset;

    if (cameraPitch > 89.0f)  cameraPitch = 89.0f;
    if (cameraPitch < -89.0f) cameraPitch = -89.0f;

    vec3 direction;
    direction.x = cos(radians(cameraYaw)) * cos(radians(cameraPitch));
    direction.y = sin(radians(cameraPitch));
    direction.z = sin(radians(cameraYaw)) * cos(radians(cameraPitch));
    cameraFront = normalize(direction);
}

void ProcessUserInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float moveSpeed = 60.0f * WORLD_SCALE;

    // RESET horizontal velocity every frame
    playerVelocity.x = 0.0f;
    playerVelocity.z = 0.0f;

    vec3 forward;

    if (ENABLE_GRAVITY)
    {
        forward = normalize(vec3(cameraFront.x, 0.0f, cameraFront.z));
    }
    else
    {
        forward = normalize(cameraFront);
    }

    vec3 right = normalize(cross(forward, cameraUp));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        playerVelocity += forward * moveSpeed;

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        playerVelocity -= forward * moveSpeed;

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        playerVelocity -= right * moveSpeed;

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        playerVelocity += right * moveSpeed;
}

// -----------------------------------------------------------------------------
// MVP UPLOAD
// -----------------------------------------------------------------------------
void SetMatrices(Shader& ShaderProgramIn)
{
    mvp = projection * view * model;
    ShaderProgramIn.setMat4("mvpIn", mvp);
}