#ifndef _CHROMA_UNIFORM_
#define _CHROMA_UNIFORM_

#include <map>
#include <string>
#include <iostream>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>


template<typename UniformType>
class UniformArray
{
public:
	std::map<std::string, UniformType> uniforms;

	void addUniform(std::string uniformName, UniformType uniformValue) {
		uniforms.insert(std::make_pair(uniformName, uniformValue));
	};


	int size() { return uniforms.size(); };

	UniformArray() {};
	~UniformArray() {};
};


class Uniform
{
private:
	UniformArray<int> intUniforms;
	UniformArray<float> floatUniforms;
	UniformArray<glm::vec2> vec2Uniforms;
	UniformArray<glm::vec3> vec3Uniforms;
	UniformArray<glm::mat4> mat4Uniforms;

	unsigned int* ShaderID;

public:
	// Adding new Uniforms

	template<class T>
	void addUniform(std::string name, T uniformType) {
		std::cout << "not a supported uniform type!" << std::endl;
	};

	template<>
	void addUniform<int>(std::string uniformName, int uniformValue) {
		intUniforms.addUniform(uniformName, uniformValue);
	};

	template<>
	void addUniform<float>(std::string uniformName, float uniformValue)
	{
		floatUniforms.addUniform(uniformName, uniformValue);
	};

	template<>
	void addUniform<glm::vec2>(std::string uniformName, glm::vec2 uniformValue)
	{
		vec2Uniforms.addUniform(uniformName, uniformValue);
	};

	template<>
	void addUniform<glm::vec3>(std::string uniformName, glm::vec3 uniformValue)
	{
		vec3Uniforms.addUniform(uniformName, uniformValue);
	};

	template<>
	void addUniform<glm::mat4>(std::string uniformName, glm::mat4 uniformValue)
	{
		mat4Uniforms.addUniform(uniformName, uniformValue);
	};

	// Setting existing Uniforms

	template<typename T>
	void setUniform(std::string name, T uniformValue) {
		std::cout << "not a supported uniform type!" << std::endl;
	};

	template<>
	void setUniform<int>(std::string uniformName, int uniformValue) {
		setInt(uniformName, uniformValue);
	};

	template<>
	void setUniform<float>(std::string uniformName, float uniformValue)
	{
		setFloat(uniformName, uniformValue);
	};

	template<>
	void setUniform<glm::vec2>(std::string uniformName, glm::vec2 uniformValue)
	{
		setVec2(uniformName, uniformValue);
	};

	template<>
	void setUniform<glm::vec3>(std::string uniformName, glm::vec3 uniformValue)
	{
		setVec3(uniformName, uniformValue);
	};

	template<>
	void setUniform<glm::mat4>(std::string uniformName, glm::mat4 uniformValue)
	{
		setMat4(uniformName, uniformValue);
	};

	// Renders
	void setUniforms();

	// Uniform Updating
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec2(const std::string& name, glm::vec2 value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setMat4(const std::string& name, glm::mat4 matrix) const;


	Uniform(unsigned int *shaderID) : ShaderID{ shaderID } {};
	~Uniform() {};

};


#endif