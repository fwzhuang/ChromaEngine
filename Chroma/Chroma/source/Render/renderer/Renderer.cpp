#include "Renderer.h"


void Renderer::clearBuffers()
{
	// DEBUG BUFFER
	m_debugBuffer->ClearBuffer();
}

void Renderer::renderDefferedComponents()
{
	// DEFFERED BUFFER
	m_GBuffer->Draw();
}

void Renderer::renderForwardComponents()
{
	// FORWARD BUFFER
	m_forwardBuffer->Draw();
}

void Renderer::renderDebug()
{
	// DEBUG BUFFER
	m_debugBuffer->Draw();	
}

void Renderer::renderPostFX()
{
	// POSTFX BUFFER
	m_screenManager->useBloom ? ((PostFXBuffer*)m_postFXBuffer)->Draw(true) : m_postFXBuffer->Draw();
	m_postFXBuffer->setUniform("exposure", m_screenManager->exposure);
	m_postFXBuffer->setUniform("gamma", m_screenManager->gamma);
}

void Renderer::Initialize()
{
	m_GBuffer = new GBuffer(m_Scene, m_postFXBuffer);
	m_debugBuffer = new DebugBuffer(m_Scene->GetRenderCamera(), m_postFXBuffer);
	m_forwardBuffer = new ForwardBuffer(m_Scene, m_postFXBuffer);
}

void Renderer::RenderScene()
{
	// Deferred
	renderDefferedComponents();

	// Forward
	renderForwardComponents();

	// Debug
	renderDebug();

	// Post FX
	renderPostFX();

	// Clear
	clearBuffers();
}


Renderer::Renderer(Scene*& Scene, const ScreenManager* ScreenManager)
{
	m_Scene = Scene;
	m_screenManager = ScreenManager;
	Initialize();
}

Renderer::~Renderer()
{
	delete m_Scene;
	delete m_screenManager;
	delete m_GBuffer;
	delete m_debugBuffer;
	delete m_postFXBuffer;
}
