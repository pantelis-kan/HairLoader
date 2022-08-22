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


struct Per_Vertex_Attribute {

	float segment_index;
	float transparency;
	float color[3];
	float thickness;
	float direction;
};

class Guides {

public:
	/* 
	A collection of x,y,z coordinates of lines 
	Each element in the vector points to a line (an array of x,y,z coordinates)
	We use a vector because we don't know the number of segments each hair strand has
	*/
	std::vector<float*> guide_points;

	int hairCount;
	float* growth_mesh_points;

	// segments for each line 
	int* line_segments;
	int total_points;
	Model* growth_mesh;
	Hair* hair;

	float* points;
	float* tangents;
	int* nearest_segments;
	float* nearest_thickness;
	float* nearest_colors;
	float* nearest_transparency;
	float* nearest_directions;
	int* min_indices;
	int total_hair_points;

	vector<Per_Vertex_Attribute> attributes;

	GLuint VAO;
	GLuint VBO[3];

	Guides(Model* growth, int* segments, int haircount,Hair* hairfile);
	~Guides();
	void SelectGuidesFromHairfile(Hair& hair, float* roots);
	void FillPointArray(int total_points);
	void Fill_Struct();
	void Fill_Tangents(int total_point_array_size);
	void SetupGuides();
	void Draw();
};

#endif