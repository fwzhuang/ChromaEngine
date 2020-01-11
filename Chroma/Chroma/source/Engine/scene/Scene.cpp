#include "Scene.h"


namespace Chroma
{
	// Scene Contents
	// Render
	Camera*             Scene::m_RenderCamera;
	Light*              Scene::m_SunLight;
	SkyBox*             Scene::m_Skybox;
	std::vector<Light*> Scene::m_Lights;
	IBL*				Scene::m_IBL;

	// Entities Components
	std::map<std::string, IEntity*>    Scene::m_Entities;
	std::map<std::string, IComponent*> Scene::m_Components;

	// UIDs
	std::set<std::string> Scene::m_EntityUIDs;
	std::set<std::string> Scene::m_TransparentEntityUIDs;
	std::set<std::string> Scene::m_AnimatedEntityUIDs;
	std::set<std::string> Scene::m_UpdatingComponentUIDs;
	std::set<std::string> Scene::m_MeshComponentUIDs;

	glm::vec3 Scene::CalculateAmbientLightColor()
	{
		return m_SunLight->getDiffuse() * m_SunLight->getIntensity() * glm::vec3(.5f);
	}

	IEntity* Scene::GetEntity(std::string UID)
	{
		if (m_Entities.find(UID) != m_Entities.end())
		{
			return m_Entities.find(UID)->second;
		}
		CHROMA_ERROR("SCENE :: Entity of UID : {0} , could not be found in scene!");
	}

	IComponent* Scene::GetComponent(std::string const& UID)
	{
		if (m_Components.find(UID) != m_Components.end())
		{
			return m_Components.find(UID)->second;
		}
		CHROMA_ERROR("SCENE :: Component of UID : {0} , could not be found in scene!");
	}


	void Scene::Init()
	{
		// init members
		m_RenderCamera = new Camera();
		m_SunLight = new Light(Light::SUNLIGHT, glm::vec3(0.2, -0.8, 0.0), 1.0f);
		m_Skybox = new SkyBox(m_RenderCamera);
		m_IBL =  new IBL(); // image based lighting

		// setting skybox to IBL environment map
		m_Skybox->setColorSpace(HDR);
		m_Skybox->setCubeMapID(m_IBL->getEnvCubeMapID());
	}

	void Scene::PostSceneBuild()
	{
		// entities
		for (std::string const& UID : m_EntityUIDs)
		{
			GetEntity(UID)->Init();
		}
	}

	void Scene::AddEntity(IEntity* const& newEntity)
	{
		// collect UID
		m_EntityUIDs.insert(newEntity->GetUID());

		// collect entity
		m_Entities[newEntity->GetUID()] = newEntity;
	}

	void Scene::AddAnimatedEntity(IEntity* const& newAnimatedEntity)
	{
		// add UID to animated entity UIDs
		m_AnimatedEntityUIDs.insert(newAnimatedEntity->GetUID());
	}

	void Scene::AddTransparentEntity(IEntity* const& newTransparentEntity)
	{
		// add UID to transparent entity UIDs
		m_TransparentEntityUIDs.insert(newTransparentEntity->GetUID());
	}

	void Scene::RemoveEntity(IEntity& RemoveEntity)
	{
	}

	float Scene::GetEntityDistanceToCamera(std::string const& UID)
	{
		return glm::length(GetEntity(UID)->GetTranslation() - m_RenderCamera->GetPosition());
	}

	void Scene::AddUpdatingComponent(IComponent* const& newUpdatingComponent)
	{
		// collect component UID
		m_UpdatingComponentUIDs.insert(newUpdatingComponent->GetUID());

		// add component
		m_Components[newUpdatingComponent->GetUID()] = newUpdatingComponent;
	}

	void Scene::AddMeshComponent(IComponent* const& newMeshComponent)
	{
		// collect component UID
		m_MeshComponentUIDs.insert(newMeshComponent->GetUID());

		// add component
		m_Components[newMeshComponent->GetUID()] = newMeshComponent;
	}

	void Scene::RemoveLight(Light& RemoveLight)
	{
	}

	void Scene::SetLights(std::vector<Light*> newLights)
	{
		m_Lights = newLights;
		for (Light* light : newLights)
			if (light->type == Light::SUNLIGHT)
				m_SunLight = light;
	}

	void Scene::SetEntities(std::vector<IEntity*> const& newEntities)
	{
		for (IEntity* entity : newEntities)
			AddEntity(entity);
	}
}

