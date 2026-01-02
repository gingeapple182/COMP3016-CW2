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
constexpr float TREE_SCALE = WORLD_SCALE * 0.12f;

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
        // CAVE WALLS – TEST PLACEMENT
        // -----------------------------------------------------------------------------

        // Wall B
        model = mat4(1.0f);
        model = translate(model, BlenderToOpenGL(-13.55f, 38.5f, 0.0f));
        model = scale(model, vec3(CAVE_SCALE));
        SetMatrices(Shaders);
        CaveWall1_B.Draw(Shaders);

        // Wall C
        model = mat4(1.0f);
        model = translate(model, BlenderToOpenGL(-29.30f, 27.96f, 0.0f));
        model = rotate(model, radians(62.62f), vec3(0, 1, 0));
        model = scale(model, vec3(CAVE_SCALE));
        SetMatrices(Shaders);
        CaveWall1_C.Draw(Shaders);

        // Wall D
        model = mat4(1.0f);
        model = translate(model, BlenderToOpenGL(6.36f, 28.76f, 0.0f));
        model = scale(model, vec3(CAVE_SCALE));
        SetMatrices(Shaders);
        CaveWall1_D.Draw(Shaders);


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

    const float movementSpeed = 1.0f * deltaTime;

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

