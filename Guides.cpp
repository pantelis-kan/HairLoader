#include "Guides.h"

using namespace std;
using glm::max;
using glm::min;

Guides::Guides(Model* growth, int* segments,int haircount, Hair* hairfile) {

	growth_mesh = growth;
	hairCount = haircount;
	hair= hairfile;

	line_segments = segments;

	all_vertices = growth_mesh->GetVertices(total_points);

	growth_mesh_points = new float[3 * total_points];
	growth_mesh_normals = new float[3 * total_points];

	FillPoints();
	//growth_mesh_points = growth_mesh->GetVertices(total_points);
	Print_Growth_Mesh_Verts();
	growth_mesh_indices = growth_mesh->GetIndices();

}

void Guides::FindBoundingBox() {

	int count = 0;


	for (int i = 0; i < this->total_points / 3; i++) {
		for (int j = 0; j < nearest_segments[i]; j++) {

			glm::vec3 point = glm::vec3(points[count], points[count+1], points[count+2]);

			bboxMin = min(bboxMin, point);
			bboxMax = max(bboxMax, point);

			count += 3;
		}
	}
}

void Guides::FillPoints() {

	std::map<tuple<float, float, float>, int> check_duplicates;

	ofstream myfile;
	myfile.open("growth_mesh_normals.txt");

	float x, y, z, x_norm, y_norm, z_norm;
	int count = 0;

	for (int i = 0; i < total_points; i++) {

		x = all_vertices[i].Position.x;
		y = all_vertices[i].Position.y;
		z = all_vertices[i].Position.z;

		x_norm = all_vertices[i].Normal.x;
		y_norm = all_vertices[i].Normal.y;
		z_norm = all_vertices[i].Normal.z;

		// Each vertex is written as many times as the number of edges that
		// leave from this vertex. This is why we need to remove duplicate vertices
		if (!check_duplicates.empty()) {

			if (check_duplicates.find(make_tuple(x, y, z)) != check_duplicates.end()) continue;
		}

		check_duplicates[make_tuple(x, y, z)] = count;

		growth_mesh_points[count] = x;
		growth_mesh_points[count + 1] = y;
		growth_mesh_points[count + 2] = z;

		growth_mesh_normals[count] = x_norm;
		growth_mesh_normals[count + 1] = y_norm;
		growth_mesh_normals[count + 2] = z_norm;


		myfile << x_norm << " " << y_norm << " " << z_norm << endl;

		count += 3;
	}




	myfile.close();

}

void Guides::Print_Growth_Mesh_Verts() {

	ofstream myfile;
	myfile.open("growth_mesh_vertices.txt");
	
	for (int i = 0; i < total_points * 3; i+=3) {

		myfile << growth_mesh_points[i] << " " << growth_mesh_points[i + 1] << " " << growth_mesh_points[i+2] << " " << endl;
	}

	myfile.close();
}

