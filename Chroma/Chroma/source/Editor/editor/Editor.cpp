#include "Editor.h"

#include <resources/ResourceManager.h>


#include <entity/Entity.h>
#include <terrain/Terrain.h>
#include <component/AnimationComponent.h>
#include <model/SkinnedMesh.h>
#include <model/Model.h>

// game
#include <thirdperson/ThirdPersonCharacterController.h>
#include <thirdperson/ThirdPersonCameraController.h>


namespace Chroma
{
	void Editor::Init()
	{
		Chroma::EditorUI::Init();
		CHROMA_INFO("CHROMA EDITOR :: Editor Initialized.");
	}

	void Editor::PopulateTestScene()
	{
		// LIGHTS
		// ____________________________________________________

		// POINT LIGHTS
		std::vector<Light*> Lights;

		// point light positions
		glm::vec3 pointLightPositions[] = {
			glm::vec3(3.5f,  1.2f,  2.0f),
			glm::vec3(0.5f,  0.2f,  -2.0f),
			glm::vec3(-3.5f,  1.2f,  4.0f),
		};
		// dancing point lights
		for (glm::vec3 pos : pointLightPositions)
		{
			Light* pointLight = new Light(pos, Light::POINT);
			pointLight->setIntensity(0.51f);
			pointLight->quadratic *= 4.0f;
			pointLight->linear *= 2.0f;
			Lights.push_back(pointLight);
		}

		// SUNLIGHT
		Light* Sun = new Light(Light::SUNLIGHT, glm::vec3(0.2, -0.8, 0.3), 2.0f);
		Sun->setDiffuse(glm::vec3(1.0));
		Sun->setIntensity(3.0);
		Lights.push_back(Sun);
		Chroma::Scene::SetLights(Lights);

		// ____________________________________________________
		// SHADERS
		// ____________________________________________________
		Shader UnlitShader("resources/shaders/fragBasic.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");
		UnlitShader.Use();
		UnlitShader.SetVec3("color", glm::vec3(1, 1, 0));
		Shader SemiTransparentShader("resources/shaders/fragPBRAlpha.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");
		Shader PBRShader("resources/shaders/fragPBR.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");

		// Lookdev Sphere
		Texture lookdevAlbedo("resources/textures/pbr/lookdev_pbr/albedo.jpg");
		lookdevAlbedo.type = Texture::ALBEDO;
		Texture lookdevNormal("resources/textures/pbr/lookdev_pbr/normal.jpg");
		lookdevNormal.type = Texture::NORMAL;
		Texture lookdevMetRoughAO("resources/textures/pbr/lookdev_pbr/MetRoughAO.jpg");
		lookdevMetRoughAO.type = Texture::METROUGHAO;
		// ____________________________________________________
		// TEXTURES
		// ____________________________________________________
		// Generic
		Texture blackAlbedo("resources/textures/colors/black.jpg");
		Texture greyAlbedo("resources/textures/colors/grey.jpg");
		Texture whiteAlbedo("resources/textures/colors/white.jpg");
		Texture gridAlbedo("resources/animation/textures/grid.jpg");
		Texture flatNormal("resources/textures/test/flat_normal.jpg");
		flatNormal.type = Texture::NORMAL;
		Texture alphaTestAlbedo("resources/textures/test/grass.png");
		Texture sandyNormal("resources/textures/test/sandy_normal.jpg");
		sandyNormal.type = Texture::NORMAL;

		// Woodplanks
		Texture PlanksAlbedo("resources/textures/pbr/hardwood_pbr/albedo.jpg");
		PlanksAlbedo.type = Texture::ALBEDO;
		Texture PlanksNormal("resources/textures/pbr/hardwood_pbr/normal.jpg");
		PlanksNormal.type = Texture::NORMAL;
		Texture PlanksMetRoughAO("resources/textures/pbr/hardwood_pbr/MetRoughAO.jpg");
		PlanksMetRoughAO.type = Texture::METROUGHAO;

		// AgedWoodplanks
		Texture agedPlanksAlbedo("resources/textures/pbr/agedplanks_pbr/albedo.jpg");
		agedPlanksAlbedo.type = Texture::ALBEDO;
		Texture agedPlanksNormal("resources/textures/pbr/agedplanks_pbr/normal.jpg");
		agedPlanksNormal.type = Texture::NORMAL;
		Texture agedPlanksMetRoughAO("resources/textures/pbr/agedplanks_pbr/MetRoughAO.jpg");
		agedPlanksMetRoughAO.type = Texture::METROUGHAO;

		// Rusted Metal
		Texture rustedIronAlbedo("resources/textures/pbr/rustediron_pbr/albedo.jpg");
		rustedIronAlbedo.type = Texture::ALBEDO;
		Texture rustedIronNormal("resources/textures/pbr/rustediron_pbr/normal.jpg");
		rustedIronNormal.type = Texture::NORMAL;
		Texture rustedIronMetRoughAO("resources/textures/pbr/rustediron_pbr/MetRoughAO.jpg");
		rustedIronMetRoughAO.type = Texture::METROUGHAO;

		// Animated Model
		Texture walkingAlbedo("resources/animation/vampire_textures/albedo.jpg");
		walkingAlbedo.type = Texture::ALBEDO;
		Texture walkingNormal("resources/animation/vampire_textures/normal.jpg");
		walkingNormal.type = Texture::NORMAL;
		Texture walkingMetRoughAO("resources/animation/vampire_textures/MetRoughAO.jpg");
		walkingMetRoughAO.type = Texture::METROUGHAO;
		// ____________________________________________________

		// ANIMATED MODEL
		// ____________________________________________________
		IEntity* AnimModelEntity = new Entity;
		AnimModelEntity->SetName("AnimationModel");
		Chroma::Scene::AddEntity(AnimModelEntity);
		AnimModelEntity->SetScale(glm::vec3(0.06f));
		AnimModelEntity->SetTranslation(glm::vec3(0.0, 10.0, 0.0));
		// mesh
		MeshComponent* AnimModelMeshComponent = new SkinnedMesh("resources/animation/vampire.fbx");
		AnimModelMeshComponent->AddTexture(walkingAlbedo);
		AnimModelMeshComponent->AddTexture(walkingNormal);
		AnimModelMeshComponent->AddTexture(walkingMetRoughAO);
		AnimModelMeshComponent->SetIsDoubleSided(true);
		AnimModelEntity->AddComponent(AnimModelMeshComponent);
		// rigid
		PhysicsComponent* AnimModelRigidComponent = new PhysicsComponent();
		AnimModelRigidComponent->SetColliderShape(ColliderShape::Box);
		AnimModelRigidComponent->SetCollisionState(ColliderState::Kinematic);
		AnimModelEntity->AddComponent(AnimModelRigidComponent);
		// animation
		AnimationComponent* AnimModelAnimationComponent = new AnimationComponent();
		Animator AnimModelAnimator;
		AnimModelAnimator.BindSkeleton(AnimModelMeshComponent);
		AnimModelAnimator.LoadAnimations("resources/animation/vampire_idle.fbx");
		AnimModelAnimator.LoadAnimations("resources/animation/vampire_walk.fbx");
		AnimModelAnimator.CompressAnimations();
		AnimModelAnimationComponent->AddAnimator(AnimModelAnimator);
		AnimModelEntity->AddComponent(AnimModelAnimationComponent);
		//// character controller
		CharacterControllerComponent* AnimModelCharacterController = new ThirdPersonCharacterController();
		// camera controller
		ICameraController* AnimModelCameraController = new ThirdPersonCameraController();
		Chroma::Scene::GetRenderCamera()->SetCustomCameraController(AnimModelCameraController);
		AnimModelCharacterController->SetCustomCameraController(AnimModelCameraController);
		// adding the component
		AnimModelEntity->AddComponent(AnimModelCharacterController);

		// SPHERES
	// Sphere Positions
		glm::vec3 spherePositions[] = {
			glm::vec3(0.f,  1.0f,  0.0f),
			glm::vec3(2.5f,  1.0f,  0.0f),
			glm::vec3(5.0f,  1.0f,  0.0f),
			glm::vec3(-2.5f,  1.0f,  0.0f),
			glm::vec3(-5.0f,  1.0f,  0.0f),
		};

		for (int i = 0; i < 5; i++)
		{
			IEntity* SphereEntity = new Entity;
			Chroma::Scene::AddEntity(SphereEntity);
			SphereEntity->SetName("Sphere");
			MeshComponent* SphereMeshComponent = new SpherePrimitive();
			PhysicsComponent* SphereRigidComponent = new PhysicsComponent();
			SphereRigidComponent->SetColliderShape(ColliderShape::Convex);
			SphereRigidComponent->SetCollisionState(ColliderState::Dynamic);
			SphereRigidComponent->SetMass(1.0f);
			SphereRigidComponent->SetFriction(3.0f);
			//SphereMeshComponent->AddTexture(sandyNormal);
			SphereMeshComponent->AddTexture(greyAlbedo);
			SphereMeshComponent->SetShader(PBRShader);
			SphereEntity->SetTranslation(spherePositions[i]);
			SphereEntity->AddComponent(SphereMeshComponent);
			SphereEntity->AddComponent(SphereRigidComponent);
		}
	}

