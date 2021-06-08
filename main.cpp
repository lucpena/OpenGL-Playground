/* Definitions */

// Used to load images for textures
#define STB_IMAGE_IMPLEMENTATION

/* Includes */

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>

// GLEW includes
#include <GL/glew.h>

// GLFW includes
#include <GLFW/glfw3.h>

// GLM includes
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>			               // New Imatrix initialization -> glm::mat4 model(1.0f);

// Common Values
#include "CommonValues.h"

// Custom classes
#include "Window.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"
#include "Skybox.h"


/* Code */

// Window dimension
const float toRadians = 3.14159265f / 180.0f;
GLuint shader;

Window mainWindow;

std::vector<Mesh*>	meshList;

std::vector<Shader> shaderList;
Shader directionalShadowShader;
Shader omniShadowShader;

// Camera
Camera camera;

// Lights
DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

unsigned pointLightCount = 0;
unsigned spotLightCount = 0;

// Skybox
Skybox skybox;

// Textures
Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture ground1Texture;
Texture sandTexture;

// Materials
Material shinyMaterial;
Material dullMaterial;

// 3D models
Model moai;
Model ferrari;
Model xingcai;
Model house;

// Delta Time
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Uniforms
GLuint uniformProjection = 0;
GLuint uniformModel = 0;
GLuint uniformView = 0;
GLuint uniformEyePosition = 0;
GLuint uniformSpecularIntensity = 0;
GLuint uniformShineness = 0;
GLuint uniformDirectionalLightTransform = 0;
GLuint uniformOmniLightPos = 0;
GLuint uniformFarPlane = 0;

// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void calcAverageNormals(unsigned* indices, unsigned indiceCount, GLfloat* vertices, unsigned verticeCount, unsigned vLenght, unsigned normalOffset) {

	for (size_t i = 0; i < indiceCount; i += 3) {

		unsigned in0 = indices[i] * vLenght;
		unsigned in1 = indices[i + 1] * vLenght;
		unsigned in2 = indices[i + 2] * vLenght;

		glm::vec3 v1(vertices[in1] - vertices[in0], 
			         vertices[in1 + 1] - vertices[in0 + 1], 
			         vertices[in1 + 2] - vertices[in0 + 2]);

		glm::vec3 v2(vertices[in2] - vertices[in0],
			         vertices[in2 + 1] - vertices[in0 + 1],
			         vertices[in2 + 2] - vertices[in0 + 2]);

		glm::vec3 normal = glm::cross(v1, v2);

		normal = glm::normalize(normal);

		in0 += normalOffset;
		in1 += normalOffset;
		in2 += normalOffset;

		vertices[in0]     += normal.x;
		vertices[in0 + 1] += normal.y;
		vertices[in0 + 2] += normal.z;

		vertices[in1]     += normal.x;
		vertices[in1 + 1] += normal.y;
		vertices[in1 + 2] += normal.z;

		vertices[in2]     += normal.x;
		vertices[in2 + 1] += normal.y;
		vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLenght; i++ ) {

		unsigned nOffset = i * vLenght + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);

		vertices[nOffset]     = vec.x;
		vertices[nOffset + 1] = vec.y;
		vertices[nOffset + 2] = vec.z;

	}

}

// Creating the objects
void CreateObjects() {

	unsigned indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
	//    x  ,   y  ,  z  ,   Tu  ,  Tv (Tex)    nx , ny  ,  nz (normals) 
		-1.0f, -1.0f, -0.6f,   0.0f, 0.0f,       0.0f, 0.0f, 0.0f,
		 0.0f, -1.0f,  1.0f,   0.5f, 0.0f,       0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, -0.6f,   1.0f, 0.0f,       0.0f, 0.0f, 0.0f,
		 0.0f,  1.0f,  0.0f,   0.5f, 1.0f,       0.0f, 0.0f, 0.0f
	};

	unsigned floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-100.0f, 0.0f, -100.0f,   0.0f,  0.0f,     0.0f, -1.0f,  0.0f,
		 100.0f, 0.0f, -100.0f,  100.0f,  0.0f,     0.0f, -1.0f,  0.0f,
		-100.0f, 0.0f,  100.0f,   0.0f, 100.0f,     0.0f, -1.0f,  0.0f,
		 100.0f, 0.0f,  100.0f,  100.0f, 100.0f,     0.0f, -1.0f,  0.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh* obj = new Mesh();
	Mesh* obj2 = new Mesh();
	Mesh* floor = new Mesh();

	obj->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj);

	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	floor->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(floor);
}

void CreateShader() {

	Shader* shader1 = new Shader();
	shader1->CreateFromFile(vShader, fShader);
	shaderList.push_back(*shader1);

	directionalShadowShader = Shader();
	directionalShadowShader.CreateFromFile("Shaders/directional_shadow_map.vert", "Shaders/directional_shadow_map.frag");
	omniShadowShader.CreateFromFile("Shaders/omni_shadow_map.vert", "Shaders/omni_shadow_map.geom", "Shaders/omni_shadow_map.frag");

}

