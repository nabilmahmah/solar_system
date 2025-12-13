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


float baseSpeed = 1.0f;
float timeScale = 1.0f;

bool shouldAnimate = true;
bool isMoonCoverd = false;   
bool isEarthCoverd = false;  



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

	//Shader allShader("./shaders/vs/L5.vs", ");
	Shader allShader("./shaders/vs/L5.vs", "./shaders/fs/L5-Multi.fs");
	//Shader allShader("./shaders/vs/L5.vs", "./shaders/fs/HW-model.fs");

	Shader lightSourceShader("./shaders/vs/LightSource.vs", "./shaders/fs/LightSource.fs");

	glEnable(GL_DEPTH_TEST); 
 
 stbi_set_flip_vertically_on_load(true);
 
	Model sun("./models/planet.glb");
	Model moon("./models/planet.glb");
	Model earth("./models/planet.glb");

	//Cube plainBall(vec3(0.0f,0.0f,0.0f),0.2f,vec3(1.0f,1.0f,1.0f));
	Cube LightSource(firstLight, 0.2f, vec3(1.0f, 1.0f, 1.0f));

	stbi_set_flip_vertically_on_load(true);

	vector<string> texturePaths = {};
	texturePaths.push_back("./textures/sun.jpg");
	texturePaths.push_back("./textures/earth.png");
	texturePaths.push_back("./textures/moon.jpg");
	vector<GLuint> textures = loadTextures(texturePaths);
 
	while (!glfwWindowShouldClose(window))
	{
		 processInput(window);

		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		/*float time = currentFrame;*/
		
		static float simTime = 0.0f;

		if (shouldAnimate)
			simTime += deltaTime * timeScale;

		float time = simTime;


		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		 
		mat4 sunModel = mat4(1.0f);
		sunModel = scale(sunModel, vec3(1.0f));
		 
		mat4 earthModel = mat4(1.0f);
		earthModel = rotate(earthModel,  time * 0.2f, vec3(0, 1, 0));
		earthModel = translate(earthModel, vec3(-5.0f, 0.0f, -5.0f)); 
		earthModel = rotate(earthModel, time * 2.0f, vec3(0, 1, 0));    
		earthModel = scale(earthModel, vec3(0.4f));                     

		
		mat4 moonOrbit = earthModel;
		moonOrbit = rotate(moonOrbit, time * 0.2f * 24.0f, vec3(0, 1, 0)); 
		moonOrbit = translate(moonOrbit, vec3(2.0f, 0.0f, 0.0f));          
		vec3 moonWorldPos = vec3(moonOrbit[3]);                            

		mat4 moonModel = moonOrbit;
		moonModel = scale(moonModel, vec3(0.15f));


#pragma region Light Settings 
		allShader.setVec3("pointLights[0].position", vec3(0.0f, 0.0f, 0.0f));
		allShader.setVec3("pointLights[0].ambient", vec3(0.1f));
		allShader.setVec3("pointLights[0].diffuse", vec3(1.0f));
		allShader.setVec3("pointLights[0].specular", vec3(1.0f));
		allShader.setFloat("pointLights[0].constant", 1.0f);
		allShader.setFloat("pointLights[0].linear", 0.14f);
		allShader.setFloat("pointLights[0].quadratic", 0.07f);
		 
		allShader.setVec3("pointLights[1].position", moonWorldPos);
		allShader.setVec3("pointLights[1].ambient", vec3(0.0f));
		allShader.setVec3("pointLights[1].diffuse", vec3(0.2f));
		allShader.setVec3("pointLights[1].specular", vec3(0.3f));
		allShader.setFloat("pointLights[1].constant", 1.0f);
		allShader.setFloat("pointLights[1].linear", 0.14f);
		allShader.setFloat("pointLights[1].quadratic", 0.07f);
#pragma endregion

		 
		mat4 projection = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		mat4 view = camera.GetViewMatrix();

		allShader.use();
		allShader.setMat4("projection", projection);
		allShader.setMat4("view", view);
		allShader.setVec3("viewPos", camera.Position);

		allShader.setVec3("sunPos", vec3(0.0f));        
		allShader.setVec3("moonPos", moonWorldPos);     
		 


		vec3 sunPos = vec3(0.0f);
		vec3 earthPos = vec3(earthModel[3]);
		vec3 moonPos = moonWorldPos;

		float alignmentThreshold = 0.9995f;
		float distanceThreshold = 0.3f;

		vec3 sunToEarth = normalize(earthPos - sunPos);
		vec3 sunToMoon = normalize(moonPos - sunPos);


		if (isEarthCoverd)
		{
			vec3 sunToEarth = earthPos - sunPos;
			vec3 sunToMoon = moonPos - sunPos;

			vec3 dir = normalize(sunToEarth);

			float alignment = dot(normalize(sunToMoon), dir);
			float proj = dot(sunToMoon, dir);
			vec3 closest = sunPos + proj * dir;
			float lateralDist = length(moonPos - closest);

			if (alignment > alignmentThreshold &&
				proj > 0.0f &&
				proj < length(sunToEarth) &&
				lateralDist < distanceThreshold)
			{
				shouldAnimate = false;
			}
		}
		


		if (isMoonCoverd)
		{
			vec3 sunToMoon = moonPos - sunPos;
			vec3 sunToEarth = earthPos - sunPos;

			vec3 dir = normalize(sunToMoon);

			float alignment = dot(normalize(sunToEarth), dir);

			float proj = dot(sunToEarth, dir);

			vec3 closest = sunPos + proj * dir;

			float lateralDist = length(earthPos - closest);

			if (alignment > alignmentThreshold &&
				proj > 0.0f &&
				proj < length(sunToMoon) &&
				lateralDist < distanceThreshold)
			{
				shouldAnimate = false;
			}
		}
		if (!shouldAnimate && isEarthCoverd)
		{
			allShader.setVec3("pointLights[0].ambient", vec3(0.0f));
			allShader.setVec3("pointLights[0].diffuse", vec3(0.0f));
			allShader.setVec3("pointLights[0].specular", vec3(0.0f));
			
			allShader.setVec3("pointLights[1].ambient", vec3(0.0f));
			//allShader.setVec3("pointLights[1].diffuse", vec3(0.0f));
			//allShader.setVec3("pointLights[1].specular", vec3(0.0f));

			//lightSourceShader.setVec3("objectColor", vec3(0.0f));
		}
		if (!shouldAnimate && isMoonCoverd)
		{

			allShader.setVec3("pointLights[0].ambient", vec3(0.0f));
			allShader.setVec3("pointLights[0].diffuse", vec3(0.0f));
			allShader.setVec3("pointLights[0].specular", vec3(0.0f));

			allShader.setVec3("pointLights[1].ambient", vec3(0.0f));
			allShader.setVec3("pointLights[1].diffuse", vec3(0.0f));
			allShader.setVec3("pointLights[1].specular", vec3(0.0f));

			//lightSourceShader.setVec3("objectColor", vec3(0.0f));
		}

		lightSourceShader.use();
		lightSourceShader.setMat4("projection", projection);
		lightSourceShader.setMat4("view", view);
		lightSourceShader.setMat4("model", sunModel);
		lightSourceShader.setVec3("objectColor", vec3(1.0f));
		glBindTexture(GL_TEXTURE_2D, textures[0]);
		sun.Draw(lightSourceShader);
		 
		

		allShader.use();
		allShader.setMat4("model", earthModel);
		glBindTexture(GL_TEXTURE_2D, textures[1]);
		earth.Draw(allShader);
		 

		allShader.setMat4("model", moonModel);
		glBindTexture(GL_TEXTURE_2D, textures[2]);
		moon.Draw(allShader);



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

	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
	{
		timeScale = 5.0f;
		shouldAnimate = true;
		isMoonCoverd = true;
		isEarthCoverd = false;
	}

	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
	{
		timeScale = 5.0f;
		shouldAnimate = true;
		isEarthCoverd = true;
		isMoonCoverd = false;
	}

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		timeScale = 1.0f;
		shouldAnimate = true;
		isMoonCoverd = false;
		isEarthCoverd = false;
	}
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
