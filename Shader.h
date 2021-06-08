#pragma once

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

#include "CommonValues.h"

#include <GL\glew.h>

#include <GLM\glm.hpp>
#include <GLM\gtc\matrix_transform.hpp>
#include <GLM\gtc\type_ptr.hpp>

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Shader {

public:

	Shader();

	void CreateFromString(const char* vertexCode, const char* fragmentCode);
	void CreateFromFile(const char* vertexLocation, const char* fragmentLocation);
	void CreateFromFile(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation);

	void Validate();

	std::string ReadFile(const char* fileLocation);

	GLuint GetProjectionLocation();
	GLuint GetModelLocation();
	GLuint GetViewLocation();
	GLuint GetAmbientIntensityLocation();
	GLuint GetAmbientColourLocation();
	GLuint GetDiffuseIntensityLocation();
	GLuint GetDirectionLocation();
	GLuint GetSpecularIntensityLocation();
	GLuint GetShinenessLocation();
	GLuint GetEyePositionLocation();
	GLuint GetOmniLightPosLocation();
	GLuint GetFarPlaneLocation();

	void SetDirectionalLight(DirectionalLight* dLight);
	void SetPointLights(PointLight* pLight, unsigned lightCount, unsigned textureUnit, unsigned offset);
	void SetSpotLights(SpotLight* pLight, unsigned lightCount, unsigned textureUnit, unsigned offset);
	void SetTexture(GLuint textureUnit);
	void SetDirectionalShadowMap(GLuint textureUnit);
	void SetDirectionalLightTransform(glm::mat4* lTransform);
	void SetOmniLightMatrices(std::vector<glm::mat4> lightMap);

	void UseShader();
	void ClearShader();

	~Shader();

private:

	int pointLightCount;
	int spotLightCount;

	GLuint shaderID,
		uniformProjection,
		uniformModel,
		uniformView,
		uniformEyePosition,
		uniformSpecularIntensity,
		uniformShineness,
		uniformTexture,
		uniformDirectionalLightTransform,
		uniformDirectionalShadowMap,
		uniformOmniLightPos,
		uniformFarPlane;

	GLuint uniformLightMatrices[6];

	// Directional Light Struct
	struct {

		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;

	} uniformDirectionalLight;



	// Point Light Struct
	GLuint uniformPointLightCount;
	struct {

		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

	} uniformPointLight[MAX_POINT_LIGHTS];

	// Spot Light Struct
	GLuint uniformSpotLightCount;
	struct {

		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

		GLuint uniformDirection;
		GLuint uniformEdge;
	} uniformSpotLight[MAX_SPOT_LIGHTS];

	struct {

		GLuint uniformShadowMap;
		GLuint uniformFarPlane;

	} uniformOmniShadowMap[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

	void CompileShader(const char* vertexCode, const char* fragmentCode);
	void CompileShader(const char* vertexCode,const char* geometryCode, const char* fragmentCode);
	void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);

	void CompileProgram();
};

