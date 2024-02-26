# New version available!
Check out the new project: https://github.com/RabbitTortoise/encosy-engine

# What is this?

This is the December 2022 version of the game engine I've been working on as part of my studies. I have used the project to explore Entity Component System -architecture and data-oriented programming in game engine development. This repository is intended to be a snapshot of my skills at the time. The project solution can be generated with CMake and it should compile fine with Visual Studio 2022 using MSVC v143. I replaced all the textures with 1-pixel versions as I’m not sure if I can include them in a public repo. This playlist has videos with PBR textures in use: https://youtube.com/playlist?list=PL7UiajzfESH-z_U4DIVp0npQNs3LKtoZS&si=aafztwpvyDlTOWAs .
  
I am currently working on an updated version as part of my final project studies and thesis. I'm keeping it in a separate repository as it is a major rework. I will add it to GitHub as soon as I have the first presentable demo ready. The new version is fixing some major design flaws the 2022 version has. It has better memory management, a better interface to work with the engine and other features that improve the usability of the engine. In the new project I have also started using more of the modern C++20 features, I’m changing the graphics API to Vulkan and I’m adding some multithreading as part of my thesis.
 
Feel free to browse the code and use it in your own project if you want. Just keep in mind that this version has some major flaws as you can't remove or add new components to an entity at runtime. There are different branches that have demos and other course projects I have made using the engine.

## How to setup
1. Clone the repository. Run following commands in the local repo:
- git submodule init
- git submodule update
2. Configure and Generate the project with CMake
3. Open with Visual Studio and set ECS_Engine as startup project
4. Build and run the project


# Branches

## main
Just the engine and a test scene.
### Controls
WASD: Move  
QE: Move up/down  
Left sift, left ctrl: Increase/Reduce Speed  
ESC: Quit the game
  
You can collect the flying spheres.


## physics
Contains the demo I made for a physics course. 3d cube collisions, no external physics library in use. Demo scene with some physics objects.
### Controls
WASD: Move  
QE: Move up/down  
Left sift, left ctrl: Increase/Reduce Speed  
ESC: Quit the game  
  
You can collect the flying spheres.


## goldrush
A minigame I made for AI course. Goal is to destroy all enemy towers. AI difficulty is randomized each game.
### Controls
Arrow keys: Move the cursor on the map  
1: Buy a gold collector  
2: Buy an attack drone  
3: Buy a tower  
ESC: Quit the game  