void RenderScene() {

	// Identity matrix for the meshes
	glm::mat4 model = glm::mat4(1.0f);

	// Rendering the floor
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	sandTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShineness);
	meshList[2]->RenderMesh();

	// Rendering Pyramid 1
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(5.0f, -1.0f, -3.0f));
	//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, -1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	plainTexture.UseTexture();
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShineness);
	//meshList[0]->RenderMesh();

	// Rendering Pyramid 2
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(1.0f, -1.0f, -3.0f));
	//model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.0f, 0.0f, 0.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	brickTexture.UseTexture();
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShineness);
	//meshList[1]->RenderMesh();

	// Rendering Moai
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(10.0f, -2.55f, -6.0f));
	model = glm::rotate(model, 270 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, 45 * toRadians, glm::vec3(0.0f, 0.0f, -1.0f));
	model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShineness);
	moai.RenderModel();

	// Rendering F1 Ferrari
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(-10.0f, -2.0f, -6.5f));
	model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformShineness);
	ferrari.RenderModel();

	// Rendering xingcai
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
	//model = glm::rotate(model, 270 * toRadians, glm::vec3(0.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShineness);
	xingcai.RenderModel();

	// Rendering the house
	model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -2.0f, -10.0f));
	model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
	//model = glm::scale(model, glm::vec3(0.015f, 0.015f, 0.015f));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
	dullMaterial.UseMaterial(uniformSpecularIntensity, uniformShineness);
	house.RenderModel();
}

