


#include <GL/glew.h>

//#define IMGUI_IMPL_OPENGL_LOADER_CUSTOM

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

//#include "imgui_impl_opengl3_loader.h"

#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <random>
#include <math.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>



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

# define PI          3.141592653589793238462643383279502884L

bool moveLight = true;
float movement = 1.0;

std::default_random_engine generator;
std::normal_distribution<float> distribution(0.0, 1.0);

bool lbutton_down = false;

glm::vec3 bboxMax;
glm::vec3 bboxMin;
float roughness;

unsigned int shadowMapFBO, varianceFBO, shadowMap,thicknessFBO,varianceTexture;

int shininess;

//unsigned int SCR_WIDTH = 1366;
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 768;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 40.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

bool mouseDown = false;
glm::vec2 mouse;
glm::vec2 mouse_i;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float MIN_DELTA_TIME = 1.0f / 60.0f; // 60 FPS

GLuint noiseTexture,specularTexture,hairColorTexture, thicknessTexture;

using namespace std;
GLuint renderingProgram;

char vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/vertex_shader.glsl";
char fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Fragment Shaders/fragment_shader.glsl";

char light_vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/light_vertex_shader.glsl";
char light_fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Fragment Shaders/light_fragment_shader.glsl";

char hair_vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/hair_vertex_shader.glsl";
char hair_fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Fragment Shaders/hair_fragment_shader.glsl";


//char growth_obj_path[] = "C:/Users/Panthelis/Desktop/growth_mesh_hairline3.obj";
char growth_obj_path[] = "C:/Users/Panthelis/Desktop/growth_mesh_hairline5.obj";
char obj_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/woman/woman1.obj";
char hair_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/straight/straight.hair";
//char hair_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/blonde/blonde.hair";

char light_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/woman/light.obj";


char guide_vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/guide_vertex_shader.glsl";
char guide_geometry_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/guide_geometry_shader.glsl";
char guide_control_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/guide_control_shader.glsl";
char guide_evaluation_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/guide_evaluation_shader.glsl";
char guide_fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/guide_fragment_shader.glsl";

char shadowMap_vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/shadowMap_vertex_shader.glsl";
char shadowMap_control_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/shadowMap_control_shader.glsl";
char shadowMap_evaluation_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/shadowMap_evaluation_shader.glsl";
char shadowMap_geometry_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/shadowMap_geometry_shader.glsl";
char shadowMap_fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/shadowMap_fragment_shader.glsl";
char varianceShadowMap_fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/varianceShadowMap_fragment_shader.glsl";

char shadowMap_head_vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/shadowMap_head_vertex_shader.glsl";
char shadowMap_head_fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/shadowMap_head_fragment_shader.glsl";


