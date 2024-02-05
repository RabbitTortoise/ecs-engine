#include <Engine/Interface/WindowManager.h>

#include <stdio.h>			// Include stdio.h, which contains printf-function

WindowManager::WindowManager()
{
	Window = nullptr;
}
WindowManager::~WindowManager()
{
	// Destroy window
	glfwDestroyWindow(Window);

	// Terminate glfw
	glfwTerminate();
}

bool WindowManager::OpenWindow()
{
	// Set c++-lambda as error call back function for glfw.
	glfwSetErrorCallback([](int error, const char* description) {

		fprintf(stderr, "Error %d: %s\n", error, description);
		});

	// Try to initialize glfw
	if (!glfwInit()) {
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window and check that creation was succesful.
	Window = glfwCreateWindow(WindowWidth, WindowHeight, "ECS Engine", NULL, NULL);
	if (!Window) {
		glfwTerminate();
		return false;
	}

	// Set current context
	glfwMakeContextCurrent(Window);
	// Load GL functions using glad
	gladLoadGL(glfwGetProcAddress);

	// Specify the key callback as c++-lambda to glfw
	glfwSetWindowSizeCallback(Window, [](GLFWwindow* window, int width, int height)
		{
			WindowWidth = width;
			WindowHeight = height;

			for (auto &e : WindowSizeUpdateSubscribers)
			{
				e(width, height);
			}
		});
	return true;
}

void WindowManager::SwapBuffer()
{
	glfwSwapBuffers(Window);
}
