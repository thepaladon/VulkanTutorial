# Learning Vulkan 
Following the [Vulkan Tutorial](https://vulkan-tutorial.com/)

## Final Screenshot
Runs with 1.5ms on my `Intel(R) Core(TM) i7-10750H CPU @ 2.60GHz`, `RTX 3070 Laptop GPU`
![image](https://github.com/thepaladon/VulkanTutorial/assets/44022509/eedd911f-5825-4fdd-ac73-855f0093572b)


## Overview

This personal project serves as my introduction to the **Vulkan API** and its features.

**My primary goal** is to better understand how a low-level graphics API works as I've mostly worked with the PlayStation 5. Due to it being closed source and therefore lacking learning materials, I decided to invest some time in Vulkan.

**My secondary goal**  is to learn how to use it well enough to be able to write a Vulkan back-end of my current ["On The Bubble"](https://store.steampowered.com/app/2707350/On_the_Bubble/) university project. The back-end of that project is hardware ray tracing and compute oriented since the renderer is entirely path-traced.

I might do some bigger things with this or I might not. For now, I'm taking it one step at a time toward the goals I've listed above. 

## Features
- Basic Vertex + Fragment Shader
- Basic Compute Shader for Particles
- ImGui + ImGuizmo
- HLSL/GLSL Shader Compilation
 
## Getting Started
It should run without issue if you have downloaded the [Vulkan SDK](https://vulkan.lunarg.com/#new_tab).

I also use vcpkg only for `glm`.  You'll need to set that up. I'll fix this at some point and move `glm` to `Dependencies`.

## Controls
- W/A/S/D - Moves the Camera
- R / F - Camera Up / Down
- Arrow keys - Rotates the Camera
- 1/2/3/4 - Gizmos modes


