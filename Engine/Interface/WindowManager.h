#pragma once

#include <glad/gl.h>		// Include glad
#include <GLFW/glfw3.h>		// Include glfw
#include <vector>
#include <functional>
#include <map>

class WindowManager
{
public:
    WindowManager();
    ~WindowManager();

    bool OpenWindow();
    void SwapBuffer();

    static GLFWwindow* GetGLFWWindow() { return Window; };

    static int GetWindowWidth() { return WindowWidth; }
    static int GetWindowHeight() { return WindowHeight; }

    template <class UserClass>
    static void RegisterToWindowSizeUpdate(void (UserClass::* Callback)(int, int), UserClass* Caller)
    {
        using std::placeholders::_1;
        using std::placeholders::_2;
        std::function<void(int, int)> f = std::bind(Callback, Caller, _1, _2);
        WindowSizeUpdateSubscribers.push_back(f);
    }


private:
    inline static GLFWwindow* Window = nullptr;
    inline static std::vector<std::function<void(int, int)>> WindowSizeUpdateSubscribers;

    inline static int WindowWidth = 1920;
    inline static int WindowHeight = 1080;
};