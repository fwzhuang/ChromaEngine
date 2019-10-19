#ifndef _SHADER_H_
#define _SHADER_H_

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include "Uniform.h"


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
private:
	// Functions
	void CheckCompileErrors(GLuint shader, std::string type);
	void CompileAndLink();
	void LoadShaderSource();

	//Attrs
	std::string fragSourcePath, vertexSourcePath, geometrySourcePath;
	std::string fragCode, vertexCode, geometryCode;

	// Uniforms
	Uniform Uniforms{ &ShaderID };

public:
	//program ID
	unsigned int ShaderID;


	// Rednering
	virtual void use();
	template<typename UniformType>
	void addUniform(std::string uniformName, UniformType uniformValue){
		Uniforms.addUniform(uniformName, uniformValue);
	};

	// Uniforms
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, glm::vec2 value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setMat4(const std::string& name, glm::mat4 matrix) const;

	//constructor reads and builds the shader
	Shader(std::string fragmentPath, std::string vertexPath, std::string geometryPath="");
	Shader();
	~Shader();
};


#endif
