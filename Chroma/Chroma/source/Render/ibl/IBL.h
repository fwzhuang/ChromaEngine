#ifndef _CHROMA_IBL_
#define _CHROMA_IBL_

//common
#include <common/PrecompiledHeader.h>
// chroma
#include <model/BoxPrimitive.h>
#include <model/NDCPlanePrimitive.h>
#include <texture/HDRTexture.h>
#include <shader/Shader.h>
#include <ChromaConfig.h>

namespace Chroma
{
	class IBL
	{
	public:

		void Draw();

		void SetIBLTexture(HDRTexture newHDRTexture);
		void LoadIBL(std::string const& newHDRTexturePath);
		inline std::string  GetSourcePath() { return m_HDRtexture.GetSourcePath(); };

		inline unsigned int GetEnvCubeMapID() const { return m_envCubeMap; };
		inline unsigned int GetIrradianceMapID() const { return m_irradianceMap; };
		inline unsigned int GetPrefilterMapID() const { return m_prefilterMap; };
		inline unsigned int GetHDRTextureID() { return m_HDRtexture.GetID(); };
		inline unsigned int GetBRDFLUTID() const { return m_brdfLUTTexture; };

		template <typename UniformType>
		void SetUniform(std::string uniformName, UniformType uniform)
		{
			m_envMapShader.Use();
			m_envMapShader.SetUniform(uniformName, uniform);
		}
		IBL();
		~IBL() {};

	private:
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[6] = {
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};
		// hdr texture
		HDRTexture m_HDRtexture;
		// capture primitives
		BoxPrimitive m_captureCube;
		NDCPlanePrimitive m_capturePlane;
		// shaders
		Shader m_envMapShader{ "resources/shaders/fragIBL.glsl", "resources/shaders/vertexCubeMap.glsl" };
		Shader m_irradienceMapShader{ "resources/shaders/fragIrradiance.glsl", "resources/shaders/vertexCubeMap.glsl" };
		Shader m_prefilterMapShader{ "resources/shaders/fragPrefilter.glsl", "resources/shaders/vertexCubeMap.glsl" };
		Shader m_brdfShader{ "resources/shaders/fragBRDF.glsl", "resources/shaders/frameBufferVertex.glsl" };
		// capture buffers
		unsigned int m_captureFBO, m_captureRBO;
		// textures
		unsigned int m_envCubeMap, m_irradianceMap, m_prefilterMap, m_brdfLUTTexture;

		// functions
		void Init();
		void ClearTextureBuffers();
		void GenerateEnvCubeMap();
		void GenerateIrradianceMap();
		void GeneratePrefilterMap();
		void GenerateBRDFLUTMap();
	};
}

#endif
