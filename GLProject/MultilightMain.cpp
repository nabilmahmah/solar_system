#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "tools/Polygon.h"
#include "tools/Cube.h"
#include <learnopengl/shader_m.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include "stb_image.h"
#include <random> 
#include <chrono>

using namespace std;
using namespace glm;

int SCR_WIDTH = 1280;
int SCR_HEIGHT = 720;

Camera camera(vec3(0.0f, 0.0f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

vec3 firstLight(0.0f, -1.5f, 0.0f);
vec3 lightPos(0.0f, -1.5f, 0.0f);

bool blinn = false;


void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);

vector<GLuint> loadTextures(vector<string> paths, GLuint wrapOption = GL_REPEAT, GLuint filterOption = GL_LINEAR) {
	vector<GLuint> textures = {};

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterOption);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterOption);

	for (string path : paths)
	{
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);


		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			std::cout << "Failed to load texture" << std::endl;
		stbi_image_free(data);

		textures.push_back(texture);
	}

	return textures;
}

int main()
{
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lecture 5", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	Shader allShader("./shaders/vs/L5.vs", "./shaders/fs/L5-Multi.fs");
	Shader lightSourceShader("./shaders/vs/LightSource.vs", "./shaders/fs/LightSource.fs");

	Model sun("./models/planet.glb");
	Model moon("./models/planet.glb");
	Model earth("./models/planet.glb");
	Cube plainBall(vec3(0.0,0.0,0.0),0.2,vec3(1.0,0.0,0.0));
	glEnable(GL_DEPTH_TEST);

	stbi_set_flip_vertically_on_load(true);

	vector<string> texturePaths = {};
	texturePaths.push_back("./textures/white.jpg");
	vector<GLuint> textures = loadTextures(texturePaths);
 
	while (!glfwWindowShouldClose(window))
	{

		allShader.use();
		allShader.setVec3("viewPos", camera.Position);

		mat4 trans = mat4(1.0f);
		float theta = (float)glfwGetTime();
		trans = glm::rotate(trans,theta , vec3(0.0f, 1.0f, 0.0f));
		trans = glm::translate(trans, vec3(2.0f,0.0f,0.0f));

		mat4 moontranslet = trans;
		#pragma region Light Settings
		// Directional light
		/*allShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		allShader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
		allShader.setVec3("dirLight.diffuse", 0.05f, 0.05f, 0.05f);
		allShader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
		*/
		// sun
		allShader.setVec3("pointLights[0].position", vec3(0,0,0));
		allShader.setVec3("pointLights[0].ambient", vec3(1,1,1));
		allShader.setVec3("pointLights[0].diffuse", vec3(1,1,1));
		allShader.setVec3("pointLights[0].specular", vec3(0.1,0.1,0.1));
		allShader.setFloat("pointLights[0].constant", 1.0f);
		allShader.setFloat("pointLights[0].linear", 0.14f);
		allShader.setFloat("pointLights[0].quadratic", 0.07f);
		// moon
		allShader.setVec3("pointLights[1].position", moontranslet[3]);
		allShader.setVec3("pointLights[1].ambient", vec3(0,0,0));
		allShader.setVec3("pointLights[1].diffuse", vec3(0.2,0.2,0.2));
		allShader.setVec3("pointLights[1].specular", vec3(0.2, 0.2, 0.2));
		allShader.setFloat("pointLights[1].constant", 1.0f);
		allShader.setFloat("pointLights[1].linear", 0.14f);
		allShader.setFloat("pointLights[1].quadratic", 0.07f);
		// SpotLight
		/*allShader.setVec3("spotLight.position", camera.Position);
		allShader.setVec3("spotLight.direction", camera.Front);
		allShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		allShader.setVec3("spotLight.diffuse", 0,0,0);
		allShader.setVec3("spotLight.specular", 0,0,0);
		allShader.setFloat("spotLight.constant", 1.0f);
		allShader.setFloat("spotLight.linear", 0.09f);
		allShader.setFloat("spotLight.quadratic", 0.032f);
		allShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(10.0f)));
		allShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));*/

#pragma endregion

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		float time = (float)glfwGetTime();

		processInput(window);

		//glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		 //glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 projection = mat4(1.0f);
		projection = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		allShader.setMat4("projection", projection);

		mat4 view = mat4(1.0f);
		view = view = camera.GetViewMatrix();
		allShader.setMat4("view", view);
		//allShader.setVec3("lightPos", lightPos);
		allShader.setVec3("objectColor",vec3(0.0f,1.0f,0.0f));
		mat4 trn = mat4(1.0f);
		trn = glm::translate(trn,vec3(2.0f, 0.0f, 0.0f));
		trn = glm::scale(trn, vec3(0.1f, 0.1f, 0.1f));
		allShader.setMat4("model", trn);
		earth.Draw(allShader);

		lightSourceShader.use();
		lightSourceShader.setMat4("projection", projection);
		lightSourceShader.setMat4("view", view);
		

		// we now draw as many light bulbs as we have point lights.
		//mat4 model = glm::mat4(1.0f);
		//for (unsigned int i = 0; i < 1; i++)
		//{
		//	model = glm::mat4(1.0f);
		//	model = glm::translate(model, pointLightPositions[i]);
		//	model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		//	lightSourceShader.setMat4("model", model);
		//	lightSourceCubes[i].transformation(model);
		//	lightSourceCubes[i].draw(lightSourceShader);
		//}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	float xpos = static_cast<float>(xposIn);
	float ypos = static_cast<float>(yposIn);

	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}
