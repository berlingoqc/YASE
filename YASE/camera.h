#ifndef CAMERA_H
#define CAMERA_H
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <GL/glew.h>

class FPSCamera
{
	glm::vec3 camera_pos = glm::vec3(0.0f, 0.5f, 7.0f);
	glm::vec3 camera_front = glm::vec3(0.0f, 0.25f, -1.0f);
	glm::vec3 camera_up = glm::vec3(0.0f, 5.0f, 0.0f);

	float	yaw = -90.0f;
	float	pitch = 0.0f;

	float	fov = 70.0f;

	float	last_x = 0;
	float	last_y = 0;

	bool	wrap_mouse = true;
	bool	first_mouse = 0;

	bool	move_y = false;

	glm::vec3	camera_speed = { 0.8f,2.0f,0.8f };



	float	tx = 1.0f;
	float	ty = 1.0f;
	float	tz = 1.0f;

	glm::vec3 getCamForward() const {
		glm::vec3 camDevant;
		camDevant.x = camera_front.x;
		camDevant.z = camera_front.z;
		if (move_y)
			camDevant.y = camera_front.y;
		else
			camDevant.y = 0.0f;
		return camDevant;
	}

public:
	FPSCamera()
	{
		
	}

	glm::mat4 getLookAt() const
	{
		return glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
	}
	glm::mat4 getView() const
	{
		return 	glm::mat4(glm::mat3(glm::lookAt(camera_pos, camera_pos + camera_front, camera_up)));
	}

	void left()
	{
		camera_pos -= glm::normalize(glm::cross(getCamForward(), camera_up)) * camera_speed.x;
	}

	void right()
	{
		camera_pos += glm::normalize(glm::cross(getCamForward(), camera_up)) * camera_speed.x;
	}

	void up()
	{
		camera_pos.y += camera_speed.y;
	}

	void down()
	{
		camera_pos -= camera_speed.y;
		
	}

	void forward()
	{
		camera_pos += camera_speed * getCamForward();
	}

	void backward()
	{
		camera_pos -= camera_speed * getCamForward();
	}

	void move(int x, int y)
	{
		if (wrap_mouse) {
			wrap_mouse = false;
			last_x = x;
			last_y = y;
		}
		else {
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

			wrap_mouse = true;
		}
	}


	void addTX(float tx)
	{
		this->tx += tx;
	}

	void addTY(float ty)
	{
		this->ty += ty;
	}

	glm::vec3 getT() const
	{
		return { tx,ty,tz };
	}

	void addFOV(float v)
	{
		fov += v;
	}

	float getFOV()
	{
		return fov;
	}
};



#endif 