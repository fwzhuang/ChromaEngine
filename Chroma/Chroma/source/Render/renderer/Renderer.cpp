#include "Renderer.h"


void Renderer::renderDefferedComponents()
{
	// GBUFFER
	mGBuffer->Draw();
}

void Renderer::renderForwardComponents()
{
	// Render Forward Components
	// Enitities
	for (IChromaEntity* entity : mScene->getEntities())
	{
		// render unlit components
		if (entity->getUnlitComponents().size() > 0)
		{
			glm::mat4 finalTransformMatrix = entity->getTransformationMatrix();
			for (IChromaComponent* component : entity->getUnlitComponents())
				((ChromaMeshComponent*)component)->DrawUpdateTransforms(*mScene->getRenderCamera(), finalTransformMatrix);
		}
		// render transparent components
		if (mScene->getTransparentEntities().size() > 0)
			renderTransparency();
	}
	// SkyBox
	if (mScreenManager->useSkybox)
		mSkybox->Draw();
}

void Renderer::renderTransparency()
{
	////glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Sorting for Transparency Shading
	std::map<float, IChromaEntity*> alpha_sorted;
	for (IChromaEntity* TransparentEntity : mScene->getTransparentEntities())
	{
		float distance = glm::length(TransparentEntity->getPosition() - mScene->getRenderCamera()->getPosition());
		alpha_sorted[distance] =  TransparentEntity;
	}
	// iterating from furthest to closest
	for (std::map<float, IChromaEntity*>::reverse_iterator it = alpha_sorted.rbegin(); it != alpha_sorted.rend(); ++it)
	{
		glm::mat4 finalTransformMatrix = it->second->getTransformationMatrix();
		for (IChromaComponent* component : it->second->getTransparentComponents())
			((ChromaMeshComponent*)component)->Draw(*mScene->getRenderCamera(), mScene->getLights(), finalTransformMatrix);
	}
}

void Renderer::renderPostFX()
{
	mScreenManager->useBloom ? mPostFXBuffer->Draw(true) : mPostFXBuffer->Draw();
	mPostFXBuffer->setUniform("exposure", mScreenManager->exposure);
	mPostFXBuffer->setUniform("gamma", mScreenManager->gamma);
}

void Renderer::Init()
{
	mGBuffer = new GBuffer(mScene, mPostFXBuffer);
	mSkybox = new SkyBox(*mScene->getRenderCamera());
}

void Renderer::RenderScene()
{
	// Deferred
	renderDefferedComponents();

	// Forward
	renderForwardComponents();

	// Post FX
	renderPostFX();
}


Renderer::Renderer(ChromaSceneManager*& Scene, const ChromaScreenManager* ScreenManager)
{
	mScene = Scene;
	mScreenManager = ScreenManager;
	Init();
}

Renderer::~Renderer()
{
}