void Guides::SelectGuidesFromHairfile(Hair& hair, float* roots) {

	//guide_points = new float[total_points * line_segments * 3];

	/*
	for each vertex in the growth mesh:
		-compute min distance between the vertex and all the hair roots (exact NN)
		-pick the discrete nearest neighbor line for that vertex
		-translate each point of the line in x,y,z so that the root sits exactly at the vertex
	*/

	// Initialize to zero

	 int* already_picked_line = new  int[hairCount]();

	double min_dist, distance;
	int min_index;

	if (roots == NULL) cout << "Roots is NULL?????" << endl;
	cout << "Total growth mesh points : " << total_points << endl;

	ofstream myfile,myfileN;
	myfile.open("moved_guide_hair.txt");
	myfileN.open("normals.txt");


	int total_point_array_size = 0;

	float* hair_points = hair.GetAllHair();

	float x_growth, y_growth, z_growth, x_root, y_root, z_root, x_norm,y_norm,z_norm;

	nearest_segments = new int[total_points];
	//min_indices = new int[total_points/3];
	min_indices = new int[total_points];
	
	int seg_count = 0;

	cout << "Total hair roots to choose from : " << hairCount << endl;

	//for each vertex in the growth mesh
	for (int i = 0; i < 3*total_points; i+=3) {

		min_dist = INT_MAX;
		min_index = 0;

		x_growth = growth_mesh_points[i];
		y_growth = growth_mesh_points[i + 1];
		z_growth = growth_mesh_points[i + 2];

		x_norm = growth_mesh_normals[i];
		y_norm = growth_mesh_normals[i + 1];
		z_norm = growth_mesh_normals[i + 2];

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
				//myfile << "Curently minimum root x,y,z: " << x_root << "  " << y_root << "  " << z_root << " with distance: " << min_dist << " , move ammount: " << ammountX << "  " << ammountY << "  " << ammountZ << "  Growth mesh vertex: " << x_growth << "  " << y_growth << "  " << z_growth <<  endl;
			}

		}

		already_picked_line[min_index / 3] = 1;
		myfile << "Vertex " << i/3 << " , Nearest Hair Root:  " << min_index / 3 << " , Min_distance : " << min_dist << " , x,y,z: " << x_growth << "  " << y_growth << "  " << z_growth << endl;

		// NN -> Nearest Neighbor
		int NN_segments = line_segments[min_index/3];
	
		min_indices[seg_count] = min_index / 3;

		float* line = new float[3*NN_segments];
		float* normal = new float[3*NN_segments];

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
		//myfile << "Moving all hair points by : " << min_ammountX << "  " << min_ammountY << "  " << min_ammountZ << endl;

		for (int g = start_line; g < start_line + 3*NN_segments; g+=3) {
			

			// Moving each point in order to sit exactly at the vertex of the growth mesh
			line[count] = hair_points[g] + min_ammountX;
			line[count + 1] = hair_points[g + 1] + min_ammountY;
			line[count + 2] = hair_points[g + 2] + min_ammountZ;

			normal[count] = x_norm;
			normal[count + 1] = y_norm;
			normal[count + 2] = z_norm;

			guide_normals.push_back(glm::vec3(x_norm,y_norm,z_norm));

			//myfile << line[count] << "  " << line[count + 1] << "  " << line[count + 2] << endl;
			myfileN << normal[count] << "  " << normal[count + 1] << "  " << normal[count + 2] << endl;
			count += 3;


		}
		
		guide_points.push_back(line);

	
		//guide_normals.push_back(normal);
	}


	total_hair_points = total_point_array_size;
	cout << "Total hair points " << total_hair_points << endl;

	FillPointArray(total_point_array_size);
	//Fill_Tangents(total_point_array_size);
	Fill_Indices(total_point_array_size);

	myfile.close();
	myfileN.close();
}


