#ifndef CAMERA_H
#define CAMERA_H

#include "header.h"
#include "model.h"

enum DIRECTION_CAM
{
	DIR_LEFT, DIR_RIGHT, DIR_FORWARD, DIR_BACKWARD
};

class FPSCamera
{

public:
	std::string name = "default";

	glm::vec3 camera_pos =  glm::vec3(0.0f, 0.5f, 7.0f);
	glm::vec3 camera_front = glm::vec3(0.0f, 0.25f, -1.0f);
	glm::vec3 camera_up = glm::vec3(0.0f, 5.0f, 0.0f);

	float	yaw = -90.0f;
	float	pitch = 0.0f;

	float	fov = 70.0f;

	float	last_x = 0;
	float	last_y = 0;

	bool	first_mouse = 0;

	bool	move_y = false;

	glm::vec3 camera_speed = { 0.8f,2.0f,0.8f };

	Vec3f p = {0.0f,0.5f, 7.0f};
	Vec3f f = {0.0f,0.25f,-1.0f};
	Vec3f s = {0.8f,2.0f,0.8f};
	Vec3f h = {0.0f,5.0f,0.0f};

	glm::vec3 getCamForward() const;	
	
	FPSCamera();

	void fuckme() {
		camera_pos.x = p.x; camera_pos.y = p.y; camera_pos.z = p.z;
		camera_front.x = f.x; camera_front.y = f.y; camera_front.z = f.z;
		camera_up.x = h.x; camera_up.y = h.y; camera_up.z = h.z;
		camera_speed.x = s.x; camera_speed.y = s.y; camera_speed.z = s.z;
	}

	template<typename Ar>
	void serialize(Ar& ar) {
		p.x = camera_pos.x; p.y = camera_pos.y; p.z = camera_pos.z;
		f.x = camera_front.x; f.y = camera_front.y; f.z = camera_front.z;
		s.x = camera_speed.x; s.y = camera_speed.y; s.z = camera_speed.z;
		h.x = camera_up.x; h.y = camera_up.y; h.z = camera_up.z;
		ar & YAS_OBJECT(nullptr, p, f, h, yaw, pitch, fov, s);
	}
	std::string getName() { return name; }
	void setName(std::string s) { name = s; }
	const glm::vec3& getCameraPosition() const { return camera_pos; }
	const glm::vec3& getCameraFront() const { return camera_front; }
	const glm::vec3& getCameraUp() const { return camera_up; }
	const glm::vec3& getCameraSpeed() const { return camera_speed; }
	void setCameraSpeed(glm::vec3 speed) {
		camera_speed = speed;
	}

	float getYaw() { return yaw; }
	float getPitch() { return pitch; }
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
		camera_pos.y -= camera_speed.y;

	}

	void forward()
	{
		camera_pos += camera_speed * getCamForward();
	}

	void backward()
	{
		camera_pos -= camera_speed * getCamForward();
	}

	void ResetMouse()
	{
		first_mouse = true;
	}

	void addFOV(float v)
	{
		fov += v;
	}

	void setFOV(float v)
	{
		fov = v;
	}

	float getFOV()
	{
		return fov;
	}

	void move(int x, int y);

};



#endif 
