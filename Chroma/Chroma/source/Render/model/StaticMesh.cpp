#include "StaticMesh.h"
#include <scene/Scene.h>
#include <resources/ModelLoader.h>
#include <render/Render.h>
#include <buffer/GBuffer.h>

std::mutex StaticMesh::m_Mutex;
std::vector<std::future<void>> StaticMesh::m_Futures;


void StaticMesh::SetupMesh()
{
	// Generate buffers
	// Vertex Array Object Buffer
	glGenVertexArrays(1, &m_MeshData.VAO);
	// Vertex Buffer and Element Buffer
	glGenBuffers(1, &m_MeshData.VBO);
	glGenBuffers(1, &m_MeshData.EBO);

	// Bind buffers
	glBindVertexArray(m_MeshData.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_MeshData.VBO);
	glBufferData(GL_ARRAY_BUFFER, m_MeshData.verts.size() * sizeof(ChromaVertex), &m_MeshData.verts[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_MeshData.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_MeshData.indices.size() * sizeof(unsigned int),
		&m_MeshData.indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ChromaVertex), (void*)0);
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ChromaVertex), (void*)offsetof(ChromaVertex, ChromaVertex::m_normal));
	// vertex uvs
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ChromaVertex), (void*)offsetof(ChromaVertex, ChromaVertex::m_texCoords));
	// vertex tangents
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(ChromaVertex), (void*)offsetof(ChromaVertex, ChromaVertex::m_tangent));
	// vertex bitangents
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ChromaVertex), (void*)offsetof(ChromaVertex, ChromaVertex::m_bitangent));
	// vertex colors // is at the 7th index as 5 and 6 are used for skinning
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(ChromaVertex), (void*)offsetof(ChromaVertex, ChromaVertex::m_color));
	// second UV set
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(ChromaVertex), (void*)offsetof(ChromaVertex, ChromaVertex::m_texCoords2));

	glBindVertexArray(0);

	// BBOX
	//CalculateBBox();
	//CalculateCentroid();

	// Cleanup
	CleanUp();

	// Mesh complete
	m_MeshData.isRenderBuffersInitialized = true;
}

void StaticMesh::UpdateUniforms(Shader& shader, Camera& RenderCam)
{
	UpdateTransformUniforms(shader, RenderCam);
	UpdateMaterialUniforms(shader);
	UpdateTextureUniforms(shader);
}