void Guides::Fill_Indices(int total_points) {

	int count = 0;

	int p0 = 0;
	int p1 = 1;
	int p2 = 2;
	int p3 = 3;

	for (int i = 0; i < this->total_points ; i++) {

		for (int j = 0; j < nearest_segments[i]-1; j++) {
			
			// special treatment for first and last vertex of the strand
			if (j == 0) {
				indices.push_back(p0);
				indices.push_back(p0);
				indices.push_back(p1);
				indices.push_back(p2);

				--p0; --p1; --p2; --p3;
			}
			else if (j == nearest_segments[i]-2) {
				indices.push_back(p0);
				indices.push_back(p1);
				indices.push_back(p2);
				indices.push_back(p2);
				p0 += 2; p1 += 2; p2 += 2; p3 += 2;
			}
			else {
				indices.push_back(p0);
				indices.push_back(p1);
				indices.push_back(p2);
				indices.push_back(p3);

			}
			

				++count; ++p0; ++p1; ++p2; ++p3;
				
		}

	}
	cout << "Number of indices : " << indices.size() << endl;
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


// Helper function to update the neighbors of a vertex
void Guides::update_neighbors(std::unordered_map<int, std::unordered_set<int>>& adjacency_map,
	const std::unordered_map<int, std::unordered_set<int>>& vertex_to_triangle,
	int vertex, int neighbor, int current_triangle) {
	// Check if the current vertex already has two neighbors
	if (adjacency_map[vertex].size() < 2) {
		// Add the neighbor if it's not already a neighbor of the current vertex
		if (adjacency_map[vertex].find(neighbor) == adjacency_map[vertex].end()) {
			// Check if the neighboring vertex is not connected to the current vertex through another triangle
			bool connected = false;
			for (int triangle : vertex_to_triangle.at(vertex)) {
				if (triangle != current_triangle &&
					(adjacency_map[vertex].count(growth_mesh_indices[3 * triangle]) ||
						adjacency_map[vertex].count(growth_mesh_indices[3 * triangle + 1]) ||
						adjacency_map[vertex].count(growth_mesh_indices[3 * triangle + 2]))) {
					connected = true;
					break;
				}
			}
			if (!connected) {
				adjacency_map[vertex].insert(neighbor);
			}
		}
	}
}


void Guides::Pick_Neighbor_Indices2()
{

	std::unordered_map<int, std::unordered_set<int>> vertex_to_triangle;

	// Iterate through each triangle in the mesh
	for (size_t i = 0; i < growth_mesh_indices.size(); i += 3) {
		// Extract the indices of the vertices for the current triangle
		int v1 = growth_mesh_indices[i];
		int v2 = growth_mesh_indices[i + 1];
		int v3 = growth_mesh_indices[i + 2];

		// Update the triangle information for each vertex
		vertex_to_triangle[v1].insert(i / 3);
		vertex_to_triangle[v2].insert(i / 3);
		vertex_to_triangle[v3].insert(i / 3);
	}

	// Iterate through each triangle in the mesh
	for (size_t i = 0; i < growth_mesh_indices.size(); i += 3) {
		// Extract the indices of the vertices for the current triangle
		int v1 = growth_mesh_indices[i];
		int v2 = growth_mesh_indices[i + 1];
		int v3 = growth_mesh_indices[i + 2];

		// Update the adjacency information for each vertex
		update_neighbors(adjacency_map, vertex_to_triangle, v1, v2, i / 3);
		update_neighbors(adjacency_map, vertex_to_triangle, v1, v3, i / 3);
		update_neighbors(adjacency_map, vertex_to_triangle, v2, v1, i / 3);
		update_neighbors(adjacency_map, vertex_to_triangle, v2, v3, i / 3);
		update_neighbors(adjacency_map, vertex_to_triangle, v3, v1, i / 3);
		update_neighbors(adjacency_map, vertex_to_triangle, v3, v2, i / 3);
	}


}



void Guides::Pick_Neighbor_Indices() {

	ofstream myfile,neighbor_file;
	myfile.open("growth_mesh_indices.txt");
	neighbor_file.open("neighbors_all.txt");
	cout << "growth_mesh_indices size: " << growth_mesh_indices.size() << endl;

	int combinations_size = growth_mesh_indices.size() / 3;
	int** combinations = new int* [combinations_size];


	for (int i = 0; i < combinations_size; i++) {
		combinations[i] = new int[3];
	}

	vector<vector<int>> picked_combinations;

	int count = 0;
	for (int i = 0; i < growth_mesh_indices.size(); i += 3) {

		int ind1 = growth_mesh_indices[i];
		int ind2 = growth_mesh_indices[i + 1];
		int ind3 = growth_mesh_indices[i + 2];

		combinations[count][0] = ind1;
		combinations[count][1] = ind2;
		combinations[count][2] = ind3;

		myfile << combinations[count][0] <<  "  " << combinations[count][1] << "  " <<  combinations[count][2] << endl;
		++count;
	}


	int picked_counter = 0;

	for (int i = 0; i < this->total_points; i++) {

		for (int j = 0; j < count; j++) {

			int ind1 = combinations[j][0];
			int ind2 = combinations[j][1];
			int ind3 = combinations[j][2];

			bool invalid_neighbor = false;

			// If index is found in a combination
			if (i == ind1 || i == ind2 || i == ind3) {

				//cout << "Picked combinations size: " << picked_combinations.size() << endl;
				// and this combination is not already picked
				for (int k = 0; k < picked_combinations.size(); k++) {

					int picked1 = picked_combinations[k][0];
					int picked2 = picked_combinations[k][1];
					int picked3 = picked_combinations[k][2];

					unordered_set<int> set1;
					unordered_set<int> set2;

					set1.insert(ind1); set1.insert(ind2); set1.insert(ind3);
					set2.insert(picked1); set2.insert(picked2); set2.insert(picked3);

					if (set1 == set2) invalid_neighbor = true;

				}


				if (invalid_neighbor) {
					/*
					vector<int> neighbor_insert;
					neighbor_insert.push_back(-1);
					neighbor_insert.push_back(-1);
					neighbors.push_back(neighbor_insert);
					*/
						continue;
				}
				else {

					int neighbor1, neighbor2;

					if (i == ind1) {
						neighbor1 = ind2;
						neighbor2 = ind3;
					}
					else if (i == ind2) {
						neighbor1 = ind1;
						neighbor2 = ind3;
					}
					else {
						neighbor1 = ind1;
						neighbor2 = ind2;
					}

					//myfile << "Index " << i << " has neighbors : " << neighbor1 << "  " << neighbor2 << endl;

					vector<int> _insert;
					_insert.push_back(i);
					_insert.push_back(neighbor1);
					_insert.push_back(neighbor2);

					picked_combinations.push_back(_insert);
					++picked_counter;


					vector<int> neighbor_insert;
					neighbor_insert.push_back(neighbor1);
					neighbor_insert.push_back(neighbor2);
					neighbors.push_back(neighbor_insert);

					neighbor_file << "Vertex " << i << " has neighbors " << neighbor1 << " and " << neighbor2 << endl;
					break;

				}

			}
			else if (i != ind1 && i != ind2 && i != ind3 && j == count - 1) {
				// No neighbors found (233 , 369 , 370)

				vector<int> neighbor_insert;
				neighbor_insert.push_back(-1);
				neighbor_insert.push_back(-1);
				neighbors.push_back(neighbor_insert);
				neighbor_file << "Vertex " << i << " has neighbors " << -1 << " and " << -1 << endl;
			}

		}
	}

	myfile.close();
	neighbor_file.close();

}



void Guides::Fill_Struct() {

	nearest_transparency = new float[total_hair_points];
	nearest_thickness = new float[total_hair_points];
	nearest_colors = new float[3*total_hair_points];
	

	float* original_thickness = hair->GetThickness();
	float* original_colors = hair->GetColors();
	float* original_transparency = hair->GetTransparency();

	ofstream myfile,myfile2, normFile,neighbFile;
	myfile.open("thickness_vals.txt");
	myfile2.open("barycentrics.txt");
	normFile.open("NearestNormals.txt");
	neighbFile.open("NeighborVertices.txt");


	int count = 0;
	int color_count = 0;
	

	for (int i = 0; i < total_points; i++) {

		int min_index = min_indices[i];
		int color_min_index = 3*min_index;
		int neighb_count = 0;

		float barycentric1 = float(rand()) / float(RAND_MAX);
		float barycentric2 = float(rand()) / float(RAND_MAX);

		if (barycentric1 + barycentric2 > 1.0) {
			barycentric1 = 1.0 - barycentric1;
			barycentric2 = 1.0 - barycentric2;
		}

		float barycentric3 = 1.0 - barycentric1 - barycentric2;
		//cout << "Point " << i << " barycentrics: " << barycentric1 << "  " << barycentric2 << " " << barycentric3 << endl;

		int neighbor_index1 = neighbors[i][0];
		int neighbor_index2 = neighbors[i][1];

		//for (int j = 0; j < nearest_segments[i] + 2; j++) {
		for (int j = 0; j < nearest_segments[i]; j++) {
			Per_Vertex_Attribute attrib;

			nearest_colors[color_count] = original_colors[color_min_index];
			nearest_colors[color_count + 1] = original_colors[color_min_index + 1];
			nearest_colors[color_count + 2] = original_colors[color_min_index + 2];

			nearest_thickness[count] = original_thickness[min_index];
			nearest_transparency[count] = original_transparency[min_index];


			if (j == nearest_segments[i] - 1) {
				attrib.segment_index = 0;

			}
			else attrib.segment_index = 1;

			float multiplier = 1.0;
			if (j == nearest_segments[i] - 2) multiplier = 0.1;

			attrib.thickness = nearest_thickness[count] * attrib.segment_index * multiplier;
			attrib.transparency = nearest_transparency[count];

			attrib.color[0] = nearest_colors[color_count];
			attrib.color[1] = nearest_colors[color_count + 1];
			attrib.color[2] = nearest_colors[color_count + 2];

			if (neighbor_index1 != -1){

				float neighb1[3];
				neighb1[0] = guide_points[neighbor_index1][neighb_count];
				neighb1[1] = guide_points[neighbor_index1][neighb_count + 1];
				neighb1[2] = guide_points[neighbor_index1][neighb_count + 2];

				float neighb2[3];
				neighb2[0] = guide_points[neighbor_index2][neighb_count];
				neighb2[1] = guide_points[neighbor_index2][neighb_count + 1];
				neighb2[2] = guide_points[neighbor_index2][neighb_count + 2];

				//glm::vec3 neighbor1 = glm::vec3(neighb1[0],neighb1[1],neighb1[2]);
				//glm::vec3 neighbor2 = glm::vec3(neighb2[0],neighb2[1],neighb2[2]);
				//glm::vec3 neighbor3 = glm::vec3(guide_points[i][neighb_count], guide_points[i][neighb_count+1], guide_points[i][neighb_count+2]);

				//attrib.neighbor1 = neighbor1;
				//attrib.neighbor2 = neighbor2;
				//attrib.neighbor3 = neighbor3;


				glm::vec3 baryc = glm::vec3(barycentric1, barycentric2, barycentric3);
				attrib.barycentrics = baryc;
				
				attrib.normal_neighb1 = glm::vec3(growth_mesh_normals[i], growth_mesh_normals[i+1], growth_mesh_normals[i+2]);
				attrib.normal_neighb2 = glm::vec3(growth_mesh_normals[neighbor_index1*3], growth_mesh_normals[neighbor_index1*3 + 1], growth_mesh_normals[neighbor_index1 * 3 + 2]);
				attrib.normal_neighb3 = glm::vec3(growth_mesh_normals[neighbor_index2*3], growth_mesh_normals[neighbor_index2*3 + 1], growth_mesh_normals[neighbor_index2 * 3 + 2]);
				

				//attrib.normals = glm::vec3(guide_normals[neighb_count], guide_normals[neighb_count+1], guide_normals[neighb_count+2]);
				
				attrib.guide_neighb2 = glm::vec3(neighb1[0], neighb1[1], neighb1[2]);
				attrib.guide_neighb3 = glm::vec3(neighb2[0], neighb2[1], neighb2[2]);
			}
			else {
				attrib.normal_neighb1 = glm::vec3(-1);
				attrib.normal_neighb2 = glm::vec3(-1);
				attrib.normal_neighb3 = glm::vec3(-1);
			
			}

			
			//Original
			attrib.normal3 = glm::vec3(guide_normals[count]);

			
			normFile << "Neighbors for hair index " << i << " , segment " << count << " : " << neighbor_index1 << " " << neighbor_index2 << endl;
			normFile << "N1 for " << count << " : " << attrib.normal_neighb1.x << " " <<  attrib.normal_neighb1.y << " " <<  attrib.normal_neighb1.z << endl;
			normFile << "N2 for " << count << " : " << attrib.normal_neighb2.x << " " <<  attrib.normal_neighb2.y << " " << attrib.normal_neighb2.z << endl;
			normFile << "N3 for " << count << " : " << attrib.normal_neighb3.x << " " << attrib.normal_neighb3.y << " "  << attrib.normal_neighb3.z << endl;
			normFile << endl;

			neighbFile << "Neighbors for "<< i << ": " << neighbor_index1 << " " << neighbor_index2 << endl;
			/*neighbFile << "P1 for " << count << " : " << attrib.neighbor1.x << attrib.neighbor1.y << attrib.neighbor1.z << endl;
			neighbFile << "P2 for " << count << " : " << attrib.neighbor2.x << attrib.neighbor2.y << attrib.neighbor2.z << endl;
			neighbFile << "P3 for " << count << " : " << guide_points[i][neighb_count] << guide_points[i][neighb_count + 1] << guide_points[i][neighb_count + 2] << endl;
			neighbFile << endl;*/
			
			attributes.push_back(attrib);
			
			++count;
			++min_index;

			color_count += 3;
			color_min_index += 3;
			neighb_count +=3;

			//myfile << attrib.thickness << endl;
			myfile2 << barycentric1 << " " << barycentric2 << " " << barycentric3 << endl;
		}
	}

	myfile.close();
	myfile2.close();
	normFile.close();
	neighbFile.close();
	cout << "NUMBER OF ELEMENTS IN attrib: " << attributes.size() << endl;
	cout << "COUNT: " << count << endl;

}

void Guides::SetupGuides() {


	Pick_Neighbor_Indices();

	//Pick_Neighbor_Indices2();

	// Print the adjacency information
	for (const auto& entry : adjacency_map) {
		std::cout << "Vertex " << entry.first << " has neighbors: ";
		for (int neighbor : entry.second) {
			std::cout << neighbor << " ";
		}
		std::cout << std::endl;
	}
	Fill_Struct();
	

	glGenVertexArrays(1, &VAO);
	glGenBuffers(3, VBO);
	glGenBuffers(1, &EBO);

	// Bind the current VAO. Every subsequent function call will affect the currently bound VAO
	glBindVertexArray(VAO);
	cout << "total_hair_points in SetupGuides: " << total_hair_points << endl;

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * total_hair_points * sizeof(float), this->points, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	glBufferData(GL_ARRAY_BUFFER, attributes.size() * sizeof(Per_Vertex_Attribute), &attributes[0], GL_STATIC_DRAW);


	//glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
	//glBufferData(GL_ARRAY_BUFFER, 3 * total_hair_points * sizeof(float), this->tangents, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glEnableVertexAttribArray(0);
	// Insert vertex data at the "location = 0" with size 3 (3 vertex locations), and offset 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
	/*glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, color));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, transparency));
	*/

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, segment_index));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, thickness));

	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, barycentrics));

	glEnableVertexAttribArray(10);
	glVertexAttribPointer(10, 3, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, normal3));

	glEnableVertexAttribArray(11);
	glVertexAttribPointer(11, 3, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, normal_neighb1));

	glEnableVertexAttribArray(12);
	glVertexAttribPointer(12, 3, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, normal_neighb2));

	glEnableVertexAttribArray(13);
	glVertexAttribPointer(13, 3, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, normal_neighb3));
	
	glEnableVertexAttribArray(14);
	glVertexAttribPointer(14, 3, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, guide_neighb2));

	glEnableVertexAttribArray(15);
	glVertexAttribPointer(15, 3, GL_FLOAT, GL_FALSE, sizeof(Per_Vertex_Attribute), (void*)offsetof(Per_Vertex_Attribute, guide_neighb3));
	
	// Unbind the VAO, so that a function doesn't accidentally change the VAO
	glBindVertexArray(0);

}

void Guides::DrawRegular() {
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0,total_hair_points);	

	glBindVertexArray(0);
}

void Guides::Draw() {

	glBindVertexArray(VAO);

	glPatchParameteri(GL_PATCH_VERTICES, 4);
	glDrawElements(GL_PATCHES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	
	/*for (int i = 0; i < growth_mesh_hair_count; i++) {
		//glLineWidth(2.0);
		glDrawArrays(GL_LINE_STRIP, pointIndex, line_segments[0]);
		pointIndex += line_segments[0];
	}*/
	
	glBindVertexArray(0);
}

Guides::~Guides() {


}



