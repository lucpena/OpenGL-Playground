#include "Skybox.h"

Skybox::Skybox() {



}

Skybox::Skybox(std::vector<std::string> faceLocations) {

	// Shader setup
	skyShader = new Shader();
	skyShader->CreateFromFile("Shaders/skybox.vert", "Shaders/skybox.frag");

	uniformProjection = skyShader->GetProjectionLocation();
	uniformView = skyShader->GetViewLocation();


	// Texture setup
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, bitDepth;

	for (size_t i = 0; i < 6; i++) {

		unsigned char* texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, STBI_rgb_alpha); // Last is 0

		if (!texData) {

			printf("Error finding %s. \n", faceLocations[i].c_str());
			return;

		}

		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
		stbi_image_free(texData);

	}

	// Defining how the texture will act
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_MIRROED_REPEAT, GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // GL_CLAMP_TO_BORDER

	// Defining filters to mipmap
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR -> blur effect 
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST -> pixelated

	
	// Mesh setup
	unsigned int skyboxIndices[] = {
		// Front
		0, 1, 2,
		2, 1, 3,
		// Right
		2, 3, 5,
		5, 3, 7,
		// Back
		5, 7, 4,
		4, 7, 6,
		// Left
		4, 6, 0,
		0, 6, 1,
		// Top
		4, 0, 5,
		5, 0, 2,
		// Bottom
		1, 6, 3,
		3, 6, 7
	};

	float skyboxVertices[] = {
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};

	skyMesh = new Mesh();
	skyMesh->CreateMesh(skyboxVertices, skyboxIndices, 64, 36);

}

void Skybox::DrawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix) {

	viewMatrix = glm::mat4(glm::mat3(viewMatrix));

	// Disabling the depth field so we can se trhough the skybox
	glDepthMask(GL_FALSE);

	skyShader->UseShader();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	skyShader->Validate();

	skyMesh->RenderMesh();

	// Turning it on again
	glDepthMask(GL_TRUE);

}

Skybox::~Skybox() {



}
