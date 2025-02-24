#include "Mesh.h"
#include "funcs.h"
#include <exception>

Mesh::Mesh(std::vector<Vertex> verticies, std::vector<Texture> textures, std::vector<unsigned int> indicies)
	:verticies(verticies), textures(textures), indicies(indicies) {
	this->setUpMesh();
}

Mesh::Mesh(Mesh&& other) noexcept
	:verticies(std::move(other.verticies)),
	 textures(std::move(other.textures)),
	 indicies(std::move(other.indicies)),
	 VAO(other.VAO), EBO(other.EBO), VBO(other.VBO)
	 {
		other.VAO = other.EBO = other.VBO =0;
	 }

Mesh& Mesh::operator=(Mesh&& other) noexcept{
	if (this != &other){

		clearResources();

		verticies = std::move(other.verticies);
		textures  = std::move(other.textures);
		indicies  = std::move(other.indicies);
		VAO = other.VAO;
		VBO = other.VBO;
		EBO = other.EBO;

		other.VAO = other.EBO = other.VBO =0;
	}

	return *this;
}


class InvalidTexture : public std::exception {
private :
	std::string message;
public:
	std::string what() {
		return this->message;
	}

	InvalidTexture(std::string msg)
		:message(msg) {}
};

Texture::Texture(std::string path, std::string directory, Texture::Type type, GLuint S_WRAP, 
				 GLuint T_WRAP) {
	this->id = funcs::TextureFromFile(path, directory, S_WRAP, T_WRAP);
	this->path = path;
	this->type = type;
}

Texture::Texture(std::string completePath, Texture::Type type, GLuint S_WRAP, GLuint T_WRAP) {
	size_t i = completePath.find_last_of('/');
	std::string directory = completePath.substr(0, i);
	std::string path = completePath.substr(i+1, completePath.size());
	this->id = funcs::TextureFromFile(path, directory, S_WRAP, T_WRAP);
	this->path = path;
	this->type = type;
}

void Mesh::setUpMesh() {
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Vertex), &verticies[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_JointIDs));
	glEnableVertexAttribArray(3);

	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
	glEnableVertexAttribArray(4);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), &indicies[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Mesh::~Mesh(){
	clearResources();
}

void Mesh::clearResources(){
	if (VAO){
		glDeleteVertexArrays(1, &VAO);
	}

	if (VBO){
		glDeleteBuffers(1, &VBO);
	}

	if (EBO){
		glDeleteBuffers(1, &EBO);
	}
}


void Mesh::draw(Shader& shader) {
	shader.use();
	int textureDiff = 0;
	int textureSpec = 0;
	for (int i = 0; i < textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		if (textures[i].type == Texture::DIFFUSE) {
			shader.setInt("material." + textures[i].type + std::to_string(++textureDiff), i);
		}
		else if (textures[i].type == Texture::SPECULAR) {
			shader.setInt("material." + textures[i].type + std::to_string(++textureSpec), i);
		}
		else {
			throw InvalidTexture("Unrecognized texture type");
		}
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
}

