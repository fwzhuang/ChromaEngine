#ifndef _CHROMA_SCENE_
#define _CHROMA_SCENE_
#include "../memory/packed_freelist.h"
#include "../cameras/Camera.h"
#include "../lights/Light.h"
#include "../entity/ChromaEntity.h"
#include <vector>


class ChromaScene
{
public:
	// scene components
	std::vector<Light*> Lights;
	std::vector<Camera*> Cameras;
	std::vector<ChromaEntity*> Entities;

	Camera* RenderCamera;


	// Add/Remove from Scene
	void addCamera(Camera*& newCamera) { Cameras.push_back(newCamera); };
	void removeCamera(Camera& removeCamera);

	void addEntity(ChromaEntity*& newEntity) { Entities.push_back(newEntity); };
	void removeEntity(ChromaEntity& removeEntity);

	void addLight(Light*& newLight) { Lights.push_back(newLight); };
	void removeLight(Light& removeLight);

	// Getters/Setters
	void setRenderCamera(Camera*& newRenderCamera) { RenderCamera = newRenderCamera; };
	void setLights(std::vector<Light*>& newLights) { Lights = newLights; };
	void setCameras(std::vector<Camera*>& newCameras) { Cameras = newCameras; };
	void setEntities(std::vector<ChromaEntity*>& newCameras) { Entities = newCameras; };

	ChromaScene();
	~ChromaScene();
};

#endif
