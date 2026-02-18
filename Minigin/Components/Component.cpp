#include "Component.h"
#include "GameObject.h"

dae::Component::Component(dae::GameObject& refOwner, const std::string& componentName)
	:m_ptrOwnerObject{ &refOwner }, m_componentName{ componentName }
{
}

dae::GameObject* dae::Component::GetOwner() const
{
	return m_ptrOwnerObject;
}

std::string dae::Component::GetComponentName() const
{
	return m_componentName;
}