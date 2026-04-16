#include "GameObject.h"
#include <string>
#include "ResourceManager.h"
#include "SceneManager.h"

#include "Components/ComponentsInclude.h"


dae::GameObject::GameObject()
	:m_transformComponent{ std::make_unique<dae::TransformComponent>(*this)}, m_ptrParent{ nullptr }
{
}

dae::GameObject::~GameObject() = default;

const glm::vec3& dae::GameObject::GetLocalPosition()
{
	return m_transformComponent->GetPosition();
}

const glm::vec3& dae::GameObject::GetWorldPosition()
{
	if (m_isPosDirty)
	{
		if (m_ptrParent == nullptr)
			m_worldPosition = GetLocalPosition();
		else
			m_worldPosition = m_ptrParent->GetWorldPosition() + GetLocalPosition();

		m_isPosDirty = false;
	}

	return m_worldPosition;
}

void dae::GameObject::SetLocalPosition(const glm::vec3& localPos)
{
	m_transformComponent->SetPosition(localPos);

	SetPositionDirty();
}

void dae::GameObject::SetLocalPosition(int x, int y)
{
	SetLocalPosition(glm::vec3(x, y, 0));
}

void dae::GameObject::SetWorldPosition(float x, float y)
{
	SetWorldPosition(glm::vec3(x, y, 0));
}

void dae::GameObject::SetWorldPosition(const glm::vec3& worldPos)
{
	if (m_ptrParent != nullptr)
		m_transformComponent->SetPosition(worldPos - m_ptrParent->GetWorldPosition());
	else
		m_transformComponent->SetPosition(worldPos);

	SetPositionDirty();
}

void dae::GameObject::DestroyMarkedChildren()
{
	m_vectChildren.erase(
		std::remove_if(
			m_vectChildren.begin(),
			m_vectChildren.end(),
			[](const auto& ptr) { return ptr->IsMarkedForDelete(); }),
		m_vectChildren.end()
	);

	for (const auto& child : m_vectChildren)
	{
		child->DestroyMarkedChildren();
	}
}

void dae::GameObject::SetParent(GameObject* ptrParent, bool keepWorldPos)
{
	if (m_pScene->GetIsIteratingObjects())
	{
		m_pScene->AddPendingHierarchyChange(this, ptrParent, keepWorldPos);
		return;
	}

	if (!CheckIfParentIsValid(ptrParent))
		return;

	//Adjust the local position to keep the world position the same
	if (ptrParent == nullptr)
	{
		if(keepWorldPos)
			SetLocalPosition(GetWorldPosition());
		else
			SetPositionDirty();
	}
	else
	{
		if (keepWorldPos)
			SetLocalPosition(GetWorldPosition() - ptrParent->GetWorldPosition());
		else
			SetPositionDirty();
	}

	//Remove this GameObject from the current parent (if it has one)
	std::unique_ptr<dae::GameObject> selfPtr = nullptr;

	if(m_ptrParent != nullptr)
		selfPtr = m_ptrParent->RemoveChild(this);
	else 
		selfPtr = m_pScene->Remove(*this);

	assert(selfPtr && "GameObject not found in parent or scene when setting parent.");

	m_ptrParent = ptrParent;
	if (m_ptrParent != nullptr)
		m_ptrParent->AddChild(std::move(selfPtr));
	else
	{
		//If the new parent is null then we need to add this GameObject to the scene root
		m_pScene->Add(std::move(selfPtr));
	}
}

const std::vector<std::unique_ptr<dae::GameObject>>& dae::GameObject::GetChildren() const
{
	return m_vectChildren;
}

std::unique_ptr<dae::GameObject> dae::GameObject::RemoveChild(GameObject* ptrChild)
{
	auto it = std::find_if(m_vectChildren.begin(), m_vectChildren.end(),
		[ptrChild](const std::unique_ptr<GameObject>& child) { return child.get() == ptrChild; });
	
	if (it == m_vectChildren.end())
		return nullptr;

	std::unique_ptr<GameObject> result = std::move(*it);
	m_vectChildren.erase(it);

	return result;
}

void dae::GameObject::AddChild(std::unique_ptr<dae::GameObject> child)
{
	child->SetScene(m_pScene);

	m_vectChildren.push_back(std::move(child));
}

bool dae::GameObject::CheckIfParentIsValid(GameObject* ptrParent) const
{
	//Check if the new parent is the same as the current parent or if its the same as this GameObject
	if (ptrParent == m_ptrParent || ptrParent == this)
		return false;

	//Check if the new parent is a (indirect) child of this GameObject
	if (IsDescendant(ptrParent))
		return false;

	return true;
}

bool dae::GameObject::HasChild(GameObject* ptrChild) const
{
	if (ptrChild == nullptr) return false;

	//use find_if to check if the vector already contains the given pointer
	auto childIt = std::find_if(m_vectChildren.begin(), m_vectChildren.end(),
		[ptrChild](const std::unique_ptr<GameObject>& ptr) { return ptr.get() == ptrChild; });

	//if the iterator equals the the end of the child vector then the parent isnt in the child vector
	if (childIt != m_vectChildren.end())
		return true;

	return false;
}

bool dae::GameObject::IsDescendant(GameObject* ptrChild) const
{
	if (ptrChild == nullptr) return false;
	for (auto& child : m_vectChildren)
	{
		if (child.get() == ptrChild) return true;
		if (child->IsDescendant(ptrChild)) return true;
	}
	return false;
}

void dae::GameObject::SetScene(Scene* scene)
{
	m_pScene = scene;

	for (auto& child : m_vectChildren)
	{
		child->SetScene(scene);
	}
}

dae::Scene* dae::GameObject::GetScene() const
{
	return m_pScene;
}

void dae::GameObject::SetPositionDirty()
{
	m_isPosDirty = true;

	std::for_each(m_vectChildren.begin(), m_vectChildren.end(), [](std::unique_ptr<dae::GameObject>& ptr) {ptr->SetPositionDirty(); });
}

void dae::GameObject::StartComponents()
{
	if(m_componentsToStart.empty())
		return;

	for (auto& comp : m_componentsToStart)
	{
		comp->Start();
	}
	m_componentsToStart.clear();
}

void dae::GameObject::Update(float deltaTime)
{
	StartComponents();

	for (const auto& comp : m_components)
	{
		comp->Update(deltaTime);
	}

	for (const auto& child : m_vectChildren)
	{
		child->Update(deltaTime);
	}
}

void dae::GameObject::FixedUpdate(float fixedDeltaTime)
{
	for (const auto& comp : m_components)
	{
		comp->FixedUpdate(fixedDeltaTime);
	}

	for (const auto& child : m_vectChildren)
	{
		child->FixedUpdate(fixedDeltaTime);
	}
}

void dae::GameObject::Render() const
{
	for (const auto& comp : m_components)
	{
		if (comp)
		{
			comp->Render();
		}
	}

	for (const auto& child : m_vectChildren)
	{
		child->Render();
	}
}

void dae::GameObject::RenderUI() const
{
	for (const auto& comp : m_components)
	{
		if (comp)
		{
			comp->RenderUI();
		}
	}
	for (const auto& child : m_vectChildren)
	{
		child->RenderUI();
	}
}
