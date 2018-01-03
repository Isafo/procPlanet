#include "Shader.h"
#include "MatrixStack.h"
#include "TriangleSoup.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

// ---- Function dectarations ---- 
void GLRenderCalls();
//! Sets up the GLFW viewport
void setupViewport(GLFWwindow *window, GLfloat *P);

void matrixMult(float* M1, float* M2, float* Mout);
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
	GLint location_depthTex;
	GLint location_depthMVP;
	GLint location_depthBiasMVP;

	float biasMatrix[16] = { 0.5f, 0.0f, 0.0f, 0.0f,
							 0.0f, 0.5f, 0.0f, 0.0f,
							 0.0f, 0.0f, 0.5f, 0.0f,
							 0.5f, 0.5f, 0.5f, 1.0f };


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


	// create and set up the FBO \_________________________________________________________________________
	GLuint lightViewFBO;
	glGenFramebuffers(1, &lightViewFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lightViewFBO);

	GLfloat border[] = { 1.0f, 0.0f, 0.0f, 0.0f };
	
	GLuint lightDepthTex;
	glGenTextures(1, &lightDepthTex);
	glBindTexture(GL_TEXTURE_2D, lightDepthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Assign the shadow map to texture channel 0 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lightDepthTex);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, lightDepthTex, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// CREATE OBJECTS ////////////////////////////////////////////////////////////////////////////////
	Shader planetShader;
	planetShader.createShader("shaders/planetShader.vert", "shaders/planetShader.frag");


	MatrixStack MVstack;
	MVstack.init();

	locationMV = glGetUniformLocation(planetShader.programID, "MV");
	locationP = glGetUniformLocation(planetShader.programID, "P");

	TriangleSoup  object;
	object.createSphere(0.65f, 1024);
	
	float translateVector[3];
	float rot = 0.0;
	float lastTime{ 0.0f };
	float currentTime{ 0.0f };
	float deltaTime{ 0.0f };
	//RENDER LOOP /////////////////////////////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(window)) {
		lastTime = currentTime;
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_A)) {
			rot += 0.3 * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			rot -= 0.3 * deltaTime;
		}

		glUseProgram(planetShader.programID);
		setupViewport(window, P);
		GLRenderCalls();

		glUniformMatrix4fv(locationP, 1, GL_FALSE, P);

		MVstack.push();
			translateVector[0] = 0.0;
			translateVector[1] = 0.0;
			translateVector[2] = -3.0;
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
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST); // Use the Z buffer
	glEnable(GL_CULL_FACE);  // Use back face culling
	glCullFace(GL_BACK);
}

void matrixMult(float* M1, float* M2, float* Mout) {
	// Compute result in a local variable to avoid conflicts
	// with overwriting if Mout is the same variable as either
	// M1 or M2.
	float Mtemp[16];
	int i, j;
	// Perform the multiplication M3 = M1*M2
	// (i is row index, j is column index)
	for (i = 0; i<4; i++) {
		for (j = 0; j<4; j++) {
			Mtemp[i + j * 4] = M1[i] * M2[j * 4] + M1[i + 4] * M2[1 + j * 4]
				+ M1[i + 8] * M2[2 + j * 4] + M1[i + 12] * M2[3 + j * 4];
		}
	}
	// Copy the result to the output variable
	for (i = 0; i<16; i++) {
		Mout[i] = Mtemp[i];
	}
}