void DirectionalShadowMapPass(DirectionalLight* light) {

	directionalShadowShader.UseShader();
	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());

	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = directionalShadowShader.GetModelLocation();

	directionalShadowShader.SetDirectionalLightTransform(&light->CalculateLightTransform());

	directionalShadowShader.Validate();

	// Rendering the scene with the props
	RenderScene();

	// Unbinding
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OmniShadowMapPass(PointLight* light) {

	omniShadowShader.UseShader();

	glViewport(0, 0, light->GetShadowMap()->GetShadowWidth(), light->GetShadowMap()->GetShadowHeight());
	
	light->GetShadowMap()->Write();
	glClear(GL_DEPTH_BUFFER_BIT);

	uniformModel = omniShadowShader.GetModelLocation();
	uniformOmniLightPos = omniShadowShader.GetOmniLightPosLocation();
	uniformFarPlane = omniShadowShader.GetFarPlaneLocation();

	glUniform3f(uniformOmniLightPos, light->GetPosition().x, light->GetPosition().y, light->GetPosition().z);
	glUniform1f(uniformFarPlane, light->GetFarPlane());
	omniShadowShader.SetOmniLightMatrices(light->CalculateLightTransform());

	omniShadowShader.Validate();

	// Rendering the scene with the props
	RenderScene();

	// Unbinding
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void RenderPass(glm::mat4 projectionMatrix, glm::mat4 viewMatrix) {

	glViewport(0, 0, 1366, 768);

	// Clear window
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Drawing the Skybox
	skybox.DrawSkybox(viewMatrix, projectionMatrix);

	// Setting up the Uniforms
	shaderList[0].UseShader();
	uniformModel = shaderList[0].GetModelLocation();
	uniformProjection = shaderList[0].GetProjectionLocation();
	uniformView = shaderList[0].GetViewLocation();
	uniformEyePosition = shaderList[0].GetEyePositionLocation();
	uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
	uniformShineness = shaderList[0].GetShinenessLocation();

	// Setting the matrices for the camera
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
	glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


	// Setting up the Light
	shaderList[0].SetDirectionalLight(&mainLight);
	shaderList[0].SetPointLights(pointLights, pointLightCount, 3, 0);
	shaderList[0].SetSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
	shaderList[0].SetDirectionalLightTransform(&mainLight.CalculateLightTransform());

	mainLight.GetShadowMap()->Read(GL_TEXTURE2);
	shaderList[0].SetTexture(1);
	shaderList[0].SetDirectionalShadowMap(2);

	// Flashlight
	glm::vec3 lowerLight = camera.getCameraPosition();
	lowerLight.y -= 0.3f;
	spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

	shaderList[0].Validate();

	RenderScene();

}

int main()
{
	// Window Initialization
	mainWindow = Window(1366, 768);
	mainWindow.Initialise();

	// Initialization
	CreateObjects();
	CreateShader();

	// Defining the Camera
	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 0.0f, 5.0f, 0.5f);

	// Textures
	brickTexture = Texture((char*)("Textures/brick.png"));
	brickTexture.LoadTextureA();

	dirtTexture = Texture((char*)("Textures/dirt.png"));
	dirtTexture.LoadTextureA();

	plainTexture = Texture((char*)("Textures/plain.png"));
	plainTexture.LoadTextureA();

	ground1Texture = Texture((char*)("Textures/ground_01.png"));
	ground1Texture.LoadTextureA();

	sandTexture = Texture((char*)("Textures/sand.jpg"));
	sandTexture.LoadTextureA();

	// Materials
	shinyMaterial = Material(4.0f, 256);
	dullMaterial = Material(0.3f, 4);

	//Models
	moai = Model();
	moai.LoadModel("Models/12329_Statue_v1_l3.obj");

	ferrari = Model();
	ferrari.LoadModel("Models/Ferrari.obj");

	xingcai = Model();
	xingcai.LoadModel("Models/xingcai.obj");

	house = Model();
	house.LoadModel("Models/house.obj");

	// Lights
	// Directional Ligh
	mainLight = DirectionalLight(8192, 8192,				// Shadow Buffer Size
								 1.0f, 0.53f, 0.3f,			// Color
								 0.1f, 0.9f,				// Ambient and Diffuse Intensity
							     -10.0f, -12.0f, 18.5f);	// Direction




	//// HOUSE - right side
	//pointLights[0] = PointLight(1024, 1024,             // Shadow Width and Height
	//							0.1f, 100.0f,			// Near and Far planes for Shadow
	//							1.0f, 1.0f, 1.0f,		// Color
	//							0.5f, 1.0f,		        // Ambient and Diffuse Intensity
	//							9.0f, 2.5f, -9.0f,		// Position
	//							0.3f, 0.2f, 0.1f);		// Constant, Linear and Exponent
	//pointLightCount++;

	//HOUSE - Backyard
	pointLights[0] = PointLight(1024, 1024,             // Shadow Width and Height
								0.1f, 100.0f,			// Near and Far planes for Shadow
								1.0f, 1.0f, 1.0f,		// Color
								0.5f, 1.0f,		        // Ambient and Diffuse Intensity
								1.5f, 0.8f, -15.5f,		// Position
								0.3f, 0.2f, 0.1f);		// Constant, Linear and Exponent
	pointLightCount++;

	// HOUSE - front door 
	pointLights[1] = PointLight(1024, 1024,             // Shadow Width and Height
								0.1f, 100.0f,			// Near and Far planes for Shadow
								1.0f, 1.0f, 1.0f,		// Color
								0.4f, 1.0f,		        // Ambient and Diffuse Intensity
								-0.7f, 0.45f, -5.0f,		// Position
								0.5f, 0.2f, 0.1f);		// Constant, Linear and Exponent
	pointLightCount++;

	// Spot Lights
	// Xingcai
	spotLights[0] = SpotLight(1024, 1024,           // Shadow Width and Height
					   		  0.1f, 100.0f,			// Near and Far planes for Shadow
							  1.0f, 1.0f, 1.0f,	    // Color
						      0.45f, 0.45f,			// Ambient and Diffuse Intensity
						      0.0f, 0.0f, 0.0f,	    // Position
	                       	  0.0f, -10.0f, 0.0f,   // Direction
						      0.3f, 0.1f, 0.05f,		// Constant, Linear and Exponent
		                      30.0f);				// Edge
	spotLightCount++;

	//// Ferrari
	spotLights[1] = SpotLight(1024, 1024,           // Shadow Width and Height
						      0.1f, 100.0f,			// Near and Far planes for Shadow
						 	  1.0f,  0.0f, 0.0f,	// Color
							  0.5f,  0.5f,			// Ambient and Diffuse Intensity
							 -9.0f, -2.0f, -6.5f,	    // Position
						      0.0f,  -1.0f, -1.0f,	// Direction
							  0.3f,  0.1f, 0.05f,	// Constant, Linear and Exponent
							  600.0f);				// Edge
	spotLightCount++;

	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	glm::mat4 projection = glm::perspective(glm::radians(60.0f), mainWindow.getBufferWidth()/mainWindow.getBufferHeight(), 0.1f, 100.0f);


	// Loop until window close
	while (!mainWindow.getShouldClose()) {
		// Delta Time setup
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		// Get + Handle user input events
		glfwPollEvents();

		// Handles camera controls
		camera.keyControl(mainWindow.getKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		if (mainWindow.getKeys()[GLFW_KEY_F]) {

			spotLights[0].Toggle();
			mainWindow.getKeys()[GLFW_KEY_F] = false;
		}

		// Calling the rendering scene with shadow
		DirectionalShadowMapPass(&mainLight);
		
		for (size_t i = 0; i < pointLightCount; i++ ) {

			OmniShadowMapPass(&pointLights[i]);

		}

		for (size_t i = 0; i < spotLightCount; i++) {

			OmniShadowMapPass(&spotLights[i]);

		}

		RenderPass(projection, camera.calculateViewMatrix());
		
		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}