#include "StaticMesh.h"


void StaticMesh::calcBBox()
{
	// collecting all bboxes within mesh components of entity and returning overall
	std::vector<std::pair<glm::vec3, glm::vec3>> bboxes;
	// once collected, calculate new min and max bbox
	glm::vec3 newMinBBox(99999.00, 99999.00, 99999.00);
	glm::vec3 newMaxBBox(0.0, 0.0, 0.0);
	for (ChromaVertex& vert : vertices)
	{
		newMinBBox = glm::min(newMinBBox, vert.getPosition());
		newMaxBBox = glm::max(newMaxBBox, vert.getPosition());
	}
	// re-establishing min and max bboxes
	m_bbox_min = newMinBBox;
	m_bbox_max = newMaxBBox;
}

void StaticMesh::calcCentroid()
{
	m_centroid = (m_bbox_min - m_bbox_max) * glm::vec3(0.5);
}

void StaticMesh::setupMesh()
{
	// Generate buffers
	// Vertex Array Object Buffer
	glGenVertexArrays(1, &VAO);
	// Vertex Buffer and Element Buffer
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind buffers
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(ChromaVertex), &vertices[0], GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int),
		&indices[0], GL_STATIC_DRAW);

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

	glBindVertexArray(0);
}

void StaticMesh::updateUniforms(const Shader* updateShader, std::vector<Light*> Lights, Camera& RenderCam, glm::mat4& TransformMatrix)
{
	updateTransformUniforms(updateShader, RenderCam, TransformMatrix);
	updateMaterialUniforms(updateShader);
	updateTextureUniforms(updateShader);
	updateLightingUniforms(updateShader, Lights, RenderCam);
}

void StaticMesh::updateLightingUniforms(const Shader* shader, std::vector<Light*> Lights, Camera& renderCam)
{
	int pointlights{ 0 };
	int dirlights{ 0 };
	int spotlights{ 0 };
	for (int i = 0; i < Lights.size(); i++)
	{
		std::string lightIndex;
		// set uniforms
		switch (Lights[i]->type) {
		case Light::POINT:
			pointlights++;
			lightIndex = "pointLights[" + std::to_string(pointlights - 1) + "]";
			break;
		case Light::SUNLIGHT :
		case Light::DIRECTIONAL :
			dirlights++;
			lightIndex = "dirLights[" + std::to_string(dirlights - 1) + "]";
			break;
		case Light::SPOT:
			spotlights++;
			lightIndex = "spotLights[" + std::to_string(spotlights - 1) + "]";
			break;
		default:
			break;
		}
		//// lights directional
		shader->setVec3(lightIndex + ".direction", Lights[i]->getDirection());
		shader->setVec3(lightIndex + ".position", Lights[i]->getPosition());
		shader->setVec3(lightIndex + ".diffuse", Lights[i]->getDiffuse());
		shader->setFloat(lightIndex + ".intensity", Lights[i]->getIntensity());
		//// lights spotlight
		shader->setFloat(lightIndex + ".spotSize", Lights[i]->getSpotSize());
		shader->setFloat(lightIndex + ".penumbraSize", Lights[i]->getPenumbraSize());
		//// lights point light falloff
		shader->setFloat(lightIndex + ".constant", Lights[i]->constant);
		shader->setFloat(lightIndex + ".linear", Lights[i]->linear);
		shader->setFloat(lightIndex + ".quadratic", Lights[i]->quadratic);
		shader->setFloat(lightIndex + ".radius", Lights[i]->getRadius());
		//// lights view pos
		shader->setVec3("viewPos", renderCam.getPosition());
	}
}

void StaticMesh::updateTextureUniforms(const Shader* shader)
{
	// updating shader's texture uniforms
	unsigned int diffuseNr{ 1 };
	unsigned int shadowmapNr{ 1 };
	unsigned int normalNr{ 1 };
	unsigned int roughnessNr{ 1 };
	unsigned int metalnessNr{ 1 };
	unsigned int metroughaoNr{ 1 };
	unsigned int aoNr{ 1 };
	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);// activate proper texture unit before binding
		// building the uniform name
		std::string name;
		std::string texturenum;
		if (textures[i].type == Texture::ALBEDO)
		{
			name = "material.texture_albedo";
			texturenum = std::to_string(diffuseNr++);
			// set use texture albedo
			shader->setBool("UseAlbedoMap", true);
		}
		if (textures[i].type == Texture::NORMAL)
		{
			name = "material.texture_normal";
			texturenum = std::to_string(normalNr++);
			// set use texture normals
			shader->setBool("UseNormalMap", true);
		}
		if (textures[i].type == Texture::METROUGHAO)
		{
			name = "material.texture_MetRoughAO";
			texturenum = std::to_string(metroughaoNr++);
			// set use texture metroughao
			shader->setBool("UseMetRoughAOMap", true);
		}
		if (textures[i].type == Texture::METALNESS)
		{
			name = "material.texture_metalness";
			texturenum = std::to_string(metalnessNr++);
		}
		if (textures[i].type == Texture::ROUGHNESS)
		{
			name = "material.texture_roughness";
			texturenum = std::to_string(roughnessNr++);
		}
		if (textures[i].type == Texture::AO)
		{
			name = "material.texture_ao";
			texturenum = std::to_string(aoNr++);
		}
		if (textures[i].type == Texture::SHADOWMAP)
		{
			name = "shadowmaps.shadowmap";
			texturenum = std::to_string(shadowmapNr++);
		}

		// setting uniform and binding texture
		shader->setInt(( name + texturenum).c_str(), i);

		glBindTexture(GL_TEXTURE_2D, textures[i].ID);
		// activate texture
	}
	glActiveTexture(GL_TEXTURE0);
}

