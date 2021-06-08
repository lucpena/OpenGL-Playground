#include "Shader.h"

Shader::Shader() {

	shaderID = 0;
	uniformModel = 0;
	uniformModel = 0;

	pointLightCount = 0;
	spotLightCount = 0;

}

void Shader::CreateFromString(const char* vertexCode, const char* fragmentCode) {

	CompileShader(vertexCode, fragmentCode);

}

void Shader::CreateFromFile(const char* vertexLocation, const char* fragmentLocation) {

	std::string vertexString = ReadFile(vertexLocation);
	std::string fragmentString = ReadFile(fragmentLocation);

	const char* vertexCode = vertexString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, fragmentCode);

}

void Shader::CreateFromFile(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation) {
	 
	std::string vertexString = ReadFile(vertexLocation);
	std::string geometryString = ReadFile(geometryLocation);
	std::string fragmentString = ReadFile(fragmentLocation);

	const char* vertexCode = vertexString.c_str();
	const char* geometryCode = geometryString.c_str();
	const char* fragmentCode = fragmentString.c_str();

	CompileShader(vertexCode, geometryCode, fragmentCode);

}

void Shader::Validate() {

	// Getting errors
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// Check if the shader is valid
	glValidateProgram(shaderID);
	glGetProgramiv(shaderID, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s' \n\n", eLog);
		return;
	}

}

std::string Shader::ReadFile(const char* fileLocation) {

	std::string content;
	std::ifstream fileStream(fileLocation, std::ios::in);

	if (!fileStream.is_open()) {
		printf("Failed to read %s! File location does not exist :/", fileLocation);
		return "";
	}

	std::string line = "";
	while (!fileStream.eof()){

		std::getline(fileStream, line);
		content.append(line + "\n");

	}

	fileStream.close();
	return content;

}

void Shader::CompileShader(const char* vertexCode, const char* fragmentCode) {

	shaderID = glCreateProgram();

	if (!shaderID) {
		printf("Error creating shader program. \n\n");

		return;
	}

	AddShader(shaderID, vertexCode, GL_VERTEX_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	CompileProgram();

}

void Shader::CompileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode){

	shaderID = glCreateProgram();

	if (!shaderID) {
		printf("Error creating shader program. \n\n");

		return;
	}

	AddShader(shaderID, vertexCode,   GL_VERTEX_SHADER);
	AddShader(shaderID, geometryCode, GL_GEOMETRY_SHADER);
	AddShader(shaderID, fragmentCode, GL_FRAGMENT_SHADER);

	CompileProgram();

}


GLuint Shader::GetProjectionLocation() { return uniformProjection; }

GLuint Shader::GetModelLocation() {	return uniformModel; }

GLuint Shader::GetViewLocation() { return uniformView; }

GLuint Shader::GetAmbientColourLocation() {	return uniformDirectionalLight.uniformColour; }

GLuint Shader::GetAmbientIntensityLocation() { return uniformDirectionalLight.uniformAmbientIntensity; }

GLuint Shader::GetDiffuseIntensityLocation() { return uniformDirectionalLight.uniformDiffuseIntensity; }

GLuint Shader::GetDirectionLocation() { return uniformDirectionalLight.uniformDirection; }

GLuint Shader::GetSpecularIntensityLocation() { return uniformSpecularIntensity; }

GLuint Shader::GetShinenessLocation() { return uniformShineness; }

GLuint Shader::GetEyePositionLocation() { return uniformEyePosition; }

GLuint Shader::GetOmniLightPosLocation() { return uniformOmniLightPos; }

GLuint Shader::GetFarPlaneLocation() { return uniformFarPlane; }



void Shader::SetDirectionalLight(DirectionalLight* dLight){

	dLight->UseLight(uniformDirectionalLight.uniformAmbientIntensity,
		             uniformDirectionalLight.uniformColour,
		             uniformDirectionalLight.uniformDiffuseIntensity,
					 uniformDirectionalLight.uniformDirection);

}

void Shader::SetPointLights(PointLight* pLight, unsigned lightCount, unsigned textureUnit, unsigned offset) {

	// Don't use more than the max number of point lights defined
	if (lightCount > MAX_POINT_LIGHTS) lightCount = MAX_POINT_LIGHTS;

	glUniform1i(uniformPointLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++) {

		pLight[i].UseLight(uniformPointLight[i].uniformAmbientIntensity,
						   uniformPointLight[i].uniformColour,
						   uniformPointLight[i].uniformDiffuseIntensity,
						   uniformPointLight[i].uniformPosition,
						   uniformPointLight[i].uniformConstant,
						   uniformPointLight[i].uniformLinear,
						   uniformPointLight[i].uniformExponent);

		pLight[i].GetShadowMap()->Read(GL_TEXTURE0 + textureUnit + i);  // all GL_TEXTURES are just one aftes another in the memory
		glUniform1i(uniformOmniShadowMap[i + offset].uniformShadowMap, textureUnit + i);
		glUniform1f(uniformOmniShadowMap[i + offset].uniformFarPlane, pLight[i].GetFarPlane());
	
	}

}

