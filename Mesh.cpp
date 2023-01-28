#include "Mesh.h"
#include <map>


using namespace std;
Mesh::Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures) {

	this->vertices = vertices;
	this->indices = indices;
	this->textures = textures;

	cout << "Created mesh with " << vertices.size() << " vertices, " << indices.size() << " indices, and " << textures.size() << " texture coordinates" << endl;

	//Write_Indices();
	setupMesh();

}

void Mesh::Write_Indices() {

	ofstream myfile;
	string filename = "mesh_indices_" + to_string(indices.size()) + ".txt";
	myfile.open(filename);

	for (auto it = indices.begin(); it != indices.end(); it++) {

		unsigned int arr = *it;

		myfile << arr << endl;
	}

	myfile.close();

}


float* Mesh::GetVertices(int& growth_mesh_points) {

	float x, y, z;
	int total_points = 0;
	for (auto it = vertices.begin(); it != vertices.end(); ++it) ++total_points;


	//ofstream myfile;
	//myfile.open("growth_mesh_verts.txt");

	cout << "Inside Mesh::GetVertices()" << endl;
	cout << "total_points inside Mesh::GetVertices(): " << total_points << endl;
	cout << "Vertices vector size: " << vertices.size() << endl;
	float* points = new float[ total_points]; 

	std::map<tuple<float, float,float>, int> check_duplicates;

	int count = 0;
	for (auto it = vertices.begin(); it != vertices.end(); ++it) {
		
		x = it->Position.x;
		y = it->Position.y;
		z = it->Position.z;

		// Each vertex is written as many times as the number of edges that
		// leave from this vertex. This is why we need to remove duplicate vertices
		if (!check_duplicates.empty()) {

			if (check_duplicates.find(make_tuple(x, y, z)) != check_duplicates.end()) continue;
		}
	
		check_duplicates[make_tuple(x,y,z)] = count;

		//myfile << x << "  " << y << "  " << z << endl;

		points[count] = x;
		points[count + 1] = y;
		points[count + 2] = z;

		count += 3;
	}
	cout << "Actual (non duplicate) vertices : " << count << endl;

	//myfile.close();
	growth_mesh_points = count;

	if(points == NULL) cout << "points array is NULL" << endl;
	return points;
}

void Mesh::setupMesh() {

	// Generate 3 buffers
	glGenVertexArrays(1, &VAO);
	glGenBuffers(2, VBO);
	glGenBuffers(1, &EBO);

	// Bind the current VAO. Every subsequent function call will affect the currently bound VAO
	glBindVertexArray(VAO);
	

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
	// vertex positions
	glEnableVertexAttribArray(0);
	// Insert vertex data at the "location = 0" with size 3 (3 vertex locations), and offset 0
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	
	// vertex normals
	glEnableVertexAttribArray(1);
	// Insert vertex data at the "location = 1" with size 3 (3 vertex normals), and offset starting from the struct member "Normal"
	// Struct memory is sequential, so the offset function is appropriate for offset operations
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	
	// vertex texture coords
	glEnableVertexAttribArray(2);
	// Insert vertex data at the "location = 1" with size 3 (3 vertex normals), and offset starting from the struct member "Normal"
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

	// Unbind the VAO, so that a function doesn't accidentally change the VAO
	glBindVertexArray(0);
	
}


void Mesh::Draw(Shader& shader)
{
	// bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;

		// If the mesh has no textures, pass 0 as the diffuse factor
		if(textures.size() == 0)
			glUniform3f(glGetUniformLocation(shader.ID, "kd"), 0.0f, 0.0f, 0.0f);

        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
             else if(name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to stream

			// set the diffuse factor as read from the obj file from assimp
			glUniform3f(glGetUniformLocation(shader.ID, "kd" ), textures[i].kd.x , textures[i].kd.y, textures[i].kd.z);

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
		
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
}


