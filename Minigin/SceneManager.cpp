#include "SceneManager.h"
#include "Loading/SceneLoader.h"

void dae::SceneManager::Update(float deltaTime)
{
	FlushPendingObjects();

	if (m_activeScene)
		m_activeScene->Update(deltaTime);

	for(auto& object : m_persistentObjects)
	{
		object->Update(deltaTime);
	}
}

void dae::SceneManager::FixedUpdate(float fixedDeltaTime)
{
	if (m_activeScene)
		m_activeScene->FixedUpdate(fixedDeltaTime);

	for (auto& object : m_persistentObjects)
	{
		object->FixedUpdate(fixedDeltaTime);
	}
}

void dae::SceneManager::Render() const
{
	if (m_activeScene)
		m_activeScene->Render();

	for (const auto& object : m_persistentObjects)
	{
		object->Render();
	}
}

void dae::SceneManager::RenderUI() const
{
	if (m_activeScene)
		m_activeScene->RenderUI();

	for (const auto& object : m_persistentObjects)
	{
		object->RenderUI();
	}
}

void dae::SceneManager::DestroyAllScenes()
{
	std::scoped_lock lock(m_loadedScenesMutex);

	m_pendingPersistentObjects.clear();
	m_persistentObjects.clear();

	m_activeScene.reset();
	m_loadedScenes.clear();
}

dae::Scene& dae::SceneManager::CreateScene()
{
	return AddScene(std::unique_ptr<Scene>(new Scene()));
}

void dae::SceneManager::LoadScene(const std::string& sceneFile)
{
	m_loadingThread = std::jthread(&SceneManager::LoadSceneAsync, this, sceneFile);
}

void dae::SceneManager::LoadSceneAsync(const std::string& sceneFile)	
{
	auto newScene = std::unique_ptr<Scene>(new Scene());

	SceneLoader loader{ *m_componentFactory };
	loader.LoadFromFile(m_dataPath + sceneFile, *newScene);

	Scene* rawPtr = &AddScene(std::move(newScene));

	m_sceneLoadedSubject.NotifyObservers({ rawPtr });
}

dae::GameObject* dae::SceneManager::CreatePersistentObject()
{
	auto temp = std::make_unique<dae::GameObject>();
	GameObject* ptr = temp.get();

	m_pendingPersistentObjects.emplace_back(std::move(temp));
	return ptr;
}

void dae::SceneManager::FlushPendingObjects()
{
	if (m_pendingPersistentObjects.empty())
		return;

	m_persistentObjects.reserve(m_persistentObjects.size() + m_pendingPersistentObjects.size());
	for (auto& object : m_pendingPersistentObjects)
	{
		m_persistentObjects.emplace_back(std::move(object));
	}
	m_pendingPersistentObjects.clear();
}

void dae::SceneManager::SetActiveScene(Scene* scene)
{
	std::scoped_lock lock(m_loadedScenesMutex);

	if (!m_loadedScenes.contains(scene) && scene != nullptr)
		return;

	// Move current active scene back to map
	if (m_activeScene)
	{
		Scene* oldPtr = m_activeScene.get();
		m_loadedScenes[oldPtr] = std::move(m_activeScene);
	}

	// Move new scene out of map and into active
	if(scene)
	{
		m_activeScene = std::move(m_loadedScenes.at(scene));
		m_loadedScenes.erase(scene);
	}
	else 
		m_activeScene.reset();
}

dae::Scene& dae::SceneManager::AddScene(std::unique_ptr<Scene> scene)
{
	Scene* rawPtr = scene.get();
	std::scoped_lock lock(m_loadedScenesMutex);
	m_loadedScenes[rawPtr] = std::move(scene);
	return *rawPtr;
}

void dae::SceneManager::RemoveScene(Scene* scene)
{
	std::scoped_lock lock(m_loadedScenesMutex);

	if (m_activeScene.get() == scene)
	{
		m_activeScene.reset();
	}

	m_loadedScenes.erase(scene);
}