void Shader::SetSpotLights(SpotLight* sLight, unsigned lightCount, unsigned textureUnit, unsigned offset) {

	// Don't use more than the max number of spot lights defined
	if (lightCount > MAX_SPOT_LIGHTS) lightCount = MAX_SPOT_LIGHTS;

	glUniform1i(uniformSpotLightCount, lightCount);

	for (size_t i = 0; i < lightCount; i++) {

		sLight[i].UseLight(uniformSpotLight[i].uniformAmbientIntensity,
			uniformSpotLight[i].uniformColour,
			uniformSpotLight[i].uniformDiffuseIntensity,
			uniformSpotLight[i].uniformPosition,
			uniformSpotLight[i].uniformDirection,
			uniformSpotLight[i].uniformConstant,
			uniformSpotLight[i].uniformLinear,
			uniformSpotLight[i].uniformExponent,
			uniformSpotLight[i].uniformEdge);

		
		sLight[i].GetShadowMap()->Read(GL_TEXTURE0 + textureUnit + i);  // all GL_TEXTURES are just one aftes another in the memory
		glUniform1i(uniformOmniShadowMap[i + offset].uniformShadowMap, textureUnit + i);
		glUniform1f(uniformOmniShadowMap[i + offset].uniformFarPlane, sLight[i].GetFarPlane());
	}

}

void Shader::SetTexture(GLuint textureUnit) {

	glUniform1i(uniformTexture, textureUnit);

}

void Shader::SetDirectionalShadowMap(GLuint textureUnit) {

	glUniform1i(uniformDirectionalShadowMap, textureUnit);

}

void Shader::SetDirectionalLightTransform(glm::mat4* lTransform) {

	glUniformMatrix4fv(uniformDirectionalLightTransform, 1, GL_FALSE, glm::value_ptr(*lTransform));

}

void Shader::SetOmniLightMatrices(std::vector<glm::mat4> lightMatrices) {

	for (size_t i = 0; i < 6; i++) {

		glUniformMatrix4fv(uniformLightMatrices[i], 1, GL_FALSE, glm::value_ptr(lightMatrices[i]));

	}

}

void Shader::UseShader() {

	glUseProgram(shaderID);

}

// Deleting program fom the GPU (garbage collection)
void Shader::ClearShader() {

	if (shaderID != 0) {
		glDeleteProgram(shaderID);
		shaderID = 0;
	}

	uniformModel = 0;
	uniformProjection = 0;

}

void Shader::AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {

	// Create empty shader with the type
	GLuint theShader = glCreateShader(shaderType);

	// Parsing the code
	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	// Getting the code lenght
	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	// Getting errors
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(theShader, 1024, NULL, eLog);
		fprintf(stderr, "Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);

}

// Compliles the program
void Shader::CompileProgram() {

	// Variables to get errors
	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	// Link program and check for errors
	glLinkProgram(shaderID);
	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shaderID, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	uniformProjection = glGetUniformLocation(shaderID, "projection");
	uniformModel = glGetUniformLocation(shaderID, "model");
	uniformView = glGetUniformLocation(shaderID, "view");
	uniformDirectionalLight.uniformColour = glGetUniformLocation(shaderID, "directionalLight.base.colour");
	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shaderID, "directionalLight.base.ambientIntensity");
	uniformDirectionalLight.uniformDirection = glGetUniformLocation(shaderID, "directionalLight.direction");
	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shaderID, "directionalLight.base.diffuseIntensity");
	uniformSpecularIntensity = glGetUniformLocation(shaderID, "material.specularIntensity");
	uniformShineness = glGetUniformLocation(shaderID, "material.shineness");
	uniformEyePosition = glGetUniformLocation(shaderID, "eyePosition");

	uniformPointLightCount = glGetUniformLocation(shaderID, "pointLightCount");

	// Setting all Point Lights
	for (int i = 0; i < MAX_POINT_LIGHTS; i++) {

		char locBuffer[100] = { '\0' };

		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].base.colour", i);
		uniformPointLight[i].uniformColour = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].base.ambientIntensity", i);
		uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].base.diffuseIntensity", i);
		uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].position", i);
		uniformPointLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].constant", i);
		uniformPointLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].linear", i);
		uniformPointLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "pointLights[%d].exponent", i);
		uniformPointLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuffer);

	}

	uniformSpotLightCount = glGetUniformLocation(shaderID, "spotLightCount");

	// Setting all Spot Lights
	for (int i = 0; i < MAX_SPOT_LIGHTS; i++) {

		char locBuffer[100] = { '\0' };

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.base.colour", i);
		uniformSpotLight[i].uniformColour = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.base.ambientIntensity", i);
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.base.diffuseIntensity", i);
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.constant", i);
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.linear", i);
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].base.exponent", i);
		uniformSpotLight[i].uniformExponent = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "spotLights[%d].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(shaderID, locBuffer);

	}

	for (size_t i = 0; i < MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS; i++) {

		char locBuffer[100] = { '\0' };

		snprintf(locBuffer, sizeof(locBuffer), "omniShadowMaps[%d].shadowMap", i);
		uniformOmniShadowMap[i].uniformShadowMap = glGetUniformLocation(shaderID, locBuffer);

		snprintf(locBuffer, sizeof(locBuffer), "omniShadowMaps[%d].farPlane", i);
		uniformOmniShadowMap[i].uniformFarPlane = glGetUniformLocation(shaderID, locBuffer);

	}


	uniformDirectionalLightTransform = glGetUniformLocation(shaderID, "directionalLightTransform");
	uniformTexture = glGetUniformLocation(shaderID, "theTexture");
	uniformDirectionalShadowMap = glGetUniformLocation(shaderID, "directionalShadowMap");

	uniformOmniLightPos = glGetUniformLocation(shaderID, "lightPos");
	uniformFarPlane = glGetUniformLocation(shaderID, "farPlane");


	for (size_t i = 0; i < 6; i++) {

		char locBuffer[100] = { '\0' };

		snprintf(locBuffer, sizeof(locBuffer), "lightMatrices[%d]", i);
		uniformLightMatrices[i] = glGetUniformLocation(shaderID, locBuffer);

	}

}


Shader::~Shader() {
	ClearShader();
}