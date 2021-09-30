#include "Hair.h"

using namespace std;

Hair::Hair(const char* filename) {
	LoadHairModel(filename);
}

void Hair::LoadHairModel(const char* filename) {
	// Load the hair model
	int result = hairfile.LoadFromFile(filename);

	cout << "Printing header arrays: " << hairfile.GetHeader().arrays << endl;

	if (hairfile.GetHeader().arrays & _CY_HAIR_FILE_SEGMENTS_BIT) cout << "File has segments array" << endl;
	if (hairfile.GetHeader().arrays & _CY_HAIR_FILE_POINTS_BIT) cout << "File has points array" << endl;
	if (hairfile.GetHeader().arrays & _CY_HAIR_FILE_THICKNESS_BIT) cout << "File has thickness array" << endl;
	if (hairfile.GetHeader().arrays & _CY_HAIR_FILE_TRANSPARENCY_BIT) cout << "File has transparency array" << endl;
	if (hairfile.GetHeader().arrays & _CY_HAIR_FILE_COLORS_BIT) cout << "File has colors array" << endl;

	// Check for errors
	switch (result) {
	case CY_HAIR_FILE_ERROR_CANT_OPEN_FILE:
		printf("Error: Cannot open hair file!\n");
		return;
	case CY_HAIR_FILE_ERROR_CANT_READ_HEADER:
		printf("Error: Cannot read hair file header!\n");
		return;
	case CY_HAIR_FILE_ERROR_WRONG_SIGNATURE:
		printf("Error: File has wrong signature!\n");
		return;
	case CY_HAIR_FILE_ERROR_READING_SEGMENTS:
		printf("Error: Cannot read hair segments!\n");
		return;
	case CY_HAIR_FILE_ERROR_READING_POINTS:
		printf("Error: Cannot read hair points!\n");
		return;
	case CY_HAIR_FILE_ERROR_READING_COLORS:
		printf("Error: Cannot read hair colors!\n");
		return;
	case CY_HAIR_FILE_ERROR_READING_THICKNESS:
		printf("Error: Cannot read hair thickness!\n");
		return;
	case CY_HAIR_FILE_ERROR_READING_TRANSPARENCY:
		printf("Error: Cannot read hair transparency!\n");
		return;
	default:
		printf("Hair file \"%s\" loaded.\n", filename);
	}
	int hairCount = hairfile.GetHeader().hair_count;
	int pointCount = hairfile.GetHeader().point_count;
	printf("Number of hair strands = %d\n", hairCount);
	printf("Number of hair points = %d\n", pointCount);
	cout << "Number of (default) segments = " << hairfile.GetHeader().d_segments << endl;

	dirs = new float[pointCount * 3];

	// Compute directions
	if (hairfile.FillDirectionArray(dirs) == 0) {
		printf("Error: Cannot compute hair directions!\n");
	}

}

void Hair::SetupHair() {

	float* points = hairfile.GetPointsArray();
	float* colors = hairfile.GetColorsArray();
	float* transparency = hairfile.GetTransparencyArray();
	float* thickness = hairfile.GetThicknessArray();

	if (!colors) cout << "No colors loaded??!!?" << endl;
	if (!thickness) cout << "No thickness loaded??!!?" << endl;
	if (!hairfile.GetSegmentsArray()) cout << "No segments loaded??!!?" << endl;

	if (!transparency) cout << "No transparency loaded??!!?" << endl;
	cout << "Default Transparency : " << hairfile.GetHeader().d_transparency << endl;

	//cout << "Size of points : " << hairfile.GetHeader().point_count << endl;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(4, VBO);

	// Bind the current VAO. Every subsequent function call will affect the currently bound VAO
	glBindVertexArray(VAO);


	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, hairfile.GetHeader().point_count * 3 * sizeof(float), points, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	// vertex positions
	glEnableVertexAttribArray(0);
	// Insert vertex data at the "location = 0" with size 3 (3 vertex locations), and offset 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	// different vbo for colors since we use a different array and not a struct that contains everything inside
	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, hairfile.GetHeader().point_count * 3* sizeof(float), colors, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


	// transfer transparency data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	glBufferData(GL_ARRAY_BUFFER, hairfile.GetHeader().point_count * sizeof(float), transparency, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,  sizeof(float), (void*)0);

	// transfer thickness data
	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);
	glBufferData(GL_ARRAY_BUFFER, hairfile.GetHeader().point_count * sizeof(float), thickness, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[3]);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);


	// Unbind the VAO, so that a function doesn't accidentally change the VAO
	glBindVertexArray(0);

}

void Hair::WriteSegmentsArray() {

	ofstream myfile;
	myfile.open("segments.txt");

	int* segments = new int[hairfile.GetHeader().hair_count];
	unsigned short* file_segments = hairfile.GetSegmentsArray();

	for (int i = 0; i < hairfile.GetHeader().hair_count; i++) {
		if (file_segments) segments[i] = file_segments[i] + 1;
		else segments[i] = hairfile.GetHeader().d_segments + 1;

		myfile << segments[i] << endl;
	}

	delete[] segments;
	myfile.close();

}

