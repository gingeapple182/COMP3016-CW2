# COMP3016 Coursework 2 – OpenGL Prototype  
**Student:** Oliver Cole  
**Module:** COMP3016 – Graphics Programming  
**Working Title:** Beneath Ancient Sands  

---

## Submission Links (Required)

- **Public GitHub Repository:** [GITHUB LINK](https://github.com/gingeapple182/COMP3016-CW2)  
- **Unlisted YouTube Walkthrough Video:**https://youtu.be/4_E4yXxabmk 

- **Features Checklist Document:** *Included in submission as:* `features_checklist.pdf` *(or .md/.docx)*  
- **AI Declaration Document:** *Included in submission as:* `ai_declaration.pdf` *(or .md/.docx)*  
- **Link Note File:** *Included in submission as:* `links.txt` *(GitHub + YouTube repeated)*

---

## Project Overview

### Concept Summary

This project is a first-person exploration prototype developed in C++ using OpenGL 4.x. The game is set within an ancient desert landscape, where the player explores a procedurally generated environment consisting of sand dunes, partially buried ruins, and a hidden cursted cave. The primary focus of the prototype is to show real-time rendering, player movement, environmental interaction, and atmosphere through audio and spatial design.  
The player navigates the environment using standard keyboard and mouse controls, moving from the open desert into a cave embedded within the terrain. As the player progresses, environmental cues such as terrain shape, asset placement, and audio changes guide exploration. The central objective is to locate an ancient Sith artefact hidden within the cave. Adittionaly, collectable coins are placed throughout the world to encourage exploration beyond the main path, these are a secondary objective and are not essential to *complete* the prototype. The prototype relies on environmental design and positional audio to direct the player.

### Proposal Alignment

The final prototype closely aligns with the original project proposal. The core concept of an ancient desert exploration experience built using OpenGL has been fully realised, including procedurally generated sand terrain, static ruins partially submerged by the environment, and a cave area that houses the main objective. External 3D models are loaded at runtime, and the player is able to freely explore the environment using real-time keyboard and mouse input as originally intended.  
Several elements described in the proposal were implemented with slight adjustments to scope. While advanced features such as dynamic lighting were initially considered, development instead prioritised robust terrain generation, collision handling, and audio-driven atmosphere to ensure a stable and polished prototype within the available timeframe. The use of audio to support immersion, particularly through dynamic background music and 3D positional sound near the artefact, directly reflects the proposal’s emphasis on atmosphere and player guidance. Overall, the project meets the original aims while making pragmatic design decisions to balance technical complexity and reliability.  

---

## Gameplay Description

### Core Player Experience

The prototype presents a first-person exploration experience set within an ancient desert environment. The player begins outside in the desert environment, navigating procedurally generated sand dunes towards partially buried ruins using standard keyboard and mouse controls.  
As the player explores the desert, they will enter a cave partially buried in desert sand. Upon entering this area, the atmosphere shifts through changing background audio, whilst also hearing muffled whispers coming from deeper in the cave, reinforcing a sense of discovery and mystery. Within the cave, there are coins scattered, encouraging exploration beyond the critical path. These collectables provide audio feedback when collected but are not required to complete the game.  
The primary objective is to locate the ancient *Sith* artefact hidden within the cave. The artefact is floating middair and emits positional audio, subtly guiding the player toward the goal without explicit UI indicators.

### Win Condition

The game is completed when the player reaches close proximity to the *Sith* artefact. A trigger-based check detects when the player enters the artefact’s interaction radius, at which point a win message is printed to the console confirming successful completion. Following this feedback, the application terminates cleanly, signalling the end of the prototype experience.  

---

## Controls

*List controls clearly and briefly.*

- **W / A / S / D** – Move  
- **Mouse** – Look   
- **ESC** – Quit  

---

## Feature Checklist

This checklist maps implemented features directly to visible behaviour in the prototype and relevant areas of the codebase, making assessment straightforward.
- **Procedural terrain generation**  
  Runtime generation of a grid-based terrain mesh with a bowl-shaped height function and Perlin noise variation.  
  *(terrain generation functions + FastNoiseLite integration)*
- **First-person camera system**  
  Mouse-controlled yaw and pitch with a `lookAt` view matrix.  
  *(camera vectors, mouse callback, view matrix construction)*
- **Velocity-based player movement**  
  Horizontal movement calculated via velocity each frame, integrated using delta time.  
  *(input handling + velocity integration)*
- **Gravity simulation**  
  Constant downward acceleration applied to vertical velocity.  
  *(playerVelocity.y gravity integration)*
- **Terrain collision (floor)**  
  Terrain height sampling prevents the player falling below the surface.  
  *(GetTerrainHeightFromInstance + vertical clamping)*
- **Minimum cave floor constraint**  
  Global minimum height prevents infinite falling inside the cave interior.  
  *(PLAYER_MIN_HEIGHT safety constraint)*
- **CPU-side wall collision**  
  Simplified collision meshes tested on the CPU using circle-vs-triangle checks in the XZ plane.  
  *(LoadCollisionMesh + CheckWallCollision)*
- **Axis-separated collision resolution**  
  Independent X and Z axis testing allows sliding along walls.  
  *(axis-separated movement tests)*
- **External model loading**  
  OBJ and FBX assets loaded at runtime using ASSIMP.  
  *(LearnOpenGL Model abstraction)*
- **Instance-based rendering**  
  Repeated assets rendered using shared models and per-instance transform data.  
  *(DrawInstances + InstanceTransform)*
- **Collectable system**  
  Optional collectable coins with proximity triggers, state tracking, audio feedback, and visual removal.  
  *(CheckCollectableTrigger + collected flag)*
- **Win condition trigger**  
  Proximity-based detection of artefact interaction, printing win message and closing the application.  
  *(CheckWinCondition)*
- **Audio system (2D + 3D)**  
  2D audio for UI-style feedback and music, 3D positional audio for artefact guidance.  
  *(irrKlang integration)*
- **Dynamic music zones**  
  Background music switches when entering or exiting the cave region.  
  *(distance-based cave trigger)*
- **Debug toggles**  
  Gravity and collision systems can be enabled or disabled for testing.  
  *(ENABLE_GRAVITY / ENABLE_COLLISIONS)*


---

## Technical Architecture Overview

The prototype is structured as a single C++ application built directly on top of OpenGL and supporting libraries. Rather than relying on an engine framework, all core systems are implemented explicitly to maintain clarity and control.  

### Core Technologies and Libraries

- **OpenGL 4.x**  
  Used for all real-time rendering, including terrain meshes, imported models, and shader execution.
- **GLFW**  
  Handles window creation, OpenGL context setup, keyboard input, mouse input, and the main application loop.
- **GLAD**  
  Loads OpenGL function pointers required for modern OpenGL usage.
- **GLM**  
  Provides vector and matrix mathematics for camera transforms, object transforms, and spatial calculations.
- **ASSIMP**  
  Imports external 3D models (OBJ and FBX) at runtime, including mesh and material data.
- **LearnOpenGL Helpers**  
  Used for shader compilation and model abstraction to simplify OpenGL boilerplate while keeping control explicit.
- **irrKlang**  
  Provides audio playback for both 2D sounds and 3D positional audio.
- **FastNoiseLite**  
  Generates Perlin noise used to add natural variation to the procedural terrain.  


### High-Level System Structure

The application follows a traditional real-time loop:
1. Input handling  
2. Physics and movement updates  
3. Collision checks and constraint enforcement  
4. Audio trigger updates  
5. Rendering  

Each system operates independently but shares common spatial data (player position, instance transforms), ensuring predictable behaviour and minimal hidden coupling.  


## Build and Runtime Requirements

### Build Requirements

- **Platform:** Windows  
- **Compiler:** Visual Studio 2022  
- **Graphics API:** OpenGL 4.x compatible GPU and drivers  
The project is built as a standard C++ OpenGL application. All third-party libraries are linked at build time, and the executable does not require Visual Studio to be installed to run.  

### Runtime Requirements

To run the compiled executable, the following must be present:
- A Windows system with OpenGL 4.x support  
- Required runtime DLLs (assimp-vc143-mtd.dll, irrKlang.dll) located alongside the executable
- Required dependencies in the Public folder:
  - `users/Public/OpenGL/include/GLAD` - glad.h
  - `users/Public/OpenGL/include/GLFW` - glfw3.h, glfw3native.h
  - `users/Public/OpenGL/include/GLM` - ext/vector_float3.hpp, gtc/type_ptr.hpp
  - `users/Public/OpenGL/learnOpenGL` - mesh.h, model.h, shader.h, shader_m.h
  - `users/Public/OpenGL/lib` - glfw3.dll, glfw3.lib, glfw3_mt.lib, glfw3dll.lib
- Correct project folder structure, including:
  - `media/` – models, textures, and audio files  
  - `shaders/` – vertex and fragment shader files  

The executable loads assets at runtime using relative paths, so maintaining the expected folder structure is essential. This solutions **was** tested on University lab computers and does build and run successfully.

### Execution Notes

- The application can be launched directly via the `.exe`.  
- No external tools or IDEs are required at runtime.  
- Audio is optional; if audio initialisation fails, the prototype continues to run without sound.

This setup ensures the prototype is portable, self-contained, and suitable for assessment and demonstration purposes.


---

## Rendering Pipeline

### Rendering Flow

Rendering is handled within a single render loop within main(), which separates update logic (input, audio, physics, collision) from visual rendering. Each frame is is organised to properly handle depth, camera behaviour and shader use.  
At the start of the frame, the colour and depth buffers are reset before drawing any new geometry. Depth testing is enabled for correct occlusion based on distance.  
```cpp
glClearColor(0.25f, 0.0f, 1.0f, 1.0f);
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glEnable(GL_CULL_FACE);
```
The camera is then constructed using the players current position and orientation. The camera is configured with yaw and pitch controlled by the mouse.
```cpp
view = lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
```
Terrain is then rendered to create a specific bowl shape, with procedurally generated noise and using vertex colour data rather than textures to ensure consistent and high quality visuals.
```cpp
terrainShaders.use();
model = mat4(1.0f);
model = translate(model, vec3(-terrainBowl.center.x, 0.0f, -terrainBowl.center.y));
SetMatrices(terrainShaders);
DrawTerrain(terrainBowl);
```
Following terrain, we then render the primary shader for imported 3D models. This includes cave walls, pillars and other decorative props, with both $$.obj$$ and $$.fbx$$ model types. These models are rendered using a custom draw function that applies translation, rotation, and scale per instance.
```cpp
Shaders.use();
DrawInstances(Shaders, CaveWall1_B, caveWall1_BPositions, LEVEL_OFFSET);
DrawInstances(Shaders, CavePillar, cavePillarPositions, LEVEL_OFFSET);

//Further expansion of DrawInstances()
void DrawInstances(Shader& shader, Model& modelAsset, const std::vector<InstanceTransform>& instances, const glm::vec3& levelOffset) {
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
```
Finally, we render the dynamic, interactable objects: Sith holocron and coins. These models are rendered seperately as they have a simple rotation animation applied to them. The coins also have multiple instances, and will be disabled from their positional vector once collected so they have a separate, unique render function.
```cpp
if (!c.collected)
{
    float spinAngle = currentFrame * 100.0f;
    model = translate(mat4(1.0f), LEVEL_OFFSET + c.transform.position);
    model = rotate(model, radians(spinAngle), vec3(0, 1, 0));
    SetMatrices(Shaders);
    Coin.Draw(Shaders);
}
```
Rendering in this order allows for predictable behavior, layering each type of rendered instance ontop in a suitable manner building from the largest to the smallest objects.  

### Shader Responsibilities

My prototype uses two shader programs, each made specifically to handle rendering for models and the terrain.  
The texture model shader ($$vertexShader.ver$$ and $$fragmentShader.frag$$) is used for all of the imported 3D models. The vertext shader applies the Model-View-Projection transformations to vertex poitions and pushes texture coordinates to the fragment shader.
```cpp
gl_Position = mvpIn * vec4(position, 1.0);
textureFrag = textureVertex;
```
The fragment shader samples the diffuse texture and outputs the colour. Lighting is not calculated.
```cpp
FragColor = texture(texture_diffuse1, textureFrag);
```
  
For the terrain we use $$terrain.vert$$ and $$terrain.frag$$. Instead of rendering textures, we use per-vertex colour data generated during the construction of the mesh. The vertex shader then passes this colour information to the fragment shader after applying the transformations.
```cpp
gl_Position = mvpIn * vec4(position, 1.0);
colourFrag = colourVertex;
```
The fragment shader then outputs the colour data as the fragment value, allowing for height variations to be communicated visually on the rendered terrain noise.
```cpp
FragColor = vec4(colourFrag, 1.0);
```
Separating the model and terrain allows focussed roles. Imported assets have standard texture rendering while the terrain gets data-driven colouring. This also allows me to be able to expand this in the future to add normal or specular maps to the models, or more detailed colour variations in the terrain.  

---

## Procedural Terrain System

### Terrain Mesh Generation

Terrain is generated procedurally at runtime instead of being loaded from a predefined heightmap or mesh. This is done by creating a grid-based mesh with each vertex representing a point on the surface of the terrain. The resolution of the mesh is controlled by `#define TERRAIN_RENDER_DIST 256`, `#define TERRAIN_MAP_SIZE (TERRAIN_RENDER_DIST * TERRAIN_RENDER_DIST)` and `constexpr float TERRAIN_SPACING = 1.0f;` allowing the terrain to be easily adjusted.
Each vertex has six floating-point values for each vertex; three for position and three for colour. This allows visual variation to be embadded directly into the terrain without having to load textures.
```cpp
std::vector<GLfloat> vertices(mapSize * 6);
```
Vertices are created in a loop that increments across the X and Z axes producing a grid. Indicies are then used to form two triangles per grid space, enabling the terrain to be rendered efficiently.
```cpp
indices[idx++] = topLeft;
indices[idx++] = bottomLeft;
indices[idx++] = topRight;

indices[idx++] = topRight;
indices[idx++] = bottomLeft;
indices[idx++] = bottomRight;
```
The generated terrain data is then uploaded to the GPU and rendered using `glDrawElements()`, ensuring the terrain is handled efficiently despite being large.  

### Height Function (Bowl Shape)

Instead of relying fully on the noise-based terrain, i chose to calculate the height of each vertex using a bowl-shape. This created a large depression in the terrain, guiding the player down towards the central cave area without any explicit markers or reliance on UI.
The height calculation is based on the distance from the center of the terrain. This distance is normalised and passed through an interpolation function to avoid sharp slopes.
```cpp
float distance = glm::length(glm::vec2(xOffset, zOffset) - terrain.center);
float t = glm::clamp(distance / terrain.bowlRadius, 0.0f, 1.0f);
float smoothT = t * t * (3.0f - 2.0f * t);
```
The values are then used to interpolate their height, this can be used to create a concave bowl or daised dome, i chose to make it act as a bowl to seem as if the cave has been *unearthed*.
```cpp
float height = inverted
    ? glm::mix(terrain.bowlHeight, terrain.bowlDepth, smoothT)
    : glm::mix(terrain.bowlDepth, terrain.bowlHeight, smoothT);
```
Doing this allows me to control natural looking slopes towards the central cave area, naturally guiding the player towards the curated playable area.  

### Noise Layer (Sand Variation)

In order to stop the generated terrain from looking too flat and uninteresting, i applied PErlin noise layered ontop of the base mesh. This allows for small-scale height variation that resemble the natural look of sand dunes.  
Noise is generated using `FastNoiseLite` with a fixed seed, ensuring the terrain remains consistent across sessions.
```cpp
FastNoiseLite noise;
noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
noise.SetFrequency(0.25f);
noise.SetSeed(1337);
```
The noise is scaled and applied to each vertex, but is intentionally dulled near the center of the terrain to ensure the cave entrance remains smooth to transition into the curated interior.
```cpp
float noiseValue = noise.GetNoise(xOffset * 0.2f, zOffset * 0.2f);
float noiseFalloff = smoothT;
height += noiseValue * NOISE_STRENGTH * noiseFalloff;
```
The noise values are also used to influence the vertex colour, providing subtle but important visual variation across the surface, helping to visualise the peaks and troughs of the sand without requiring texture maps.
<img width="1283" height="752" alt="image" src="https://github.com/user-attachments/assets/378d9167-c0ab-4832-b75b-c588d993d67b" />  

### Design Rationale

Procedual generation was chosen to make the environment have controlled randomness, much like how a desert isnt uniform and flat. Generating the terrain at runtime demonstrates core procedural generation teqhniques whilst remaining lightweight and modifiable.  
Using a noise seed ensures the layout is stable between sessions, enabling consistent testing, collision behaviour and visuals. The bowl-shaped height function demonstrates the flexibility of the PCG while alos helping to guide players, reducing the likelihood of players missing the objective.  
Embedding colour data directly into the terrain vertices allows the terrain to be visually readable despite not introducting texture or lighting complexity. This aligns with the projects focus on procedural generation and stability rather than losing focus on visual fidelity.  

---

## Asset rendering and Placement Workflow

### Imported Model Rendering

All non-terrain geometry in the prototype is rendered using external 3D models loaded at runtime through **ASSIMP**. Models are loaded during initialisation using the `Model` abstraction provided by **LearnOpenGL**, which handles mesh data, materials and texture binding.
```cpp
Model CaveWall1_B("media/cave/CaveWalls1/CaveWalls1_B.obj");
Model CavePillar("media/Ruins/Pillar_egyptian/obj/objPillar.obj");
Model Holocron("media/Statues/Holocron/Sith Holocron.FBX");
```
Each model is rendered using a shared shader and draw function that applies per-instance transformations. This allows the same model to be reused numerous times in the scene with individual positions, rotations and scales, reducing memory usage and increasing efficiency.
```cpp
void DrawInstances(Shader& shader, Model& modelAsset, const std::vector<InstanceTransform>& instances, const glm::vec3& levelOffset) {
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
```

### Instance-Based World Layout

Each model is placed using structured instance data, each object is defined using `InstanceTransform`, which stores position, rotation and scale values.
```cpp
struct InstanceTransform
{
    vec3 position;
    float rotationY;
    vec3 scale;
};
```
Collections of these transforms are stored in vectors per asset type, allowing large groups of models (for example cave walls or stone pillars) to be rendered efficiently using a single model reference.  
`std::vector<InstanceTransform> cavePillarPositions;`  
This allows for rapid iteration and ensures visual placement and collision meshes all reference the same spatial data.  

### Blender as a World Layout Tool

Blander was used an an external world-layout tool to organise, scale and position assets visually before transferring their data to the C++ codebase. I created a dedicated Blender scene containing all environment assets arranged into the desired layout. Thsi allowed me to easily and quickly rather than relying on manual nuerical positioning.  
Using Blender to achieve this had some advantages:
- Accurate spatial relationships between models
- Consistent and controllable position, rotation and scaling
- Easy composition of complex structures
To streamline the workflow further, a custom python script was used to extract the positional, rotational and scale data in a format suitable to be pasted into my project. The output of the script held the name of the object from blender, the Positional function with correct data, rotation on the Z axis and manually chosen scale.
```python
import bpy, math

print("cave walls")
for obj in bpy.context.selected_objects:
    l = obj.location
    r = obj.rotation_euler
    print(f"// {obj.name}")
    print("{")
    print(f"    BlenderToOpenGL({l.x:.2f}f, {l.y:.2f}f, {l.z:.2f}f),")
    print(f"    {math.degrees(r.z):.2f}f,")
    print("    vec3(CAVE_SCALE)")
    print("},\n")
```
The exported data could then be pasted directly into the relevant instance vectors within `main.cpp`, allowing for quick and efficient data transfer.  


### Coordinate System Alignment

As Blender and OpenGL use different world coordinate systems, a conversion function was implemented to translate Blender positions into OpenGl suitable values, ensuring consistent placement when importing layout data.
```cpp
vec3 BlenderToOpenGL(float bx, float by, float bz)
{
    return vec3(bx, bz, -by);
}
```


### Benefits and Design Rationale

Using Blender as a layout and positional data authoring tool significantly improved development efficiency and consistency. Complex environments could be curated and adjusted visually and re-exported without changing models in the solution files. This helped ensure collision meshes and visual models remained spatially alligned.
Separating layout authoring mirrors industry workflows, where level design and functional systems are developed in parallel. For the scope of this prototype, this provided a practical balance of control and flexibility.  
<img width="1891" height="880" alt="image" src="https://github.com/user-attachments/assets/8f1d6734-deec-4c18-a81c-0bcd0754393e" />


---

## Player Physics and Movement

### Movement Model (Velocity-Based)

Player movement is implemented using velocity rather than directly changing position values. Each frame, player input determines desired horizontal movement, applied to the players velocity vecotr. The velocity is then integrated using *delta time* to predict the players next position.
Movement is reset and recalculated every frame to ensure responsiveness and avoiding accumulated drift. Vertical movement is handled separately through *gravity*, allowing horizontal traversal to remain consistent regardless of terrain height.
```cpp
// Reset horizontal velocity every frame
playerVelocity.x = 0.0f;
playerVelocity.z = 0.0f;
```
Directional input is calculated relative to the orientation of the camera, ensuring movement matches what the player would expect to happen.
```cpp
vec3 forward = normalize(vec3(cameraFront.x, 0.0f, cameraFront.z));
vec3 right = normalize(cross(forward, cameraUp));
```
The player position is not immediately applied, instead, a predicted next position is calculated using the velocity and delta time. This allows collision or floor contraints to be enacted before movement is commited. This allows smooth movement while ensuring collisions and constraints can be handled effectively.  
```cpp
vec3 nextPosition = playerPosition + playerVelocity * deltaTime;
```

### Gravity

Gravity is applied as a constant acceleration to the players vertical velocity, simulating a basic physics model. Each frame gravity modified the Y component of the players velocity.
```cpp
const float GRAVITY = -25.0f * WORLD_SCALE;
playerVelocity.y += GRAVITY * deltaTime;
```
The gravity value is relative to the world scale and is tuned to feel appropriate for the set movement speed and terrain. This prevents movement from feeling too sluggish or floaty.  
For development and testing purposes, gravity can be toggled on or off using the `ENABLE_GRAVITY` bool. This allows free movement during debugging for me to inspect the curated level.  

### Terrain Collision (Floor)

To prevent th eplayer from falling through the sand terrain, terrain height is sampled at the players predicted next position each frame. This makes sure collision is calculated before the player has moved too far, helping immerse the player in the world rather than letting them see behind the curtains.
```cpp
float terrainHeight = GetTerrainHeightFromInstance(
    terrainBowl, nextPosition.x, nextPosition.z);
```
A minimum allowed vvertical position is calculated using the terrain height and the players height values. If the player is predicted to fall below this value, the player gets clamped to the terrain and their vertical velocity is reset.
```cpp
float minY = terrainHeight + PLAYER_HEIGHT;

if (nextPosition.y < minY)
{
    nextPosition.y = minY;
    playerVelocity.y = 0.0f;
}
```
This ensures the player remains grounded on the procedurally generarted bumpy terrain while still allowing for smooth movement across the dunes.  

### Minimum Height Constraint (Cave Floor Handling)

Inside the cave environment, there is no continuous terrain mesh, the interior flooring is comprised of platform assets with uneven geometry to simulate the rocky interior of a cave. This would add additional issues with gaps and inconsistent movement height, creating an uncomfortable exploration experience, this could also allow the player to fall below some models ruining the immersion.  
The cave is located at the bottom-most point of the bowl, so to address this issue, I decided to introduce a minimum height constant, `PLAYER_MIN_HEIGHT`. This acts as a safety floor within the cave, preventing the player from falling below the floor.
```cpp
float absoluteMinY = PLAYER_MIN_HEIGHT + PLAYER_HEIGHT;

if (nextPosition.y < absoluteMinY)
{
    nextPosition.y = absoluteMinY;
    playerVelocity.y = 0.0f;
}
```
This solution was chosen as a stability measure to ensure consistent traversal while within the cave environment, guaranteeing the player remains within the curated space even where the floor models arent uniformly positioned.  
A potential future solution would be to introduce another floor mesh, for the interior of the cave, which would create more precise walking interaction, however as this is a prototype, a min_height is suitable.

*Explain your cave safety floor decision (this is a design decision section).*
- *Inside the cave there isn’t a continuous terrain “floor” mesh.*
- *Some areas/gaps could allow the player to fall below intended play space.*
- *A global minimum height (`PLAYER_MIN_HEIGHT`) prevents infinite falling.*
- *This is a pragmatic stability choice to keep gameplay consistent.*
- *Future improvement: mesh-based floor collision for cave geometry.*

---

## Collision System

### Collision Mesh Workflow (Blender → CPU)

Collision is handled on the CPU using simplified, low-poly meshes authored separately from the visally rendered models. These collision meshes were create din blender by duplicaiting the models and heavily simplifying them through decimation, then exporting them with a `_COL.obj` suffix. This ensured that the collision mesh matched the visuals.
collision meshes are never rendered and exist just for collision detection. At runtime, they are loaded through ASSIMP into a `CollisionMesh` struct containing only the vertex and index data.
```cpp
struct CollisionMesh
{
    std::vector<glm::vec3> vertices;
    std::vector<unsigned int> indices;
};
```
Each mesh is loaded at startup using a loader function, this function extracts vertex positions and triangle indicies for use in collision checks.
```cpp
CollisionMesh LoadCollisionMesh(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

    aiMesh* mesh = scene->mMeshes[0];

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        result.vertices.emplace_back(mesh->mVertices[i].x,
                                     mesh->mVertices[i].y,
                                     mesh->mVertices[i].z);
}
```
Separating the visual and collision geometry avoids unnecessary GPU overhead.  


### Wall Collision Method (XZ Circle vs Triangle)

The player is represented as a circle in the XY plane, defined by a set `PLAYER_RADIUS`. This is suitable for the prototype as we dont require a full 3D physics simulation.
Collision is tested against the triangles of the low-poly `_COL.obj` models projected into the XY plane. For each triangle, the distance between the players position and the edges are calculated. If the distane is smaller than the `PLAYER_RADIUS` then a collision is detected.
```cpp
float DistancePointToSegmentXZ(
    const glm::vec2& p,
    const glm::vec2& a,
    const glm::vec2& b)

bool CircleIntersectsTriangleXZ(
    const glm::vec2& circleCenter,
    float radius,
    const glm::vec3& v0,
    const glm::vec3& v1,
    const glm::vec3& v2)
```   

### Transforming Collision Instances

Collision meshes are reused across each instance of the same models. To ensure geometry alligns correctly with the rendered models, each collision mesh is transformed into the world space using the same translation, rotation and scale values as its corresponding rendered model.
The transformation is perfomed on the CPU by constructing a model matrix and applying it to the vertices of the `_COL.obj`
```cpp
glm::mat4 BuildModelMatrix(const InstanceTransform& inst, const glm::vec3& levelOffset)

glm::vec4 worldPos = modelMatrix * glm::vec4(v, 1.0f);
```   

### Axis-Separated Resolution

Collision resolution is handled using *axis-seoarated movement*, where the horizontal movement is tested independantly along the X and Y axes. After predicting the players next position, each axis is checked against the active collision meshes.
```cpp
// X axis test
vec3 testX(nextPosition.x, playerPosition.y, playerPosition.z);
// Z axis test
vec3 testZ(nextPosition.x, playerPosition.y, nextPosition.z);
```
If a collision is detected along either axis, movement along the axis is reverted while allowing the other to still be tested and acted upon. This approach allows for *sliding* along the walls stopping them from feeling *sticky*. Axis-separated movement provides stable and predictable collision handling without requiring complex physics processes.  


### Design Rationale

A simple custom CPU-based collision system was chosed over PhysX to maintain full control over behaviour and to keep the prototype focussed on OpenGl rendering and spatial logic. This approach maked collision logic transparent and easier to debug, while avoiding the extra learning curve of a full physics framework.
Using simplified collision meshes and a 2D collision plane allows for a simple but effective system in the prototype. The system is capable of supporting complex collision meshes and repeated asset placement making it suitable for expansion.  

---

## World Interaction and Proximity Triggers

### Interaction Trigger Model

World interaction within the prototype is driven by a proximity-based trigger system. Rather than using individual collison meshes or physics triggers, interactions are detected using distance checks between the players position and the worl-space position on the XZ axes of interactive objects.  
This system is used across multiple gameplay features, including collectable coins, the win condition (*Sith holocron*) and audio triggers. (expanded upon in the **Audio Design** section.)  This is constructed using the following approach:
- A fixed world position
- Defined trigger radius
- Per-frame distance check
- State change upon entering radius
Distance checks are performed on the XZ plane, as vertical precision is not required so 3d positioning would introduce uneccesary complexity.
```cpp
float dist = glm::distance(
    glm::vec2(playerPosition.x, playerPosition.z),
    glm::vec2(targetPosition.x, targetPosition.z)
);
```
Once the distacne falls below the specified trigger radius, the relevant interaction logic is executed. This keeps logic lightweight and predictable.  


### Win Trigger

The primary objective of the prototype is to find the ancient *Sith holocron* hidden within the cave. The artefact uses the proximity-based trigger system, where once the player is within the radius they are considered to have *found* the artefact and will win the game. This logic is triggered in the main update/render loop.
```cpp
if (CheckWinCondition(playerPosition, artefactTransform.transform, TRIGGER_RADIUS))
{
    std::cout << "\n=================================\n";
    std::cout << " YOU FOUND THE ARTEFACT!\n";
    std::cout << "        YOU WIN\n";
    std::cout << "=================================\n";

    glfwSetWindowShouldClose(window, true);
}
```
Upon triggering the win condition, we print to the console to say that the artefact has been found, then it closes the window via `glfwSetWindowShouldClose()` function. This provdes feedback to the player that the gae has been won, then exits cleanly.  
In addition to acting as the win trigger, the holocron also is rendered as visually distinct from its surrounding envornment, the artefact has a rotation animation applied each frame, creating a slow spinning motion. This helps it stand out as the objective without relying on UI indicators.  
Rotation is applied during rendering by constructing a model matrix using the models world position and a `delta time` based rotation value.
```cpp
for (const auto& c : holocronPositions)
{
    float spinAngle = c.transform.rotationY + (currentFrame * -50.0f);
    model = mat4(1.0f);
    model = translate(model, LEVEL_OFFSET);
    model = translate(model, c.transform.position);
    model = rotate(model, radians(spinAngle), vec3(0, 1, 0));
    model = scale(model, c.transform.scale);

    SetMatrices(Shaders);
    Holocron.Draw(Shaders);
}
```
The animation presentation reinforces the importance of the artefact and draws the players attention.

https://github.com/user-attachments/assets/4c27b9ce-07ad-4138-b2b5-70ca49605640


### Collectable Coins

Collectable coins are implemented using the same proximity-based interaction system. These coins are optional and are nto required to win the game. The coins are placed throughout the cave to ecourage exploration and provide a fun secondary objective to the protoytype. The coins also double as my personal signature as i have changed the texture on them to be my face, as evidenc ethat this prototype has been created and built by me.
<img width="1277" height="749" alt="image" src="https://github.com/user-attachments/assets/35abec9a-66c0-4744-945b-2b6231ac130b" />  
Each coin instance aintains its own state, inclusing a `collected` flag, when a player enters the radius of the coins, that coin is marked as collected, a global `COLLECTABLES_FOUND` is incremented and audio feedback is triggered. Coins are then visually removed from the worls by skipping their rendering once collected. 
```cpp
if (!c.collected)
{
    float spinAngle = currentFrame * 100.0f;
    model = translate(mat4(1.0f), LEVEL_OFFSET + c.transform.position);
    model = rotate(model, radians(spinAngle), vec3(0, 1, 0));
    SetMatrices(Shaders);
    Coin.Draw(Shaders);
}
```

---

## Audio Design

Audio is used throughout the prototype to reinforce exploration, guide the player towards objectives and provide feedback on interaction. The audio system is built using **IrrKlang** and is integrated directly to the main update loop so that audio feedback responds immediately to triggers.

### irrKlang Setup

The irrKlang sound engine is created during initialisation and persists until the program terminates where its shut down properly to release its held resources.
```cpp
// Program initialisation
ISoundEngine* soundEngine = createIrrKlangDevice();
// Program termination
soundEngine->drop();
```
If the engine fails to initialise, the prototype continues to run wihtout audio. Preventing a hard failure is audio hardware  or drivers are unavailable, ensuring the prototype remains playable in all environments. Failures are logged to the console for debugging purposes.  


### 2D Sound Effects

Collectable coin pickups use **2D audio** to provide immediate feedback when a coin is collected. Because the sound represents a UI-stype confirmation, positional audio is not required.
```cpp
soundEngine->play2D("audio/coin_pickup.wav", false);
```
Using 2D audio help keep feedback consistent and clear regardless of player orientation or position.  

2D audio is also used for the background ambient sounds. Background sounds change dynamically based on player location to reinforce environmental transitions. This is also implemented using triggers, however for this one, while the player is within the trigger radius, encompassing the entire curated cave level, it will play the cave music, while outside of that radius it plays a lighter desert theme.
```cpp
{
    vec2 playerXZ(playerPosition.x, playerPosition.z);
    vec2 caveXZ(CAVE_ZONE_CENTER.x, CAVE_ZONE_CENTER.z);
    bool nowInside = length(playerXZ - caveXZ) <= CAVE_TRIGGER_RADIUS;
    if (nowInside && !isInsideCave)
    {
        isInsideCave = true;
        if (outsideMusic)
        {
            outsideMusic->stop();
            outsideMusic->drop();
            outsideMusic = nullptr;
        }

        caveMusic = soundEngine->play2D("media/Audio/cave themeb4.ogg", true, false, true);
        caveMusic->setVolume(0.8f);
    }
    if (!nowInside && isInsideCave)
    {
        isInsideCave = false;
        if (caveMusic)
        {
            caveMusic->stop();
            caveMusic->drop();
            caveMusic = nullptr;
        }

        outsideMusic = soundEngine->play2D("media/Audio/caravan.ogg.ogg", true, false, true);
        outsideMusic->setVolume(0.9f);
    }
}
```
When the player exits the cave, the flag changes back so the outside music resumesm and the cave music stops playing, ensuring that only one background track is active at a time, and behaviour remains predictable.

https://github.com/user-attachments/assets/546249dc-bf9d-44ca-afff-fc7bf79ef682

### 3D Positional Audio

The *Sith holocron* emits a continuous 3D positional eerie whispering sound, reinforcing its mystical intrigue and importance, guiding the player towards it wihtout relying on UI indicatiors.
The sound is placed at the location of the holocrons world position and played using irrKlangs 3D audio functuonality.
```cpp
artefactHum = soundEngine->play3D(
    "media/Audio/whispering.wav",
    vec3df(relicWorldPos.x, relicWorldPos.y, relicWorldPos.z),
    true,    // loop
    false,   // start immediately
    true     // track sound
);
```
The 3D positional audio allows the volume of the audio to increase as the pleyer moves closer towards the holocrom, again guiding them to the main objective.  
To achieve this, we track the listeneer position every frame and irrKlang calculartes the distance from the £d sound location centerpoint.
```cpp
        soundEngine->setListenerPosition(vec3df(cameraPosition.x, cameraPosition.y, cameraPosition.z), vec3df(cameraFront.x, cameraFront.y, cameraFront.z));
```  

### Design Rationale

Audio in the prototype is data-driven and reactive to the player input, tying audio feedback tot he players actions helps provde an immersive and atmospheric experience.
Using a combination of 2D and 2D audio gives player guidance and rerinforces immersion.

### 3D Positional Audio (Artefact Hum)

*Explain the artefact hum and how it responds to distance.*
- *Audio source placed at artefact world position.*
- *Min/max distance values define attenuation range.*
- *Listener position updated each frame to camera position.*

### Dynamic Music Zones (Cave Trigger)

*Explain music switching logic.*
- *Distance check to cave zone centre.*
- *On enter: stop outside track, start cave track.*
- *On exit: stop cave track, start outside track.*
- *Uses clean stop/drop to avoid overlap and resource leaks.*

---

## Game Programming Patterns and Structure

This prototype follows a small number of clear and deliberate programming patterns aimed at maintaining control, readability, and stability within an unmanaged C++ OpenGL environment. Rather than relying on engine-level abstractions, patterns are implemented explicitly to keep behaviour transparent and easy to reason about.

### Data-Driven Instancing

Repeated environment assets (such as cave walls, platforms, and pillars) are placed using **data-driven instancing** rather than individual draw calls.  
Each placed object is defined using an `InstanceTransform` struct containing position, rotation, and scale, with collections of these stored in `std::vector` containers per asset type.

This allows:
- reuse of a single model across many placements  
- clean separation between asset data and rendering logic  
- efficient iteration and layout changes without modifying draw code  

### Separation of Responsibilities

Although implemented within a single translation unit, systems are logically separated by responsibility:

- **Rendering** – handled via shared draw helpers and shader programs  
- **Movement & physics** – velocity-based movement, gravity, and constraints  
- **Collision** – CPU-side collision using simplified meshes  
- **Audio** – trigger-based 2D and 3D sound playback  

Within the main loop, update logic (input, physics, collision, audio) is processed before rendering, ensuring predictable and deterministic behaviour each frame.

### Explicit Collision Participation

Collision handling uses an **explicit opt-in approach**.  
Only meshes registered in collision groups are considered during wall collision checks, allowing decorative assets to exist without unnecessary collision overhead.

Collision meshes are authored separately from visual geometry and reused across multiple instances, ensuring spatial consistency while keeping collision logic lightweight.

### Proximity-Based Interaction Pattern

All interactive behaviour in the prototype is driven using **distance-based proximity checks** rather than physics triggers or event systems.  
This pattern is consistently applied for:
- collectable coins  
- the win condition (artefact)  
- cave music transitions  

Using a shared interaction model keeps gameplay logic simple, predictable, and easy to extend.

### Lightweight Data Structures

Gameplay and world logic rely on small, purpose-built structs rather than inheritance-heavy hierarchies. Examples include:
- transform-only structs for world placement  
- state-based structs for collectables  
- grouped data for collision handling  

This approach avoids unnecessary complexity while remaining flexible enough for expansion.

### Debug Toggles for Development

The prototype includes simple runtime toggles (e.g. gravity and collision enables) used during development and testing.  
These allow systems to be isolated for debugging and environment inspection without altering core logic, reflecting standard development practice in iterative engine work.

### Design Rationale

These patterns prioritise:
- clarity over abstraction  
- explicit control in an unmanaged environment  
- stability and debuggability  

Together, they support a clean and understandable architecture while meeting the technical and educational goals of the COMP3016 module.


---

## Error Handling and Stability

The prototype prioritises stability and predictable behaviour by explicitly handling common failure points and ensuring clean shutdown of external systems. Rather than assuming ideal runtime conditions, defensive checks are used throughout initialisation and execution to prevent crashes and undefined behaviour.

### Window and Context Creation

Window creation is validated immediately after attempting to initialise GLFW. If the window fails to instantiate, the program prints an error message and exits cleanly.

```cpp
GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Oliver Cole", NULL, NULL);

if (window == NULL)
{
    cout << "GLFW Window did not instantiate\n";
    glfwTerminate();
    return -1;
}
```
This prevents further OpenGL calls from executing without a valid rendering context.  


### OpenGL Function Loading(GLAD)
OpenGL function pointers are loaded using GLAD after context creation. Failure to load GLAD results in early termination to avoid undefined behavioour when calling OpenGL functions.
```cpp
if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
{
    cout << "GLAD failed to initialise\n";
    return -1;
}
```


### Audio System Initialisation (irrKlang)
The audio engine is initialised independently of rendering and gameplay systems. If audio initialisation fails, the prototype continues to run without audio rather than terminating.
```cpp
soundEngine = createIrrKlangDevice();

if (!soundEngine)
{
    std::cerr << "[AUDIO] Failed to initialise irrKlang\n";
}
else
{
    std::cout << "[AUDIO] irrKlang initialised successfully\n";
}
```
This ensures the prototype remains playable even on systems without compatible audio hardware or drivers.  


### Collision Mesh Loading
Collision meshes are loaded at runtime using ASSIMP. If a collision mesh fails to load or contains no mesh data, the loader logs an error and returns an empty `CollisionMesh`.
```cpp
if (!scene || !scene->HasMeshes())
{
    std::cerr << "[COLLISION] Failed to load: " << path << std::endl;
    return result;
}
```
This prevents crashes due to invalid mesh access while still allowing the application to continue running.  


### Defensive Gameplay Constraints
Several hard constraints are enforced to prevent unstable gameplay states:
- Terrain height clamping prevents falling through procedural terrain
- Minimum cave height prevents infinite falling in interior spaces
- Axis-separated wall collision avoids jitter and tunnelling
Example of cave floor safety constraint:
```cpp
float absoluteMinY = PLAYER_MIN_HEIGHT + PLAYER_HEIGHT;

if (nextPosition.y < absoluteMinY)
{
    nextPosition.y = absoluteMinY;
    playerVelocity.y = 0.0f;
}
```
These constraints prioritise consistent player experience over physical realism.  


### Controlled Shutdown and Resource Cleanuo
Before exiting, external systems are explicitly released to prevent resource leaks. The irrKlang engine is dropped and GLFW is terminated cleanly.
```cpp
if (soundEngine)
{
    soundEngine->drop();
    soundEngine = nullptr;
}

glfwTerminate();
```


### Design and Rationale
Error handling in this prototype is intentionally simple and explicit. The focus is on:
- Avoiding hard crashes
- Preventing undefined behaviour
- Ensuring the prototype remains playable under failure conditions

This approach is appropriate for a real-time OpenGL prototype and aligns with good practice when working in an unmanaged C++ environment.

---

## Testing and Validation

Testing for this prototype was carried out through repeated runtime execution, targeted movement scenarios, and console-based validation. As this is a real-time OpenGL application without a unit testing framework, validation focused on confirming correct behaviour under normal use and edge-case conditions.

### Terrain Collision Validation

Terrain collision was validated by traversing the procedural desert at multiple locations and elevations. The player was repeatedly moved across steep slopes, flat regions, and noisy dune areas to ensure terrain height sampling correctly prevented the player from falling through the ground.

Terrain collision is handled by sampling the terrain height at the player’s predicted next position:

```cpp
float terrainHeight = GetTerrainHeightFromInstance(
    terrainBowl, nextPosition.x, nextPosition.z);
```
If the player is predicted to fall below the terrain surface, their position is clamped and vertical velocity reset:
```cpp
float minY = terrainHeight + PLAYER_HEIGHT;

if (nextPosition.y < minY)
{
    nextPosition.y = minY;
    playerVelocity.y = 0.0f;
}
```
This behaviour was tested by:
- Moving at high speed across uneven terrain
- Approaching the cave entrance from multiple angles
- Repeatedly descending and ascending the bowl interior  


### Cave Interior Stability Testing
The cave interior does not use a continuous terrain mesh. To validate stability in this area, repeated traversal tests were performed across uneven platform geometry and gaps between assets.  
The global minimum height constraint was confirmed to prevent infinite falling.
```cpp
float absoluteMinY = PLAYER_MIN_HEIGHT + PLAYER_HEIGHT;

if (nextPosition.y < absoluteMinY)
{
    nextPosition.y = absoluteMinY;
    playerVelocity.y = 0.0f;
}
```
This ensured the player remained within the intended cave world-space regardless of interior geometry variation.  


### Wall Collision testing
Wall collision was tested against cave walls, archways and pilars.
Collision resolution was validated by moving into surfaces at different angles and speeds. Axis-separated collision resolution allowed the player to slide along walls rather than becoming stuck.
```cpp
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
```
Following this testing, I removed the _COL.obj for the archways, as they became impenetrable and the player needed to be able to walk through them.  


###Collectable interaction Testing
Collectable coins were tested by repeatedly entering and exiting their trigger radius from different directions. Each coin maintains its own collected state, ensuring it can only be collected once.
```cpp
if (CheckCollectableTrigger(playerPosition, c, COLLECTABLE_RADIUS))
{
    c.collected = true;
    COLLECTABLES_FOUND++;
    coinCollectSound = soundEngine->play2D("media/Audio/coins.wav", false);
}
```
Validation included:
- Confirming audio plays once per coin
- Confirming collected coins are no longer rendered
- Verifying COLLECTABLES_FOUND increments correctly

Console output provided immediate confirmation during testing.  


### Win Condition Testing
The win condition was tested by approaching the artefact from multiple angles and distances. The trigger activates once the player enters the defined interaction radius.
```cpp
if (CheckWinCondition(playerPosition, artefactTransform.transform, TRIGGER_RADIUS))
{
    std::cout << "\n=================================\n";
    std::cout << " YOU FOUND THE ARTEFACT!\n";
    std::cout << "        YOU WIN\n";
    std::cout << "=================================\n";

    glfwSetWindowShouldClose(window, true);
}
```
Testing confirmed:
- The trigger fires reliably regardless of approach direction
- The win message prints correctly to the console
- The application exits cleanly after completion


```
[COLLECTABLE] Found 1 / 3
[COLLECTABLE] Found 2 / 3

=================================
 YOU FOUND THE ARTEFACT!
        YOU WIN
=================================
```

---

## Screenshots

PCG Desert terrain
<img width="1282" height="753" alt="image" src="https://github.com/user-attachments/assets/562804d1-d8d4-4d16-af24-df5df5fb7d2a" />
Cave exterior
<img width="1284" height="756" alt="image" src="https://github.com/user-attachments/assets/3a49d0c2-40af-4ed7-b64c-b61c29b1df15" />
Cave interior
<img width="1281" height="754" alt="image" src="https://github.com/user-attachments/assets/73a988ed-8299-4379-8a2c-c909c4f7eea9" />
Coin
<img width="1277" height="749" alt="image" src="https://github.com/user-attachments/assets/35abec9a-66c0-4744-945b-2b6231ac130b" />
Holocron

https://github.com/user-attachments/assets/4c27b9ce-07ad-4138-b2b5-70ca49605640

Blender scene layout
<img width="1891" height="880" alt="image" src="https://github.com/user-attachments/assets/8f1d6734-deec-4c18-a81c-0bcd0754393e" />

---

## Use of AI  

AI tools were used during the development of this prototype as a **supportive aid** rather than as an automated solution. Their role was limited to assisting with understanding approaches, exploring alternative implementations, and helping structure written explanations. All final design decisions, implementations, and submitted code were written, tested, and validated by me.

### How AI Was Used

AI was primarily used throughout the project, acting as a helpful tool, particularly when working with unfamiliar OpenGL concepts. At the start of most discussions, I provided the coursework brief, my original project proposal, and relevant sections of my existing codebase. This allowed discussions to remain grounded in the actual requirements and current implementation rather than abstract examples, while keeping it consistent with the brief and expected outcomes.

AI assistance included:
- Discussing different technical approaches (e.g. collision handling strategies, terrain height sampling methods).
- Explaining OpenGL concepts to help me understand why certain approaches work.
- Helping plan the structure and wording of report sections after systems were already implemented.
- Suggesting possible solutions which I then evaluated and adapted.
- Bug-fixing and testing.

In cases where AI produced code, this was often used as a starting point, as often the AI would generate functional, but not precise code that I would need to edit and adjust to fit the needs of my prototype. Any generated code was carefully scrutinised, tested in isolation, and modified or rewritten to align with my intended behaviour and architectural decisions.

### Validation and Authorial Control

All code used in the final submission was:
- reviewed line-by-line by me,
- tested within the prototype,
- adjusted or rewritten where behaviour was inefficient, incorrect, or unclear.

AI-generated output was **never copied blindly** into the project, I would often ask for extre explanations or reasoning for given code before implementing it into my project. In many cases, AI suggestions were rejected entirely after testing or analysis revealed logical errors, inefficiencies, or incorrect assumptions about my codebase.

### Limitations Encountered

While AI proved helpful at times, it was also frequently difficult to use effectively. Common issues included:
- Misunderstanding specific requirements or constraints I had already defined.
- Generating inefficient or incorrect code for the task at hand.
- Losing context across longer discussions, leading to contradictory or unusable suggestions.
- Often refusing to change methods resulting in frustrating *arguments*.
- Struggling with debugging complex issues; if a problem could not be resolved quickly, AI responses often became increasingly inaccurate or unhelpful.

In several instances, this led to frustration and wasted time, requiring me to step back and resolve issues independently.

### Overall Reflection

Overall, AI was a useful learning and planning tool, particularly when exploring unfamiliar OpenGL concepts and structuring written explanations. However, it proved unreliable as a direct coding solution and could not be depended upon to produce correct or context-aware implementations consistently.  
This experience reinforced the importance of:
- Understanding systems deeply rather than relying on generated output,
- Validating all suggested solutions through testing,
- Maintaining full ownership of design and implementation decisions.

AI supported my learning process but did not replace it, and the final prototype reflects my own understanding, decisions, and implementation work.

Here are some screenshots of messages i sent out or received from AI:  
Initial chat message:
<img width="652" height="781" alt="image" src="https://github.com/user-attachments/assets/28192b7b-ad5e-48c1-bdab-d415c918a66a" />
<img width="664" height="789" alt="image" src="https://github.com/user-attachments/assets/fa0bff21-f853-4384-b1ef-9340006568fa" />
Asking AI to generate repetitive code for me to hasten development:
<img width="654" height="804" alt="image" src="https://github.com/user-attachments/assets/c9dc3c79-4fe3-4d81-8a9e-339b4c5dd5c3" />
<img width="666" height="600" alt="image" src="https://github.com/user-attachments/assets/ce652472-0127-453d-bc9e-b84994a92790" />
Asking for guidance on how to use Blender:
<img width="657" height="781" alt="image" src="https://github.com/user-attachments/assets/da7f19ee-ff64-4eed-9a00-6eef17828515" />
Start of discussion about collectables trigger system:
<img width="483" height="221" alt="image" src="https://github.com/user-attachments/assets/20637a40-171d-4cb6-8ea8-e834bda98cb8" />
Me getting frustrated with AI for it not seeming to listen to my requests, so I ask it for clarification and explanation instead of Code that was inefficient:
<img width="667" height="595" alt="image" src="https://github.com/user-attachments/assets/8403eeea-9025-4005-b15b-50e7c4e63763" />
More frustration in setting up terrain collision:
<img width="682" height="462" alt="image" src="https://github.com/user-attachments/assets/d6a5f498-4fb9-4787-a309-991161cc7e22" />
AI assisting with use of VS2022:
<img width="663" height="785" alt="image" src="https://github.com/user-attachments/assets/0860b48d-d6b3-4989-832d-f65e45cf4306" />




---

## Evaluation

### What I Achieved

The primary goal of this coursework was to create an interactive OpenGL prototype demonstrating real-time rendering, movement, interaction, and atmosphere. This was successfully achieved through the development of a first-person exploration experience set within a procedurally generated desert and cave environment.  

Relative to my original proposal, the final prototype includes:
- A fully procedural terrain system used to shape and guide player exploration,
- First-person camera control with mouse-based yaw and pitch,
- Velocity-based player movement with gravity and collision handling,
- Runtime loading and instancing of external 3D assets,
- A custom CPU-side collision system using simplified meshes,
- Interactive collectables and a clear win condition,
- An integrated audio system featuring both 2D and 3D positional sound.

The project meets the core technical requirements of the COMP3016 brief while demonstrating a cohesive and playable prototype that integrates multiple graphics programming concepts.  


### What Worked Well

One of the strongest aspects of the prototype is the **stability of player movement and collision handling**. By using velocity-based movement with predictive collision checks, the player experience remains smooth and responsive even when navigating uneven procedural terrain or complex cave geometry.  

The **procedural terrain system** worked particularly well as both a technical feature and a design tool. The bowl-shaped height function naturally guides the player towards the cave without requiring explicit UI direction, demonstrating how procedural generation can support gameplay flow as well as natural looking visual variety.  

The **audio system** enhanced immersion. Dynamic music switching when entering and exiting the cave, combined with 3D positional audio emitted by the *Sith holocron*, provided subtle but effective guidance. This reinforced exploration and discovery without relying on HUD elements or markers.  

Finally, using **Blender as an external layout tool** proved highly effective. This allowed complex environments to be composed visually and transferred into the codebase accurately, improving consistency between visual assets and collision geometry while speeding up iteration.  

AI proved to be a very useful tool in creating this prototype, however after spending time using it, it has made me re-evaluate my trust of AI-generated content, as it worked much better in areas I already had good understanding, since i could then analyse and critique the generated code for better suitability. It is an excellent learning tool but shouold never be relied upon.  


### Limitations

Despite the successful implementation, several limitations remain due to time and scope constraints.

The prototype does not include a **dynamic lighting system**. All rendering is unlit, relying on textures and vertex colour alone. While this maintains visual clarity, it limits depth, mood variation, and realism, particularly within the cave interior as i had hoped to have dark and moody lighting in the cave, except for a red pulsing glow emmitted from the *Sith holocron*.  

Collision handling is intentionally simplified. Wall collision operates in the XZ plane only, without full swept volume or push-out resolution. While this provides stable behaviour, it is not physically accurate and would not scale well to more complex interactions.  

User feedback is also limited. Collectable progress is reported only via console output, and there is no on-screen UI or HUD. While acceptable for a prototype, this reduces accessibility and player awareness.  


### Future Improvements

Given additional development time, several clear improvements could be made.

A priority improvement would be implementing **mesh-based floor collision** inside the cave to replace the global minimum height constraint, allowing more precise interaction with interior geometry.

Collision response could be improved by introducing **vector-based push-out resolution**, enabling smoother interaction with complex shapes and corners.

A simple **UI or HUD system** could be added to display collectable progress and contextual feedback directly to the player rather than relying on console output.

Finally, introducing a **lighting system**—such as a directional light for the desert and ambient or point lighting within the cave for the holocron, would greatly enhance atmosphere and visual depth while expanding the rendering pipeline.


---

## Conclusion

This project demonstrates the successful development of a real-time OpenGL exploration prototype built entirely in C++, showcasing core graphics programming principles including procedural geometry generation, shader-based rendering, first-person camera control, collision handling, and audio-driven interaction. Through the creation of a curated desert and cave environment, the prototype integrates rendering, movement, interaction, and atmosphere into a cohesive and playable experience.  

The work aligns strongly with the aims of the COMP3016 module by evidencing an understanding of how modern graphics applications are structured beneath engine-level abstractions. Implementing systems such as terrain generation, collision detection, instancing, and audio integration from first principles reinforced key concepts in real-time rendering pipelines, spatial reasoning, and system-level problem solving.  

From a learning perspective, this project significantly improved my confidence working with OpenGL and C++ in an unmanaged environment. I developed a deeper understanding of how gameplay systems interact with rendering logic, how stability and predictability must be actively designed for, and how tools such as procedural generation and audio can be used to guide player experience without relying on UI. The challenges encountered—particularly around collision handling and terrain interaction—proved valuable in developing robust debugging and design decision skills.  

Overall, *Beneath Ancient Sands* fulfils the objectives set out in the original proposal and provides a solid foundation for further expansion. The project represents both a technical achievement and a meaningful step forward in my understanding of real-time graphics programming.  



---


## Asset Usage & Licensing

This project makes use of a combination of original assets, procedurally generated geometry, and third-party models sourced under permissive Creative Commons licences. Care was taken to ensure all externally sourced assets comply with the coursework requirements and are appropriately credited.

### CC0 (Public Domain) Assets

The majority of third-party assets used within the project are licensed under Creative Commons Zero (CC0). These assets are free to use, modify, and distribute without restriction or attribution. CC0 assets were chosen where possible to minimise licensing overhead and ensure full compatibility with both coursework submission and future portfolio use.

Sources for CC0 assets:

**Turbosquid**

["Cave Walls 1 - Base"](https://www.turbosquid.com/3d-models/3d-cave-walls-1-base-2076385)  
["Cave Walls 2 - Base"](https://www.turbosquid.com/3d-models/cave-walls-2-base-3d-model-2081619)  
["Cave Wall 3 - Base"](https://www.turbosquid.com/3d-models/3d-cave-wall-3-base-2080922)  
["Cave Walls 4 - Base"](https://www.turbosquid.com/3d-models/cave-walls-4-base-3d-model-2082475)  
["Cave Platform 2 - Base"](https://www.turbosquid.com/3d-models/3d-cave-platform-2-base-2068878)  
["Ancient egyptian pillar"](https://www.turbosquid.com/3d-models/ancient-egyptian-pillar-3d-model-2251671)  
["Coin"](https://www.turbosquid.com/3d-models/3d-model-gold-coin-1500193) - edited with image of my face to act as signature  
["Ultra low poly R2-D2"](https://www.turbosquid.com/3d-models/3d-model-of-ultra-r2-d2-r2/1001640)  

**Freesound**  

[Whispering Man](https://freesound.org/people/szegvari/sounds/540060/)  
[FX - Coins](https://freesound.org/people/bolkmar/sounds/485051/)  

**OpenGameArt**  

[Desert theme](https://opengameart.org/content/desert-theme)  
[Cave Theme](https://opengameart.org/content/cave-theme)  


These assets were modified within Blender (e.g. scale adjustment, decimation, collision mesh creation, texture reassignment) to better adjust and fix the models, i also used Blender to decimate models to create a low-poly version for collision logic.

### CC Attribution (CC-BY) Assets

One asset used was licensed under Creative Commons Attribution (CC-BY). These assets permit use and modification provided the original creator is credited. All CC-BY assets used in the project are listed below with appropriate attribution.

**Sketchfab**

["Sith Holocron (Star Wars)"](https://skfb.ly/6E6Vs) by Yanez Designs is licensed under Creative Commons Attribution (http://creativecommons.org/licenses/by/4.0/).

All CC-BY assets were:

Used strictly within the scope of the coursework

Modified where necessary for optimisation or integration

Clearly attributed in accordance with licence requirements

### Included libraries

I included external libraries to assist with the creation of my prototype, here are their licensing details:  

[ASSIMP](https://github.com/assimp/assimp?tab=readme-ov-file)  
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

[IrrKlang](https://github.com/jonas2602/irrKlang/tree/master)  
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

### Excluded Licences

Assets licensed under Non-Commercial (NC), No Derivatives (ND), or with unclear or unspecified licences were deliberately avoided to ensure compliance with coursework rules and to maintain clean asset provenance.

Summary

By prioritising CC0 assets and correctly attributing CC-BY assets, the project ensures:

Full legal compliance

Transparency in asset sourcing

Good professional practice aligned with industry expectations
