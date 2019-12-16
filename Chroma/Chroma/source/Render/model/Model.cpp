#include "Model.h"
#include <texture/stb_image.h>

void Model::Draw(Shader &shader)
{
	for (ChromaMeshComponent*& mesh : m_meshes)
		mesh->Draw(shader);
}

void Model::Draw(Camera& RenderCamera, std::vector<Light*> Lights, glm::mat4& transformMatrix)
{
	for (ChromaMeshComponent*& mesh : m_meshes)
			mesh->Draw(RenderCamera, Lights, transformMatrix);
}

void Model::Draw(Shader& shader, Camera& RenderCamera, std::vector<Light*> Lights, glm::mat4& transformMatrix)
{
	for (ChromaMeshComponent*& mesh : m_meshes)
		mesh->Draw(shader, RenderCamera, Lights, transformMatrix);
}

void Model::DrawUpdateMaterials(Shader& shader)
{
	for (ChromaMeshComponent*& mesh : m_meshes)
		mesh->DrawUpdateMaterials(shader);
}

void Model::DrawUpdateTransforms(Camera& renderCam, glm::mat4& modelMatrix)
{
	for (ChromaMeshComponent* mesh : m_meshes)
		mesh->DrawUpdateTransforms(renderCam, modelMatrix);
}

void Model::SetShader(Shader* const& newShader)
{
	for (ChromaMeshComponent* mesh : m_meshes)
		mesh->SetShader(newShader);
}


std::pair<glm::vec3, glm::vec3> Model::GetBBox()
{
	CalculateBBox();
	return std::make_pair(m_BBoxMin, m_BBoxMax);
}

glm::vec3 Model::GetCentroid()
{
	CalculateCentroid();
	return m_Centroid;
}

void Model::AddTexture(Texture texture_val)
{
	for (ChromaMeshComponent* mesh : m_meshes)
		mesh->AddTexture(texture_val);
}

void Model::SetMat4(std::string name, glm::mat4 value)
{
	for (ChromaMeshComponent* mesh : m_meshes)
		mesh->SetMat4(name, value);
}

void Model::SetInt(std::string name, int value)
{
	for (ChromaMeshComponent* mesh : m_meshes)
		mesh->SetInt(name, value);
}

void Model::SetFloat(std::string name, float value)
{
	for (ChromaMeshComponent* mesh : m_meshes)
		mesh->SetFloat(name, value);
}

void Model::SetJointUniforms(Shader& skinnedShader)
{
	for (ChromaMeshComponent* mesh : m_meshes)
	{
		((SkinnedMesh*)mesh)->SetJointUniforms(skinnedShader);
	}	
}


Model::~Model()
{
	for (ChromaMeshComponent* mesh : m_meshes)
		delete mesh;
}

void Model::CalculateBBox()
{
	// collecting all bboxes within mesh components of entity and returning overall
	std::vector<std::pair<glm::vec3, glm::vec3>> bboxes;
	for (ChromaMeshComponent*& mesh : m_meshes)
		bboxes.push_back(mesh->GetBBox());

	// once collected, calculate new min and max bbox
	glm::vec3 newMinBBox(99999.00, 99999.00, 99999.00);
	glm::vec3 newMaxBBox(0.0, 0.0, 0.0);
	for (std::pair<glm::vec3, glm::vec3> MinMaxBBoxes : bboxes)
	{
		newMinBBox = glm::min(newMinBBox, MinMaxBBoxes.first);
		newMaxBBox = glm::max(newMaxBBox, MinMaxBBoxes.second);
	}
	// re-establishing min and max bboxes
	m_BBoxMin = newMinBBox;
	m_BBoxMax = newMaxBBox;
}

void Model::CalculateCentroid()
{
	m_Centroid = m_BBoxMin + ((m_BBoxMin - m_BBoxMax) * glm::vec3(0.5));
}

