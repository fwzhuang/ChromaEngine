#ifndef _CHROMA_SCENE_
#define _CHROMA_SCENE_
#include "memory/packed_freelist.h"
#include "camera/Camera.h"
#include "light/Light.h"
#include "entity/ChromaEntity.h"
#include <vector>


class ChromaSceneManager
{
public:
	// scene components
	std::vector < std::shared_ptr<Light>> Lights;
	std::vector<ChromaEntity*> Entities;
	std::vector<ChromaEntity*> TransparentEntities;

	Camera* RenderCamera{ new Camera };
	Light* SunLight = new Light(Light::SUNLIGHT, glm::vec3(0.2, -0.8, 0.0), 1.0f);

	void addEntity(ChromaEntity*& newEntity) { Entities.push_back(newEntity); };
	void removeEntity(ChromaEntity& removeEntity);

	void addLight(std::shared_ptr<Light>& newLight) { Lights.push_back(newLight); };
	void removeLight(Light& removeLight);

	// Getters/Setters
	void setRenderCamera(Camera*& newRenderCamera) { RenderCamera = newRenderCamera; };
	void setLights(std::vector<std::shared_ptr<Light>>& newLights);
	void setEntities(std::vector<ChromaEntity*>& newCameras) { Entities = newCameras; };

	Camera* getRenderCamera() { return RenderCamera; };
	std::vector<std::shared_ptr<Light>>& getLights() { return Lights; };

	ChromaSceneManager();
	~ChromaSceneManager();
};

#endif
