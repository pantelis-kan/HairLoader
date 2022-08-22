#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include "Utils.h"
#include "ErrorChecking.h"
#include "Transform.hpp"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Hair.h"
#include "Guides.h"

#include <chrono>

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 768;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 40.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


using namespace std;
GLuint renderingProgram;

char vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/vertex_shader.glsl";
char fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Fragment Shaders/fragment_shader.glsl";

char light_vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/light_vertex_shader.glsl";
char light_fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Fragment Shaders/light_fragment_shader.glsl";

char hair_vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/hair_vertex_shader.glsl";
char hair_fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Fragment Shaders/hair_fragment_shader.glsl";

//char growth_obj_path[] = "C:/Users/Panthelis/Desktop/growth_mesh1.obj";
char growth_obj_path[] = "C:/Users/Panthelis/Desktop/growth_mesh_hairline.obj";
char obj_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/woman/woman1.obj";
char hair_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/straight/straight.hair";
//char hair_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/blonde/blonde.hair";

char light_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/woman/light.obj";


char guide_vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/guide_vertex_shader.glsl";
char guide_geometry_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/guide_geometry_shader.glsl";
char guide_control_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/guide_control_shader.glsl";
char guide_evaluation_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/guide_evaluation_shader.glsl";
char guide_fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/guide_fragment_shader.glsl";

float cameraX, cameraY, cameraZ;
float objLocX, objLocY, objLocZ;
float objRotX, objRotY, objRotZ;


Hair hair(hair_path);


// allocate variables used in display() function, so that they won’t need to be allocated during rendering
// These are the uniform (static) variables
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat;
glm::vec3 light_color(1.0f, 1.0f,1.0f);


void init(GLFWwindow* window) {


	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 0.0f;
	objLocX = 0.0f; objLocY = -2.0f; objLocZ = 0.0f;
	objRotX = 0.0f; objRotY = 0.0f; objRotZ = 0.0f;

}

void display(GLFWwindow* window, double CurrentTime, Shader& shader, Shader& light_shader, Shader& hair_shader, 
				Model& model,Model& light, Guides& growth_mesh_guides,Model& growth_mesh, Shader& guide_shader) {

	// per-frame time logic
	// --------------------
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Specify the color of the background 
	glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
	// Clean the back buffer and assign the new color to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();


	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
	shader.setMat4("projection", projection);

	glm::mat4 vMat, mMat, inverse_world;
	
	//vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	
	vMat = camera.GetViewMatrix();
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ));
	mMat = glm::scale(mMat, glm::vec3(0.2f, 0.2f, 0.2f));
	mMat = glm::rotate(mMat, -45.0f, glm::vec3(0, 1, 0)); // rotate on y

	inverse_world = glm::inverse(mMat);

	// The normal matrix is defined as 'the transpose of the inverse of the upper-left 3x3 part of the model matrix
	// Used for non-uniform scaling of the model. In a uniform scaling, there is no visible result whether we use normal matrix or not
	glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(mMat)));

	//glm::mat4 model_view = vMat * mMat;
	shader.setMat3("normal_matrix",normal_matrix);
	shader.setMat4("model",mMat);
	shader.setMat4("view", vMat);

	shader.SetVector3f("lightColor",light_color);
	shader.SetVector3f("camera_position",camera.Position);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glm::vec3 light_pos(-25.0f, 0.0f, 0.0f);
	

	const float radius = glm::length(light_pos);
	float rotateX = sin(glfwGetTime()) * radius;
	float rotateZ = cos(glfwGetTime()) * radius;

	shader.SetVector3f("lightPos", glm::vec3(rotateX, 0.0f, rotateZ));
	shader.setFloat("specular_strength", 0.5);
	model.Draw(shader);


	// Render hair

	hair_shader.use();
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ));
	mMat = glm::scale(mMat, glm::vec3(0.26f, 0.26f, 0.26f));
	mMat = glm::rotate(mMat, glm::radians(-145.0f), glm::vec3(0, 1, 0));
	mMat = glm::rotate(mMat, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	mMat = glm::translate(mMat, glm::vec3(0.0f, -9.0f, -11.0f));
	hair_shader.setMat4("model", mMat);
	hair_shader.setMat4("view", vMat);
	hair_shader.setMat4("projection", projection);
	hair_shader.setFloat("default_transparency", hair.Default_Transparency());
	//hair.DrawHair();



	// Render guide lines
	guide_shader.use();
	guide_shader.setMat4("model", mMat);
	guide_shader.setMat4("view", vMat);
	guide_shader.setMat4("projection", projection);
	guide_shader.setMat4("InverseWorldMatrix", inverse_world);
	guide_shader.SetVector3f("camera_front",camera.Front);
	guide_shader.SetVector3f("camera_position", camera.Position);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//growth_mesh.Draw(hair_shader);
	growth_mesh_guides.Draw();


	// Render light cube
	light_shader.use();
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,0.0f,0.0f));
	mMat = glm::scale(mMat, glm::vec3(0.9f, 0.9f, 0.9f));
	mMat = glm::translate(mMat, glm::vec3(rotateX, 0.0f, rotateZ));
	//model_view = vMat * mMat;
	light_shader.setMat4("projection", projection);
	light_shader.setMat4("model", mMat);
	light_shader.setMat4("view", vMat);
	light.Draw(light_shader);

}

