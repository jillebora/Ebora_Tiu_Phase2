#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

/*
	Handles keybaord input
	Rensponsible for camera movement, projection switching, 
	pause/play controls, and applciation exit

*/
class InputHandler
{
	public:
		// Creates a new input handler instance.
		InputHandler();

		void Register(GLFWwindow* window);

		void ProcessInput(float dt, float& yaw, float& pitch);

		float CameraSpeed = 60.f;

		bool IsOrtho() const;

	private:
		//Key handler
		void OnKey(GLFWwindow* window, int key, int action);

		//static key callback
		static void GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

		static InputHandler* s_instance;

		bool keyW = false;
		bool keyS = false;
		bool keyA = false;
		bool keyD = false;

		// Projection mode: true (1 key) = ortho, false = persp 
		bool isOrtho = true;


		GLFWwindow* windowHandle = nullptr;

};

