#include "Camera.h"


Camera::Camera() {}

Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed) {

	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();

}

glm::mat4 Camera::calculateViewMatrix() {

	return glm::lookAt(position, position + front, up);

}


void Camera::keyControl(bool* keys, GLfloat deltaTime) {

	GLfloat velocity = moveSpeed * deltaTime;
	GLfloat lookVelocity = ( moveSpeed * deltaTime ) * 15.0f;

	if (keys[GLFW_KEY_W]) {

		position += front * velocity;

	}

	if (keys[GLFW_KEY_S]) {

		position -= front * velocity;

	}

	if (keys[GLFW_KEY_A]) {

		position -= right * velocity;

	}

	if (keys[GLFW_KEY_D]) {

		position += right * velocity;

	}

	if (keys[GLFW_KEY_E]) {

		position += glm::vec3(0.0f, 1.0f, 0.0f) * velocity;

	}

	if (keys[GLFW_KEY_Q]) {

		position -= glm::vec3(0.0f, 1.0f, 0.0f) * velocity;

	}

	if (keys[GLFW_KEY_UP]) {

		pitch += lookVelocity;

	}

	if (keys[GLFW_KEY_DOWN]) {

		pitch -= lookVelocity;

	}

	if (keys[GLFW_KEY_LEFT]) {

		yaw -= lookVelocity;

	}

	if (keys[GLFW_KEY_RIGHT]){

		yaw += lookVelocity;

	}
	
}


void Camera::mouseControl(GLfloat xChange, GLfloat yChange) {

	xChange *= turnSpeed;
	yChange *= turnSpeed;


	//yaw   += std::fmod(xChange, 45);
	//pitch += std::fmod(yChange, 45);

	yaw += xChange;
	pitch += yChange;

	if (pitch > 89.0f) {
		pitch = 89.0f;
	}

	if (pitch < -89.0f) {
		pitch = -89.0f;
	}

	update();

}

glm::vec3 Camera::getCameraPosition() {

	return position;

}

glm::vec3 Camera::getCameraDirection() {

	return glm::normalize(front);

}


void Camera::update() {

	// Calculating the Front Vector
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front   = glm::normalize(front);

	// Calculating the Right vector
	right = glm::normalize(glm::cross(front, worldUp));

	// Calculating the UP vector
	up = glm::normalize(glm::cross(right, front));


}

Camera::~Camera() {



}