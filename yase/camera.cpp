#include "camera.h"


glm::vec3 FPSCamera::getCamForward() const {
		glm::vec3 camDevant;
		camDevant.x = camera_front.x;
		camDevant.z = camera_front.z;
		if (move_y)
			camDevant.y = camera_front.y;
		else
			camDevant.y = 0.0f;
		return camDevant;
	}

	FPSCamera::FPSCamera()
	{
		
	}

	void FPSCamera::move(int x, int y)
	{
		if (first_mouse) {
			last_x = x;
			last_y = y;
			first_mouse = false;
		}

		GLfloat xoffset = x - last_x;
		GLfloat yoffset = last_y - y;
		last_x = x;
		last_y = y;

		GLfloat sensitivity = 0.3;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw += xoffset;
		pitch += yoffset;

		if (pitch > 89.0f) pitch = 89.0f;
		if (pitch < -89.0f) pitch = -89.0f;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		camera_front = glm::normalize(front);
	}
