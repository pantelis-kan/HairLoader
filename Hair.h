#ifndef HAIR_H
#define HAIR_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include "Shader.h"
#include "cyHairFile.h"
#include "Model.h"

#include <string>
#include <iostream>
#include <vector>
#include <fstream>


class Hair {

	public:
		Hair(const char* filename);
		void LoadHairModel(const char* filename);
		void SetupHair();
		void DrawHair();
		float Default_Transparency();
		void WriteRootOutput();
		int* GetSegments();
		float* GetRoots();
		float* GetAllHair();
		int GetHairCount();
		float* GetThickness();
		void WriteSegmentsArray();
		float* GetColors();
		float* GetTransparency();
		~Hair();

	private:

		cyHairFile hairfile;
		GLuint VAO, VBO[4]; // we don't need indices, since each strand of hair doesn't share vertices with another strand
		
		float* dirs; // Normal directions for shading

		float* roots;

};

#endif