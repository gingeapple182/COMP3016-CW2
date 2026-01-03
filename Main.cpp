//STD
#include <iostream>

//GLAD
#include <glad/glad.h>

//GLM
#include "glm/ext/vector_float3.hpp"
#include <glm/gtc/type_ptr.hpp> // Access to value_ptr for uniforms

//ASSIMP
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//LEARNOPENGL
#include <learnopengl/shader_m.h>
#include <learnopengl/model.h>

//GENERAL
#include "main.h"

using namespace std;
using namespace glm;

// -----------------------------------------------------------------------------
// WINDOW STATE
// -----------------------------------------------------------------------------
int windowWidth;
int windowHeight;

// -----------------------------------------------------------------------------
// VAO / BUFFER ENUMS
// (Not actively used for models, but kept for consistency with earlier labs)
// -----------------------------------------------------------------------------
enum VAO_IDs { Triangles, Indices, Colours, Textures, NumVAOs = 2 };
GLuint VAOs[NumVAOs];

enum Buffer_IDs { ArrayBuffer, NumBuffers = 4 };
GLuint Buffers[NumBuffers];

// -----------------------------------------------------------------------------
// CAMERA SPACE REFERENCE
//
// cameraPosition : where the camera exists in WORLD space
// cameraFront    : direction the camera is facing (normalised)
// cameraUp       : what "up" means for the camera (usually +Y)
//
// These feed directly into the VIEW matrix via lookAt().
// -----------------------------------------------------------------------------
vec3 cameraPosition = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

// -----------------------------------------------------------------------------
// CAMERA ROTATION (Euler angles)
//
// Yaw   : rotation around the WORLD Y axis (left/right)
// Pitch : rotation around camera-local X axis (up/down)
//
// Pitch is clamped to prevent gimbal lock.
// -----------------------------------------------------------------------------
float cameraYaw = -90.0f;
float cameraPitch = 0.0f;

// Mouse tracking
bool  mouseFirstEntry = true;
float cameraLastXPos = 800.0f / 2.0f;
float cameraLastYPos = 600.0f / 2.0f;

// -----------------------------------------------------------------------------
// TRANSFORM MATRICES (Rendering Pipeline)
//
// model      : object ? world space
// view       : world ? camera space
// projection : camera ? clip space
//
// mvp = projection * view * model
// -----------------------------------------------------------------------------
mat4 mvp;
mat4 model;
mat4 view;
mat4 projection;

// -----------------------------------------------------------------------------
// TIME (used for movement and frame-independent motion)
// -----------------------------------------------------------------------------
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// -----------------------------------------------------------------------------
// WORLD & ASSET SCALE SYSTEM
//
// WORLD_SCALE defines what "1 unit" means in the scene.
// All asset scales should be relative to this.
//
// If WORLD_SCALE changes later, everything scales consistently.
// -----------------------------------------------------------------------------
constexpr float WORLD_SCALE = 1.0f;

// Asset-specific scale values (relative to WORLD_SCALE)
constexpr float CAVE_SCALE = WORLD_SCALE * 0.5f;
constexpr float RUIN_SCALE = WORLD_SCALE * 1.0f;
constexpr float STATUE_SCALE = WORLD_SCALE * 0.2f;

// Converts Blender world coordinates to OpenGL world coordinates
// Blender: X = left/right, Y = forward, Z = up
// OpenGL:  X = left/right, Y = up,      Z = -forward
vec3 BlenderToOpenGL(float bx, float by, float bz)
{
    return vec3(
        bx,     // X stays X
        bz,     // Blender Z (up) -> OpenGL Y (up)
        -by      // Blender Y (forward) -> OpenGL -Z (forward)
    );
}

// 
struct InstanceTransform
{
    vec3 position;     // OpenGL world position
    float rotationY;   // Y-axis rotation in degrees (Blender Z)
    vec3 scale;        // Usually uniform
};

// -----------------------------------------------------------------------------
// Asset locations
// -----------------------------------------------------------------------------

std::vector<InstanceTransform> caveWall1_APositions = {
    {
        BlenderToOpenGL(16.82f, 96.48f, 0.00f),
        0.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(42.46f, 86.03f, 0.00f),
        0.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(27.51f, 120.49f, 0.00f),
        0.00f,
        vec3(CAVE_SCALE)
    }
};

std::vector<InstanceTransform> caveWall1_BPositions = {
    {
        BlenderToOpenGL(-2.81f, 138.38f, 0.00f),
        199.00f,
        vec3(CAVE_SCALE)
    }
};

std::vector<InstanceTransform> caveWall1_CPositions = {
    {
        BlenderToOpenGL(3.95f, 66.21f, 0.00f),
        60.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-57.57f, 149.02f, 0.00f),
        60.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-38.99f, 113.24f, 0.00f),
        158.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(80.91f, 149.79f, 0.00f),
        -13.00f,
        vec3(CAVE_SCALE)
    }
};

