#include "Model.h"
#include "Utils.h"

Model::Model(char* pathname) {
	loadModel(pathname);
}

void Model::Draw(Shader& shader) {

	for (int i = 0; i < meshes.size(); i++) {
		meshes[i].Draw(shader);
	}
}

vector<Vertex> Model::GetVertices(int& growth_mesh_points) {

	Mesh& mesh = this->meshes[0];
	
	growth_mesh_points = mesh.vertices.size();

	return mesh.vertices;
	//return mesh.GetVertices(growth_mesh_points);
}


void Model::loadModel(string path) {

	Assimp::Importer import;
	//const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));

	processNode(scene->mRootNode, scene);

}


/*
  Process each node recursively in order to maintain the parent-child relation between the nodes 
  Can also be implemented iteratively if we don't want to keep the relation, but it's better to do it recursively
*/
void Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {

	vector<Vertex> vertices;
	vector<unsigned int> indices;
	vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; i++) {
		Vertex vertex;
		glm::vec3 vector;

		vector.x = mesh->mVertices[i].x;
		vector.y = mesh->mVertices[i].y;
		vector.z = mesh->mVertices[i].z;

		vertex.Position = vector;

		if (mesh->HasNormals()) {
			vector.x = mesh->mNormals[i].x;
			vector.y = mesh->mNormals[i].y;
			vector.z = mesh->mNormals[i].z;

			vertex.Normal = vector;
		}

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			glm::vec2 vec;
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = vec;
		}
		else
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	// process indices
	/*
	Assimp's interface defines each mesh as having an array of faces, where each face represents a single triangle. 
	A face contains the indices of the vertices we need to draw in what order for its primitive.
	*/
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	// process material
	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		vector<Texture> diffuseMaps = loadMaterialTextures(material,aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		vector<Texture> specularMaps = loadMaterialTextures(material,aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	return Mesh(vertices, indices, textures);

}


vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName) {

	vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		string full = directory + "/" + str.C_Str();
		Texture texture;
		texture.id = Utils::loadTexture(full.c_str());
		texture.type = typeName;
		texture.path = str.C_Str();

		aiColor3D color(0.f, 0.f, 0.f);
		mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);

		glm::vec3 diffuse(color.r,color.g,color.b);
		cout << "kd = ( " << color.r << "  " << color.g << "  " << color.b << " )" << endl;
		texture.kd = glm::vec3(color.r, color.g, color.b);

		textures.push_back(texture);
	}
	return textures;
}