#ifndef _CHROMA_COMPONENT_
#define _CHROMA_COMPONENT_

//stl
#include <string>
#include <typeinfo>

// chroma
#include <memory/ChromaUID.h>
#include <shader/Shader.h>
#include <camera/Camera.h>
#include <light/Light.h>
#include <texture/Texture.h>
#include <ChromaConfig.h>

// forward declarations
class IEntity;

class IComponent
{
protected:
	// name
	ChromaUID UID;
	std::string m_Name;

	// parent
	IEntity* m_ParentEntity{ nullptr };

public:

	// Acessors
	ChromaUID GetUID() const { return UID; };

	std::string GetName() const { return m_Name; };
	void SetName(std::string newName) { m_Name = newName; };

	IEntity* GetParentEntity() const {   return m_ParentEntity;	}
	virtual void SetParentEntity(IEntity* const& parentEntity) { m_ParentEntity = parentEntity; }

	IComponent() {};
	virtual ~IComponent() {};
};

#endif