#include "GBuffer.h"
#include <component/ChromaMeshComponent.h>

void GBuffer::initialize()
{
	// create the buffer
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

	// SCENE DATA
	// - position color buffer
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	// - gViewPosition for SSAO viewspace position 
	glGenTextures(1, &gViewPosition);
	glBindTexture(GL_TEXTURE_2D, gViewPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gViewPosition, 0);

	// - fragposLightSpace color buffer for shadowmapping
	glGenTextures(1, &gFragPosLightSpace);
	glBindTexture(GL_TEXTURE_2D, gFragPosLightSpace);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gFragPosLightSpace, 0);

	// SURFACE DATA
	// - albebo buffer
	glGenTextures(1, &gAlbedo);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gAlbedo, 0);

	// - normal buffer
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gNormal, 0);

	// - gViewPosition for SSAO viewspace position 
	glGenTextures(1, &gViewNormal);
	glBindTexture(GL_TEXTURE_2D, gViewNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT5, GL_TEXTURE_2D, gViewNormal, 0);

	// - metalness/rougness/ambient occlusion buffer
	glGenTextures(1, &gMetRoughAO);
	glBindTexture(GL_TEXTURE_2D, gMetRoughAO);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT6, GL_TEXTURE_2D, gMetRoughAO, 0);

	// - tell OpenGL which color attachments we'll use for rendering 
	unsigned int attachments[7] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5, GL_COLOR_ATTACHMENT6 };
	glDrawBuffers(7, attachments); 
	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &gRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, gRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCREEN_WIDTH, SCREEN_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gRBO);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// configure shaders
	configureShaders();
}

void GBuffer::updateTransformUniforms()
{
	lightingPassShader.use();
	lightingPassShader.setVec2("scale", scale);
	lightingPassShader.setVec2("offset", offset);
}

void GBuffer::configureShaders()
{
	lightingPassShader.use();
	lightingPassShader.setInt("gPosition", 0);
	lightingPassShader.setInt("gNormal", 1);
	lightingPassShader.setInt("gAlbedo", 2);
	lightingPassShader.setInt("gMetRoughAO", 3);
	lightingPassShader.setInt("gFragPosLightSpace", 4);
	lightingPassShader.setInt("gShadowmap", 5);
	lightingPassShader.setInt("SSAO", 6);
}

void GBuffer::bindAllGBufferTextures()
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedo);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, gMetRoughAO);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, gFragPosLightSpace);
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, mShadowbuffer->getTexture());
	glActiveTexture(GL_TEXTURE6);
	glBindTexture(GL_TEXTURE_2D, mSSAOBuffer->getTexture());
}

void GBuffer::setLightingUniforms()
{
	lightingPassShader.setLightingUniforms(m_scene->getLights(), *m_scene->getRenderCamera());
	lightingPassShader.setVec3("ambient", m_scene->getAmbientColor());
}

void GBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GBuffer::calculateShadows()
{
	// 1. calculate shadows
	mShadowbuffer->calculateShadows();
}

void GBuffer::drawGeometryPass()
{
	// 1. geometry pass: render scene's geometry/color data into gbuffer
	Bind();
	geometryPassShader.use();
	geometryPassShader.setMat4("view", m_scene->getRenderCamera()->viewMat);
	geometryPassShader.setMat4("projection", m_scene->getRenderCamera()->projectionMat);
	geometryPassShader.setMat4("lightSpaceMatrix", mShadowbuffer->getLightSpaceMatrix());
	// Render Scene
	for (IChromaEntity* entity : m_scene->getEntities())
	{
		glm::mat4 finalTransformMatrix = entity->getTransformationMatrix();
		for (IChromaComponent* component : entity->getLitComponents())
		{
			finalTransformMatrix = finalTransformMatrix * ((ChromaMeshComponent*)component)->getTransformationMatrix();
			geometryPassShader.setMat4("model", finalTransformMatrix);
			((ChromaMeshComponent*)component)->DrawUpdateMaterials(geometryPassShader);
		}
	}
	unBind();
}

void GBuffer::drawLightingPass()
{
	
	// clear color buffer
	glClear(GL_COLOR_BUFFER_BIT);
	// use the lighting pass shader
	lightingPassShader.use();
	// updating transforms
	updateTransformUniforms();
	// activating textures
	bindAllGBufferTextures();
	// set lighting uniforms
	setLightingUniforms();
}

void GBuffer::blitDepthBuffer()
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_postFXBuffer->getFBO());// write to default HDR Framebuffer
	glBlitFramebuffer(
		0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST
	);
}

void GBuffer::Draw()
{
	// 0. calculate shadow textures
	calculateShadows();

	// 1. geometry pass: render scene's geometry/color data into gbuffer
	drawGeometryPass();

	// 1.5 SSAO Pass : draw SSAO in ViewSpace to be used during lighting pass
	((SSAOBuffer*)mSSAOBuffer)->Draw(gViewPosition, gViewNormal, m_scene);

	// 2. Render pass to PostFX buffer
	m_postFXBuffer->Bind();

	// 2.5 lighting pass: calculate lighting using gbuffer textures
	drawLightingPass();
	renderQuad();

	// 4. copy content of geometry's depth buffer to HDR buffer
	blitDepthBuffer();

	// 5. Unbind postFX buffer
	m_postFXBuffer->unBind();
}

GBuffer::GBuffer(ChromaScene*& Scene, Framebuffer*& PostFXBuffer)
{
	setupQuad();
	initialize();
	m_scene = Scene;
	mShadowbuffer = new ShadowBuffer(m_scene);
	m_postFXBuffer = PostFXBuffer;
}

GBuffer::~GBuffer()
{
}