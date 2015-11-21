#include "Shader.h"
#include "MatrixStack.h"
#include "TriangleSoup.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

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
	GLint location_depthTex;
	GLint location_depthP;
	GLint location_depthMV;

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glDrawBuffer(GL_NONE); // No color buffer is drawn to.

	//Assign the shadow map to texture channel 0 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, lightDepthTex);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, lightDepthTex, 0);

	GLuint depthTexture;
	glGenTextures(1, &depthTexture);
	glBindTexture(GL_TEXTURE_2D, depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB_FLOAT32_ATI, 1024, 1024,
		0, GL_RGB, GL_FLOAT, NULL);
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
		depthTexture, 0);

	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE) {
		printf("FB error, status: 0x%x\n", Status);
		return false;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// CREATE OBJECTS ////////////////////////////////////////////////////////////////////////////////
	Shader ssaoShader;
	ssaoShader.createShader("vertexshader.glsl", "fragmentshader.glsl");
	Shader depthShader;
	depthShader.createShader("depthShaderV.glsl", "depthShaderF.glsl");

	MatrixStack MVstack;
	MVstack.init();

	locationMV = glGetUniformLocation(ssaoShader.programID, "MV");
	locationP = glGetUniformLocation(ssaoShader.programID, "P");
	location_depthTex = glGetUniformLocation(ssaoShader.programID, "depthTex");
	location_depthMV = glGetUniformLocation(depthShader.programID, "MV");
	location_depthP = glGetUniformLocation(ssaoShader.programID, "P");

	float translateVector[3] = { 0.0f, 0.0f, 0.0f };

	float test[3] = { 0.0f, 0.0f, 0.0f };

	TriangleSoup  object;
	object.readOBJ("meshes/trex.obj");

	TriangleSoup  sphere;
	sphere.createSphere(0.3, 20);


	float rot = 0.0;
	//RENDER LOOP /////////////////////////////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(window)) {

		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_A)){
			rot += 0.02;
		}
		if (glfwGetKey(window, GLFW_KEY_D)){
			rot -= 0.02;
		}

		// Create depth map \________________________________________________________________________
		
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lightViewFBO);
		setupViewport(window, P);

		glClear(GL_DEPTH_BUFFER_BIT);
		glUseProgram(depthShader.programID);
		glUniformMatrix4fv(locationP, 1, GL_FALSE, P);

		MVstack.push();
			MVstack.push();
				translateVector[0] = 0.0;
				translateVector[1] = -0.25;
				translateVector[2] = -2.0;
				MVstack.translate(translateVector);
				MVstack.rotY(rot * 3.1415);
				glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
				object.render();
			MVstack.pop();

			MVstack.push();
				translateVector[0] = 1.0;
				translateVector[1] = 0.0;
				translateVector[2] = -2.5;
				MVstack.translate(translateVector);
				glUniformMatrix4fv(location_depthMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
				sphere.render();
			MVstack.pop();
		MVstack.pop();

		glReadBuffer(GL_NONE);

		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		//SCENEGRAPH //////////////////////////////////////////////////////////////////////////////////
		glUseProgram(ssaoShader.programID);
		setupViewport(window, P);
		GLRenderCalls();

		glUniformMatrix4fv(locationP, 1, GL_FALSE, P);

		MVstack.push();
			translateVector[0] = 0.0;
			translateVector[1] = -0.25;
			translateVector[2] = -2.0;
			MVstack.translate(translateVector);
			MVstack.rotY(rot * 3.1415);
			glUniformMatrix4fv(locationMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
			object.render();
		MVstack.pop();

		MVstack.push();
			translateVector[0] = 1.0;
			translateVector[1] = 0.0;
			translateVector[2] = -2.5;
			MVstack.translate(translateVector);
			glUniformMatrix4fv(location_depthMV, 1, GL_FALSE, MVstack.getCurrentMatrix());
			sphere.render();
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