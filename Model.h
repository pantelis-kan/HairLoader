#ifndef MODEL_H
#define MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

#include "Shader.h"
#include "Mesh.h"

#include <string>
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


using namespace std;

class Model {

	public:
		Model(char* pathname);
		void Draw(Shader& shader);
		vector<Vertex> GetVertices(int& growth_mesh_points); // Retrieve Vertices for the first mesh (used for the growth mesh)
		vector<unsigned int> GetIndices() { Mesh& mesh = this->meshes[0]; return mesh.indices; }

	private:
		// A model is a collection of meshes
		vector<Mesh> meshes;
		string directory;
		string fullpath;

		void loadModel(string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
			string typeName);
		
};


#endif