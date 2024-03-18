# Meteor In Galaxy (LabWork3016)
## Description
  CW1 of COMP3015
  This project uses OpenGL shader to create a scene.<br />
  My goal is to create a galaxy-themed meteor viewer.<br />
  Adding keyboard control for the user to control the camera when the program is running.<br />
  The goal is to make a skybox, a fog system, texture on objects, custom 3D objects, and lighting.<br />

## BlinnPhong
BlinnPhong is a reflection model, having a higher speed than the normal Phong model, it is useful for lighting reflection, since we're making the shader look better with it. <br />
![image](https://github.com/Raymondwaimcheng/MeteorInGalaxy_LabWork3016/assets/147705610/3f9890e6-3fcc-44ac-9dcd-e38aa04fe9ce)


## Skybox
Skybox is the background image of the sky, creating a suitable background and making the scene look better.<br />
![image](https://github.com/Raymondwaimcheng/MeteorInGalaxy_LabWork3016/assets/147705610/0d5ac59e-1b8b-4060-9462-8978dca67989) <br />

Skybox is loaded using loadHdrCudeMap() to load all six skybox map images, only take hdr file type picture.<br />
![image](https://github.com/Raymondwaimcheng/MeteorInGalaxy_LabWork3016/assets/147705610/04d82b1b-2517-42f9-a890-8a298236cdf6) <br />

Active with the texture2.<br />

## Fog
Fog is a visual technique that makes the object at a certain distance start to become more unseeable, the texture will not load in detail, creating a fog effect similar to real life. <br />
![image](https://github.com/Raymondwaimcheng/MeteorInGalaxy_LabWork3016/assets/147705610/84880094-6886-494f-9c14-d701e0fed262) <br />

## Finds
Using multiple vert and frag shader files is better, for different texture displays and making sure each texture and object can have different effects.<br />
![image](https://github.com/Raymondwaimcheng/MeteorInGalaxy_LabWork3016/assets/147705610/d8b42d8b-d7fb-417b-94e2-a3b87fc7525c) <br />
Make sure the file is compiled inside compile(). <br />

## Control
Keyboard Control the camera movement.<br />
* Up:       W
* Down:     S
* Forward:  A
* Backward: D
<br />
Keyboard control is different in shading, I put my input tracker inside the mainloop of the scenerunner.h. <br />
![image](https://github.com/Raymondwaimcheng/MeteorInGalaxy_LabWork3016/assets/147705610/e96fc994-bab8-45c2-b7bd-1a5cd0ffc4cf) <br />

So it can react to the user's input when the key is pressed. <br />


## Demo
[[https://www.youtube.com/watch?v=JX5A6sSk9WE](https://youtu.be/meFtA9O0jGU)](https://youtu.be/1o-oDDCkJ14)

## Path
* Include Directories <br />
C:\Users\Public\OpenGL\include <br />
* Library Directories <br />
C:\Users\Public\OpenGL\lib <br />
* Linker - Additional Dependencies <br />
  glfw3.lib <br />
  opengl32.lib <br />
<br />
* GLFW <br />
#include<GLFW/glfw3.h> <br />
* glm <br />
#include<glm/glm/glm.hpp> <br />
* GLAD <br />
#include<glad/glad.h> <br />

## System
* Coding using Visual Studio 2022
* Window 7 11 Operating System
* OpenGL 4
