#include "AnimConstraintComponent.h"

void AnimConstraintComponent::Init()
{
	CHROMA_INFO("IK Component {0} Initialized!", m_UID.data);
}

void AnimConstraintComponent::Update()
{
	CHROMA_INFO("Updating IK Component : {0}", m_UID.data);
}

void AnimConstraintComponent::Destroy()
{
	CHROMA_INFO("IK Component {0} Destroyed!", m_UID.data);
}

void AnimConstraintComponent::Serialize(ISerializer*& serializer)
{
	
}

std::string AnimConstraintComponent::GetTypeString() const
{
	return "Anim Constraint";
}