std::vector<InstanceTransform> caveWall1_DPositions = {
    {
        BlenderToOpenGL(-11.23f, 125.48f, 0.00f),
        69.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-19.33f, 104.81f, 0.00f),
        159.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(59.19f, 150.46f, 0.00f),
        340.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-56.86f, 129.93f, 0.00f),
        159.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(80.85f, 77.62f, 0.00f),
        249.00f,
        vec3(CAVE_SCALE)
    }
};

std::vector<InstanceTransform> caveWall2_APositions = {
    {
        BlenderToOpenGL(0.00f, 0.00f, 0.00f),
        0.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(49.74f, 140.21f, 0.00f),
        163.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(18.31f, 65.78f, 0.00f),
        152.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(73.03f, 86.96f, 0.00f),
        248.75f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-2.98f, 119.03f, 0.00f),
        248.00f,
        vec3(CAVE_SCALE)
    }
};

std::vector<InstanceTransform> caveWall2_BPositions = {
    {
        BlenderToOpenGL(21.24f, -13.29f, 0.00f),
        294.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-22.12f, -13.29f, 0.00f),
        242.00f,
        vec3(CAVE_SCALE)
    }
};

std::vector<InstanceTransform> caveWall2_CPositions = {
    {
        BlenderToOpenGL(50.12f, 66.44f, 0.00f),
        250.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(71.21f, 119.85f, 0.00f),
        340.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(-0.34f, 89.78f, 0.00f),
        160.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(19.94f, 137.42f, 0.00f),
        70.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(99.52f, 129.61f, 0.00f),
        340.00f,
        vec3(CAVE_SCALE)
    }
};

std::vector<InstanceTransform> caveWall3Positions = {
    {
        BlenderToOpenGL(-12.00f, 32.00f, 0.00f),
        75.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(19.00f, 32.00f, 0.00f),
        75.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(24.44f, 169.37f, 0.00f),
        354.00f,
        vec3(CAVE_SCALE)
    },
    {
        BlenderToOpenGL(100.01f, 96.51f, 0.00f),
        67.00f,
        vec3(CAVE_SCALE)
    }
};


std::vector<InstanceTransform> caveWall4_APositions = {
    {
        BlenderToOpenGL(-35.79f, 162.31f, 0.00f),
        13.00f,
        vec3(CAVE_SCALE)
    }
};

std::vector<InstanceTransform> caveWall4_DPositions = {
    {
        BlenderToOpenGL(53.34f, 107.70f, -5.03f),
        0.00f,
        vec3(CAVE_SCALE)
    }
};

// Platforms


