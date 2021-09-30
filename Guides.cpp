#include "Guides.h"

Guides::Guides(Model* growth, int* segments,int haircount,Hair* hairfile) {

	growth_mesh = growth;
	hairCount = haircount;

	line_segments = segments;

	hair = hairfile;

	growth_mesh_points = growth_mesh->GetVertices(&total_points);
}

void Guides::SelectGuidesFromHairfile(Hair& hair, float* roots) {

	//guide_points = new float[total_points * line_segments * 3];

	/*
	for each vertex in the growth mesh:
		-compute min distance between the vertex and all the hair roots (exact NN or approximate NN with LSH)
		-pick the discrete nearest neighbor line for that vertex
		-translate each point of the line in x,y,z so that the root sits exactly at the vertex
	*/

	// Initialize to zero

	short int* already_picked_line = new short int[hairCount]();

	double min_dist, distance;
	int min_index;


	ofstream myfile;
	myfile.open("distances.txt");


	int total_point_array_size = 0;

	float* hair_points = hair.GetAllHair();

	float x_growth, y_growth, z_growth, x_root, y_root, z_root;

	nearest_segments = new int[total_points];
	min_indices = new int[total_points/3];
	cout << "Total growth mesh points : " << total_points << endl;
	int seg_count = 0;

	cout << "Total hair roots to choose from : " << hairCount << endl;

	//for each vertex in the growth mesh
	for (int i = 0; i < total_points; i+=3) {

		min_dist = INT_MAX;
		min_index = 0;

		x_growth = growth_mesh_points[i];
		y_growth = growth_mesh_points[i + 1];
		z_growth = growth_mesh_points[i + 2];

		float ammountX, ammountY, ammountZ;
		float min_ammountX = 0.0, min_ammountY = 0.0, min_ammountZ = 0.0;

		//for each hair root
		for (int j = 0; j < 3 * hairCount; j+=3) {

			// Ignore already picked lines
			if (already_picked_line[j / 3] == 1) continue;

			x_root = roots[j];
			y_root = roots[j + 1];
			z_root = roots[j + 2];

			distance = Utils::Distance(x_growth,y_growth,z_growth , x_root,y_root,z_root, ammountX, ammountY, ammountZ);

			if (distance < min_dist) {
				min_dist = distance;
				min_index = j;

				min_ammountX = ammountX;
				min_ammountY = ammountY;
				min_ammountZ = ammountZ;
				myfile << "Curently minimum root x,y,z: " << x_root << "  " << y_root << "  " << z_root << " with distance: " << min_dist << " , move ammount: " << ammountX << "  " << ammountY << "  " << ammountZ << endl;
			}

		}

		already_picked_line[min_index / 3] = 1;
		myfile << "Vertex " << i/3 << " , Nearest Hair Root:  " << min_index << " , Min_distance : " << min_dist << " , x,y,z: " << x_growth << "  " << y_growth << "  " << z_growth << endl;

		// NN -> Nearest Neighbor
		int NN_segments = line_segments[min_index/3];
	
		min_indices[seg_count] = min_index / 3;

		float* line = new float[3*NN_segments];

		nearest_segments[seg_count] = NN_segments;
		++seg_count;

		int start_line = 0;

		// The min_index root is the nearest to the i-th vertex
		for (int k = 0; k < min_index/3; k++) {
			
			start_line += line_segments[k];
		}

		start_line = 3 * start_line; // for each x,y,z coordinate
		total_point_array_size += NN_segments;

		int count = 0;
		myfile << "Moving all hair points by : " << min_ammountX << "  " << min_ammountY << "  " << min_ammountZ << endl;

		for (int g = start_line; g < start_line + 3*NN_segments; g+=3) {
			

			// Moving each point in order to sit exactly at the vertex of the growth mesh
			line[count] = hair_points[g] + min_ammountX;
			line[count + 1] = hair_points[g + 1] + min_ammountY;
			line[count + 2] = hair_points[g + 2] + min_ammountZ;

			myfile << line[count] << "  " << line[count + 1] << "  " << line[count + 2] << endl;
			count += 3;


		}
		
		guide_points.push_back(line);
	}

	FillPointArray(total_point_array_size);

	total_hair_points = total_point_array_size;
	cout << "Total hair points " << total_hair_points << endl;
	myfile.close();
}


