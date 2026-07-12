#include "inputHandler.h"
#include <glm/glm.hpp>

// Static pointer used by GLFW callbacks to access the active InputHandler.
InputHandler* InputHandler::s_instance = nullptr;

// Intializes all key state and default camera settings
InputHandler::InputHandler()
	: keyW(false), keyS(false), keyA(false), keyD(false),
	isOrtho(true),
	windowHandle(nullptr)
{
}

// Registers the inpput handler with GLFW window and assigns keyboard callback function
void InputHandler::Register(GLFWwindow* window)
{
	windowHandle = window;
	s_instance = this;

	glfwSetKeyCallback(window, GLFWKeyCallback);

}

/*
	Processes camera movement and input states

	W/S camera pitch
	A/D camera yaw
*/
void InputHandler::ProcessInput(float dt, float& yaw, float& pitch)
{
	if (keyA)
		yaw -= CameraSpeed * dt;

	if (keyD)
		yaw += CameraSpeed * dt;

	if (keyW)
		pitch += CameraSpeed * dt;

	if (keyS)
		pitch -= CameraSpeed * dt;

	if (pitch > 89.f)
		pitch = 89.f;

	if (pitch < -89.f)
		pitch = -89.f;
}

// Handles GLFW Keyboard Events 
// Tracks WASD key states
// 1 = ortho
// 2 = persp
// Esc = Exit Application
void InputHandler::OnKey(GLFWwindow* window, int key, int action)
{
	if (key == GLFW_KEY_W)
		keyW = (action != GLFW_RELEASE);
	if (key == GLFW_KEY_S)
		keyS = (action != GLFW_RELEASE);
	if (key == GLFW_KEY_A)
		keyA = (action != GLFW_RELEASE);
	if (key == GLFW_KEY_D)
		keyD = (action != GLFW_RELEASE);

	if (action == GLFW_PRESS)
	{
		if (key == GLFW_KEY_1)
			isOrtho = true;			// orthographic
		if (key == GLFW_KEY_2)		// perspective
			isOrtho = false;
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

}

// Static GLFW callback function
// This function forwards keyboard events
// to the active InputHandler instance
void InputHandler::GLFWKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (s_instance)
		s_instance->OnKey(window, key, action);
}

// Returns the currently selected projection mode.
bool InputHandler::IsOrtho() const
{
	return isOrtho;
}