void StaticMesh::UpdateTextureUniforms(Shader& shader)
{
	// UV Modifiers
	shader.SetUniform("UVMultiply", m_Material.GetUVMultiply());

	// updating shader's texture uniforms
	unsigned int diffuseNr{ 1 };
	unsigned int normalNr{ 1 };
	unsigned int roughnessNr{ 1 };
	unsigned int metalnessNr{ 1 };
	unsigned int metroughaoNr{ 1 };
	unsigned int aoNr{ 1 };
	unsigned int translucencyNr{ 1 };
	for (int i = 0; i < GetNumTextures(); i++)
	{
		// building the uniform name
		std::string name;
		std::string texturenum;
		Chroma::Type::Texture textureType = GetTextureSet()[i].m_Type;

		switch(textureType)
		{
		case Chroma::Type::Texture::kAlbedo:
			{
				name = "material.texture_albedo";
				texturenum = std::to_string(diffuseNr++);
				// set use texture albedo
				shader.SetUniform("UseAlbedoMap", true);
				break;
			}
		case Chroma::Type::Texture::kNormal:
			{
				name = "material.texture_normal";
				texturenum = std::to_string(normalNr++);
				// set use texture normals
				shader.SetUniform("UseNormalMap", true);
				break;
			}
		case Chroma::Type::Texture::kMetRoughAO:
			{
				name = "material.texture_MetRoughAO";
				texturenum = std::to_string(metroughaoNr++);
				// set use texture metroughao
				shader.SetUniform("UseMetRoughAOMap", true);
				break;
			}
		case Chroma::Type::Texture::kMetalness:
			{
				name = "material.texture_metalness";
				texturenum = std::to_string(metalnessNr++);
				break;
			}
		case Chroma::Type::Texture::kRoughness:
			{
				name = "material.texture_roughness";
				texturenum = std::to_string(roughnessNr++);
				break;
			}
		case Chroma::Type::Texture::kAO:
			{
				name = "material.texture_ao";
				texturenum = std::to_string(aoNr++);
				break;
			}
		case Chroma::Type::Texture::kTranslucency:
		{
			name = "material.texture_translucency";
			texturenum = std::to_string(translucencyNr++);
			shader.SetUniform("UseTranslucencyMap", true);
			break;
		}
		}

		// Activate Texture before binding
		glActiveTexture(GL_TEXTURE0 + i);
		// Bind Texture
		glBindTexture(GL_TEXTURE_2D, GetTextureSet()[i].ID);
		// Set Unitform
		shader.SetUniform(( name + texturenum).c_str(), i);
	}

	if (m_Material.GetIsForwardLit())
	{
		// Set LightSpace Matrix
		shader.SetUniform("lightSpaceMatrix", static_cast<ShadowBuffer*>(Chroma::Render::GetShadowBuffer())->GetLightSpaceMatrix());
		// Set PBR Lighting Texture Uniforms
		UpdatePBRLightingTextureUniforms(shader);
		// Shadows
		glActiveTexture(GL_TEXTURE0 + GetNumTextures() + 4);
		shader.SetUniform("shadowmap", GetNumTextures() + 4);
		glBindTexture(GL_TEXTURE_2D_ARRAY, static_cast<ShadowBuffer*>(Chroma::Render::GetShadowBuffer())->GetTexture());
	}
	if (m_Material.GetUsesSceneNoise())
	{
		// BRDF LUT
		glActiveTexture(GL_TEXTURE0 + GetNumTextures() + 4);
		shader.SetUniform("noise", GetNumTextures() + 4);
		glBindTexture(GL_TEXTURE_2D, Chroma::Scene::GetSceneNoiseTex().ID);
	}

	glActiveTexture(GL_TEXTURE0);

}

void StaticMesh::UpdatePBRLightingTextureUniforms(Shader& shader)
{
	// Irradiance
	glActiveTexture(GL_TEXTURE0 + GetNumTextures() + 1);
	shader.SetUniform("irradianceMap", GetNumTextures() + 1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Chroma::Scene::GetIBL()->GetIrradianceMapID());
	// Prefilter Map
	glActiveTexture(GL_TEXTURE0 + GetNumTextures() + 2);
	shader.SetUniform("prefilterMap", GetNumTextures() + 2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Chroma::Scene::GetIBL()->GetPrefilterMapID());
	// BRDF LUT
	glActiveTexture(GL_TEXTURE0 + GetNumTextures() + 3);
	shader.SetUniform("brdfLUT", GetNumTextures() + 3);
	glBindTexture(GL_TEXTURE_2D, Chroma::Scene::GetIBL()->GetBRDFLUTID());
}

void StaticMesh::UpdateTransformUniforms(Shader& shader, Camera& renderCam)
{
	shader.SetUniform("model", GetWorldTransform());
	shader.SetUniform("view", renderCam.GetViewMatrix());
	shader.SetUniform("projection", renderCam.GetProjectionMatrix());
}

void StaticMesh::UpdateMaterialUniforms(Shader& shader)
{
	shader.SetUniform("UseAlbedoMap", false);
	shader.SetUniform("UseNormalMap", false);
	shader.SetUniform("UseMetRoughAOMap", false);
	m_Material.GetUniformArray().SetUniforms(shader.ShaderID);

	if (m_Material.GetUsesGameTime())
	{
		shader.SetUniform("gameTime", (float)GAMETIME);
	}
}

void StaticMesh::Draw(Shader& shader)
{
	//shader.Use();
	shader.Use();
	// draw mesh
	BindDrawVAO();
}

void StaticMesh::Draw(Camera& RenderCamera)
{
	GetShader().Use();
	UpdateUniforms(GetShader(), RenderCamera);
	BindDrawVAO();
}

