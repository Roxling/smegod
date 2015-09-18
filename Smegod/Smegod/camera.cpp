#include "camera.h"
#include <iostream>
#include "shaders.h"

Camera::Camera(float fov, int width, int height, float near, float far, GLuint mshader_program)
{
	projection = glm::perspective(fov, (float)width / (float)height, near, far);
	shader_program = mshader_program;
	projection_location = glGetUniformLocation(shader_program, "projection");
	view_location = glGetUniformLocation(shader_program, "view");
	cout << projection_location <<" : " << view_location << endl;
}

void Camera::translateLocal(float dx, float dy, float dz)
{
	position = position + dz*glm::normalize(front) +
						  dy*glm::normalize(up) +
						  dx*glm::normalize(glm::cross(front,up));
}

void Camera::rotateLocalX(float deg)
{
	auto vec = glm::cross(front, up);
	if (glm::length(vec) > 0) {

		rotate(deg, vec);
	}

}
void Camera::rotateLocalY(float deg)
{
	rotate(deg, up);
}

void Camera::rotate(float deg, glm::vec3 axis)
{
	//somethings fishy here!
	glm::mat3 rot = glm::mat3(glm::rotate(identity, glm::radians(deg), axis));
	front = rot * front;
	//up = rot * up;
}

glm::mat4 & Camera::getView()
{
	world = glm::lookAt(position, position + front, up);
	return world;
}

void Camera::setupShader(GLuint program)
{
	
}

void Camera::render()
{
	glUniformMatrix4fv(projection_location, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(getView()));
}


