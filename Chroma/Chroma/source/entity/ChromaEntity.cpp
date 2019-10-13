#include "ChromaEntity.h"


void ChromaEntity::Draw(Shader& shader)
{
	for (ChromaComponent* component : RenderableComponents)
		component->Draw(shader);
}

void ChromaEntity::Draw(Shader& shader, Camera& RenderCamera, std::vector<Light*>& Lights)
{
	for (ChromaComponent* component : RenderableComponents)
		component->Draw(shader, RenderCamera, Lights, transformMatrix);
}

void ChromaEntity::Draw(Camera& RenderCamera, std::vector<Light*>& Lights)
{
	for (ChromaComponent* component : RenderableComponents)
		component->Draw(RenderCamera, Lights, transformMatrix);
}

ChromaEntity::ChromaEntity()
{
}


ChromaEntity::~ChromaEntity()
{
}

// ADDING/REMOVING COMPONENTS
void ChromaEntity::addComponent(ChromaComponent* newComponent)
{
	// TODO: Consider shared_ptr to prevent memory duplication
	Components.push_back(newComponent);
	if (newComponent->isRenderable)
		RenderableComponents.push_back(newComponent);
}

void ChromaEntity::removeComponent(ChromaComponent* removeMe)
{
	// all components 
	int componentIndex = findIndexInVector(Components, removeMe);
	if (componentIndex > 0)
		Components.erase(Components.begin() + componentIndex);	
	// renderable components
	// TODO: Consider using shared_ptr to better manage memory
	componentIndex = findIndexInVector(RenderableComponents, removeMe);
	if (componentIndex)
		RenderableComponents.erase(RenderableComponents.begin() + componentIndex);
}


// TRANSFORMATIONS
void ChromaEntity::scale(glm::vec3 scalefactor)
{
	transformMatrix = glm::scale(transformMatrix, scalefactor);
}

void ChromaEntity::translate(glm::vec3 translatefactor)
{
	transformMatrix = glm::translate(transformMatrix, translatefactor);
}

void ChromaEntity::rotate(float degrees, glm::vec3 rotationaxis)
{
	transformMatrix = glm::rotate(transformMatrix, glm::radians(degrees), rotationaxis);
}

void ChromaEntity::setScale(glm::vec3 newscale)
{
	transformMatrix = glm::scale(identityMatrix, newscale);
}

void ChromaEntity::setPosition(glm::vec3 newposition)
{
	transformMatrix = glm::translate(identityMatrix, newposition);
}


// VECTOR FUNCTIONS
int findIndexInVector(const std::vector<ChromaComponent*>& componentsVector, ChromaComponent*& component)
{
	// Find given element in vector
	auto it = std::find(componentsVector.begin(), componentsVector.end(), component);

	if (it != componentsVector.end())
		return distance(componentsVector.begin(), it);
	else
	{
		std::cout << "ChromaComponent: " << component->getName() << " Type: <"  
			<< component->getType() << "> " << "not found in Chroma Entity" << std::endl;
		return -1;
	}
}
