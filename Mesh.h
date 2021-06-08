#pragma once

#include <GL/glew.h>

class Mesh {

public:

	Mesh();

	void CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned numOfVertices, unsigned numOfIndices);
	void RenderMesh();
	void ClearMesh();

	~Mesh();


private:

	// VAO - Vertex Array Object
	// VBO - Vertex Buffer Object
	GLuint  VAO, VBO, IBO;
	GLsizei	indexCount;

};