char thickness_vertex_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/thickness_vertex_shader.glsl";
char thickness_geometry_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/thickness_geometry_shader.glsl";
char thickness_control_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/thickness_control_shader.glsl";
char thickness_evaluation_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/thickness_evaluation_shader.glsl";
char thickness_fragment_shader_path[] = "C:/OpenGLtemplate/Shaders/Vertex Shaders/thickness_fragment_shader.glsl";


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
				Model& model,Model& light, Guides& growth_mesh_guides,Model& growth_mesh, Shader& guide_shader,
				Shader& shadowMap_shader, Shader& varianceShadowMap_shader, float beta_m, float beta_n, float specular_intensity) {


	// per-frame time logic
	// --------------------
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Specify the color of the background 
	glClearColor(0.12f, 0.12f, 0.12f, 1.0f);
	// Clean the back buffer and assign the new color to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	glm::vec3 light_pos(-25.0f, 0.0f, 0.0f);

	const float radius = glm::length(light_pos);
	double rotateX = 0.0;
	double rotateZ = 0.0;

	float x = 0.0, y = 0.0;

	if (moveLight) {
		rotateX = sin(glfwGetTime()) * radius;
		rotateZ = cos(glfwGetTime()) * radius;
	}
	else {

		glfwSetTime(0);


	}

	glEnable(GL_TEXTURE_2D);
	//rotateX = -8.26888; 
	//rotateZ = -23.5929;
	//glm::vec3 lightPos = glm::vec3(rotateX, 0.0f, rotateZ);
	glm::vec3 lightPos = glm::vec3(-8.26888, 0.0f, -23.5929);

	glm::mat4 light_mMat;
	light_mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	light_mMat = glm::scale(light_mMat, glm::vec3(0.9f, 0.9f, 0.9f));
	//if (moveLight) light_mMat = glm::translate(light_mMat, glm::vec3(rotateX, 0.0f, rotateZ));
	if (moveLight) light_mMat = glm::translate(light_mMat, lightPos);

	//cout << rotateX << " " << rotateZ << endl;

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
	glm::mat4 vMat;
	vMat = camera.GetViewMatrix();

	glm::mat4 mMat, inverse_world;
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ));
	mMat = glm::scale(mMat, glm::vec3(0.2f, 0.2f, 0.2f));
	mMat = glm::rotate(mMat, -45.0f, glm::vec3(0, 1, 0)); // rotate on y


	// Write to shadow map texture by rendering the guides
	shadowMap_shader.use();
	float nearPlane = 1.5f; // near plane distance
	float farPlane = 17.5; // far plane distance
	float fov = 90.0f; // field of view in degrees

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	// create the perspective projection matrix
	
	//lightProjection = glm::perspective(glm::radians(fov), 1.0f, nearPlane, farPlane);
	
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

	lightProjection[2][2] *= -1.0f; // Flip the z-axis

	lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));


	lightSpaceMatrix = lightProjection * lightView * light_mMat ;
	


	glm::mat4 guides_mMat;
	guides_mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ));
	guides_mMat = glm::scale(guides_mMat, glm::vec3(0.26f, 0.26f, 0.26f));
	guides_mMat = glm::rotate(guides_mMat, glm::radians(-145.0f), glm::vec3(0, 1, 0));
	guides_mMat = glm::rotate(guides_mMat, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	guides_mMat = glm::translate(guides_mMat, glm::vec3(0.0f, -11.0f, -11.0f));

	shadowMap_shader.setMat4("model", guides_mMat);
	shadowMap_shader.setMat4("view", vMat);
	shadowMap_shader.setMat4("projection", projection);
	shadowMap_shader.setMat4("light_view", lightView);
	shadowMap_shader.setMat4("light_model", light_mMat);
	shadowMap_shader.setMat4("light_projection", lightProjection);
	shadowMap_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shadowMap_shader.setFloat("nearPlane", nearPlane);
	shadowMap_shader.setFloat("farPlane", farPlane);


	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	growth_mesh_guides.Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Variance shadow map
	varianceShadowMap_shader.use();
	varianceShadowMap_shader.setMat4("model", guides_mMat);
	varianceShadowMap_shader.setMat4("view", vMat);
	varianceShadowMap_shader.setMat4("projection", projection);
	varianceShadowMap_shader.setMat4("light_view", lightView);
	varianceShadowMap_shader.setMat4("light_model", light_mMat);
	varianceShadowMap_shader.setMat4("light_projection", lightProjection);
	varianceShadowMap_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	varianceShadowMap_shader.setFloat("nearPlane", nearPlane);
	varianceShadowMap_shader.setFloat("farPlane", farPlane);
	varianceShadowMap_shader.setInt("shadowMap", shadowMap);

	glActiveTexture(GL_TEXTURE0 + shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, varianceFBO);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClear(GL_COLOR_BUFFER_BIT);
	growth_mesh_guides.Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	

	shader.use();
	inverse_world = glm::inverse(mMat);

	// The normal matrix is defined as 'the transpose of the inverse of the upper-left 3x3 part of the model matrix
	// Used for non-uniform scaling of the model. In a uniform scaling, there is no visible result whether we use normal matrix or not
	glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(mMat)));

	//glm::mat4 model_view = vMat * mMat;
	
	shader.setMat3("normal_matrix", normal_matrix);
	shader.setMat4("model", mMat);
	shader.setMat4("view", vMat);
	shader.setMat4("projection", projection);
	shader.SetVector3f("lightColor", light_color);
	shader.SetVector3f("camera_position", camera.Position);
	
	

	shader.SetVector3f("lightPos",lightPos);
	shader.setFloat("specular_strength", 0.5);
	model.Draw(shader);
	


	// Render light cube
	light_shader.use();
	
	light_shader.setMat4("projection", projection);
	light_shader.setMat4("model", light_mMat);
	light_shader.setMat4("view", vMat);
	light.Draw(light_shader);

	

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glm::mat4 bspline_mat = glm::mat4(vec4(-1, 3, -3, 1),
		vec4(3, -6, 3, 0),
		vec4(-3, 0, 3, 0),
		vec4(1, 4, 1, 0));
	// Render guide lines

	guide_shader.use();
	guide_shader.setMat4("model", guides_mMat);
	guide_shader.setMat4("view", vMat);
	guide_shader.setMat4("projection", projection);
	guide_shader.setMat4("InverseWorldMatrix", inverse_world);
	guide_shader.SetVector3f("camera_front", camera.Front);
	guide_shader.SetVector3f("camera_position", camera.Position);
	guide_shader.setMat4("bspline_mat", bspline_mat);
	//guide_shader.setInt("noiseTexture", noiseTexture);
	//guide_shader.setInt("specularTexture", specularTexture);
	guide_shader.setInt("hairColorTexture", hairColorTexture);
	guide_shader.SetVector3f("lightPos", lightPos);
	guide_shader.setFloat("specular_strength", 0.5);
	guide_shader.SetVector3f("lightColor", light_color);
	guide_shader.SetVector3f("bboxMax", bboxMax);
	guide_shader.SetVector3f("bboxMin", bboxMin);
	guide_shader.setFloat("roughness", roughness);
	guide_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	guide_shader.setInt("shadowMap", shadowMap);
	guide_shader.setFloat("nearPlane", nearPlane);
	guide_shader.setFloat("farPlane", farPlane);
	guide_shader.setInt("varianceMap", varianceTexture);

	/*glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);

	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, specularTexture);*/

	glActiveTexture(GL_TEXTURE0 + hairColorTexture);
	glBindTexture(GL_TEXTURE_2D, hairColorTexture);

	glActiveTexture(GL_TEXTURE0 + shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glActiveTexture(GL_TEXTURE0 + varianceTexture);
	glBindTexture(GL_TEXTURE_2D, varianceTexture);


	// Render Hair
	growth_mesh_guides.Draw();




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
	/*auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
		return;
	}*/

	if (!lbutton_down) {
		double m_x, m_y;
		glfwGetCursorPos(window, &m_x, &m_y);
		mouse_i = vec2(m_x, m_y);

	}
	else {
		// Grab the current location of the mouse and find the delta between it and the last
		double m_x, m_y;
		glfwGetCursorPos(window, &m_x, &m_y);
		mouse = vec2(m_x, m_y);
		vec2 mouse_d;
		mouse_d.x = mouse.x - mouse_i.x;
		mouse_d.y = mouse_i.y - mouse.y;
		mouse_i = mouse;

		camera.ProcessMouseMovement(mouse_d.x, mouse_d.y);
	}

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
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		moveLight = !moveLight;
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

void mouse_callback2(GLFWwindow* window, int button, int action, int mods) {

	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (GLFW_PRESS == action)
			lbutton_down = true;
		else if (GLFW_RELEASE == action)
			lbutton_down = false;
	}


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

	// Make sure that the target system is compatible with at least OpenGL4.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	char texture_image_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/woman/head.tga";

	// class GLFWwindow: Create a window that you can draw to
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hair Loader1", NULL, NULL);

	// Make the newly created window the current context (stores the entire OpenGL state)
	glfwMakeContextCurrent(window);

	// Initialize the GLEW library
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_callback2);


	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST); // enable depth testing
	glDepthFunc(GL_LESS);

	glEnable(GL_MULTISAMPLE); // enabled by default on some drivers, but not all so always enable to make sure

	// Disable vsync
	glfwSwapInterval(0);

	// Setup Dear ImGui context
	/*IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");*/

	
	// Initialize the window instance
	init(window);


	Shader shader(vertex_shader_path, fragment_shader_path);
	Shader light_shader(light_vertex_shader_path, light_fragment_shader_path);
	Shader hair_shader(hair_vertex_shader_path, hair_fragment_shader_path);
	Shader guide_shader(guide_vertex_shader_path, guide_fragment_shader_path, guide_control_shader_path, guide_evaluation_shader_path,
		guide_geometry_shader_path);

	Shader thickness_shader(thickness_vertex_shader_path, thickness_fragment_shader_path, thickness_control_shader_path,
		thickness_evaluation_shader_path, thickness_geometry_shader_path);

	Shader varianceShadowMap_shader(shadowMap_vertex_shader_path, varianceShadowMap_fragment_shader_path, shadowMap_control_shader_path, shadowMap_evaluation_shader_path, shadowMap_geometry_shader_path);
	Shader shadowMap_shader(shadowMap_vertex_shader_path, shadowMap_fragment_shader_path,shadowMap_control_shader_path,shadowMap_evaluation_shader_path, shadowMap_geometry_shader_path);

	Model model(obj_path);
	Model light(light_path);
	Model growth_mesh(growth_obj_path);

	hair.SetupHair();
	hair.WriteRootOutput();
	hair.WriteSegmentsArray();

	Guides growth_mesh_guides(&growth_mesh, hair.GetSegments(), hair.GetHairCount(), &hair);
	growth_mesh_guides.SelectGuidesFromHairfile(hair, hair.GetRoots());
	growth_mesh_guides.SetupGuides();
	growth_mesh_guides.FindBoundingBox();

	 bboxMin = growth_mesh_guides.bboxMin;
	 bboxMax = growth_mesh_guides.bboxMax;

	 cout << "bboxMin: " << bboxMin.x << " " << bboxMin.y << " " << bboxMin.z << endl;
	 cout << "bboxMax: " << bboxMax.x << " " << bboxMax.y << " " << bboxMax.z << endl;

	/*noiseTexture = Utils::loadTexture("noise128.jpg");
	cout << "noise texture int: " << noiseTexture << endl;

	specularTexture = Utils::loadTexture("specular_test.jpg");
	cout << "specular texture int: " << specularTexture << endl;*/ 

	hairColorTexture = Utils::loadTexture("hairColor5.jpg");
	cout << "hairColor texture int: " << hairColorTexture << endl;


	//glDepthFunc(GL_LESS);    // set the depth function to "less than"

	 GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	 
	// Shadow map creation
	

	// create depth texture
	glGenTextures(1, &shadowMap);
	cout << "shadowMap: " << shadowMap << endl;
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RG, GL_FLOAT, NULL);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);


	glGenTextures(1, &varianceTexture);
	cout << "varianceTexture: " << varianceTexture << endl;
	glBindTexture(GL_TEXTURE_2D, varianceTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// attach depth texture as FBO's depth buffer
	glGenFramebuffers(1, &shadowMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	
	GLint depthAttachment = GL_DEPTH_ATTACHMENT;
	GLint depthTexture;
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, depthAttachment,GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,&depthTexture);
	if (depthTexture == 0) {
		std::cerr << "Depth texture is not attached to the framebuffer" << std::endl;
		// Handle error
		exit(-1);
	}
	
	
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &varianceFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, varianceFBO);

	// Attach the shadow map texture to the framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, varianceTexture, 0);

	// Check the framebuffer status for errors
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error: Framebuffer object is not complete." << std::endl;
	}

	// Unbind the framebuffer object and texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);


	while (!glfwWindowShouldClose(window)) {

		/*auto& io = ImGui::GetIO();
		if (io.DeltaTime <= 0.0f) io.DeltaTime = 0.00001f;

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		// render your GUI
		//ImGui::Begin("Hair Lighting");
		ImGui::SliderFloat("beta_m", &beta_m, 0.2f, 1.0f);
		ImGui::SliderFloat("beta_n", &beta_n, 0.2f, 1.0f);
		ImGui::SliderFloat("specular intensity", &specular_intensity, 0.5f, 1.0f);
		//ImGui::End();*/
		

		static float beta_m = 0.3;
		static float beta_n = 0.3;
		static float specular_intensity = 0.5;

		processInput(window);
		display(window, glfwGetTime(), shader,light_shader,hair_shader, model,
				light,growth_mesh_guides,growth_mesh,guide_shader, shadowMap_shader, varianceShadowMap_shader,
				beta_m, beta_n,specular_intensity);


		// Render dear imgui into screen
		//ImGui::Render();
		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Outputs (paints) to the screen
		glfwSwapBuffers(window);

		//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Handles window-related events (such as key pressing)
		glfwPollEvents();

	}

	//_CrtDumpMemoryLeaks();

	// Cleanup
	/*ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();*/

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
