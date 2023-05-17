#ifndef GUIDES_H
#define GUIDES_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include "Shader.h"
#include "cyHairFile.h"
#include "Model.h"
#include "Hair.h"

#include "Utils.h"
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <unordered_set>


struct Per_Vertex_Attribute {

	float segment_index;
	float transparency;
	float color[3];
	float thickness;
	float direction;

	glm::vec3 neighbor1;
	glm::vec3 neighbor2;
	glm::vec3 neighbor3;

	glm::vec3 barycentrics;
	glm::vec3 normal1;
	glm::vec3 normal2;
	glm::vec3 normal3;
};

class Guides {

public:
	/* 
	A collection of x,y,z coordinates of lines 
	Each element in the vector points to a line (an array of x,y,z coordinates)
	We use a vector because we don't know the number of segments each hair strand has
	*/
	std::vector<float*> guide_points;
	std::vector<glm::vec3> guide_normals;

	int hairCount;
	float* growth_mesh_points;
	float* growth_mesh_normals;

	// segments for each line 
	int* line_segments;
	int total_points;
	Model* growth_mesh;
	Hair* hair;

	glm::vec3 bboxMin = glm::vec3(1000000.0);
	glm::vec3 bboxMax = glm::vec3(-1000000.0);;


	float* points;
	float* tangents;
	int* nearest_segments;
	float* nearest_thickness;
	float* nearest_colors;
	float* nearest_transparency;
	float* nearest_directions;
	int* min_indices;
	int total_hair_points;

	vector<unsigned int> indices;
	vector<unsigned int> growth_mesh_indices;
	vector<vector<int>> neighbors;
	std::vector<Vertex> all_vertices;

	vector<Per_Vertex_Attribute> attributes;

	GLuint VAO,EBO;
	GLuint VBO[3];

	//Guides(Model* growth, int* segments, int haircount,Hair* hairfile);
	Guides(Model* growth, int* segments, int haircount,Hair* hairfile);
	~Guides();
	void FillPoints();
	void SelectGuidesFromHairfile(Hair& hair, float* roots);
	void Pick_Neighbor_Indices();
	void Print_Growth_Mesh_Verts();
	void FillPointArray(int total_points);
	void Fill_Struct();
	void Fill_Tangents(int total_point_array_size);
	void Fill_Indices(int total_point_array_size);
	void FindBoundingBox();
	void SetupGuides();
	void Draw();
	void DrawRegular();
};

#endif