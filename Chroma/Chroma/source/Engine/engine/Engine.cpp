#include "Engine.h"


namespace Chroma
{

#ifdef DEBUG
	bool Engine::m_DrawProfilingWindow;
#endif

	void Engine::Update()
	{
		CHROMA_PROFILE_FUNCTION();
		// Scene
		Chroma::Scene::Update();

		// Physics
		Chroma::Physics::Update();

		// Update Components
		UpdateComponents();

	}

	void Engine::UpdateComponents()
	{
		CHROMA_PROFILE_FUNCTION();
		// Animation Components
		Chroma::JobSystem::Execute([] {
			for (UID const& ComponentUID : Chroma::Scene::GetAnimationComponentUIDs()) 
			{
				Chroma::Scene::GetComponent(ComponentUID)->Update();
			}});

		// Character Controller Components
		Chroma::JobSystem::Execute([] {
			for (UID const& ComponentUID : Chroma::Scene::GetCharacterControllerUIDs())
			{
				Chroma::Scene::GetComponent(ComponentUID)->Update();
			}});


		// IK Components
		Chroma::JobSystem::Execute([] {
			for (UID const& ComponentUID : Chroma::Scene::GetIKComponentUIDs())
			{
				Chroma::Scene::GetComponent(ComponentUID)->Update();
			}});


		// UI Components
		Chroma::JobSystem::Execute([] {
			for (UID const& ComponentUID : Chroma::Scene::GetUIComponentUIDs())
			{
				Chroma::Scene::GetComponent(ComponentUID)->Update();
			}});

		//// Animation Components
		//for (UID const& ComponentUID : Chroma::Scene::GetAnimationComponentUIDs())
		//{
		//	Chroma::Scene::GetComponent(ComponentUID)->Update();
		//}

		//// Character Controller Components
		//for (UID const& ComponentUID : Chroma::Scene::GetCharacterControllerUIDs())
		//{
		//	Chroma::Scene::GetComponent(ComponentUID)->Update();
		//}

		//// IK Components
		//for (UID const& ComponentUID : Chroma::Scene::GetIKComponentUIDs())
		//{
		//	Chroma::Scene::GetComponent(ComponentUID)->Update();
		//}

		//// UI Components
		//for (UID const& ComponentUID : Chroma::Scene::GetUIComponentUIDs())
		//{
		//	Chroma::Scene::GetComponent(ComponentUID)->Update();
		//}
	}

	void Engine::Draw()
	{
		CHROMA_PROFILE_FUNCTION();
		Chroma::Render::RenderScene();

		Chroma::UI::Draw();
		Chroma::Screen::Update();
	}

	void Engine::Tick()
	{
#ifdef DEBUG
#ifndef EDITOR

		CHROMA_PROFILE_BEGIN_SESSION("Engine");
#endif
#endif
		CHROMA_PROFILE_FUNCTION();
		// update time
		Chroma::Time::Update();

		// process input
		ProcessInput();

		// update while lag is less than framerate cap
		while (Chroma::Time::GetLag() >= Chroma::Time::GetMSPerFrame())
		{
			Update();
			Chroma::Time::DecreaseLag(Chroma::Time::GetMSPerFrame());
		}
		// consider Sleep if Render misaligning with update https://dewitters.com/dewitters-gameloop/

		// Render Scene
		Chroma::JobSystem::Wait();
		Draw();

#ifdef DEBUG
#ifndef EDITOR

		if (Chroma::Input::IsPressed(Chroma::Input::TAB))
			m_DrawProfilingWindow = m_DrawProfilingWindow ? false : true;

		if (m_DrawProfilingWindow)
			Chroma::UI::ShowProfilingWindow();
		CHROMA_PROFILE_END_SESSION();
#endif
#endif

	}

	void Engine::Init()
	{
		// Logging
		Chroma::Log::Init();
		CHROMA_INFO_UNDERLINE;
		CHROMA_INFO("Chroma Engine Initializing...");
		CHROMA_INFO_UNDERLINE;

		// Core
		Chroma::Core::Init();

		// Job System
		Chroma::JobSystem::Initialize();

		// Scene
		Chroma::Scene::Init();
		CHROMA_INFO("Scene Initialized.");

		// Scene Manager
		Chroma::SceneManager::Init();
		CHROMA_INFO("Scene Manager Initialized.");

		// Input
		Chroma::JobSystem::Execute(Chroma::Input::Init);
		Chroma::Input::BindCamera(Chroma::Scene::GetRenderCamera());
		CHROMA_INFO("Input Initialized.");

		// Time
		Chroma::JobSystem::Execute(Chroma::Time::Init);
		CHROMA_INFO("Time Initialized.");

		// Renderer
		Chroma::Render::Init();
		CHROMA_INFO("Renderer Initialized.");

		// GUI
		Chroma::JobSystem::Execute(Chroma::UI::Init);
		CHROMA_INFO("GUI Initialized.");

		// PhysicsEngine
		Chroma::JobSystem::Execute(Chroma::Physics::Init);
		Chroma::JobSystem::Wait();
		Chroma::Physics::BindDebugBuffer(Chroma::Render::GetDebugBuffer());
		CHROMA_INFO("Physics Initialized.");

		// Final
		CHROMA_INFO_UNDERLINE;
		CHROMA_INFO("Chroma Engine Successfully Initialized.");
		CHROMA_INFO_UNDERLINE;

	}

	void Engine::ProcessInput()
	{
		CHROMA_PROFILE_FUNCTION();
		// update input
		Chroma::Input::Update();

		// Camera
		Chroma::Scene::GetRenderCamera()->Update();
	}
}


