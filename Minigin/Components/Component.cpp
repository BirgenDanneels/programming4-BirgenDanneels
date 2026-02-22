#include "Component.h"
#include "GameObject.h"

dae::Component::Component(dae::GameObject& refOwner)
	:m_ptrOwnerObject{ &refOwner }
{
}

dae::GameObject* dae::Component::GetOwner() const
{
	return m_ptrOwnerObject;
}