float* Hair::GetColors() {

	//ofstream myfile;
	//myfile.open("original_colors.txt");

	float* colors = new float[3*hairfile.GetHeader().point_count];
	float* file_colors = hairfile.GetColorsArray();

	int color_index = 0;

	for (int i = 0; i < 3*hairfile.GetHeader().point_count; i+=3) {


			colors[color_index] = file_colors[i];
			colors[color_index+1] = file_colors[i+1];
			colors[color_index+2] = file_colors[i+2];

			//myfile << colors[color_index] << "   " << colors[color_index + 1] << "   " << colors[color_index + 2] << endl;

			color_index += 3;

	}

	//myfile.close();
	return colors;

}

float* Hair::GetTransparency() {

	float* transparency = new float[hairfile.GetHeader().point_count];
	float* file_transparency = hairfile.GetTransparencyArray();

	for (int i = 0; i < hairfile.GetHeader().point_count; i++) {

		if (file_transparency) transparency[i] = file_transparency[i];
		else transparency[i] = hairfile.GetHeader().d_transparency;
	}

	return transparency;
}


float* Hair::GetThickness() {

	float* thickness = new float[hairfile.GetHeader().point_count];
	float* file_thickness = hairfile.GetThicknessArray();

	for (int i = 0; i < hairfile.GetHeader().point_count; i++) {

		if (file_thickness) thickness[i] = file_thickness[i];
		else thickness[i] = hairfile.GetHeader().d_thickness;
	}

	return thickness;
}

int* Hair::GetSegments() {

	/*
	If the hairfile has a segment array then create a new array 
	and copy the segments array
	Else, for each element copy the default segments
	*/

	int* segments = new int[hairfile.GetHeader().hair_count];
	unsigned short* file_segments = hairfile.GetSegmentsArray();

	for (int i = 0; i < hairfile.GetHeader().hair_count; i++) {

		if (file_segments) segments[i] = file_segments[i] + 1 ;
		else segments[i] = hairfile.GetHeader().d_segments + 1;
	}

	return segments;
}

void Hair::WriteRootOutput() {

	int pointCount = hairfile.GetHeader().point_count;
	float x, y, z;
	int segment_stride;
	float* points = hairfile.GetPointsArray();

	ofstream myfile;
	myfile.open("roots.txt");

	// Segments for each hair 
	unsigned short* segments = hairfile.GetSegmentsArray();

	if (segments) segment_stride = segments[0];
	else segment_stride = hairfile.GetHeader().d_segments + 1;
	
	int hairCount = hairfile.GetHeader().hair_count;
	roots = new float[ hairCount* 3];

	cout << "SEGMENT STRIDE : " << segment_stride << endl;
	// x y z coordinates for each root vertex
	int count = 0;

	for (int i = 0; i < pointCount*3; i += 3*segment_stride) {
		
		x = points[i];
		y = points[i + 1];
		z = points[i + 2];

		roots[count] = x;
		roots[count + 1] = y;
		roots[count + 2] = z;
		count += 3;

		myfile << x << " " << y << " " << z << endl;
	}

	myfile.close();
}


void Hair::DrawHair() {

	glBindVertexArray(VAO);

	// Draw arrays
	int pointIndex = 0;
	int hairCount = hairfile.GetHeader().hair_count;
	const unsigned short* segments = hairfile.GetSegmentsArray();

	/*
	- Lines : faster to render, but no real life width and no 2d textures (only 1d)
	- Polygons : real life width and 2d textures, but slower to render
	

	Target : From the generated guide hair, use tesselation to create interpolated hair on the GPU 
	*/

	// Way too costly. Can be done with a single draw call and geometry shader to 
	// differentiate each strand from another 
	if (segments) {
		// If segments array exists
		for (int hairIndex = 0; hairIndex < hairCount; hairIndex++) {
			glDrawArrays(GL_LINE_STRIP, pointIndex, segments[hairIndex] + 1);
			pointIndex += segments[hairIndex] + 1;
		}
	}
	else {
		// If segments array does not exist, use default segment count
		int dsegs = hairfile.GetHeader().d_segments;
		for (int hairIndex = 0; hairIndex < hairCount; hairIndex++) {
			glLineWidth(hairfile.GetHeader().d_thickness);
			glDrawArrays(GL_LINE_STRIP, pointIndex, dsegs + 1);
			pointIndex += dsegs + 1;
		}
	}
	glBindVertexArray(0);
}


float* Hair::GetRoots() {
	return roots;
}

int Hair::GetHairCount() {
	return hairfile.GetHeader().hair_count;
}

float Hair::Default_Transparency() {
	return hairfile.GetHeader().d_transparency;
}

float* Hair::GetAllHair() {
	return hairfile.GetPointsArray();
}

Hair::~Hair() {

}