void StaticMesh::updateTransformUniforms(const Shader* shader, Camera& renderCam, glm::mat4& modelMatrix)
{

	glm::mat4 finalTransform = getTransformationMatrix() * modelMatrix;
	shader->setMat4("model", finalTransform);
	shader->setMat4("view", renderCam.viewMat);
	shader->setMat4("projection", renderCam.projectionMat);
}

void StaticMesh::updateMaterialUniforms(const Shader* shader)
{
	shader->setFloat("roughness", 0.4f);
	shader->setVec3("color", glm::vec3(1, 0, 0));
	shader->setFloat("metalness", 0.0f);
	shader->setBool("UseAlbedoMap", false);
	shader->setBool("UseNormalMap", false);
	shader->setBool("UseMetRoughAOMap", false);
	//shader->setVec3("ambient", ambient);
}

void StaticMesh::Draw(Shader &shader)
{
	shader.use();
	// draw mesh
	BindDrawVAO();
}

void StaticMesh::Draw(Camera& RenderCamera, std::vector<Light*> Lights, glm::mat4& transformMatrix)
{
	mShader->use();
	updateUniforms(mShader, Lights, RenderCamera, transformMatrix);
	BindDrawVAO();
}

void StaticMesh::Draw(Shader& shader, Camera& RenderCamera, std::vector<Light*> Lights, glm::mat4& transformMatrix)
{
	shader.use();
	updateUniforms(&shader, Lights, RenderCamera, transformMatrix);
	BindDrawVAO();
}

void StaticMesh::DrawUpdateMaterials(Shader& shader)
{
	shader.use();
	updateMaterialUniforms(&shader);
	updateTextureUniforms(&shader);
	BindDrawVAO();
}

void StaticMesh::DrawUpdateTransforms(Camera& renderCam, glm::mat4& modelMatrix)
{
	mShader->use();
	updateTransformUniforms(mShader, renderCam, modelMatrix);
	BindDrawVAO();
}

void StaticMesh::BindDrawVAO()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0); // reset to default
}


void StaticMesh::bindShader(Shader* newShader)
{
	mShader = newShader;
}

void StaticMesh::bindTextures(std::vector<Texture> textures_val)
{
	for (unsigned int i = 0; textures_val.size(); i++)
	{
		bool skip{ false };
		for (unsigned int j = 0; j < textures.size(); j++)
		{
			if (std::strcmp(textures[j].path.data(), textures_val[j].path.data()) == 0)
			{
				skip = true;
				break;
			}
		}
		if (!skip)
			textures.push_back(textures_val[i]);
	}
}

void StaticMesh::bindTexture(Texture texture_val)
{
	bool skip{false};
	for (unsigned int i = 0; i < textures.size(); i++)
	{
		skip = false;
		if (std::strcmp(textures[i].path.data(), texture_val.path.data()) == 0)
		{
			skip = true;
			break;
		}
	}
	if (!skip)
	{
		textures.push_back(texture_val);
	}
}


std::pair<glm::vec3, glm::vec3> StaticMesh::getBBox()
{
	calcBBox();
	return std::make_pair(m_bbox_min, m_bbox_max);
}

glm::vec3 StaticMesh::getCentroid()
{
	calcCentroid();
	return m_centroid;
}

void StaticMesh::setMat4(std::string name, glm::mat4 value)
{
	mShader->use();
	mShader->setMat4(name, value);
}

void StaticMesh::setInt(std::string name, int value)
{
	mShader->use();
	mShader->setInt(name, value);
}

void StaticMesh::setFloat(std::string name, float value)
{
	mShader->use();
	mShader->setFloat(name, value);
}

StaticMesh::StaticMesh(std::vector<ChromaVertex> vertices_val, std::vector<unsigned int> indices_val, std::vector<Texture> textures_val)
{
	isRenderable = true;
	vertices = vertices_val;
	indices = indices_val;
	textures = textures_val;
	

	setupMesh();
}

StaticMesh::StaticMesh()
{
	isRenderable = true;
}

StaticMesh::~StaticMesh()
{
	delete mShader;
}