void StaticMesh::Draw(Shader& shader, Camera& RenderCamera)
{
	shader.Use();
	UpdateUniforms(shader, RenderCamera);
	BindDrawVAO();
}

void StaticMesh::DrawUpdateMaterials(Shader& shader)
{
	shader.Use();
	UpdateMaterialUniforms(shader);
	UpdateTextureUniforms(shader);
	BindDrawVAO();
}

void StaticMesh::DrawUpdateTransforms(Camera& renderCam)
{
	m_Material.Use();
	UpdateTransformUniforms(GetShader(), renderCam);
	BindDrawVAO();
}

void StaticMesh::BindDrawVAO()
{
	if (m_MeshData.isRenderBuffersInitialized)
	{
		glBindVertexArray(m_MeshData.VAO);
		glDrawElements(GL_TRIANGLES, m_MeshData.indices.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); // reset to default
	}
	else if (m_MeshData.isRenderBuffersInitialized == false && m_MeshData.isLoaded)
		SetupMesh();
}

void StaticMesh::Init()
{
	m_Type = Chroma::Type::Component::kStaticMeshComponent;
	CMPNT_INITIALIZED
}

void StaticMesh::Destroy()
{
	// Material 
	m_Material.Destroy();
	// verts
	m_MeshData.verts.clear();
	// indices
	m_MeshData.indices.clear();
	// vao
	glDeleteVertexArrays(1, &m_MeshData.VAO);
	// buffers
	glDeleteBuffers(1, &m_MeshData.VBO);
	glDeleteBuffers(1, &m_MeshData.EBO);

	CMPNT_DESTROYED
}

void StaticMesh::Serialize(ISerializer*& serializer)
{
	CMPNT_SERIALIZE_BEGIN

	// Properties
	// Transform
	serializer->AddProperty("m_Translation", &m_Translation);
	serializer->AddProperty("m_Rotation", &m_Rotation);
	serializer->AddProperty("m_Scale", &m_Scale);

	// File Properties
	serializer->AddProperty("m_SourcePath", &m_SourcePath);

	// Material 
	SerializeMaterial(serializer);
}


void StaticMesh::CleanUp()
{
	// verts
	m_MeshData.verts.clear();
	// textures
	m_MeshData.textures.clear();
	CHROMA_INFO("Static Mesh Component : {0} Cleaned Up", m_UID.m_Data );
}


glm::vec3 StaticMesh::GetCentroid()
{
	return GetParentEntity()->GetTranslation() +  m_Centroid;
}

void StaticMesh::SetMat4(std::string name, glm::mat4 value)
{
	GetShader().Use();
	GetShader().SetUniform(name, value);
}

void StaticMesh::SetInt(std::string name, int value)
{
	GetShader().Use();
	GetShader().SetUniform(name, value);
}

void StaticMesh::SetFloat(std::string name, float value)
{
	GetShader().Use();
	GetShader().SetUniform(name, value);
}

StaticMesh::StaticMesh(std::vector<ChromaVertex> vertices_val, std::vector<unsigned int> indices_val, std::vector<Texture> textures_val)
{
	m_MeshData.verts = vertices_val;
	m_MeshData.indices = indices_val;
	m_Material.SetTextureSet(textures_val);

	SetupMesh();
}

StaticMesh::StaticMesh(MeshData const& newMeshData)
{
	m_MeshData = newMeshData;
	m_Material.SetTextureSet(newMeshData.textures);

	SetupMesh();
}

void StaticMesh::LoadFromFile(const std::string& sourcePath)
{
	Chroma::ResourceManager::LoadModel(sourcePath, &m_MeshData);
}



static void testFunc(std::string arg)
{
	CHROMA_INFO("Test {0}", arg);
}

StaticMesh::StaticMesh(const std::string& sourcePath)
{
	LoadFromFile(sourcePath);

}

StaticMesh::StaticMesh()
{
}

StaticMesh::~StaticMesh()
{
	Destroy();
}

