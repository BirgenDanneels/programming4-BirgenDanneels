#include "SceneManager.h"
#include "Scene.h"

void dae::SceneManager::Update(float deltaTime)
{
	for(auto& scene : m_scenes)
	{
		scene->Update(deltaTime);
	}
}

void dae::SceneManager::FixedUpdate(float fixedDeltaTime)
{
	for (const auto& scene : m_scenes)
	{
		scene->FixedUpdate(fixedDeltaTime);
	}
}

void dae::SceneManager::Render() const
{
	for (const auto& scene : m_scenes)
	{
		scene->Render();
	}
}

void dae::SceneManager::RenderUI() const
{
	for (const auto& scene : m_scenes)
	{
		scene->RenderUI();
	}
}

void dae::SceneManager::DispatchGameEvents()
{
	for (const auto& scene : m_scenes)
	{
		scene->DispatchGameEvents();
	}
}

void dae::SceneManager::DestroyAllScenes()
{
	m_scenes.clear();
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_scenes.emplace_back(new Scene());
	return *m_scenes.back();
}
