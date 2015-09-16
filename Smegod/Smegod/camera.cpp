#include "camera.h"
#include <iostream>

Camera::Camera(float fov, int width, int height, float near, float far)
{
	projection = glm::perspective(fov, (float)width / (float)height, near, far);
}

void Camera::translateLocal(float dx, float dy, float dz)
{
	position = position + dz*glm::normalize(front) +
						  dy*glm::normalize(up) +
						  dx*glm::normalize(glm::cross(front,up));
}


void Camera::rotate(float deg, glm::vec3 axis)
{
	//somethings fishy here!
	glm::mat3 rot = glm::mat3(glm::rotate(identity, glm::radians(deg), axis));
	up = rot * up;
	front = rot * front;
}

glm::mat4 & Camera::getView()
{
	world = glm::lookAt(position, position + front , up);
	return world;
}