double lastTime = glfwGetTime();
int nbFrames = 0;

void GetFPS() {

	double currentTime = glfwGetTime();
	nbFrames++;
	if (currentTime - lastTime >= 1.0) { // If last printf() was more than 1 sec ago
		// printf and reset timer
		//printf("%f ms/frame\n", 1000.0 / double(nbFrames));
		cout << camera.Position.x << " " << " " << camera.Position.y << " " << camera.Position.z << "  |  " << camera.Front.x << " " << camera.Front.y << " " << camera.Front.z  << endl;
		nbFrames = 0;
		lastTime += 1.0;
	}
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
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

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
	glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

int main(void) {

	// Initialize the GLFW library
	if (!glfwInit()) { exit(EXIT_FAILURE); }

	// MSAA 8x
	//glfwWindowHint(GLFW_SAMPLES, 8);

	// Make sure that the target system is compatible with at least OpenGL4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	char texture_image_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/woman/head.tga";
	//char texture_image_path[] = "C:/SOIL2-master/bin/img_mars.jpg";



	// class GLFWwindow: Create a window that you can draw to
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hair Loader", NULL, NULL);

	// Make the newly created window the current context (stores the entire OpenGL state)
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Initialize the GLEW library
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	// Sets the minimum number of screen updates to wait for until the buffers are swapped by glfwSwapBuffers.
	// In other words, it enables VSync. Sets the swap interval for the current OpenGL context.
	//glfwSwapInterval(1);

	// Disable vsync
	glfwSwapInterval(0);

	Shader shader(vertex_shader_path, fragment_shader_path);
	Shader light_shader(light_vertex_shader_path,light_fragment_shader_path);
	Shader hair_shader(hair_vertex_shader_path, hair_fragment_shader_path);
	//Shader guide_shader(guide_vertex_shader_path, guide_fragment_shader_path, guide_geometry_shader_path);
	Shader guide_shader(guide_vertex_shader_path , guide_fragment_shader_path, guide_control_shader_path , guide_evaluation_shader_path ,
						 guide_geometry_shader_path);
	//Shader guide_shader(guide_vertex_shader_path, guide_fragment_shader_path);

	// Initialize the window instance
	init(window);

	Model model(obj_path);
    Model light(light_path);

	Model growth_mesh(growth_obj_path);

	hair.SetupHair();
	hair.WriteRootOutput();
	hair.WriteSegmentsArray();

	Guides growth_mesh_guides(&growth_mesh, hair.GetSegments(),hair.GetHairCount(),&hair);
	growth_mesh_guides.SelectGuidesFromHairfile(hair,hair.GetRoots());

	growth_mesh_guides.SetupGuides();

	GLint MaxPatchVertices = 0;
	glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
	printf("Max supported patch vertices %d\n", MaxPatchVertices);

	while (!glfwWindowShouldClose(window)) {

		 //GetFPS();
		//glfwSetKeyCallback(window, key_callback);
		processInput(window);
		display(window, glfwGetTime(), shader,light_shader,hair_shader, model,light,growth_mesh_guides,growth_mesh,guide_shader);

		// Outputs (paints) to the screen
		glfwSwapBuffers(window);

		// Handles window-related events (such as key pressing)
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
