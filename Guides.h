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
#include <unordered_map>


struct Per_Vertex_Attribute {

	float segment_index;
	float transparency;
	float color[3];
	float thickness;

	glm::vec3 barycentrics;
	glm::vec3 normal3;
	glm::vec3 normal_neighb1;
	glm::vec3 normal_neighb2;
	glm::vec3 normal_neighb3;

	glm::vec3 guide_neighb2;
	glm::vec3 guide_neighb3;
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
	int* nearest_segments;
	float* nearest_thickness;
	float* nearest_colors;
	float* nearest_transparency;
	int* min_indices;
	int total_hair_points;

	vector<unsigned int> indices;
	vector<unsigned int> growth_mesh_indices;
	std::vector<glm::vec3> barycentrics;
	int maxHairPerTriangle;
	vector<vector<int>> neighbors;
	std::vector<Vertex> all_vertices;

	vector<Per_Vertex_Attribute> attributes;

	GLuint VAO,EBO;
	GLuint VBO[3];

	std::unordered_map<int, std::unordered_set<int>> adjacency_map;

	//Guides(Model* growth, int* segments, int haircount,Hair* hairfile);
	Guides(Model* growth, int* segments, int haircount,Hair* hairfile);
	~Guides();
	void FillPoints();
	void SelectGuidesFromHairfile(Hair& hair, float* roots);
	void Print_Growth_Mesh_Verts();
	void FillPointArray(int total_points);
	void Fill_Struct();
	void Fill_Indices(int total_point_array_size);
	void FindBoundingBox();
	void SetupGuides();
	void Draw();
	void DrawRegular();
	void Pick_Neighbor_Indices();
	void generateBarycentrics(int N);

	void Pick_Neighbor_Indices2();
	void update_neighbors(std::unordered_map<int, std::unordered_set<int>>& adjacency_map,
		const std::unordered_map<int, std::unordered_set<int>>& vertex_to_triangle,
		int vertex, int neighbor, int current_triangle);
};

#endif