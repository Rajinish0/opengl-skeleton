/*
* FOR EXPLORING INSTANCING
*/


#include <iostream>
#include "shader.h"
#include "camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#endif
#include "stb_image.h"
#include "funcs.h"
#include "window.h"
#include "model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "model.h"
#include "animation.h"
#include "animator.h"
#include "audio_manager.h"
#include <ft2build.h> // checking if build was good


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);


float vertices[] = {
	// positions          // texture Coords
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

float planeVertices[] = {
	// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
	 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};


/*
* just realized this is kind of stupid, because
* i could have used planeVertices and passed 6 instead of
* 36 in glDrawArrays
*/
float grassVertices[]{
	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
	 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
	 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f
};

float quadvertices[]{
	-1.0f, -1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, 1.0f, 0.0f,
	 1.0f,  1.0f, 1.0f, 1.0f,
	 1.0f,  1.0f, 1.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f
};

float simpleQuad[]{
	-0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
	 0.5f,-0.5f, 0.0f, 1.0f, 0.0f,
	-0.5f,-0.5f, 0.0f, 0.0f, 1.0f,


	-0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
	 0.5f,-0.5f, 0.0f, 1.0f, 0.0f,
	 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
};

unsigned int indicies[] = {
	0, 1, 2,
	0, 2, 3
};

glm::vec3 cubePositions[] = {
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
};



unsigned int cubeVBO, cubeVAO, planeVAO, planeVBO,
grassVAO, grassVBO, fbo, rbo, quadVAO, uboBlock,
quadVBO, squareVAO, squareVBO, instanceVBO,
asteroidVBO;
unsigned int vertexShader;
unsigned int fragShader;
unsigned int texture, texture2,
grassTexture, fbtexture;
float dt = 0.007f;
float lastTime = NULL;

Camera cam;
std::vector<glm::vec3> vegetation;




int main() {
	// stbi_set_flip_vertically_on_load(true);

	Window window(800, 600);


	glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window.window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initliaze GLAD" << std::endl;
		return -1;
	}

	Shader animShader {"shaders/anim_vertex.glsl", "shaders/anim_frag.glsl"};

	// Model planet{"models/planet/planet.obj"};
	// Model asteriod{ "models/rock/rock.obj" };


	glEnable(GL_DEPTH_TEST);


	glViewport(0, 0, 800, 600);

	glm::mat4 trans(1.0f);
	glm::mat4 model(1.0f);
	glm::mat4 lmodel(1.0f);
	glm::mat4 view(1.0f);
	glm::mat4 proj(1.0f);
	glm::vec3 lpos(1.2f, 0.0f, 1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 200.0f);


	Model animModel("models/salsa2/salsa_dancing.dae");
	Animation danceAnimation("models/salsa2/salsa_dancing.dae", &animModel);
	Animator animator(&danceAnimation);
	std::cout << "ANIMATOR LOADED" << std::endl;

	float dT = 0.0f, 
		  lF = 0.0f;

	while (!window.shouldClose())
	{
		processInput(window.window);
		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		view = cam.getView();

		float cF = glfwGetTime();
		dT = cF - lF;
		lF = cF;

		animShader.use();
		animShader.setMatrix("proj", proj);
		animShader.setMatrix("view", view);
		animator.updateAnimation(dT);
		auto transforms = animator.getFinalBoneMatrices();
		for (int i =0; i < transforms.size(); ++i){
			animShader.setMatrix("finalBonesMatrices["+
				std::to_string(i) + "]", transforms[i] );
		}

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, -0.4f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5));
		animShader.setMatrix("model", model);
		animModel.draw(animShader);


		window.update();

	}

	glfwTerminate();
	return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{

	if (!lastTime) {
		lastTime = glfwGetTime();
		return;
	}

	float cTime = glfwGetTime();
	dt = (cTime - lastTime) * 10;
	lastTime = cTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.move(Camera::UP, dt);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.move(Camera::DOWN, dt);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.move(Camera::RIGHT, dt);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.move(Camera::LEFT, dt);

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		cam.yaw -= cam.sensitivity;
		cam.updateDirection();
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		cam.yaw += cam.sensitivity;
		cam.updateDirection();
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		//cam.pitch += cam.sensitivity;
		cam.incPitch(cam.sensitivity);
		cam.updateDirection();
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		cam.incPitch(-cam.sensitivity);
		//cam.pitch -= cam.sensitivity;
		cam.updateDirection();
	}
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	cam.handleMouse(xPos, yPos);
}
