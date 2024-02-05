#include <stdio.h>
#include <vector>
#include <iostream>
#include <string>
#include <Windows.h>

#include <Engine/Interface/IECS.h>
#include <Engine/Core/Application.h>


int main()
{
    //Enable Finnish letters in the windows console.
    SetConsoleCP(1252);
    SetConsoleOutputCP(1252);
    //Sets the used locale settings to system default. If finnish keyboard is chosen as locale then finnish letter work in the console.
    setlocale(LC_ALL, "");

    /* initialize random seed: */
    srand(static_cast<unsigned int>(time(NULL)));


    Application* app = new Application();
    app->Init();
    app->Start();
    delete app;
    app = nullptr;

    std::cout << "GAME CLOSED" << std::endl;
    return 0;
}