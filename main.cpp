#include "Shader.h"
#include "MatrixStack.h"
#include "TriangleSoup.hpp"

// ---- Function dectarations ---- 
void GLRenderCalls();
//! Sets up the GLFW viewport
void setupViewport(GLFWwindow *window, GLfloat *P);
// --------------------------------

int main() {
	GLfloat I[16] = { 1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat P[16] = { 2.42f, 0.0f, 0.0f, 0.0f,
		0.0f, 2.42f, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, -1.0f,
		0.0f, 0.0f, -0.2f, 0.0f };
	GLint locationP;
	GLint locationMV;

	float translation[3];

	//GL INITIALIZATION ////////////////////////////////////////////////////////////////////////////////
	// start GLEW extension handler
	if (!glfwInit()) {
		fprintf(stderr, "ERROR: could not start GLFW3\n");
		return 1;
	}

	//create GLFW window and select context
	GLFWwindow* window = glfwCreateWindow(640, 480, "hej", NULL, NULL);
	if (!window) {
		fprintf(stderr, "ERROR: could not open window with GLFW3\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);

	//start GLEW extension handler
	glewExperimental = GL_TRUE;
	glewInit();

	// get version info
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	printf("Renderer: %s\n", renderer);
	printf("OpenGL version supported %s\n", version);

	// CREATE OBJECTS ////////////////////////////////////////////////////////////////////////////////
	Shader ssaoShader;
	ssaoShader.createShader("vertexshader.glsl", "fragmentshader.glsl");

	MatrixStack MVstack;
	MVstack.init();

	locationMV = glGetUniformLocation(ssaoShader.programID, "MV");
	locationP = glGetUniformLocation(ssaoShader.programID, "P");

	float translateVector[3] = { 0.0f, 0.0f, 0.0f };

	float test[3] = { 0.0f, 0.0f, 0.0f };

	TriangleSoup  object;
	//object.createSphere(0.5, 150);
	object.readOBJ("meshes/trex.obj");

	float rot = 0.0;
	//RENDER LOOP /////////////////////////////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(window)) {

		//NECESSARY RENDER CALLS /////////////////////////////////////////////////////////////////////
		//modifyMesh(&mTest, window, mouse);
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_A)){
			rot += 0.02;
		}
		if (glfwGetKey(window, GLFW_KEY_D)){
			rot -= 0.02;
		}

		GLRenderCalls();

		glUseProgram(ssaoShader.programID);

		setupViewport(window, P);
		glUniformMatrix4fv(locationP, 1, GL_FALSE, P);

		//SCENEGRAPH //////////////////////////////////////////////////////////////////////////////////
		//camera transforms 
		MVstack.push();
			translateVector[0] = 0.0;
			translateVector[1] = -0.25;
			translateVector[2] = -2.0;
			MVstack.translate(translateVector);
			MVstack.rotY(rot * 3.1415);
			glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
			object.render();
		MVstack.pop();

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void setupViewport(GLFWwindow *window, GLfloat *P) {
	int width, height;

	glfwGetWindowSize(window, &width, &height);

	P[0] = P[5] * height / width;

	glViewport(0, 0, width, height);
}

void GLRenderCalls() {
	// Set the clear color and depth, and clear the buffers for drawing
	glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST); // Use the Z buffer
	glEnable(GL_CULL_FACE);  // Use back face culling
	glCullFace(GL_BACK);
}