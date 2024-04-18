#pragma once

#include "glm/glm.hpp"
#include "Transform.h"

class FreeCamera 
{
public:
	FreeCamera(float fov = 50.0f);

	void UpdateCamera(float winWidth, float winHeight);
	void CameraInput(float deltaTime, glm::vec3 move_input, glm::vec2 rotate_input);

	glm::mat4 GetProjection() const;
	glm::mat4 GetView();

private:

	float m_NearPlane = 0.01f;
	float m_FarPlane = 1000.f;
	float m_FOV = 50.0f;

	glm::vec3 m_ImagePlanePos; //unused, later for rt
	float m_AspectRatio;

	Transform m_Transform;

	// For movement
	float m_ViewScalar = 2.0f;
	float m_MoveScalar = 150.0f;
};
