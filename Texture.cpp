#include "Texture.h"

Texture::Texture() {

	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = nullptr;;

}

Texture::Texture(const char* fileLoc) {

	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = fileLoc;

}

bool Texture::LoadTexture() {

	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, 0); // Last is 0 STBI_rgb_alpha

	if (!texData) {

		printf("Error finding %s. \n", fileLocation);
		return false;

	}

	// Bind the Texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Defining how the texture will act
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_MIRROED_REPEAT, GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP_TO_BORDER

	// Defining filters to mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR -> blur effect 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST -> pixelated

	// Loading data 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData); // Change to RBG (gambiarra)
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unbind the texture and freeing the memory
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);

	return true;
}


bool Texture::LoadTextureA() {

	unsigned char* texData = stbi_load(fileLocation, &width, &height, &bitDepth, STBI_rgb_alpha);

	if (!texData) {

		printf("Error finding %s. \n", fileLocation);
		return false;

	}

	// Bind the Texture
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Defining how the texture will act
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // GL_MIRROED_REPEAT, GL_CLAMP_TO_EDGE
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT); // GL_CLAMP_TO_BORDER

	// Defining filters to mipmap
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); // GL_LINEAR -> blur effect 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // GL_NEAREST -> pixelated

	// Loading data 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unbind the texture and freeing the memory
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(texData);

	return true;
}

void Texture::UseTexture() {

	// GL_TEXTURE0 -> Texture Unit
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureID);

}

void Texture::ClearTexture() {

	glDeleteTextures(1, &textureID);
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";

}

Texture::~Texture() {

	ClearTexture();

}