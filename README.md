# Vulkan Final Project

## Description

A 3D scene in a Mario style, built with Vulkan. The scene represents a small level: Mario jumps under a floating block, a star comes out of the block, there is a warp pipe to one side, and Lakitu floats up from that area. There is also a sky sphere in a Mario style sky, centered on the camera. The project loads OBJ models, applies independent MVP matrices per object, uses textures, Phong lighting, and an FPS camera with perspective projection.

## Rubric requirements covered

- **3D model loading / animation:** more than 5 object instances in the scene: brick ground, sky sphere, Mario, Goomba, warp pipe, block, star, Lakitu, and sparkle particles around the star
- **Different OBJ types:** more than 3 different OBJ shapes: `plane.obj`, `mario.obj`, `goomba.obj`, `warp_pipe.obj`, `marioblock.obj`, `star.obj`, `lakitu.obj`
- **Textures:** more than 5 different textures: `bricks.bmp`, `mariosky.bmp`, `mario.bmp`, `goomba.png`, `warp_pipe.png`, `marioblock.png`, `rainbowStarMario.bmp`, `lakitu.bmp`
- **Model matrices:** every object has its own `model` matrix, with position, rotation and scale
- **Animation:** several objects update their model matrix every frame: Mario jumps vertically under the block, Goomba moves side to side, the floating block stays static, the star rises out of the block, Lakitu rises and floats from the pipe area, and several particles move around the star
- **Camera:** FPS camera with keyboard and mouse input, and perspective projection
- **Lighting:** Phong lighting model in the fragment shader
- **Advanced feature:** sky sphere and particle animation. The sky sphere follows the camera position, and the particles are small animated stars around the main star
- **Structure and quality:** uses a `SceneObject` structure, vectors for the objects, and separate functions for loading resources, updating matrices, updating lights, drawing and cleaning up memory

## Advanced feature

The advanced features chosen are the **sky sphere** and **particle animation**. The sky sphere uses `sphere.obj` with the `mariosky.bmp` texture. Each frame it is repositioned to the camera's position so the player is always inside the sky, and face culling is disabled so the sphere is visible from the inside.

The scene has several sparkle particles around the main star. Each particle uses the `star.obj` model at a very small scale, with its own model matrix. The particles are animated every frame using `sin` and `cos` functions: they drift smoothly on X and Y, move up and down on Z, and rotate around their center.

## Controls

- `W`: move forward
- `S`: move backward
- `A`: move left
- `D`: move right
- `Q`: move down
- `E`: move up
- Mouse: look around
- `Esc`: quit

## How to build

Before building, unzip `glm.zip` and `include.zip` so that `glm/` and `include/` sit directly inside the project root, next to `main.cpp`.

1. Open `VulkanTest.sln` in Visual Studio 2022
2. Select the `Debug` configuration and `x64` platform
3. Build the `VulkanTest` project
4. Run from Visual Studio

The shaders are already compiled as `vert.spv` and `frag.spv`. If you modify `shader.vert` or `shader.frag`, you will need the Vulkan SDK installed to recompile them, then run `compile_shaders.bat` from the project folder.

## Main files

- `main.cpp`: the Mario scene, camera, animation, descriptors and render loop
- `vulkan_setup.cpp`: Vulkan initialization, swapchain, pipeline and depth buffer
- `model.cpp`: loading OBJ models with tinyobjloader
- `texture.cpp`: loading textures with stb_image
- `shader.vert`: MVP transformation and normals
- `shader.frag`: texture and Phong lighting

## References and sources

- Vulkan template provided on eStudy
- Course notes: Camera, Load OBJ, Texture and Light
- Mario models and textures provided in the practice ZIPs
- tinyobjloader for loading OBJ files
- stb_image for loading images
- LearnOpenGL Camera: https://learnopengl.com/Getting-started/Camera
