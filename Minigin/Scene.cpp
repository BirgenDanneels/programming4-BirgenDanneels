#include <algorithm>
#include "Scene.h"

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");

	object->SetScene(this);

	if (m_isIteratingObjects)
	{
		m_pendingObjects.emplace_back(std::move(object));
	}
	else
	{
		m_objects.emplace_back(std::move(object));
	}
}

void dae::Scene::AddPendingHierarchyChange(GameObject* child, GameObject* newParent, bool keepWorldPos)
{
	m_pendingHierarchyChanges.push_back({ child, newParent, keepWorldPos });
}

void dae::Scene::FlushPendingObjects()
{
	if (m_pendingObjects.empty())
		return;

	m_objects.reserve(m_objects.size() + m_pendingObjects.size());
	for (auto& object : m_pendingObjects)
	{
		m_objects.emplace_back(std::move(object));
	}
	m_pendingObjects.clear();
}

std::unique_ptr<GameObject> Scene::RemoveFromContainer(
	std::vector<std::unique_ptr<GameObject>>& container,
	const GameObject& object)
{
	auto it = std::find_if(container.begin(), container.end(),
		[&object](const std::unique_ptr<GameObject>& child) { return child.get() == &object; });

	if (it == container.end())
		return nullptr;

	std::unique_ptr<GameObject> result = std::move(*it);
	container.erase(it);

	return result;
}

void dae::Scene::DestroyMarkedObjects()
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[](const auto& ptr) { return ptr->IsMarkedForDelete(); }),
		m_objects.end()
	);

	for (const auto& object : m_objects)
	{
		object->DestroyMarkedChildren();
	}
}

GameObject* dae::Scene::CreateGameObject()
{
	auto temp = std::make_unique<dae::GameObject>();
	GameObject* ptr = temp.get();

	Add(std::move(temp));

	return ptr;
}

std::unique_ptr<dae::GameObject> Scene::Remove(const GameObject& object)
{
	if (auto removed = RemoveFromContainer(m_objects, object))
		return removed;

	return RemoveFromContainer(m_pendingObjects, object);
}

void Scene::RemoveAll()
{
	m_objects.clear();
	m_pendingObjects.clear();
}

void Scene::Update(float deltaTime)
{
	m_isIteratingObjects = true;
	for(auto& object : m_objects)
	{
		object->Update(deltaTime);
	}
	m_isIteratingObjects = false;

	ApplyPendingHierarchyChanges();
	FlushPendingObjects();

	DispatchGameEvents();

	DestroyMarkedObjects();
}

void dae::Scene::FixedUpdate(float fixedDeltaTime)
{
	m_isIteratingObjects = true;
	for (const auto& object : m_objects)
	{
		object->FixedUpdate(fixedDeltaTime);
	}
	m_isIteratingObjects = false;

	ApplyPendingHierarchyChanges();
	FlushPendingObjects();

	DispatchGameEvents();

	DestroyMarkedObjects();
}

void Scene::Render() const
{
	m_isIteratingObjects = true;
	for (const auto& object : m_objects)
	{
		object->Render();
	}
	m_isIteratingObjects = false;
}

void dae::Scene::RenderUI() const
{
	m_isIteratingObjects = true;
	for (const auto& object : m_objects)
	{
		object->RenderUI();
	}
	m_isIteratingObjects = false;
}

void dae::Scene::DispatchGameEvents()
{
	m_gameEventQueue.Dispatch();
}

void dae::Scene::ApplyPendingHierarchyChanges()
{
	for (const auto& change : m_pendingHierarchyChanges)
	{
		change.child->SetParent(change.newParent, change.keepWorldPos);
	}
	m_pendingHierarchyChanges.clear();
}

