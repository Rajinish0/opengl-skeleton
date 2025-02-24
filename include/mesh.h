#ifndef MESH_HEADER
#define MESH_HEADER 


#include <iostream>
#include <string>
#include <vector>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;

	glm::vec3 tangent;
	glm::vec3 bitanget;

	/*
	at most MAX_BONE_INFLUENCE bones can influence the pos
	of this vertex each influence is weighed by the weight
	defined in m_Weights
	*/
	int m_JointIDs[MAX_BONE_INFLUENCE];
	float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
	enum Type{
		DIFFUSE,
		SPECULAR
	};
	unsigned int id;
	Type type;
	std::string path;

	Texture() 
		:id(0) {};
	Texture(unsigned int id, Type type, std::string path)
		:id(id), type(type), path(path) {}
	Texture(std::string path, std::string directory, Type type, 
			GLuint S_WRAP = GL_REPEAT, 
			GLuint T_WRAP = GL_REPEAT);
	Texture(std::string completePath, Type type,
			GLuint S_WRAP = GL_REPEAT, 
			GLuint T_WRAP = GL_REPEAT);
};


class Mesh {
public:
	std::vector<Vertex> verticies;
	std::vector<Texture> textures;
	std::vector<unsigned int> indicies;
	
	Mesh(std::vector<Vertex> verticies, std::vector<Texture> textures, std::vector<unsigned int> indicies);
	Mesh(Mesh&&) noexcept;

	Mesh(const Mesh&) = default;
	Mesh& operator=(const Mesh&) = default;

	Mesh& operator=(Mesh&&) noexcept;
	void draw(Shader& shader);
	unsigned int VAO, VBO, EBO;
	~Mesh();

private:
	void setUpMesh();
	void clearResources();
};

#endif