void Guides::FillPointArray(int total_points) {

	points = new float[total_points*3];

	cout << "Total points : " << total_points << endl;

	ofstream myfile;
	myfile.open("guides.txt");


	int count = 0;
	int pointcount = 0;
	for (auto it = guide_points.begin(); it != guide_points.end(); it++) {
		
		float* arr = *it;

		for (int i = 0; i < 3*nearest_segments[count]; i+=3) {

			points[pointcount] = arr[i];
			points[pointcount + 1] = arr[i + 1];
			points[pointcount + 2] = arr[i + 2];

			myfile << points[pointcount] << "  " << points[pointcount + 1] << "  " << points[pointcount + 2] << endl;

			pointcount += 3;

		}
		
		++count;
	}
	myfile.close();
}


void Guides::Fill_Struct() {

	nearest_transparency = new float[total_hair_points];
	nearest_thickness = new float[total_hair_points];
	nearest_colors = new float[3*total_hair_points];

	float* original_thickness = hair->GetThickness();
	float* original_colors = hair->GetColors();
	float* original_transparency = hair->GetTransparency();

	int count = 0;
	int color_count = 0;

	for (int i = 0; i < total_points/3; i++) {

		int min_index = min_indices[i];
		int color_min_index = 3*min_index;

		for (int j = 0; j < nearest_segments[i]; j++) {
			Per_Vertex_Attribute attrib;

			nearest_colors[color_count] = original_colors[color_min_index];
			nearest_colors[color_count + 1] = original_colors[color_min_index + 1];
			nearest_colors[color_count + 2] = original_colors[color_min_index + 2];
			
			nearest_thickness[count] = original_thickness[min_index];
			nearest_transparency[count] = original_transparency[min_index];

			attrib.segment_index = j + 1;
			attrib.thickness = nearest_thickness[count];
			attrib.transparency = nearest_transparency[count];
			
			attrib.color[0] = nearest_colors[color_count];
			attrib.color[1] = nearest_colors[color_count + 1];
			attrib.color[2] = nearest_colors[color_count + 2];

			//cout << attrib.color[0] << " " << attrib.color[1] << "  " << attrib.color[2] << endl;

			attributes.push_back(attrib);
			++count;
			++min_index;

			color_count += 3;
			color_min_index += 3;
		}
	}
	cout << "COUNT: " << count << endl;

}


void Guides::SetupGuides() {

	Fill_Struct();

	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);

	// Bind the current VAO. Every subsequent function call will affect the currently bound VAO
	glBindVertexArray(VAO);
	cout << "total_hair_points : " << total_hair_points << endl;

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 3*total_hair_points * sizeof(float), this->points, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, attributes.size() * sizeof(Per_Vertex_Attribute), &attributes[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	// vertex positions
	glEnableVertexAttribArray(0);
	// Insert vertex data at the "location = 0" with size 3 (3 vertex locations), and offset 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	// vertex positions
	glEnableVertexAttribArray(1);
	// Insert vertex data at the "location = 0" with size 3 (3 vertex locations), and offset 0
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, color));

	glEnableVertexAttribArray(2);
	// Insert vertex data at the "location = 0" with size 3 (3 vertex locations), and offset 0
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, transparency));

	glEnableVertexAttribArray(3);
	// Insert vertex data at the "location = 0" with size 3 (3 vertex locations), and offset 0
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, segment_index));
	
	glEnableVertexAttribArray(4);
	// Insert vertex data at the "location = 0" with size 3 (3 vertex locations), and offset 0
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, thickness));


	// Unbind the VAO, so that a function doesn't accidentally change the VAO
	glBindVertexArray(0);

}


void Guides::Draw() {

	glBindVertexArray(VAO);
	//int growth_mesh_hair_count = total_hair_points / line_segments[0];
	int pointIndex = 0;

	glDrawArrays(GL_LINE_STRIP, 0,  total_hair_points);

	
	/*for (int i = 0; i < growth_mesh_hair_count; i++) {
		//glLineWidth(2.0);
		glDrawArrays(GL_LINE_STRIP, pointIndex, line_segments[0]);
		pointIndex += line_segments[0];
	}*/
	
	glBindVertexArray(0);
}

Guides::~Guides() {


}



