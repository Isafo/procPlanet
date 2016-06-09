#include "Plane.h"

Plane::Plane(float x, float y, float z, float dX, float dZ) {
	position[0] = x;
	position[1] = y;
	position[2] = z;
	createPlane(dX, dZ);
}


Plane::~Plane(void) 
{
}

void Plane::createPlane(float dX, float dZ) {

	GLfloat vertex_array_data[] = {
		//		Vertex										Normals						Texture  
		-dX / 2.0f,  0.0f,  -dZ / 2.0f,						0.0f, 1.0f, 0.0f,			0, 1,	
		 dX / 2.0f,  0.0f,  -dZ / 2.0f,						0.0f, 1.0f, 0.0f,			0, 0,	
		 dX / 2.0f,  0.0f,   dZ / 2.0f,						0.0f, 1.0f, 0.0f,			1, 0,	
		-dX / 2.0f,  0.0f,   dZ / 2.0f,						0.0f, 1.0f, 0.0f,			1, 1,	 
	};

	static const GLuint index_array_data[] = {
		2, 1, 0,
		2, 3, 0	
	};

	nverts = 4;
	ntris = 2;

	vertexarray = new GLfloat[8 * nverts]; // coordinates, normals and texture coordinates
	indexarray = new GLuint[3 * ntris];

	for (int i = 0; i < 8 * nverts; i++) {
		vertexarray[i] = vertex_array_data[i];
	}

	for (int i = 0; i < 3 * ntris; i++) {
		indexarray[i] = index_array_data[i];
	}

	// Generate one vertex array object (VAO) and bind it
	glGenVertexArrays(1, &(vao));
	glBindVertexArray(vao);

	// Generate two buffer IDs
	glGenBuffers(1, &vertexbuffer);
	glGenBuffers(1, &indexbuffer);

	// Activate the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Present our vertex coordinates to OpenGL
	glBufferData(GL_ARRAY_BUFFER,
		8 * nverts * sizeof(GLfloat), vertexarray, GL_STATIC_DRAW);

	// Specify how many attribute arrays we have in our VAO
	glEnableVertexAttribArray(0); // Vertex coordinates
	glEnableVertexAttribArray(1); // Normals
	glEnableVertexAttribArray(2); // Texture coordinates

	// Specify how OpenGL should interpret the vertex buffer data:
	// Attributes 0, 1, 2 (must match the lines above and the layout in the shader)
	// Number of dimensions (3 means vec3 in the shader, 2 means vec2)
	// Type GL_FLOAT
	// Not normalized (GL_FALSE)
	// Stride 8 floats (interleaved array with 8 floats per vertex)
	// Array buffer offset 0, 3 or 6 floats (offset into first vertex)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GLfloat), (void*)0); // xyz coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat))); // normals
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
		8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat))); // texcoords

	// Activate the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexbuffer);
	// Present our vertex indices to OpenGL
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		3 * ntris * sizeof(GLuint), indexarray, GL_STATIC_DRAW);

	// Deactivate (unbind) the VAO and the buffers again.
	// Do NOT unbind the index buffer while the VAO is still bound.
	// The index buffer is an essential part of the VAO state.
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Plane::render() {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, nverts, GL_UNSIGNED_INT, (void*)0);
	// (mode, vertex count, type, element array buffer offset)
	glBindVertexArray(0);
}