void Model::LoadModel(std::string path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_LimitBoneWeights);

	if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
			return;
		}
	m_directory = path.substr(0, path.find_last_of('/'));
	ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	// process node's meshes (if it has any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(ProcessMesh(mesh, scene));
	}
	// check if node has children, if so recursively search for meshes
	for (unsigned int i = 0; i< node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

ChromaMeshComponent* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<unsigned int> m_indices;
	std::vector<Texture> m_textures;

	// check if mesh is skinned
	m_IsSkinned = mesh->HasBones();

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// process vertex
		// positions
		glm::vec3 position;
		position.x = mesh->mVertices[i].x;
		position.y = mesh->mVertices[i].y;
		position.z = mesh->mVertices[i].z;

		// normals
		glm::vec3 normal;
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;

		// tangents
		glm::vec3 tangent;
		tangent.x = mesh->mTangents[i].x;
		tangent.y = mesh->mTangents[i].y;
		tangent.z = mesh->mTangents[i].z;

		// bitangents 
		glm::vec3 bitangent;
		bitangent.x = mesh->mBitangents[i].x;
		bitangent.y = mesh->mBitangents[i].y;
		bitangent.z = mesh->mBitangents[i].z;

		// texture coords
		glm::vec2 UV1;
		if (mesh->mTextureCoords[0])
		{
			UV1.x = mesh->mTextureCoords[0][i].x;
			UV1.y = mesh->mTextureCoords[0][i].y;
		}
		else
			UV1 = glm::vec2(0.0f, 0.0f);

		// add new vertex
		if (m_IsSkinned)
		{
			ChromaSkinnedVertex vertex;
			vertex.SetPosition(position);
			vertex.SetNormal(normal);
			vertex.SetTangent(tangent);
			vertex.SetBitangent(bitangent);
			vertex.SetTexCoords(UV1);
			m_skinnedVertices.push_back(vertex);
		}
		else
		{
			ChromaVertex vertex;
			vertex.SetPosition(position);
			vertex.SetNormal(normal);
			vertex.SetTangent(tangent);
			vertex.SetBitangent(bitangent);
			vertex.SetTexCoords(UV1);
			m_vertices.push_back(vertex);
		}
	}

	// process indices (faces)
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			m_indices.push_back(face.mIndices[j]);
	}

	// process material, fetching textures
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		// diffuse textures
		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material,
			aiTextureType_DIFFUSE, Texture::ALBEDO);
		m_textures.insert(m_textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		// specular textures
		std::vector<Texture> specularMaps = LoadMaterialTextures(material,
			aiTextureType_SPECULAR, Texture::METALNESS);
		m_textures.insert(m_textures.end(), specularMaps.begin(), specularMaps.end());
		// normal textures
		std::vector<Texture> normalMaps = LoadMaterialTextures(material,
			aiTextureType_HEIGHT, Texture::NORMAL);
		m_textures.insert(m_textures.end(), normalMaps.begin(), normalMaps.end());
	}

	// build Mesh
	if (m_IsSkinned)
	{
		// Process Skeleton
		Skeleton skeleton;
		skeleton.SetGlobalTransform(AIToGLM(scene->mRootNode->mTransformation));

		for (int i = 0; i < mesh->mNumBones; i++)
		{
			// fetch assimp bone, copy data to chroma joint
			aiBone* bone = mesh->mBones[i];
			Joint newJoint;
			// name
			newJoint.SetName(bone->mName.C_Str());
			// offset matrix - joint matrix, relative to its parent
			newJoint.SetLocalBindTransform(AIToGLM(bone->mOffsetMatrix));
			// ID
			newJoint.SetID(i);
			
			// store joint IDs and Weights to skelton and verts
			for (int j = 0; j < bone->mNumWeights; j++)
			{
				// update joint
				aiVertexWeight vertexWeight = bone->mWeights[j];
				std::pair<unsigned int, float> skinningData;
				skinningData.first = i;
				skinningData.second = vertexWeight.mWeight;
				SetSkinningData(m_skinnedVertices[vertexWeight.mVertexId], skinningData);
			}
			// add new joint
			skeleton.AddJoint(newJoint);
		}
		return new SkinnedMesh(m_skinnedVertices, m_indices, m_textures, skeleton, AIToGLM(scene->mRootNode->mTransformation));
	}
	else
		return new StaticMesh(m_vertices, m_indices, m_textures);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::TYPE typeName)
{
	std::vector<Texture> m_textures;
	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip{ false };
		for (unsigned int j = 0; j < m_textures.size(); j++)
		{
			if (std::strcmp(m_textures[j].path.data(), str.C_Str()) == 0)
			{
				m_textures.push_back(m_textures[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;
			texture.loadFromFile(str.C_Str(), m_directory);
			texture.type = typeName;
			m_textures.push_back(texture);
			m_textures.push_back(texture);
		}
	}
	return m_textures;
}

void Model::SetSkinningData(ChromaSkinnedVertex& vert, std::pair<int, float> const& jointIDWeight)
{
	// check for existing weights
	for (int i = 0; i < MAX_VERT_INFLUENCES; i++)
	{
		// if no weights been set, break loop and set a weight
		// this is likely when jointindex = 0
		if (glm::length(vert.m_jointWeights) == 0.0)
		{
			break;
		}
		// set weight if it's already existing but not if the weight is already 1.0
		if (jointIDWeight.first == vert.m_jointIDs[i] && glm::length(vert.m_jointWeights) != 1.0)
		{
			std::cout << "setting joint ID : " << jointIDWeight.first << " to weight : " << jointIDWeight.second;
			vert.m_jointWeights[i] = jointIDWeight.second;
			return;
		}
	}

	for (int i = 0; i < MAX_VERT_INFLUENCES; i++)
	{
		// if ID is 0 it moset likely has not yet been set
		if (vert.m_jointIDs[i] == 0)
		{
			vert.m_jointIDs[i] = jointIDWeight.first;
			vert.m_jointWeights[i] = jointIDWeight.second;
			return;
		}
	}
}
