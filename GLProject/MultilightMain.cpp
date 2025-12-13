//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <vector>
//#include "tools/Polygon.h"
//#include "tools/Cube.h"
//#include <learnopengl/shader_m.h>
//#include <learnopengl/camera.h>
//#include <learnopengl/model.h>
//#include "stb_image.h"
//#include <random> 
//#include <chrono>
//
//using namespace std;
//using namespace glm;
//
//int SCR_WIDTH = 1280;
//int SCR_HEIGHT = 720;
//
//Camera camera(vec3(0.0f, 0.0f, 4.0f));
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;
//
//float deltaTime = 0.0f;
//float lastFrame = 0.0f;
//
//vec3 firstLight(0.0f, -1.5f, 0.0f);
//vec3 lightPos(0.0f, -1.5f, 0.0f);
//
//bool blinn = false;
//
//
//void processInput(GLFWwindow* window);
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//
//vector<GLuint> loadTextures(vector<string> paths, GLuint wrapOption = GL_REPEAT, GLuint filterOption = GL_LINEAR) {
//	vector<GLuint> textures = {};
//
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapOption);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapOption);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterOption);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterOption);
//
//	for (string path : paths)
//	{
//		unsigned int texture;
//		glGenTextures(1, &texture);
//		glBindTexture(GL_TEXTURE_2D, texture);
//		int width, height, nrChannels;
//		unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
//
//
//		if (data)
//		{
//			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//			glGenerateMipmap(GL_TEXTURE_2D);
//		}
//		else
//			std::cout << "Failed to load texture" << std::endl;
//		stbi_image_free(data);
//
//		textures.push_back(texture);
//	}
//
//	return textures;
//}
//
//int main()
//{
//	glfwInit();
//	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Lecture 5", NULL, NULL);
//	glfwMakeContextCurrent(window);
//	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//	glfwSetCursorPosCallback(window, mouse_callback);
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
//
//	Shader allShader("./shaders/vs/L5.vs", "./shaders/fs/L5-Multi.fs");
//	//Shader allShader("./shaders/vs/L5.vs", "./shaders/fs/HW-model.fs");
//
//	Shader lightSourceShader("./shaders/vs/LightSource.vs", "./shaders/fs/LightSource.fs");
//
//	Model sun("./models/planet.glb");
//	Model moon("./models/planet.glb");
//	Model earth("./models/planet.glb");
//
//	//Cube plainBall(vec3(0.0f,0.0f,0.0f),0.2f,vec3(1.0f,1.0f,1.0f));
//	glEnable(GL_DEPTH_TEST);
//
//	stbi_set_flip_vertically_on_load(true);
//
//	vector<string> texturePaths = {};
//	texturePaths.push_back("./textures/sun.jpg");
//	texturePaths.push_back("./textures/earth.png");
//	texturePaths.push_back("./textures/moon.jpg");
//	vector<GLuint> textures = loadTextures(texturePaths);
// 
//	while (!glfwWindowShouldClose(window))
//	{
//
//		allShader.use();
//		allShader.setVec3("viewPos", camera.Position);
// 
//
//	#pragma region Light Settings
//		//
//		// sun
//		//
//		allShader.setVec3("pointLights[0].position", vec3(0.0f, 0.0f, 0.0f));
//		allShader.setVec3("pointLights[0].ambient", vec3(1.0f, 1.0f, 1.0f));
//		allShader.setVec3("pointLights[0].diffuse", vec3(1.0f, 1.0f, 1.0f));
//		allShader.setVec3("pointLights[0].specular", vec3(1.0f,1.0f,1.0f));
//		allShader.setFloat("pointLights[0].constant", 1.0f);
//		allShader.setFloat("pointLights[0].linear", 0.0f);
//		allShader.setFloat("pointLights[0].quadratic", 0.0f);
//		//
//		// moon
//		//
//		allShader.setVec3("pointLights[1].position", vec3(0.0f, 0.0f, 0.0f));
//		allShader.setVec3("pointLights[1].ambient", vec3(0.0f, 0.0f, 0.0f));
//		allShader.setVec3("pointLights[1].diffuse", vec3(0.2f,0.2f,0.2f));
//		allShader.setVec3("pointLights[1].specular", vec3(0.2f, 0.2f, 0.2f));
//		allShader.setFloat("pointLights[1].constant", 1.0f);
//		allShader.setFloat("pointLights[1].linear", 0.14f);
//		allShader.setFloat("pointLights[1].quadratic", 0.07f);
//		//
//	
//
//#pragma endregion
// 
//
//
//		float currentFrame = static_cast<float>(glfwGetTime());
//		deltaTime = currentFrame - lastFrame;
//		lastFrame = currentFrame;
//		float time = (float)glfwGetTime();
//
//		processInput(window);
//		 
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		mat4 projection = mat4(1.0f);
//		projection = perspective(radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//		allShader.setMat4("projection", projection);
//
//		mat4 view = mat4(1.0f);
//		view = view = camera.GetViewMatrix();
//		allShader.setMat4("view", view); 
//		allShader.setVec3("objectColor",vec3(1.0f,1.0f,1.0f));
//	 
//
//		mat4 sunModel = mat4(1.0f);
//	
//		
//		//sunModel = rotate(sunModel, time * 0.3f, vec3(0, 1, 0));
//		sunModel = scale(sunModel, vec3(1.0f));
//		allShader.setMat4("model", sunModel);
//		 
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, textures[0]); 
//
//		allShader.setInt("diffuseTex", 0);
//
//		//allShader.setMat4("model", sunModel);
//
//		// disable moon light before draw the sun
//		allShader.setVec3("pointLights[1].ambient", vec3(0.0f));
//		allShader.setVec3("pointLights[1].diffuse", vec3(0.0f));
//		allShader.setVec3("pointLights[1].specular", vec3(0.0f));
//
//		sun.Draw(allShader);
//
//	
//		//
//		mat4 earthModel = mat4(1.0f);
//		
//		// rotate around sun
//		earthModel = rotate(earthModel, time * 0.5f, vec3(0, 1, 0));
//
//		// move it away from sun
//		earthModel = translate(earthModel, vec3(5.0f, 0.0f, 0.0f));
//
//		// rotete around it self
//		earthModel = rotate(earthModel, time * 2.0f, vec3(0, 1, 0));
//
//		// scale smaller
//		earthModel = scale(earthModel, vec3(0.4f));
//
//		allShader.setMat4("model", earthModel);
//	
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, textures[1]); 
//
//		allShader.setInt("diffuseTex", 0);
//		mat4 moonModel = earthModel;
//		// rotate around earth
//		moonModel = rotate(moonModel, time * 0.5f, vec3(0, 1, 0));
//
//		// move it away from earth
//		moonModel = translate(moonModel, vec3(2.0f, 0, 0));
//
//		// rotate around it self
//		moonModel = rotate(moonModel, time * 1.0f, vec3(0, 1, 0));
//
//		// scale smaller
//		moonModel = scale(moonModel, vec3(0.2f));
//
//		//allShader.setMat4("model", earthModel);
//		
//		// enable moon light after draw the sun
//		allShader.setVec3("pointLights[0].ambient", vec3(0.0f));
//		allShader.setVec3("pointLights[1].position", moonModel[3]);
//		allShader.setVec3("pointLights[1].ambient", vec3(0.00f));
//		allShader.setVec3("pointLights[1].diffuse", vec3(0.10f));
//		allShader.setVec3("pointLights[1].specular", vec3(0.1f));
//		//
//		earth.Draw(allShader);
//
//
//		//
//
//		
//
//	
//		
//
//		allShader.setMat4("model", moonModel);
//		
//		glActiveTexture(GL_TEXTURE0);
//		glBindTexture(GL_TEXTURE_2D, textures[2]);  
//
//		allShader.setInt("diffuseTex", 0);
//
//		//allShader.setMat4("model", moonModel);
//		moon.Draw(allShader);
//
//
//		//
//
//		lightSourceShader.use();
//		lightSourceShader.setMat4("projection", projection);
//		lightSourceShader.setMat4("view", view);
// 
//
//		glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	glfwTerminate();
//	return 0;
//}
//
//void processInput(GLFWwindow* window)
//{
//	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//		glfwSetWindowShouldClose(window, true);
//	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//		camera.ProcessKeyboard(FORWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//		camera.ProcessKeyboard(BACKWARD, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//		camera.ProcessKeyboard(LEFT, deltaTime);
//	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//		camera.ProcessKeyboard(RIGHT, deltaTime);
//}
//
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//	float xpos = static_cast<float>(xposIn);
//	float ypos = static_cast<float>(yposIn);
//
//	if (firstMouse)
//	{
//		lastX = xpos;
//		lastY = ypos;
//		firstMouse = false;
//	}
//
//	float xoffset = xpos - lastX;
//	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//	lastX = xpos;
//	lastY = ypos;
//
//	camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//	glViewport(0, 0, width, height);
//}
