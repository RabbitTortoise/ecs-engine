#pragma once

#include <glad/gl.h>		// Include glad
#include <GLFW/glfw3.h>		// Include glfw

#include <vector>
#include <iostream>

#include <Engine/Interface/WindowManager.h>
#include <Engine/Interface/IECS.h>
#include <Engine/Core/ScriptingCore.h>

class Application
{
public:
    Application()
    {  
        ApplicationWindowManager = nullptr;
        ScriptManager = nullptr;

        WorldEntityManager = nullptr;
        WorldSystemsManager = nullptr;
    }

    ~Application() {
        delete ApplicationWindowManager;
        ApplicationWindowManager = nullptr;
        delete ScriptManager;
        ScriptManager = nullptr;
        
    }

    void Init()
    {   
       ApplicationInit();
    }

    void Start()
    {
        ApplicationLoop();
    }


private:

    EntityManager* WorldEntityManager;
    SystemsManager* WorldSystemsManager;

    WindowManager* ApplicationWindowManager;
    ScriptingCore* ScriptManager;

    void ApplicationInit()
    {
        std::cout << "Application Start" << std::endl;
        //Create new Window
        ApplicationWindowManager = new WindowManager();

        if (!ApplicationWindowManager->OpenWindow())
        {
            std::cout << "COULDN'T OPEN WINDOW!" << std::endl;
        }

        // Specify the key callback as c++-lambda to glfw
        glfwSetKeyCallback(ApplicationWindowManager->GetGLFWWindow(), [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            // Close window if escape is pressed by the user.
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GLFW_TRUE);
            }
        });

        //Initialize ECS
        IECS::Init();
        WorldEntityManager = IECS::GetEntityManager();
        WorldSystemsManager = IECS::GetSystemsManager();
        //Initialize core systems.
        WorldSystemsManager->InitSystems();
        
        std::cout << "ECS Setup complete" << std::endl;

        //Load all scripts.
        ScriptManager = new ScriptingCore();
        ScriptManager->CreateScripts();
        ScriptManager->InitScripts();

        //Initialize new systems created by scripts.
        WorldSystemsManager->InitSystems();

        std::cout << "Script Setup complete" << std::endl;
    }

    void ApplicationLoop()
    {
        // Get time using glfwGetTime-function, for delta time calculation.
        float prevTime = (float)glfwGetTime();
        while (!glfwWindowShouldClose(ApplicationWindowManager->GetGLFWWindow())) 
        {
            // Compute application frame time (delta time) and update application
            float curTime = (float)glfwGetTime();
            float deltaTime = curTime - prevTime;
            prevTime = curTime;
            //std::cout << "DeltaTime: " << deltaTime << std::endl;

            // Poll other window events.
            glfwPollEvents();

            //Collision Update
            WorldSystemsManager->UpdateCollisionSystems(deltaTime);

            //Scripting Update
            ScriptManager->UpdateScripts(deltaTime);
            //System Update
            WorldSystemsManager->UpdateSystems(deltaTime);

            // Render the game frame and swap OpenGL back buffer to be as front buffer.
            WorldSystemsManager->UpdateRenderSystems(deltaTime);
            ApplicationWindowManager->SwapBuffer();

            WorldEntityManager->DeleteMarkedEntities();
        }
    }
};



