#ifndef MODEL_HEADER
#define MODEL_HEADER

#include "mesh.h"
#include <map>
#include "funcs.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


struct JointInfo{
	int id;
	/*
	this is the inverse bind transformation 
	*/
	glm::mat4 offset;
};

class Model {
public:
	Model(std::string path);
	void draw(Shader& shader);
	std::vector<Mesh> meshes;
	auto& getBoneInfoMap() { return m_BoneInfoMap;}
	int& getBoneCount() {return m_BoneCounter;}

private:
	std::map<std::string, JointInfo> m_BoneInfoMap;
	int m_BoneCounter =0;

	void setVertexDefaultData(Vertex& vertex){
		for (int i=0; i < MAX_BONE_INFLUENCE; ++i){
			vertex.m_JointIDs[i] = -1;
			vertex.m_Weights[i] = 0.0f;
		}
	}

	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> cachedTextures;
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::Type typeName);
	void extractBoneWeightForVertices(std::vector<Vertex>&, aiMesh *mesh, const aiScene *scene);
	void setVertexBoneData(Vertex&, int boneId, float weight);
};



#endif

