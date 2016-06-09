#pragma once
#include "GL/glew.h"

class Plane
{
public:
	Plane()
	{
		vao = 0;
		vertexbuffer = 0;
		indexbuffer = 0;
		vertexarray = nullptr;
		indexarray = nullptr;
		nverts = 0;
		ntris = 0;

		position[0] = 0.0f;
		position[1] = 0.0f;
		position[2] = 0.0f;
	};

	Plane(float x, float y, float z, float dX, float dZ);
	~Plane(void);

	void render();

private:
	GLuint vao;          // Vertex array object, the main handle for geometry
	int nverts; // Number of vertices in the vertex array
	int ntris;  // Number of triangles in the index array (may be zero)
	GLuint vertexbuffer; // Buffer ID to bind to GL_ARRAY_BUFFER
	GLuint indexbuffer;  // Buffer ID to bind to GL_ELEMENT_ARRAY_BUFFER
	GLfloat *vertexarray; // Vertex array on interleaved format: x y z nx ny nz s t
	GLuint *indexarray;   // Element index array

	float position[4];


	void createPlane(float xSize, float zSize);
};