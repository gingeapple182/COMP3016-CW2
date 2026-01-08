# COMP3016 CW2 Proposal – Oliver Cole
## Working title:
Beneath ancient sands
## Overview:
I aim to create an exploration game written in C++ using OpenGL. The game is set in
an ancient city buried in the desert sands. The player will explore the environment using
keyboard and mouse, traversing procedurally generated sand terrain and some ruined
ancient structures to locate a hidden ancient artefact.
The player will be within a desert environment bounded by the partially submerged
ruins which guide the player to explore forwards. The primary gameplay objective is to
locate an ancient artefact within the ruins.
## Details:
The environment will consist of a procedurally generated desert terrain representing
sand dunes, as well as static ruins structured sourced from free assets online. These
structures will be positioned to look as if submerged by the sands over time, creating an
abandoned and ancient vibe.
A main artefact will be the players goal to obtain and will be visually highlighted as
the objective, possibly by rotating on a pedestal. The atmosphere will be supported
through use of directional lighting and music, changing when near the artefact for
suspense and guide progression.
The project will be developed in C++ using OpenGL techniques. It will demonstrate realtime rendering, keyboard/mouse interaction, procedural content generation and
loading/rendering of external 3D models.
## Libraries and Tools:
- GLAD OpenGl loading
- GLFW Window creationg and input handling
- ASSIMP Loading external 3D models
- Irrklang Audio
## Advanced:
The project aims to include advanced features such as dynamic lighting and the use of
audio to enhance atmosphere and immersion.



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
