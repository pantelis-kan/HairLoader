


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
#include "Marschner.h"


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

unsigned int shadowMapFBO, opacityFBO,resolvedFBO, shadowMap,opacityTexture,additionalTexture,varianceTexture,resolvedDepth;

int shininess;

unsigned int SCR_WIDTH = 1366;
//unsigned int SCR_WIDTH = 1280;
unsigned int SCR_HEIGHT = 768;


// camera
Camera camera(glm::vec3(0.0f, 0.0f, 40.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

const int numTextures = 4; // 4 RGBA textures = 16 layers
GLuint opacityLayerTextures[numTextures];

const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;


bool mouseDown = false;
glm::vec2 mouse;
glm::vec2 mouse_i;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
float MIN_DELTA_TIME = 1.0f / 60.0f; // 60 FPS

GLuint noiseTexture,specularTexture,hairColorTexture, highlightTexture, Marschner_M, Marschner_N;

using namespace std;
GLuint renderingProgram;

char vertex_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/vertex_shader.glsl";
char fragment_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/fragment_shader.glsl";

char compute_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/blurring.glsl";


char light_vertex_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/light_vertex_shader.glsl";
char light_fragment_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/light_fragment_shader.glsl";

char hair_vertex_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/hair_vertex_shader.glsl";
char hair_fragment_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/hair_fragment_shader.glsl";



char growth_obj_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Objects/growth_mesh_hairline14.obj";
//char growth_obj_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Objects/growth_mesh_hairline8.obj";
char obj_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Objects/woman/woman1.obj";
//char obj_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Objects/woman/shadow_test.obj";
char hair_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Objects/straight/straight.hair";
//char hair_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Objects/blonde/blonde.hair";

//char light_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Objects/woman/light.obj";
char light_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Objects/woman/arrow.obj";


char guide_vertex_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/guide_vertex_shader.glsl";
char guide_geometry_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/guide_geometry_shader.glsl";
char guide_control_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/guide_control_shader.glsl";
char guide_evaluation_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/guide_evaluation_shader.glsl";
char guide_fragment_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/guide_fragment_shader.glsl";

char shadowMap_vertex_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/shadowMap_vertex_shader.glsl";
char shadowMap_control_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/shadowMap_control_shader.glsl";
char shadowMap_evaluation_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/shadowMap_evaluation_shader.glsl";
char shadowMap_geometry_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/shadowMap_geometry_shader.glsl";
char shadowMap_fragment_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/shadowMap_fragment_shader.glsl";
char opacityShadowMap_fragment_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/opacityShadowMap_fragment_shader.glsl";

char shadowMap_head_vertex_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/shadowMap_head_vertex_shader.glsl";
char shadowMap_head_fragment_shader_path[] = "C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Shaders/shadowMap_head_fragment_shader.glsl";


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
glm::vec3 light_color(1.0f, 0.95f,0.75f);
//glm::vec3 light_color(0.9f, 0.9f,0.9f);

struct alignas(16) XYZBarycentric {
	glm::vec3 xyz;
};
std::vector<XYZBarycentric> barycentrics;
//float barycentrics[128 * 3];

// Function to generate barycentric coordinates for a triangle
void generateBarycentrics(int maxHairPerTriangle) {

	for (int i = 0; i < maxHairPerTriangle; i++)
	{
		float barycentric1 = rand() / static_cast<float>(RAND_MAX);
		float barycentric2 = rand() / static_cast<float>(RAND_MAX);
		float barycentric3 = 0;

		//make sure that the total sum of the barycentric coordinates is no more than 1
		if (barycentric1 + barycentric2 >= 1)
		{
			barycentric1 = 1 - barycentric1;
			barycentric2 = 1 - barycentric2;
		}
		barycentric3 = 1 - barycentric1 - barycentric2;
		
		XYZBarycentric barycentric;
		barycentric.xyz = glm::vec3(barycentric1, barycentric2, barycentric3);
		
		//barycentrics.push_back(glm::vec3(barycentric1, barycentric2, barycentric3));
		barycentrics.push_back(barycentric);

	}

}


void init(GLFWwindow* window) {


	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 0.0f;
	objLocX = 0.0f; objLocY = -2.0f; objLocZ = 0.0f;
	objRotX = 0.0f; objRotY = 0.0f; objRotZ = 0.0f;

}

void display(GLFWwindow* window, double CurrentTime, Shader& shader, Shader& light_shader, Shader& computeShader, Shader& hair_shader, 
				Model& model,Model& light, Guides& growth_mesh_guides,Model& growth_mesh, Shader& guide_shader,
				Shader& shadowMap_shader, Shader& opacityShadowMap_shader, Shader& shadowMap_head_shader,
				float* lightPosX, float* lightPosY, float* lightPosZ,float* nearPlaneHair,float* farPlaneHair,
				float* LayerSizeX, float* LayerSizeY, float* LayerSizeZ, float* LayerSizeW, float* opacityVal
	,float* HeadPosX, float* HeadPosY, float* HeadPosZ, float* shadowBias, float* minBias,float *headShadowBias) {


	// per-frame time logic
	// --------------------
	double currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	// Specify the color of the background 
	//glClearColor(0.35f, 0.35f, 0.35f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Clean the back buffer and assign the new color to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	glm::vec3 light_pos(-50.0f, 1.0f, -2.0f);

	const float radius = glm::length(light_pos);
	double rotateX = 0.0;
	double rotateZ = 0.0;

	float x = 0.0, y = 0.0;

	if (moveLight) {
		rotateX = (sin(glfwGetTime()) * radius) / 2 ;
		rotateZ = (cos(glfwGetTime()) * radius) / 2;
	}
	else {

		glfwSetTime(0);


	}

	glEnable(GL_TEXTURE_2D);
	//rotateX = -8.26888; 
	//rotateZ = -23.5929;
	
	//glm::vec3 lightPos = glm::vec3(rotateX, 0.0f, rotateZ);
	//glm::vec3 lightPos = glm::vec3(-8.26888, 20.0f, -13.5929);
	glm::vec3 lightPos = glm::vec3(*lightPosX, *lightPosY, *lightPosZ);
	glm::vec3 lightDirRad = glm::radians(lightPos);


	glm::vec3 lightNorm = normalize(lightPos);

	glm::mat4 light_mMat;
	light_mMat = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	light_mMat = glm::scale(light_mMat, glm::vec3(0.9f, 0.9f, 0.9f));
	//if (moveLight) light_mMat = glm::translate(light_mMat, normalize(lightDirRad));
	//if (moveLight) light_mMat = glm::translate(light_mMat, glm::vec3(-18.2,20,-19.5));
	if (moveLight) light_mMat = glm::translate(light_mMat, glm::vec3(0,0,0));
	light_mMat = glm::rotate(light_mMat, 100*glm::radians(lightPos.x), glm::vec3(0, 1, 0));
	light_mMat = glm::rotate(light_mMat, -100*glm::radians(lightPos.y), glm::vec3(1, 0, 0));
	light_mMat = glm::rotate(light_mMat, 100*glm::radians(lightPos.z), glm::vec3(0, 0, 1));

	//cout << rotateX << " " << rotateZ << endl;

	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 200.0f);
	glm::mat4 vMat;
	vMat = camera.GetViewMatrix();

	glm::mat4 mMat, inverse_world;
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ));
	mMat = glm::translate(mMat, glm::vec3(*HeadPosX,*HeadPosY, *HeadPosZ));
	mMat = glm::scale(mMat, glm::vec3(0.2f, 0.2f, 0.2f));
	mMat = glm::rotate(mMat, -45.0f, glm::vec3(0, 1, 0)); // rotate on y


	// Write to shadow map texture by rendering the guides
	shadowMap_shader.use();
	// works for head
	//float nearPlane = -10.5f; // near plane distance
	float nearPlane = *nearPlaneHair; // near plane distance
	//float nearPlane = -20.5f; // near plane distance
	float farPlane = *farPlaneHair; // far plane distance
	float fov = 45.0f; // field of view in degrees

	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	// create the perspective projection matrix
	
	//lightProjection = glm::perspective(glm::radians(fov), 1.0f, nearPlane, farPlane);
	
	lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f,20.0f, nearPlane, farPlane);
	//lightProjection[2][2] *= -1.0f; // Flip the z-axis



	glm::vec3 right = cross(-lightNorm, glm::vec3(0.0, 1.0, 0.0));
	lightView = glm::mat4(
		right.x, right.y, right.z, 0.0f,
		0.0, 1.0, 0.0, 0.0f,
		-lightNorm.x, -lightNorm.y, -lightNorm.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);

	lightView = glm::lookAt(glm::vec3(0.0f) , glm::vec3(lightNorm.x , lightNorm.y, lightNorm.z), glm::vec3(0, 1, 0));

	lightSpaceMatrix = lightProjection * lightView ;



	glm::mat4 guides_mMat;
	guides_mMat = glm::translate(glm::mat4(1.0f), glm::vec3(objLocX, objLocY, objLocZ));
	guides_mMat = glm::scale(guides_mMat, glm::vec3(0.26f, 0.26f, 0.26f));
	guides_mMat = glm::rotate(guides_mMat, glm::radians(-145.0f), glm::vec3(0, 1, 0));
	guides_mMat = glm::rotate(guides_mMat, glm::radians(-90.0f), glm::vec3(1, 0, 0));
	guides_mMat = glm::translate(guides_mMat, glm::vec3(0.0f, -41.0f, -11.0f));

	
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



	// Render head depth map
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	shadowMap_head_shader.use();
	shadowMap_head_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shadowMap_head_shader.setMat4("model", mMat);
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	model.Draw(shadowMap_head_shader);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_CULL_FACE);

	/*
	computeShader.use();
	// Bind the texture as an image to be written to by the compute shader
	GLuint imageUnit = 0;
	glBindImageTexture(imageUnit, blurringMap, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R16F);
	computeShader.setInt("uDepthTexture", shadowMap);
	// Dispatch the compute shader
	glDispatchCompute(1024 / 16, 1024 / 16, 1); // Assuming a work group size of 16x16
	// Make sure writing to image has finished before reading
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	*/


	glBindFramebuffer(GL_READ_FRAMEBUFFER, shadowMapFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, resolvedFBO);
	glBlitFramebuffer(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT, 0, 0, SHADOW_WIDTH, SHADOW_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);


	// Opacity shadow map
	//glm::vec4 layerSize = glm::vec4(4.0,2.0,1.0,2.0);
	//glm::vec4 layerSize = glm::vec4(0.005, 0.01,0.02, 0.03);
	glm::vec4 layerSize = glm::vec4(*LayerSizeX, *LayerSizeY, *LayerSizeZ, *LayerSizeW);
	float opacity = 1.12;
	opacityShadowMap_shader.use();
	opacityShadowMap_shader.setMat4("model", guides_mMat);
	opacityShadowMap_shader.setMat4("view", vMat);
	opacityShadowMap_shader.setMat4("projection", projection);
	opacityShadowMap_shader.setMat4("light_view", lightView);
	opacityShadowMap_shader.setMat4("light_model", light_mMat);
	opacityShadowMap_shader.setMat4("light_projection", lightProjection);
	opacityShadowMap_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	opacityShadowMap_shader.setFloat("nearPlane", nearPlane);
	opacityShadowMap_shader.setFloat("farPlane", farPlane);
	opacityShadowMap_shader.setFloat("Opacity", *opacityVal);
	opacityShadowMap_shader.setInt("shadowMap", shadowMap);
	//opacityShadowMap_shader.setInt("shadowMap", resolvedDepth);
	opacityShadowMap_shader.SetVector4f("LayerSize",layerSize);
	
	glActiveTexture(GL_TEXTURE0 + shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	
	glBindFramebuffer(GL_FRAMEBUFFER, opacityFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// Enable blending
	glEnable(GL_BLEND);

	// Set the blending function and equation for additive blending
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	growth_mesh_guides.Draw();
	glDisable(GL_BLEND);
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	// End opacity Map
	
	// reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);




	shader.use();
	inverse_world = glm::inverse(mMat);

	// The normal matrix is defined as 'the transpose of the inverse of the upper-left 3x3 part of the model matrix
	// Used for non-uniform scaling of the model. In a uniform scaling, there is no visible result whether we use normal matrix or not
	glm::mat3 normal_matrix = glm::mat3(glm::transpose(glm::inverse(mMat)));

	//glm::mat4 model_view = vMat * mMat;
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	shader.setMat3("normal_matrix", normal_matrix);
	//shader.setMat4("model", guides_mMat);
	shader.setMat4("model", mMat);
	shader.setMat4("view", vMat);
	shader.setMat4("projection", projection);
	shader.SetVector3f("lightColor", light_color);
	shader.SetVector3f("camera_position", camera.Position);
	shader.setInt("shadowMap", shadowMap);
	//shader.setInt("shadowMap", resolvedDepth);
	shader.SetVector3f("lightPos", lightPos);
	shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	shader.setFloat("shadowBias", *headShadowBias);
	shader.setFloat("minBias", *minBias);
	shader.setFloat("specular_strength", 0.25);
	
	glActiveTexture(GL_TEXTURE0 + shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);	

	model.Draw(shader);
	glDisable(GL_CULL_FACE);



	// Render light cube
	light_shader.use();
	
	light_shader.setMat4("projection", projection);
	light_shader.setMat4("model", light_mMat);
	light_shader.setMat4("view", vMat);
	light.Draw(light_shader);

	

	glm::mat4 bspline_mat = glm::mat4(vec4(-1, 3, -3, 1),
		vec4(3, -6, 3, 0),
		vec4(-3, 0, 3, 0),
		vec4(1, 4, 1, 0));
	// Render guide lines

	//glm::vec3 camera_direction = -glm::vec3(vMat[2][0], vMat[2][1], vMat[2][2]);

	glm::vec3 camera_direction = glm::vec3(camera.Position.x, camera.Position.y , camera.Position.z + 5.0);
	mat4 viewMatrixInverse = glm::transpose(glm::inverse(vMat));
	glm::vec4 camera_position_view = viewMatrixInverse * glm::vec4(camera.Position,1.0);


	glm::mat3 guide_normal_matrix = glm::mat3(glm::transpose(glm::inverse(guides_mMat)));

	//glm::mat4 model_view = vMat * mMat;

	shader.setMat3("normal_matrix", normal_matrix);
	guide_shader.use();
	guide_shader.setMat4("model", guides_mMat);
	guide_shader.setMat3("guide_normal_matrix", guide_normal_matrix);
	guide_shader.setMat4("view", vMat);
	guide_shader.setMat4("projection", projection);
	guide_shader.setMat4("InverseWorldMatrix", inverse_world);
	guide_shader.SetVector3f("camera_front", camera.Front);
	guide_shader.SetVector3f("camera_right", camera.Right);
	guide_shader.SetVector3f("camera_position", camera.Position);
	guide_shader.SetVector3f("camera_position_view", camera_position_view);
	guide_shader.SetVector3f("camera_direction", camera_direction);
	guide_shader.setMat4("bspline_mat", bspline_mat);
	guide_shader.setInt("noiseTexture", noiseTexture);
	guide_shader.setInt("specularTexture", specularTexture);
	guide_shader.setInt("hairColorTexture", hairColorTexture);
	guide_shader.SetVector3f("lightPos", lightPos);
	guide_shader.setFloat("specular_strength", 0.5);
	guide_shader.SetVector3f("lightColor", light_color);
	guide_shader.SetVector3f("bboxMax", bboxMax);
	guide_shader.SetVector3f("bboxMin", bboxMin);
	guide_shader.setFloat("roughness", roughness);
	guide_shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	guide_shader.setInt("shadowMap", shadowMap);
	//guide_shader.setInt("shadowMap", resolvedDepth);
	guide_shader.setFloat("nearPlane", nearPlane);
	guide_shader.setFloat("farPlane", farPlane);
	guide_shader.setFloat("shadowBias", *shadowBias);
	guide_shader.setFloat("minBias", *minBias);
	guide_shader.setInt("opacityMap", opacityTexture);
	//guide_shader.setInt("varianceMap", varianceTexture);
	guide_shader.setInt("Marschner_M", Marschner_M);
	guide_shader.setInt("Marschner_N", Marschner_N);
	guide_shader.setMat4("light_model", light_mMat);
	guide_shader.SetVector4f("LayerSize", layerSize);

	glActiveTexture(GL_TEXTURE0 + noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	
	glActiveTexture(GL_TEXTURE0 + specularTexture);
	glBindTexture(GL_TEXTURE_2D, specularTexture);

	glActiveTexture(GL_TEXTURE0 + hairColorTexture);
	glBindTexture(GL_TEXTURE_2D, hairColorTexture);
	
	glActiveTexture(GL_TEXTURE0 + shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);

	glActiveTexture(GL_TEXTURE0 + opacityTexture);
	glBindTexture(GL_TEXTURE_2D, opacityTexture);
	/*
	glActiveTexture(GL_TEXTURE0 + varianceTexture);
	glBindTexture(GL_TEXTURE_2D, varianceTexture);
	*/
	glActiveTexture(GL_TEXTURE0 + Marschner_M);
	glBindTexture(GL_TEXTURE_2D, Marschner_M);

	glActiveTexture(GL_TEXTURE0 + Marschner_N);
	glBindTexture(GL_TEXTURE_2D, Marschner_N);

	glActiveTexture(GL_TEXTURE0 + additionalTexture);
	glBindTexture(GL_TEXTURE_2D, additionalTexture);


	// Render Hair
	//glEnable(GL_CULL_FACE);  // Enable face culling
	//glCullFace(GL_FRONT);   // Cull front faces

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
	//glDisable(GL_CULL_FACE);

	// Set the polygon mode to GL_FILL for both front and back faces
	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	growth_mesh_guides.Draw();
	glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);


	// Restore rendering settings
	glDisable(GL_BLEND);
	//glDisable(GL_CULL_FACE);



}



// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
	auto& io = ImGui::GetIO();
	if (io.WantCaptureMouse || io.WantCaptureKeyboard) {
		return;
	}

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

	glfwWindowHint(GLFW_SAMPLES, 8);

	// class GLFWwindow: Create a window that you can draw to
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Hair Loader", NULL, NULL);

	// Make the newly created window the current context (stores the entire OpenGL state)
	glfwMakeContextCurrent(window);

	// Initialize the GLEW library
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetMouseButtonCallback(window, mouse_callback2);
	//glfwSetCursorPosCallback(window, mouse_callback);

	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST); // enable depth testing
	glDepthFunc(GL_LESS);

	glEnable(GL_MULTISAMPLE); // enabled by default on some drivers, but not all so always enable to make sure

	// Disable vsync
	glfwSwapInterval(0);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 430");

	
	// Initialize the window instance
	init(window);


	Shader shader(vertex_shader_path, fragment_shader_path);
	Shader light_shader(light_vertex_shader_path, light_fragment_shader_path);
	Shader hair_shader(hair_vertex_shader_path, hair_fragment_shader_path);
	Shader guide_shader(guide_vertex_shader_path, guide_fragment_shader_path, guide_control_shader_path, guide_evaluation_shader_path,
		guide_geometry_shader_path);

	Shader compute_shader(compute_shader_path);

	Shader opacityShadowMap_shader(shadowMap_vertex_shader_path, opacityShadowMap_fragment_shader_path, shadowMap_control_shader_path, shadowMap_evaluation_shader_path, shadowMap_geometry_shader_path);
	Shader shadowMap_shader(shadowMap_vertex_shader_path, shadowMap_fragment_shader_path,shadowMap_control_shader_path,shadowMap_evaluation_shader_path, shadowMap_geometry_shader_path);
	Shader shadowMap_head_shader(shadowMap_head_vertex_shader_path, shadowMap_head_fragment_shader_path);

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
	cout << "noise texture int: " << noiseTexture << endl;*/

	specularTexture = Utils::loadTexture("C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Textures/shift.jpg");
	cout << "specular texture int: " << specularTexture << endl;

	//hairColorTexture = Utils::loadTexture("C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Textures/hairColor5.jpg");
	hairColorTexture = Utils::loadTexture("C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Textures/hairbase.jpg");
	cout << "hairColor texture int: " << hairColorTexture << endl;



	//glDepthFunc(GL_LESS);    // set the depth function to "less than"

	 GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	 
	// Shadow map creation
	
	 
	// create depth texture
	glGenTextures(1, &shadowMap);
	cout << "shadowMap: " << shadowMap << endl;
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, GL_TRUE);


	// create blurring texture
	glGenTextures(1, &resolvedDepth);
	glBindTexture(GL_TEXTURE_2D, resolvedDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D, 0);


	

	glGenTextures(1, &varianceTexture);
	glBindTexture(GL_TEXTURE_2D, varianceTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//glGenerateMipmap(GL_TEXTURE_2D_MULTISAMPLE);
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA, SHADOW_WIDTH, SHADOW_HEIGHT, GL_TRUE);
	
	
	Marschner_M = Utils::loadTexture("C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Textures/M.bmp");
	//Marschner_M = Utils::loadTexture("C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Textures/M_texture.png");
	Marschner_N = Utils::loadTexture("C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Textures/N_texture12.png");
	//Marschner_N = Utils::loadTexture("C:/Users/Panthelis/Desktop/Nvidia_HairWorks/Hair_Rendering_Research/Hair Samples (.hair file extension)/VisualStudio Code/HairLoader/Data/Textures/N.bmp");

	
	glGenTextures(1, &opacityTexture);
	cout << "opacityTexture: " << opacityTexture << endl;
	glBindTexture(GL_TEXTURE_2D, opacityTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA, SHADOW_WIDTH, SHADOW_HEIGHT, GL_TRUE);
	
	// attach depth texture as FBO's depth buffer
	glGenFramebuffers(1, &shadowMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, varianceTexture, 0);

	GLint depthAttachment = GL_DEPTH_ATTACHMENT;
	GLint depthTexture;
	glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, depthAttachment,GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,&depthTexture);
	if (depthTexture == 0) {
		std::cerr << "Depth texture is not attached to the framebuffer" << std::endl;
		// Handle error
		exit(-1);
	}
	/*
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	*/

	GLenum drawBuffersD[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffersD);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	glGenFramebuffers(1, &opacityFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, opacityFBO);
	// Attach the shadow map texture to the framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, opacityTexture, 0);
	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffers);
	// Check the framebuffer status for errors
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error: Opacity Framebuffer object is not complete." << std::endl;
		exit(-1);
	}
	

	glGenFramebuffers(1, &resolvedFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, resolvedFBO);
	// Attach the shadow map texture to the framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, resolvedDepth, 0);
	GLenum drawBuffersR[] = { GL_DEPTH_ATTACHMENT };
	glDrawBuffers(1, drawBuffersR);
	// Check the framebuffer status for errors
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Error: Resolved Framebuffer object is not complete." << std::endl;
		exit(-1);
	}


	// Unbind the framebuffer object and texture
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

	//Assume that we will never need more than 128 strands per triangle. Should me more than enough
	int maxHairPerTriangle = 128;
	generateBarycentrics(maxHairPerTriangle);
	// Generate and bind a buffer object
	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	// Allocate memory for the buffer object and fill it with data
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::vec3) * barycentrics.size(), barycentrics.data(), GL_STATIC_DRAW);
	// Bind the buffer object to a binding point in your shader
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo); // Binding point 16
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0); // unbind

	float minPos = -1.0;
	float maxPos = 1.0;

	float lightPosX = minPos;
	float lightPosY = minPos;
	float lightPosZ = minPos;

	float HeadPosX = -4.733;
	float HeadPosY = 0.035;
	float HeadPosZ = -7.135;

	float nearPlane = -19.5;
	float farPlane = 21.5;

	float minLayerSize = 0.0001;
	float maxLayerSize = 2.0;

	float LayerSizeX = 0.017;
	float LayerSizeY = 0.046;
	float LayerSizeZ = 0.072;
	float LayerSizeW = 0.091;

	float minOpacity = 0.01;
	float maxOpacity = 2.0;
	float opacityVal = 0.5;

	float shadowBias = 0.0001;
	float headShadowBias = 0.04;
	float minBias = 0.007;

	while (!glfwWindowShouldClose(window)) {

		auto& io = ImGui::GetIO();
		if (io.DeltaTime <= 0.0f) io.DeltaTime = 0.00001f;

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();


		// render your GUI
		ImGui::Begin("Hair Lighting");
		ImGui::SliderFloat("lightPosX", &lightPosX, minPos, maxPos);
		ImGui::SliderFloat("lightPosY", &lightPosY, minPos, maxPos);
		ImGui::SliderFloat("lightPosZ", &lightPosZ, minPos, maxPos);

		ImGui::SliderFloat("HeadPosX", &HeadPosX, -20, 20);
		ImGui::SliderFloat("HeadPosY", &HeadPosY, -20, 20);
		ImGui::SliderFloat("HeadPosZ", &HeadPosZ, -20, 20);

		ImGui::SliderFloat("nearPlane", &nearPlane, -40.0, 40.0);
		ImGui::SliderFloat("farPlane", &farPlane, -40.0, 40.0);

		ImGui::SliderFloat("LayerSizeX", &LayerSizeX, minLayerSize, maxLayerSize);
		ImGui::SliderFloat("LayerSizeY", &LayerSizeY, minLayerSize, maxLayerSize);
		ImGui::SliderFloat("LayerSizeZ", &LayerSizeZ, minLayerSize, maxLayerSize);
		ImGui::SliderFloat("LayerSizeW", &LayerSizeW, minLayerSize, maxLayerSize);

		ImGui::SliderFloat("shadowBias", &shadowBias, 0.0001, 0.1);
		ImGui::SliderFloat("headShadowBias", &headShadowBias, 0.0001, 0.1);
		ImGui::SliderFloat("minBias", &minBias, 0.0001, 0.1);

		ImGui::SliderFloat("Opacity", &opacityVal, minOpacity, maxOpacity);
		ImGui::End();
		


		processInput(window);
		display(window, glfwGetTime(), shader,light_shader,compute_shader,hair_shader, model,
				light,growth_mesh_guides,growth_mesh,guide_shader, shadowMap_shader, opacityShadowMap_shader,
			shadowMap_head_shader,&lightPosX, &lightPosY, &lightPosZ, &nearPlane, &farPlane,
			&LayerSizeX, &LayerSizeY, &LayerSizeZ, &LayerSizeW,&opacityVal, &HeadPosX, &HeadPosY, &HeadPosZ
			, &shadowBias, &minBias,&headShadowBias);


		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Outputs (paints) to the screen
		glfwSwapBuffers(window);

		 ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