// Temple
std::vector<InstanceTransform> templePositions = {
    {
        BlenderToOpenGL(-36.20f, 122.53f, -7.33f),
        116.00f,
        vec3(RUIN_SCALE)
    },
    {
        BlenderToOpenGL(-52.09f, 135.41f, -7.33f),
        26.00f,
        vec3(RUIN_SCALE)
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

    // -------------------------------------------------------------------------
    // SHADERS & MODELS
    //
    // LearnOpenGL handles VAOs/VBOs internally for models.
    // -----------------------------------------------------------------------------
    Shader Shaders("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
    // Cave walls 
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

	// Cave platforms
    Model CavePlatform2_1("media/cave/CavePlatform2/CavePlatform2_1.obj");
	Model CavePlatform2_2("media/cave/CavePlatform2/CavePlatform2_2.obj"); 
	Model CavePlatform2_3("media/cave/CavePlatform2/CavePlatform2_3.obj");
	Model CavePlatform2_4("media/cave/CavePlatform2/CavePlatform2_4.obj"); 

    // Ruins
	Model TempleOfApollo("media/ruins/temple of apollo.obj");

    Shaders.use();

    // -------------------------------------------------------------------------
    // VIEWPORT & CALLBACKS
    // -----------------------------------------------------------------------------
    glViewport(0, 0, 1280, 720);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // -------------------------------------------------------------------------
    // MODEL TRANSFORM REFERENCE
    //
    // Standard per-object transform order:
    //
    // model = mat4(1.0f);                    // Reset to WORLD space
    // model = translate(model, position);    // Place object in world
    // model = rotate(model, angle, axis);    // Optional rotation
    // model = scale(model, vec3(ASSET_SCALE)); // Convert asset units ? world units
    //
    // IMPORTANT:
    // - Always reset model per object
    // - Never "undo" transforms — reset instead
    // -----------------------------------------------------------------------------

    // -------------------------------------------------------------------------
    // PROJECTION MATRIX
    //
    // FOV        : 45 degrees
    // Aspect     : window width / height
    // Near plane : 0.1
    // Far plane  : 100.0
    // -----------------------------------------------------------------------------
    projection = perspective(
        radians(45.0f),
        (float)windowWidth / (float)windowHeight,
        0.1f,
        100.0f
    );

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

        // Clear buffers
        glClearColor(0.25f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Cull back-facing triangles for performance
        glEnable(GL_CULL_FACE);

        // ---------------------------------------------------------------------
        // VIEW MATRIX
        // ---------------------------------------------------------------------
        view = lookAt(
            cameraPosition,
            cameraPosition + cameraFront,
            cameraUp
        );

        // ---------------------------------------------------------------------
        // SCENE ANCHOR
        //
        // The tree acts as a spatial anchor.
        // Rocks are positioned using offsets from this point.
        // ---------------------------------------------------------------------
        vec3 treeBasePosition = vec3(3.0f, -2.0f, -1.5f);

        // ---------------------------------------------------------------------
        // CAVE WALLS 
        // ---------------------------------------------------------------------

        for (const auto& instance : caveWall1_APositions) {
            model = mat4(1.0f);
			model = translate(model, instance.position);
			model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
			model = scale(model, instance.scale);
			SetMatrices(Shaders);
			CaveWall1_A.Draw(Shaders);
        }

        for (const auto& instance : caveWall1_BPositions) {
            model = mat4(1.0f);
            model = translate(model, instance.position);
            model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
            model = scale(model, instance.scale);
            SetMatrices(Shaders);
            CaveWall1_B.Draw(Shaders);
        }
        
        for (const auto& instance : caveWall1_CPositions) {
            model = mat4(1.0f);
            model = translate(model, instance.position);
            model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
            model = scale(model, instance.scale);
            SetMatrices(Shaders);
            CaveWall1_C.Draw(Shaders);
        }

        for (const auto& instance : caveWall1_DPositions) {
            model = mat4(1.0f);
            model = translate(model, instance.position);
            model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
            model = scale(model, instance.scale);
            SetMatrices(Shaders);
            CaveWall1_D.Draw(Shaders);
        }

        for (const auto& instance : caveWall2_APositions) {
            model = mat4(1.0f);
            model = translate(model, instance.position);
            model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
            model = scale(model, instance.scale);
            SetMatrices(Shaders);
            CaveWall2_A.Draw(Shaders);
        }

        for (const auto& instance : caveWall2_BPositions) {
            model = mat4(1.0f);
            model = translate(model, instance.position);
            model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
            model = scale(model, instance.scale);
            SetMatrices(Shaders);
            CaveWall2_B.Draw(Shaders);
        }

        for (const auto& instance : caveWall2_CPositions) {
            model = mat4(1.0f);
            model = translate(model, instance.position);
            model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
            model = scale(model, instance.scale);
            SetMatrices(Shaders);
            CaveWall2_C.Draw(Shaders);
        }

        for (const auto& instance : caveWall3Positions) {
            model = mat4(1.0f);
            model = translate(model, instance.position);
            model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
            model = scale(model, instance.scale);
            SetMatrices(Shaders);
            CaveWall3.Draw(Shaders);
        }

        for (const auto& instance : caveWall4_APositions) {
            model = mat4(1.0f);
            model = translate(model, instance.position);
            model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
            model = scale(model, instance.scale);
            SetMatrices(Shaders);
            CaveWall4_A.Draw(Shaders);
        }

        for (const auto& instance : caveWall4_DPositions) {
            model = mat4(1.0f);
            model = translate(model, instance.position);
            model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
            model = scale(model, instance.scale);
            SetMatrices(Shaders);
            CaveWall4_D.Draw(Shaders);
        }

        // Platforms

        // Temple
        for (const auto& instance : templePositions) {
            model = mat4(1.0f);
            model = translate(model, instance.position);
            model = rotate(model, radians(instance.rotationY), vec3(0, 1, 0));
            model = scale(model, instance.scale);
            SetMatrices(Shaders);
            TempleOfApollo.Draw(Shaders);
        }









        // Swap buffers & poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// -----------------------------------------------------------------------------
// CALLBACKS
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

void ProcessUserInput(GLFWwindow* WindowIn)
{
    if (glfwGetKey(WindowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(WindowIn, true);

    const float movementSpeed = 10.0f * deltaTime;

    if (glfwGetKey(WindowIn, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += movementSpeed * cameraFront;

    if (glfwGetKey(WindowIn, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= movementSpeed * cameraFront;

    if (glfwGetKey(WindowIn, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= normalize(cross(cameraFront, cameraUp)) * movementSpeed;

    if (glfwGetKey(WindowIn, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += normalize(cross(cameraFront, cameraUp)) * movementSpeed;
}

// -----------------------------------------------------------------------------
// MVP UPLOAD
//
// Must be called AFTER any change to model or view.
// -----------------------------------------------------------------------------
void SetMatrices(Shader& ShaderProgramIn)
{
    mvp = projection * view * model;
    ShaderProgramIn.setMat4("mvpIn", mvp);
}