	void Editor::PopulateTestScene2()
	{
		// POPULATE SCENE CONTENTS
	// ------------------------------------------------------------------------------------------
	// LIGHTS
	// ____________________________________________________

	// CONSTS
		const float SUNLIGHT_SPIN_SPEED = .8f;
		const float SUNLIGHT_DISTANCE = 15.0f;
		// POINT LIGHTS
		std::vector<Light*> Lights;

		// point light positions
		glm::vec3 pointLightPositions[] = {
			glm::vec3(3.5f,  1.2f,  2.0f),
			glm::vec3(0.5f,  0.2f,  -2.0f),
			glm::vec3(-3.5f,  1.2f,  4.0f),
		};
		// point lights
		for (glm::vec3 pos : pointLightPositions)
		{
			Light* pointLight = new Light(pos, Light::POINT);
			pointLight->setIntensity(0.51f);
			pointLight->quadratic *= 4.0f;
			pointLight->linear *= 2.0f;
			Lights.push_back(pointLight);
		}

		// SUNLIGHT
		Light* Sun = new Light(Light::SUNLIGHT, glm::vec3(0.2, -0.8, 0.3), 2.0f);
		Sun->setDiffuse(glm::vec3(1.0));
		Sun->setIntensity(3.0);
		Lights.push_back(Sun);
		Chroma::Scene::SetLights(Lights);

		// ____________________________________________________
		// ENTITIES
		// ____________________________________________________

		IEntity* HumanEntity = new Entity;
		HumanEntity->SetName("Human Model");
		Chroma::Scene::AddEntity(HumanEntity);
		HumanEntity->SetScale(glm::vec3(40.0f));
		HumanEntity->SetTranslation(glm::vec3(-5, 0, 0));

		IEntity* HumanEntity2 = new Entity;
		HumanEntity2->SetName("Human Model2");
		Chroma::Scene::AddEntity(HumanEntity2);
		HumanEntity2->SetScale(glm::vec3(40.0f));
		HumanEntity2->SetTranslation(glm::vec3(5, 0, 0));

		// ____________________________________________________
		// SHADERS
		// ____________________________________________________

		Shader SemiTransparentShader("resources/shaders/fragPBRAlpha.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");
		Shader PBRSkinShaderExperimental("resources/shaders/fragSSSS.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");
		Shader PBRSkinShader("resources/shaders/fragSSSS_backup.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");

		// ____________________________________________________
		// TEXTURES
		// ____________________________________________________

		//// Default
		// Generic
		Texture greyAlbedo("resources/textures/colors/grey.jpg");
		Texture gridAlbedo("resources/animation/textures/grid.jpg");
		Texture flatNormal("resources/textures/test/flat_normal.jpg");

		//// Jacket
		//Texture jacketAlbedo("resources/human/textures/jacket/Jacket_Colour.jpg");
		//jacketAlbedo.type = Texture::ALBEDO;
		//Texture jacketNormal = Chroma::ResourceManager::LoadTexture("resources/human/textures/jacket/Jacket_Normal.jpg");
		//jacketNormal.type = Texture::NORMAL;
		//Texture jacketMetRoughAO = Chroma::ResourceManager::LoadTexture("resources/human/textures/jacket/MetRoughAO.jpg");
		//jacketMetRoughAO.type = Texture::METROUGHAO;

		// Head
		Texture headAlbedo = Chroma::ResourceManager::LoadTexture("resources/human/textures/head/head_albedo.jpg");
		headAlbedo.type = Texture::ALBEDO;
		Texture headNormal = Chroma::ResourceManager::LoadTexture("resources/human/textures/head/head_normal.jpg");
		headNormal.type = Texture::NORMAL;
		Texture headMetRoughAO = Chroma::ResourceManager::LoadTexture("resources/human/textures/head/head_metroughao.jpg");
		headMetRoughAO.type = Texture::METROUGHAO;
		Texture headTranslucency = Chroma::ResourceManager::LoadTexture("resources/human/textures/head/head_translucency.jpg");
		headTranslucency.type = Texture::TRANSLUCENCY;



		// ____________________________________________________
		// MODELS
		// ____________________________________________________

		////Jacket
		//MeshComponent* JacketMeshComponent = new Model("resources/human/Jacket/Jacket.fbx");
		//JacketMeshComponent->SetShader(PBRShader);
		//JacketMeshComponent->AddTexture(jacketAlbedo);
		//JacketMeshComponent->AddTexture(jacketNormal);
		//JacketMeshComponent->AddTexture(jacketMetRoughAO);
		//HumanEntity->AddComponent(JacketMeshComponent);

		// Head
		MeshComponent* HeadMeshComponent = new Model("resources/human/Head/Head.fbx");
		HeadMeshComponent->SetIsForwardLit(true);
		HeadMeshComponent->SetIsLit(false);
		HeadMeshComponent->SetShader(PBRSkinShader);
		HeadMeshComponent->AddTexture(headAlbedo);
		HeadMeshComponent->AddTexture(headNormal);
		HeadMeshComponent->AddTexture(headMetRoughAO);
		HeadMeshComponent->AddTexture(headTranslucency);
		HumanEntity->AddComponent(HeadMeshComponent);


		// Head
		MeshComponent* HeadMeshComponent2 = new Model("resources/human/Head/Head.fbx");
		HeadMeshComponent2->SetIsForwardLit(true);
		HeadMeshComponent2->SetIsLit(false);
		HeadMeshComponent2->SetShader(PBRSkinShaderExperimental);
		HeadMeshComponent2->AddTexture(headAlbedo);
		HeadMeshComponent2->AddTexture(headNormal);
		HeadMeshComponent2->AddTexture(headMetRoughAO);
		HeadMeshComponent2->AddTexture(headTranslucency);
		HumanEntity2->AddComponent(HeadMeshComponent2);

		// TERRAIN
		// ____________________________________________________
		IEntity* TerrainEntity = new Entity;
		Chroma::Scene::AddEntity(TerrainEntity);
		MeshComponent* TerrainMeshComponent = new Terrain;
		TerrainMeshComponent->SetShader(PBRSkinShaderExperimental);
		TerrainMeshComponent->AddTexture(gridAlbedo);
		TerrainMeshComponent->AddTexture(flatNormal);
		TerrainMeshComponent->m_UVMultiply = glm::vec2(8.0f);
		TerrainEntity->AddComponent(TerrainMeshComponent);
		TerrainEntity->SetScale(glm::vec3(10.0, 1.0, 10.0));
		// ____________________________________________________


		// Eyelashes 
		MeshComponent* EyelashesMeshComponent = new Model("resources/human/Head/Eyelashes.fbx");
		HumanEntity->AddComponent(EyelashesMeshComponent);

		// Eyebrows
		MeshComponent* EyebrowsMeshComponent = new Model("resources/human/Head/Eyebrows.fbx");
		HumanEntity->AddComponent(EyebrowsMeshComponent);
	}

	void Editor::Tick()
	{
		// UI
		DrawUI();
	}

	void Editor::DrawUI()
	{
		Chroma::EditorUI::Draw();
	}

	
}