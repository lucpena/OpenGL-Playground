#include "Mesh.h"

Mesh::Mesh() {

	VAO = 0;
	VBO = 0;
	IBO = 0;

	indexCount = 0;
}

// Creates the mesh
void Mesh::CreateMesh(GLfloat* vertices, unsigned int* indices, unsigned numOfVertices, unsigned numOfIndices) {

	indexCount = numOfIndices;

	// Creates and Binds the VAO
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// Creates and Binds the IBO
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * numOfIndices, indices, GL_STATIC_DRAW);

	// Creates and Binds the VBO
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * numOfVertices, vertices, GL_STATIC_DRAW);

	// Handlig the first three vertices
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, 0);
	glEnableVertexAttribArray(0);

	// Handling the 4th and 5th attribute from vertices
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void *)(sizeof(vertices[0]) * 3));
	glEnableVertexAttribArray(1);

	// Handling the normals from the vertices
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]) * 8, (void*)(sizeof(vertices[0]) * 5));
	glEnableVertexAttribArray(2);

	// Unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Unbind VAO
	glBindVertexArray(0);

	// Unbind IBO
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

// Render the mesh
void Mesh::RenderMesh() {

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

	// Unbing
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

// Deletes buffer from GPU (garbage collection)
void Mesh::ClearMesh() {
	
	if (IBO != 0) {
		glDeleteBuffers(1, &IBO);
		IBO = 0;
	}

	if (VBO != 0) {
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	if (VAO != 0) {
		glDeleteVertexArrays(1, &VAO);
		VAO = 0;
	}

	indexCount = 0;

}

Mesh::~Mesh() {

	ClearMesh();

}