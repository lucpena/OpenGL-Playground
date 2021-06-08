#pragma once

#include <vector>
#include <string>

#include <GL/glew.h>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>		

#include "CommonValues.h"

#include "Mesh.h"
#include "Shader.h"

class Skybox {

public:
	Skybox();

	Skybox(std::vector<std::string> faceLocations);
	void DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);


	~Skybox();

private:

	Mesh* skyMesh;
	Shader* skyShader;

	GLuint textureID;
	GLuint uniformProjection, uniformView;
};

