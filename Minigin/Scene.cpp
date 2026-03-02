#include <algorithm>
#include "Scene.h"

using namespace dae;

void Scene::Add(std::unique_ptr<GameObject> object)
{
	assert(object != nullptr && "Cannot add a null GameObject to the scene.");

	object->SetScene(this);
	m_objects.emplace_back(std::move(object));
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
	auto it = std::find_if(m_objects.begin(), m_objects.end(),
		[&object](const std::unique_ptr<GameObject>& child) { return child.get() == &object; });

	if (it == m_objects.end())
		return nullptr;

	std::unique_ptr<GameObject> result = std::move(*it);
	m_objects.erase(it);

	return result;
}

void Scene::RemoveAll()
{
	m_objects.clear();
}

void Scene::Update(float deltaTime)
{
	for(auto& object : m_objects)
	{
		object->Update(deltaTime);
	}

	//Remove marked objects 
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[](const auto& ptr) { return ptr->IsMarkedForDelete(); }),
		m_objects.end()
	);
}

void dae::Scene::FixedUpdate(float fixedDeltaTime)
{
	for(const auto& object : m_objects)
	{
		object->FixedUpdate(fixedDeltaTime);
	}

	//Remove marked objects 
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[](const auto& ptr) { return ptr->IsMarkedForDelete(); }),
		m_objects.end()
	);
}

void Scene::Render() const
{
	for (const auto& object : m_objects)
	{
		object->Render();
	}
}

