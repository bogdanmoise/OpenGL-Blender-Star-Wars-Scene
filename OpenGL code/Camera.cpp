#include "Camera.hpp"

namespace gps {

	//Camera constructor
	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraUpDirection = cameraUp;

		this->cameraYawAngle = -90.0f;
		this->cameraPitchAngle = 0.0f;
	}

	glm::vec3 Camera::getCameraTarget()
	{
		return this->cameraTarget;
	}

	//return the view matrix, using the glm::lookAt() function
	glm::mat4 Camera::getViewMatrix() {
		return glm::lookAt(cameraPosition, cameraTarget, cameraUpDirection);
	}

	glm::vec3 Camera::getCameraPosition()
	{
		return this->cameraPosition;
	}

	glm::mat4 Camera::getPicture(glm::vec3 picturePos)
	{
		return glm::lookAt(picturePos, cameraTarget, cameraUpDirection);
	}

	glm::vec3 Camera::getCameraNextPosition(MOVE_DIRECTION direction, float speed)
	{
		switch (direction)
		{
		case MOVE_LEFT:
			return (this->cameraPosition - glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)) * speed);
		case MOVE_RIGHT:
			return (this->cameraPosition + glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)) * speed);
		case MOVE_FORWARD:
			return (this->cameraPosition + this->cameraFrontDirection * speed);
		case MOVE_BACKWARD:
			return (this->cameraPosition - this->cameraFrontDirection * speed);
		}
	}

	//update the camera internal parameters following a camera move event
	void Camera::move(MOVE_DIRECTION direction, float speed) {
		switch (direction)
		{
		case MOVE_LEFT:
			this->cameraTarget -= glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)) * speed;
			this->cameraPosition -= glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)) * speed;
			
			break;
		case MOVE_RIGHT:
			this->cameraTarget += glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)) * speed;
			this->cameraPosition += glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)) * speed;
			
			break;
		case MOVE_FORWARD:
			this->cameraTarget += this->cameraFrontDirection * speed;
			this->cameraPosition += this->cameraFrontDirection * speed;
			
			break;
		case MOVE_BACKWARD:
			this->cameraTarget -= this->cameraFrontDirection * speed;
			this->cameraPosition -= this->cameraFrontDirection * speed;
			
		}
		this->cameraFrontDirection = glm::normalize(this->cameraTarget - this->cameraPosition);
	}

	//update the camera internal parameters following a camera rotate event
	//yaw - camera rotation around the y axis
	//pitch - camera rotation around the x axis
	void Camera::rotate(float pitch, float yaw) {
		cameraPitchAngle += pitch * greater;
		cameraYawAngle += yaw;

		this->cameraTarget.x = this->cameraPosition.x + cos(glm::radians(cameraYawAngle)) * cos(glm::radians(cameraPitchAngle));
		this->cameraTarget.y = this->cameraPosition.y + sin(glm::radians(cameraPitchAngle));
		this->cameraTarget.z = this->cameraPosition.z + sin(glm::radians(cameraYawAngle)) * cos(glm::radians(cameraPitchAngle));;

		this->cameraFrontDirection = glm::normalize(this->cameraTarget - this->cameraPosition);
		this->cameraRightDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection));
	}
}