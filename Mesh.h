
#ifndef MESH_H
#define MESH_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include "Shader.h"

#include <string>
#include <iostream>
#include <vector>

using namespace std;


struct Vertex {

	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};



struct Texture {
	unsigned int id;
	string type; // diffuse, specular etc.
	string path;
	glm::vec3 kd = glm::vec3(0.0f, 0.0f, 0.0f);
};



class Mesh {

	public:
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
	
	
		Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);

		// draws current mesh using a shader
		void Draw(Shader& shader);
		float* GetVertices(int* growth_mesh_points);

	private:
		//  Render data. Each mesh has its own vao, vbo and ebo
		GLuint VAO, VBO[2], EBO;
		// Initialize buffers
		void setupMesh();

};


#endif