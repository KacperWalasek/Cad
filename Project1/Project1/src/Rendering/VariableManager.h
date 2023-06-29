#pragma once
#include <map>
#include <string>
#include <memory>
#include <algorithm>
#include <iostream>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class IShaderVariable
{
public:
	std::string name;

	virtual void Apply(unsigned int shaderId) const = 0;
};

template<typename T>
class ShaderVariable : public IShaderVariable
{
	unsigned int getLoc(unsigned int shaderId) const
	{
		const char* nameCstr = name.c_str();
		return glGetUniformLocation(shaderId, nameCstr);
	}
public:
	ShaderVariable(std::string name, T value)
		: name(name), value(value) {}

	std::string name;
	T value;

	virtual void Apply(unsigned int shaderId) const override {}
};

class VariableManager
{
	std::map<std::string, std::unique_ptr<IShaderVariable>> shaderVariable;
public:
	template<typename T>
	void AddVariable(std::string name, T value) 
	{
		shaderVariable.insert({ name, std::make_unique<ShaderVariable<T>>(name, value)});
	}

	template<typename T>
	void SetVariable(std::string name, T value)
	{
		auto it = shaderVariable.find(name);
		if (it == shaderVariable.end())
			return;
		ShaderVariable<T>* v = dynamic_cast<ShaderVariable<T>*>(it->second.get());
		if (!v)
			return;
		v->value = value;
	}

	template<typename T>
	T GetVariable(std::string name)
	{
		auto it = shaderVariable.find(name);
		if (it == shaderVariable.end())
			return T();
		ShaderVariable<T>* v = dynamic_cast<ShaderVariable<T>*>(it->second.get());
		if (!v)
			return T();
		return v->value;
	}

	void Apply(unsigned int shaderId) const
	{
		for (const auto& var : shaderVariable)
			var.second->Apply(shaderId);
	}

};

template<>
inline void ShaderVariable<glm::fmat4x4>::Apply(unsigned int shaderId) const
{
	unsigned int loc = getLoc(shaderId);
	
	glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

template<>
inline void ShaderVariable<glm::fvec4>::Apply(unsigned int shaderId) const
{
	unsigned int loc = getLoc(shaderId);

	glUniform4f(loc, value.x, value.y, value.z, value.w);
}

template<>
inline void ShaderVariable<int>::Apply(unsigned int shaderId) const
{
	unsigned int loc = getLoc(shaderId);

	glUniform1i(loc, value);
}

template<>
inline void ShaderVariable<float>::Apply(unsigned int shaderId) const
{
	unsigned int loc = getLoc(shaderId);

	glUniform1f(loc, value);
}

template<>
inline void ShaderVariable<bool>::Apply(unsigned int shaderId) const
{
	unsigned int loc = getLoc(shaderId);

	glUniform1i(loc, value);
}

template<>
inline void ShaderVariable<std::vector<int>>::Apply(unsigned int shaderId) const
{
	unsigned int loc = getLoc(shaderId);

	glUniform1iv(loc, value.size(), value.data());
}

template<>
inline void ShaderVariable<std::vector<bool>>::Apply(unsigned int shaderId) const
{
	unsigned int loc = getLoc(shaderId);

	std::vector<int> iVal;
	std::transform(value.begin(), value.end(), std::back_inserter(iVal), [](const bool& v) {
		return (int)v;
		});

	glUniform1iv(loc, value.size(), iVal.data());
}