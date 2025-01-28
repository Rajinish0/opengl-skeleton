#include "model.h"
#include "assimp_helper.h"

Model::Model(std::string path) {
	loadModel(path);
}

void Model::draw(Shader& shader) {
	for (Mesh mesh : meshes)
		mesh.draw(shader);
}

void Model::loadModel(std::string path) {
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cerr << "Assimp error:" << importer.GetErrorString() << std::endl;
		return;
	}

	directory = path.substr(0, path.find_last_of('/'));
	std::cout << directory << std::endl;
	processNode(scene->mRootNode, scene);

}


void Model::processNode(aiNode* node, const aiScene* scene) {
	for (int i = 0; i < node->mNumMeshes; ++i) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (int i = 0; i < node->mNumChildren; ++i) {
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indicies;
	std::vector<Texture> textures;

	for (int i = 0; i < mesh->mNumVertices; ++i) {
		Vertex vertex;

		setVertexDefaultData(vertex);
		glm::vec3 vector(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);
		vertex.position = vector;

		glm::vec3 normal(
			mesh->mNormals[i].x,
			mesh->mNormals[i].y,
			mesh->mNormals[i].z
		);

		vertex.normal = normal;

		if (mesh->mTextureCoords[0]) {
			glm::vec2 vec(
				mesh->mTextureCoords[0][i].x,
				mesh->mTextureCoords[0][i].y
			);

			vertex.texCoord = vec;
		}
		else
			vertex.texCoord = glm::vec2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (int i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i]; // this is gonna be a triangle because of the flag we set at the start
		for (int j = 0; j < face.mNumIndices; ++j) {
			indicies.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, Texture::DIFFUSE);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, Texture::SPECULAR);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	}

	extractBoneWeightForVertices(vertices, mesh, scene);

	return Mesh(vertices, textures, indicies);
}

void Model::setVertexBoneData(Vertex& vertex, int boneId, float weight){
	for (int i =0; i < MAX_BONE_INFLUENCE; ++i){
		if (vertex.m_JointIDs[i] < 0){
			vertex.m_Weights[i] = weight;
			vertex.m_JointIDs[i] = boneId;
			break;
		}
	}
}

void Model::extractBoneWeightForVertices(std::vector<Vertex>& vertices, aiMesh *mesh, const aiScene *scene){
	for (int boneIdx =0; boneIdx < mesh->mNumBones; ++boneIdx){
		int boneId = -1;
		std::string boneName = mesh->mBones[boneIdx]->mName.C_Str();
		if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()){
			JointInfo newBoneInfo;
			newBoneInfo.id = m_BoneCounter;
			newBoneInfo.offset = AssimpHelper::ConvertMatrixToGLMFormat(mesh->mBones[boneIdx]->mOffsetMatrix);
			m_BoneInfoMap[boneName] = newBoneInfo;
			boneId = m_BoneCounter;
			m_BoneCounter++;
		} else {
			boneId = m_BoneInfoMap[boneName].id;
		}
		assert(boneId != -1);
		auto weights = mesh->mBones[boneIdx]->mWeights;
		int numWeights = mesh->mBones[boneIdx]->mNumWeights;
		for (int i =0; i < numWeights; i++){
			int vertexId = weights[i].mVertexId;
			float weight = weights[i].mWeight;
			assert(vertexId <= vertices.size());
			setVertexBoneData(vertices[vertexId], boneId, weight);
		}
	}
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* material, aiTextureType type, Texture::Type typeName) {
	std::vector<Texture> textures;

	/*
	* TO DO:
	* USE HASHMAP FOR cachedTextures
	* better to have smth like lru cache for a bigger proj
	*/
	for (int i = 0; i < material->GetTextureCount(type); ++i) {
		aiString str;
		material->GetTexture(type, i, &str);
		std::cout << str.C_Str() << std::endl;
		bool cached = false;
		for (int j = 0; j < cachedTextures.size(); ++j) {
			if (std::strcmp(cachedTextures[j].path.data(), str.C_Str()) == 0) {
				textures.push_back(cachedTextures[j]);
				cached = true;
				break;
			}
		}
		if (!cached) {
			Texture texture;
			// std::cout << str.C_Str() << std::endl;
			// std::string s = str.C_Str();
			// int i1 = s.find_last_of('/');
			// std::cout << i1 << std::endl;
			// std::cout << s.find_last_of('/') <<' ' <<  s.size() << std::endl;
			// std::cout << s.substr(i1, 27) << std::endl;
			// std::cout << s << std::endl;
			texture.id = funcs::TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			cachedTextures.push_back(texture);
		}
	}

	return textures;
}
