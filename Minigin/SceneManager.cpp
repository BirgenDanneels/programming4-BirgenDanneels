#include "SceneManager.h"
#include "Loading/SceneLoader.h"

void dae::SceneManager::Update(float deltaTime)
{
	if (m_sceneReady.load())
	{
		std::lock_guard lock(m_pendingMutex);

		m_scenes.push_back(std::move(m_pendingScene));
		m_sceneReady = false;

		m_sceneLoadedSubject.NotifyObservers();
	}

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

void dae::SceneManager::DestroyAllScenes()
{
	m_scenes.clear();
	std::lock_guard lock(m_pendingMutex);
	m_pendingScene.reset();
}

dae::Scene& dae::SceneManager::CreateScene()
{
	m_scenes.emplace_back(new Scene());
	return *m_scenes.back();
}

void dae::SceneManager::LoadScene(const std::string& sceneFile)
{
	m_loadingThread = std::jthread(&SceneManager::LoadSceneAsync, this, sceneFile);
}

void dae::SceneManager::LoadSceneAsync(const std::string& sceneFile)	
{
	auto newScene = std::unique_ptr<Scene>(new Scene());

	SceneLoader loader{ *m_componentFactory };
	loader.LoadFromFile(sceneFile, *newScene);

	{
		std::lock_guard lock(m_pendingMutex);
		m_pendingScene = std::move(newScene);
	}

	m_sceneReady = true;
}
