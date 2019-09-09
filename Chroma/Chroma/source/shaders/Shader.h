#ifndef _SHADER_H_
#define _SHADER_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

struct Material {
	glm::vec3 ambient;
	float ambientBrightness;
	glm::vec3 diffuse;
	glm::vec3 specular;
	float shininess;
	float roughness;
};

class Shader
{
public:
	//program ID
	unsigned int id;

	//constructor reads and builds the shader
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath);
	// use/activate the shader;
	void use();
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 vec3) const;
	void setMat4(const std::string& name, glm::mat4 matrix);
};


#endif