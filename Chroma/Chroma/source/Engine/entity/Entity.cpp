#include "Entity.h"
#include <component/MeshComponent.h>
#include <component/PhysicsComponent.h>
#include <component/AnimationComponent.h>

std::vector<ChromaVertex> Entity::GetVertices()
{
	// collecting all vertices within mesh components of entity
	std::vector<ChromaVertex> verts;
	for (IComponent* meshComponent : m_MeshComponents)
	{
		std::vector<ChromaVertex> m_vertices = ((MeshComponent*)meshComponent)->GetVertices();
		for (ChromaVertex vert : m_vertices)
			verts.push_back(vert);
	}

	return verts;
}

std::pair<glm::vec3, glm::vec3> Entity::GetBBox()
{
	CalculateBBox();
	return std::make_pair(m_BBoxMin, m_BBoxMax);
}

glm::vec3 Entity::GetCentroid()
{
	CalculateCentroid();
	return m_Centroid;
}

void Entity::Draw(Shader& shader)
{
	for (IComponent* component : m_RenderableComponents)
		((MeshComponent*)component)->Draw(shader);
}

void Entity::Draw(Shader& shader, Camera& RenderCamera, std::vector<Light*> Lights)
{
	for (IComponent* component : m_RenderableComponents)
		((MeshComponent*)component)->Draw(shader, RenderCamera, Lights, m_transformMatrix);
}

void Entity::Draw(Camera& RenderCamera, std::vector<Light*> Lights)
{
	for (IComponent* component : m_RenderableComponents)
		((MeshComponent*)component)->Draw(RenderCamera, Lights, m_transformMatrix);
}

// ADDING/REMOVING COMPONENTS
void Entity::addMeshComponent(MeshComponent*& newMeshComponent)
{
	// bind parent entity
	SetParentEntity(newMeshComponent);

	// add mesh component
	m_MeshComponents.push_back(newMeshComponent);

	// TODO: Consider shared_ptr to prevent memory duplication
	if (newMeshComponent->m_IsRenderable)
		m_RenderableComponents.push_back(newMeshComponent);
	if (newMeshComponent->m_IsLit)
		m_LitComponents.push_back(newMeshComponent);
	if (newMeshComponent->m_CastShadows)
		m_ShadowCastingComponents.push_back(newMeshComponent);
	if (newMeshComponent->m_IsTransparent)
		m_TransparentComponents.push_back(newMeshComponent);
	if (newMeshComponent->m_IsLit == false)
		m_UnLitComponents.push_back(newMeshComponent);
	if (newMeshComponent->m_IsTransparent || newMeshComponent->m_IsLit == false)
		m_TransparentComponents.push_back(newMeshComponent);
}

void Entity::addPhysicsComponent(PhysicsComponent*& newPhysicsComponent)
{
	// bind parent entity
	SetParentEntity(newPhysicsComponent);

	// add physics component
	m_PhysicsComponents.push_back(newPhysicsComponent);

	// build rigidBody
	newPhysicsComponent->BuildRigidBody();

	// add rigid body to physics world
	m_ParentScene->GetPhysics()->addBodyToWorld(newPhysicsComponent);
}

void Entity::addAnimationComponent(AnimationComponent*& newAnimationComponent)
{
	// bind parent entity
	SetParentEntity(newAnimationComponent);
}

void Entity::CalculateBBox()
{
	// collecting all bboxes within mesh components of entity and returning overall
	std::vector<std::pair<glm::vec3, glm::vec3>> bboxes;
	for (IComponent* meshComponent : m_MeshComponents)
		bboxes.push_back(((MeshComponent*)meshComponent)->GetBBox());
	// once collected, calculate new min and max bbox
	glm::vec3 newMinBBox(99999.00, 99999.00, 99999.00);
	glm::vec3 newMaxBBox(0.0, 0.0, 0.0);
	for (std::pair<glm::vec3, glm::vec3> MinMaxBBoxes : bboxes)
	{
		newMinBBox = glm::min(newMinBBox, MinMaxBBoxes.first);
		newMaxBBox = glm::max(newMaxBBox, MinMaxBBoxes.second);
	}
	// re-establishing min and max bboxes
	// scale by entity's current size
	glm::vec3 scale = getScale(m_transformMatrix);
	m_BBoxMin = newMinBBox * scale;
	m_BBoxMax = newMaxBBox * scale;
}

void Entity::CalculateCentroid()
{
	m_Centroid = (m_BBoxMin - m_BBoxMax) * glm::vec3(0.5);
}

void Entity::addEmptyComponent(IComponent*& newComponent)
{
	// bind parent entity
	SetParentEntity(newComponent);

	// TODO: Consider shared_ptr to prevent memory duplication
	m_Components.push_back(newComponent);
}

void Entity::removeEmptyComponent(IComponent*& removeMe)
{
	// all components 
	int componentIndex = findIndexInVector(m_Components, removeMe);
	if (componentIndex > 0)
		m_Components.erase(m_Components.begin() + componentIndex);	
	// renderable components
	// TODO: Consider using shared_ptr to better manage memory
	componentIndex = findIndexInVector(m_RenderableComponents, removeMe);
	if (componentIndex)
		m_RenderableComponents.erase(m_RenderableComponents.begin() + componentIndex);
}

void Entity::updatePhysicsComponentsTransforms()
{
	for (IComponent* physicsComponent : m_PhysicsComponents)
	{
		if (((PhysicsComponent*)physicsComponent)->getColliderState() == Kinematic) // check if physics object is kinematic
			((PhysicsComponent*)physicsComponent)->SetWorldTransform(m_transformMatrix);
	}
}


// TRANSFORMATIONS
void Entity::scale(glm::vec3 scalefactor)
{
	m_transformMatrix = glm::scale(m_transformMatrix, scalefactor);
	updatePhysicsComponentsTransforms();
}

void Entity::translate(glm::vec3 translatefactor)
{
	m_transformMatrix = glm::translate(m_transformMatrix, translatefactor);
	updatePhysicsComponentsTransforms();
}

void Entity::rotate(float degrees, glm::vec3 rotationaxis)
{
	m_transformMatrix = glm::rotate(m_transformMatrix, glm::radians(degrees), rotationaxis);
	updatePhysicsComponentsTransforms();
}

void Entity::setScale(glm::vec3 newscale)
{
	m_transformMatrix = glm::scale(m_transformMatrix, newscale);
	updatePhysicsComponentsTransforms();
}

void Entity::SetPosition(glm::vec3 newposition)
{
	m_transformMatrix[3] = glm::vec4(newposition, 1);
	updatePhysicsComponentsTransforms();
}

void Entity::setRotation(float degrees, glm::vec3 rotationaxis)
{
	glm::vec3 existingTranslation = GetPosition();
	m_transformMatrix = glm::translate(m_transformMatrix, existingTranslation);
	rotate(degrees, rotationaxis);
	updatePhysicsComponentsTransforms();
}


// VECTOR FUNCTIONS
int findIndexInVector(const std::vector<IComponent*>& componentsVector, IComponent*& component)
{
	// Find given element in vector
	auto it = std::find(componentsVector.begin(), componentsVector.end(), component);

	if (it != componentsVector.end())
		return distance(componentsVector.begin(), it);
	else
	{
		std::cout << "ChromaComponent: " << component->GetName() << "not found in Chroma Entity" << std::endl;
		return -1;
	}
}


Entity::Entity()
{
}


Entity::~Entity()
{
}