#include "IComponent.h"

IComponent::IComponent()
{
	m_UID = UID::GenerateNewUID();
	std::cout << "New Entity UID" << m_UID << std::endl;
}