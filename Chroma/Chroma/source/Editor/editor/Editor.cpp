#include "Editor.h"

#include <resources/ResourceManager.h>


#include <entity/Entity.h>
#include <terrain/Terrain.h>
#include <component/AnimationComponent.h>
#include <model/SkinnedMesh.h>
#include <model/Model.h>
#include <bipedal/BipedalAnimationStateMachine.h>
#include <component/animation/IKAnimConstraint.h>
#include <component/CharacterPhysicsComponent.h>

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
			pointLight->m_Quadratic *= 4.0f;
			pointLight->m_Linear *= 2.0f;
			Lights.push_back(pointLight);
		}

		// SUNLIGHT
		Light* Sun = new Light(Light::SUNLIGHT, glm::vec3(0.2, -0.8, 0.3), 2.0f);
		Sun->setDiffuse(glm::vec3(1.0));
		Sun->setIntensity(3.0);
		Lights.push_back(Sun);
		Chroma::Scene::SetLights(Lights);

		// BACKGROUND
		//Chroma::Scene::GetIBL()->LoadIBL("resources/textures/ibl/grey_ibl/grey.hdr");

		// ____________________________________________________
		// SHADERS
		// ____________________________________________________
		Shader UnlitShader("resources/shaders/fragBasic.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");
		UnlitShader.Use();
		UnlitShader.SetUniform("color", glm::vec3(1, 1, 0));
		Shader SemiTransparentShader("resources/shaders/fragPBRAlpha.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");
		Shader PBRShader("resources/shaders/fragPBR.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");

		// Lookdev Sphere
		Texture lookdevAlbedo("resources/textures/pbr/lookdev_pbr/albedo.jpg");
		lookdevAlbedo.m_Type = Texture::ALBEDO;
		Texture lookdevNormal("resources/textures/pbr/lookdev_pbr/normal.jpg");
		lookdevNormal.m_Type = Texture::NORMAL;
		Texture lookdevMetRoughAO("resources/textures/pbr/lookdev_pbr/MetRoughAO.jpg");
		lookdevMetRoughAO.m_Type = Texture::METROUGHAO;
		// ____________________________________________________
		// TEXTURES
		// ____________________________________________________
		// Generic
		Texture blackAlbedo("resources/textures/colors/black.jpg");
		Texture greyAlbedo("resources/textures/colors/grey.jpg");
		Texture whiteAlbedo("resources/textures/colors/white.jpg");
		Texture gridAlbedo("resources/animation/textures/grid.jpg");
		Texture flatNormal("resources/textures/test/flat_normal.jpg");
		flatNormal.m_Type = Texture::NORMAL;
		Texture alphaTestAlbedo("resources/textures/test/grass.png");

		// Animated Model
		Texture walkingAlbedo("resources/animation/vampire_textures/albedo.jpg");
		walkingAlbedo.m_Type = Texture::ALBEDO;
		Texture walkingNormal("resources/animation/vampire_textures/normal.jpg");
		walkingNormal.m_Type = Texture::NORMAL;
		Texture walkingMetRoughAO("resources/animation/vampire_textures/MetRoughAO.jpg");
		walkingMetRoughAO.m_Type = Texture::METROUGHAO;

		// Floor Panels
		Texture woodBoardsAlbedo("resources/textures/pbr/hardwood_pbr/albedo.jpg");
		woodBoardsAlbedo.m_Type = Texture::ALBEDO;
		Texture woodBoardsNormal("resources/textures/pbr/hardwood_pbr/normal.jpg");
		woodBoardsNormal.m_Type = Texture::NORMAL;
		Texture woodBoardsMetRoughAO("resources/textures/pbr/hardwood_pbr/MetRoughAO.jpg");
		woodBoardsMetRoughAO.m_Type = Texture::METROUGHAO;
		// ____________________________________________________

		// ANIMATED MODEL
		// ____________________________________________________
		IEntity* AnimModelEntity = new Entity;
		AnimModelEntity->SetName("AnimationModel");
		Chroma::Scene::AddEntity(AnimModelEntity);
		AnimModelEntity->SetScale(glm::vec3(0.03f));
		//AnimModelEntity->SetTranslation(glm::vec3(0.0, 5.0, 0.0));
		// mesh
		MeshComponent* AnimModelMeshComponent = new SkinnedMesh("resources/animation/vampire.fbx");
		AnimModelMeshComponent->AddTexture(walkingAlbedo);
		AnimModelMeshComponent->AddTexture(walkingNormal);
		AnimModelMeshComponent->AddTexture(walkingMetRoughAO);
		AnimModelMeshComponent->SetIsDoubleSided(true);
		AnimModelEntity->AddComponent(AnimModelMeshComponent);
		// rigid
		PhysicsComponent* AnimModelRigidComponent = new CharacterPhysicsComponent();
		AnimModelRigidComponent->SetColliderShape(ColliderShape::Capsule);
		AnimModelRigidComponent->SetCollisionState(ColliderState::Kinematic);
		AnimModelEntity->AddComponent(AnimModelRigidComponent);
		// animation
		AnimationComponent* AnimModelAnimationComponent = new AnimationComponent();
		Animator AnimModelAnimator;
		AnimModelAnimator.BindSkeleton(AnimModelMeshComponent);
		AnimModelAnimator.LoadAnimations("resources/animation/locomotion/Walk.fbx");
		AnimModelAnimator.LoadAnimations("resources/animation/locomotion/Idle.fbx");
		AnimModelAnimator.LoadAnimations("resources/animation/locomotion/Run.fbx");
		AnimModelAnimator.LoadAnimations("resources/animation/locomotion/Jump.fbx");
		AnimModelAnimator.LoadAnimations("resources/animation/locomotion/Roll.fbx");
		AnimModelAnimator.CompressAnimations();
		AnimationStateMachine* AnimModelAnimationStateMachine = new BipedalAnimationStateMachine;
		// animation state
		AnimModelAnimationComponent->SetAnimator(AnimModelAnimator);
		AnimModelAnimationComponent->SetAnimationStateMachine(AnimModelAnimationStateMachine);
		AnimModelEntity->AddComponent(AnimModelAnimationComponent);
		//// character controller
		CharacterControllerComponent* AnimModelCharacterController = new ThirdPersonCharacterController();
		// camera controller
		ICameraController* AnimModelCameraController = new ThirdPersonCameraController();
		Chroma::Scene::GetRenderCamera()->SetCustomCameraController(AnimModelCameraController);
		AnimModelCharacterController->SetCustomCameraController(AnimModelCameraController);
		// adding the component
		AnimModelEntity->AddComponent(AnimModelCharacterController);
		// ik
		AnimConstraintComponent* AnimModelIKComponent = new IKAnimConstraint();
		AnimModelEntity->AddComponent(AnimModelIKComponent);

		// ____________________________________________________


		// ____________________________________________________

		// IK TEST MODEL
		// ____________________________________________________
		IEntity* IKTestEntity = new Entity;
		IKTestEntity->SetName("IKTestEntity");
		Chroma::Scene::AddEntity(IKTestEntity);
		IKTestEntity->SetScale(glm::vec3(0.5f));
		//IKTestEntity->SetTranslation(glm::vec3(5.0, 0.0, 0.0));
		// mesh
		MeshComponent* IKTestMeshComponent = new SkinnedMesh("resources/animation/test_tentacle/test_tentacle_iktest.fbx");
		IKTestMeshComponent->SetShader(PBRShader);
		IKTestMeshComponent->AddTexture(gridAlbedo);
		IKTestMeshComponent->AddTexture(flatNormal);
		IKTestMeshComponent->GetMaterial().SetUVMultiply(8.0f);
		IKTestEntity->AddComponent(IKTestMeshComponent);
		// rigid
		PhysicsComponent* IKTestRigidComponent = new PhysicsComponent();
		IKTestRigidComponent->SetColliderShape(ColliderShape::Capsule);
		IKTestRigidComponent->SetCollisionState(ColliderState::Kinematic);
		IKTestEntity->AddComponent(IKTestRigidComponent);
		// animation
		AnimationComponent* IKTestAnimationComponent = new AnimationComponent();
		Animator IKTestAnimator;
		IKTestAnimator.BindSkeleton(IKTestMeshComponent);
		IKTestAnimator.LoadAnimations("resources/animation/test_tentacle/test_tentacle.fbx");
		IKTestAnimator.CompressAnimations();
		IKTestAnimationComponent->SetAnimator(IKTestAnimator);
		IKTestEntity->AddComponent(IKTestAnimationComponent);
		// ik
		AnimConstraintComponent* IKTestIKComponent = new IKAnimConstraint();
		IKTestEntity->AddComponent(IKTestIKComponent);
		// ____________________________________________________



		//// CAPSULE
		//// ____________________________________________________
		//IEntity* CapsuleEntity = new Entity;
		//Chroma::Scene::AddEntity(CapsuleEntity);
		//// mesh
		//MeshComponent* CapsuleMeshComponent = new Model("resources/primitives/capsule.fbx");
		//CapsuleMeshComponent->SetShader(&PBRShader);
		//CapsuleMeshComponent->AddTexture(walkingAlbedo);
		//CapsuleMeshComponent->AddTexture(walkingNormal);
		//CapsuleMeshComponent->AddTexture(walkingMetRoughAO);
		////CapsuleMeshComponent->SetShader(&PBRShader);
		////CapsuleMeshComponent->AddTexture(greyAlbedo);
		//CapsuleEntity->AddComponent(CapsuleMeshComponent);
		//// rigid
		//PhysicsComponent* CapsuleRigidComponent = new PhysicsComponent();
		//CapsuleRigidComponent->SetColliderShape(ColliderShape::Box);
		//CapsuleRigidComponent->SetCollisionState(ColliderState::Kinematic);
		//CapsuleEntity->AddComponent(CapsuleRigidComponent);
		//// transform
		//CapsuleEntity->SetTranslation(glm::vec3(0,1.75,0.0));
		//// character controller
		//CharacterControllerComponent* CapsuleCharacterController = new ThirdPersonCharacterController();
		//// camera controller
		//ICameraController* CapsuleCameraController = new ThirdPersonCameraController();
		//Chroma::Scene::GetRenderCamera()->SetCustomCameraController(CapsuleCameraController);
		//CapsuleCharacterController->SetCustomCameraController(CapsuleCameraController);
		//// adding the component
		//CapsuleEntity->AddComponent(CapsuleCharacterController);
		// ____________________________________________________


		// TERRAIN
		// ____________________________________________________
		// ground
		IEntity* TerrainEntity = new Entity;
		Chroma::Scene::AddEntity(TerrainEntity);

		MeshComponent* TerrainMeshComponent = new Model("resources/assets/level/ground.fbx");
		TerrainMeshComponent->SetShader(PBRShader);
		TerrainMeshComponent->AddTexture(woodBoardsAlbedo);
		TerrainMeshComponent->AddTexture(woodBoardsNormal);
		TerrainMeshComponent->AddTexture(woodBoardsMetRoughAO);
		TerrainMeshComponent->GetMaterial().SetUVMultiply(8.0);
		TerrainEntity->AddComponent(TerrainMeshComponent);

		// rigid
		PhysicsComponent* TerrainPhysicsComponent = new PhysicsComponent();
		TerrainPhysicsComponent->SetColliderShape(ColliderShape::Convex);
		TerrainPhysicsComponent->SetCollisionState(ColliderState::Static);
		TerrainEntity->AddComponent(TerrainPhysicsComponent);

		// 10 degrees
		IEntity* TerrainEntity2 = new Entity;
		Chroma::Scene::AddEntity(TerrainEntity2);

		MeshComponent* TerrainMesh2Component = new Model("resources/assets/level/10degrees.fbx");
		TerrainMesh2Component->SetShader(PBRShader);
		TerrainMesh2Component->AddTexture(woodBoardsAlbedo);
		TerrainMesh2Component->AddTexture(woodBoardsNormal);
		TerrainMesh2Component->AddTexture(woodBoardsMetRoughAO);
		TerrainMesh2Component->GetMaterial().SetUVMultiply(8.0);
		TerrainEntity2->AddComponent(TerrainMesh2Component);

		// rigid
		PhysicsComponent* TerrainPhysicsComponent2 = new PhysicsComponent();
		TerrainPhysicsComponent2->SetColliderShape(ColliderShape::Convex);
		TerrainPhysicsComponent2->SetCollisionState(ColliderState::Static);
		TerrainEntity2->AddComponent(TerrainPhysicsComponent2);


		// 20 degrees
		IEntity* TerrainEntity3 = new Entity;
		Chroma::Scene::AddEntity(TerrainEntity3);

		MeshComponent* TerrainMesh3Component = new Model("resources/assets/level/20degrees.fbx");
		TerrainMesh3Component->SetShader(PBRShader);
		TerrainMesh3Component->AddTexture(woodBoardsAlbedo);
		TerrainMesh3Component->AddTexture(woodBoardsNormal);
		TerrainMesh3Component->AddTexture(woodBoardsMetRoughAO);
		TerrainMesh3Component->GetMaterial().SetUVMultiply(8.0);
		TerrainEntity3->AddComponent(TerrainMesh3Component);

		// rigid
		PhysicsComponent* TerrainPhysicsComponent3 = new PhysicsComponent();
		TerrainPhysicsComponent3->SetColliderShape(ColliderShape::Convex);
		TerrainPhysicsComponent3->SetCollisionState(ColliderState::Static);
		TerrainEntity3->AddComponent(TerrainPhysicsComponent3);

		// 35 degrees
		IEntity* TerrainEntity4 = new Entity;
		Chroma::Scene::AddEntity(TerrainEntity4);

		MeshComponent* TerrainMesh4Component = new Model("resources/assets/level/35degrees.fbx");
		TerrainMesh4Component->SetShader(PBRShader);
		TerrainMesh4Component->AddTexture(woodBoardsAlbedo);
		TerrainMesh4Component->AddTexture(woodBoardsNormal);
		TerrainMesh4Component->AddTexture(woodBoardsMetRoughAO);
		TerrainMesh4Component->GetMaterial().SetUVMultiply(8.0);
		TerrainEntity4->AddComponent(TerrainMesh4Component);

		// rigid
		PhysicsComponent* TerrainPhysicsComponent4 = new PhysicsComponent();
		TerrainPhysicsComponent4->SetColliderShape(ColliderShape::Convex);
		TerrainPhysicsComponent4->SetCollisionState(ColliderState::Static);
		TerrainEntity4->AddComponent(TerrainPhysicsComponent4);



		// ____________________________________________________

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
			pointLight->m_Quadratic *= 4.0f;
			pointLight->m_Linear *= 2.0f;
			Lights.push_back(pointLight);
		}

		// SUNLIGHT
		Light* Sun = new Light(Light::SUNLIGHT, glm::vec3(-10.0, -1.0, -0.1), 2.0f);
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
		HumanEntity->SetTranslation(glm::vec3(-6.8, 0, 0));


		IEntity* LookDevEntity = new Entity;
		LookDevEntity->SetName("LookDev Sphere");
		Chroma::Scene::AddEntity(LookDevEntity);
		LookDevEntity->SetScale(glm::vec3(0.7f));
		LookDevEntity->SetTranslation(glm::vec3(6.2, 7.1, 0));

		// ____________________________________________________
		// SHADERS
		// ____________________________________________________

		Shader SemiTransparentShader("resources/shaders/fragPBRAlpha.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");
		Shader PBRSkinShaderExperimental("resources/shaders/fragSSSS.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");
		Shader PBRSkinShader("resources/shaders/fragSSSS_backup.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");
		Shader PBRShader("resources/shaders/fragPBR.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");

		// ____________________________________________________
		// TEXTURES
		// ____________________________________________________

		//// Default
		// Generic
		Texture greyAlbedo("resources/textures/colors/grey.jpg");
		Texture gridAlbedo("resources/animation/textures/grid.jpg");
		Texture flatNormal("resources/textures/test/flat_normal.jpg");


		// Head
		Texture headAlbedo = Chroma::ResourceManager::LoadTexture("resources/human/textures/head/head_albedo.jpg");
		headAlbedo.m_Type = Texture::ALBEDO;
		Texture headNormal = Chroma::ResourceManager::LoadTexture("resources/human/textures/head/head_normal.jpg");
		headNormal.m_Type = Texture::NORMAL;
		Texture headMetRoughAO = Chroma::ResourceManager::LoadTexture("resources/human/textures/head/head_metroughao.jpg");
		headMetRoughAO.m_Type = Texture::METROUGHAO;
		Texture headTranslucency = Chroma::ResourceManager::LoadTexture("resources/human/textures/head/head_translucency.jpg");
		headTranslucency.m_Type = Texture::TRANSLUCENCY;

		// Lookdev Sphere
		//Texture lookDevAlbedo = Chroma::ResourceManager::LoadTexture("resources/textures/pbr/lookdev_pbr/albedo.jpg");
		//lookDevAlbedo.m_Type = Texture::ALBEDO;
		//Texture lookDevNormal = Chroma::ResourceManager::LoadTexture("resources/textures/pbr/lookdev_pbr/normal.jpg");
		//lookDevNormal.m_Type = Texture::NORMAL;
		//Texture lookDevMetRoughAO = Chroma::ResourceManager::LoadTexture("resources/textures/pbr/lookdev_pbr/MetRoughAO.jpg");
		//lookDevMetRoughAO.m_Type = Texture::METROUGHAO;

		// ____________________________________________________
		// MODELS
		// ____________________________________________________

		// HEAD		
		MeshComponent* HeadMeshComponent = new Model("resources/human/Head/Head.fbx");
		Material HeadMeshMaterial;
		HeadMeshMaterial.SetShader(PBRShader);
		HeadMeshMaterial.AddTexture(headAlbedo);
		HeadMeshMaterial.AddTexture(headNormal);
		HeadMeshMaterial.AddTexture(headMetRoughAO);
		HeadMeshMaterial.AddTexture(headTranslucency);
		HeadMeshMaterial.AddUniform("TestUniform", 1);
		HeadMeshMaterial.AddUniform("UVMultiply", glm::vec2(3.0));
		HeadMeshMaterial.AddUniform("Color", glm::vec3(0.5));
		HeadMeshMaterial.AddUniform("ColorAlpha", glm::vec4(0.5));
		HeadMeshComponent->SetMaterial(HeadMeshMaterial);
		HumanEntity->AddComponent(HeadMeshComponent);

		// Eyelashes 
		MeshComponent* EyelashesMeshComponent = new Model("resources/human/Head/Eyelashes.fbx");
		HumanEntity->AddComponent(EyelashesMeshComponent);

		// Eyebrows
		MeshComponent* EyebrowsMeshComponent = new Model("resources/human/Head/Eyebrows.fbx");
		HumanEntity->AddComponent(EyebrowsMeshComponent);

		// Head Physics
		PhysicsComponent* HeadPhysicsComponent = new PhysicsComponent();
		HeadPhysicsComponent->SetColliderShape(Box);
		HeadPhysicsComponent->SetCollisionState(Kinematic);
		HumanEntity->AddComponent(HeadPhysicsComponent);

		// LOOKDEVSPHERE
		//MeshComponent* lookDevMeshComponent = new Model("resources/lookdev/sphere.obj");
		//lookDevMeshComponent->SetShader(PBRShader);
		//lookDevMeshComponent->AddTexture(lookDevAlbedo);
		//lookDevMeshComponent->AddTexture(lookDevNormal);
		//lookDevMeshComponent->AddTexture(flatNormal);
		//LookDevEntity->AddComponent(lookDevMeshComponent);

		//// LookDev Physics
		//PhysicsComponent* LookDevPhysicsComponent = new PhysicsComponent();
		//LookDevPhysicsComponent->SetColliderShape(Box);
		//LookDevPhysicsComponent->SetCollisionState(Kinematic);
		//LookDevEntity->AddComponent(LookDevPhysicsComponent);


	}

	void Editor::PopulateTestScene3()
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
			pointLight->m_Quadratic *= 4.0f;
			pointLight->m_Linear *= 2.0f;
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
		Shader PBRShader("resources/shaders/fragPBR.glsl", "resources/shaders/vertexLitShadowsNormals.glsl");

	
		// ____________________________________________________
		// TEXTURES
		// ____________________________________________________
		// Generic
		Texture blackAlbedo("resources/textures/colors/black.jpg");
		Texture greyAlbedo("resources/textures/colors/grey.jpg");
		Texture whiteAlbedo("resources/textures/colors/white.jpg");
		Texture gridAlbedo("resources/animation/textures/grid.jpg");
		Texture flatNormal("resources/textures/test/flat_normal.jpg");
		flatNormal.m_Type = Texture::NORMAL;


		Texture headMetRoughAO = Chroma::ResourceManager::LoadTexture("resources/human/textures/head/head_metroughao.jpg");
		headMetRoughAO.m_Type = Texture::METROUGHAO;
		// ____________________________________________________

		// ANIMATED MODEL
		// ____________________________________________________
		IEntity* ClothModelEntity = new Entity;
		ClothModelEntity->SetName("ClothModel");
		Chroma::Scene::AddEntity(ClothModelEntity);
		ClothModelEntity->SetScale(glm::vec3(10.0f));
		//AnimModelEntity->SetTranslation(glm::vec3(0.0, 10.0, 0.0));
		// mesh
		MeshComponent* ClothModelMeshComponent = new Model("resources/lookdev/cloth.fbx");
		//ClothModelMeshComponent->SetShader(PBRShader);
		ClothModelMeshComponent->AddTexture(greyAlbedo);
		ClothModelMeshComponent->AddTexture(flatNormal);
		ClothModelMeshComponent->AddTexture(headMetRoughAO);

		ClothModelEntity->AddComponent(ClothModelMeshComponent);
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