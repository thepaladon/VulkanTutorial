#include "FreeCamera.h"

#include "glm/glm.hpp"

FreeCamera::FreeCamera(float fov) : m_FOV(fov), m_ImagePlanePos(), m_AspectRatio(0)
{
}

void FreeCamera::UpdateCamera(float winWidth, float winHeight)
{
	m_AspectRatio =
		winWidth / winHeight;

	const glm::vec3 viewDirection = m_Transform.Forward();
	const glm::vec3 right = m_Transform.Right();
	const glm::vec3 up = m_Transform.Up();

	const float left = m_AspectRatio * 0.5f;
	constexpr float top = 0.5f;
	const float m_Dist = 0.5f / tan(m_FOV * glm::pi<float>() / 360.f);
	m_ImagePlanePos = m_Dist * viewDirection - left * right + top * up;
}

void FreeCamera::CameraInput(float deltaTime, glm::vec3 move_input, glm::vec2 rotate_input)
{
		glm::vec3 moveDirection = glm::vec3();
		moveDirection += move_input.z * m_Transform.Forward();
		moveDirection += move_input.y * glm::vec3(0.0f, 1.0f, 0.0f);
		moveDirection += move_input.x * m_Transform.Right();

		if (moveDirection != glm::vec3())
			moveDirection = glm::normalize(moveDirection);
		moveDirection *= m_MoveScalar * deltaTime;

		m_Transform.Translate(moveDirection);

		// Rotate left/right
		rotate_input.x *= -1;
		m_Transform.AngleAxisGlobal(rotate_input.x * m_ViewScalar * deltaTime, glm::vec3(0, 1, 0));

		// Rotate up/down
		m_Transform.AngleAxisLocal(rotate_input.y * m_ViewScalar * deltaTime, glm::vec3(1, 0, 0));
}

glm::mat4 FreeCamera::GetProjection() const
{
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearPlane, m_FarPlane);
	return projectionMatrix;
}

glm::mat4 FreeCamera::GetView()
{
	return m_Transform.GetInverseModelMatrix();
}
