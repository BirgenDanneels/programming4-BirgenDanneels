#pragma once
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include "Scene.h"
#include "Singleton.h"
#include <thread>
#include <mutex>
#include "Loading/ComponentFactory.h"
#include "Events/Subject.h"

namespace dae
{
	struct OnSceneLoadedEvent final
	{
		Scene* loadedScene{};
	};

	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene();

		void LoadScene(const std::string& sceneFile);
		void SetActiveScene(Scene* scene);

		GameObject* CreatePersistentObject();

		void Update(float deltaTime);
		void FixedUpdate(float fixedDeltaTime);
		void Render() const;
		void RenderUI() const;
		void DestroyAllScenes();

		Subject<OnSceneLoadedEvent>& GetSceneLoadedSubject() { return m_sceneLoadedSubject; }

		void SetComponentFactory(ComponentFactory& factory) { m_componentFactory = &factory; }
		void SetDataPath(const std::string& path) { m_dataPath = path; }

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		void LoadSceneAsync(const std::string& sceneFile);
		void FlushPendingObjects();

		Scene& AddScene(std::unique_ptr<Scene> scene);
		void RemoveScene(Scene* scene);

		std::vector<std::unique_ptr<GameObject>> m_persistentObjects{};
		std::vector<std::unique_ptr<GameObject>> m_pendingPersistentObjects{};

		std::unordered_map<Scene*, std::unique_ptr<Scene>> m_loadedScenes{};
		std::unique_ptr<Scene> m_activeScene{};

		std::mutex m_loadedScenesMutex{};
		std::jthread m_loadingThread;

		Subject<OnSceneLoadedEvent> m_sceneLoadedSubject{};

		ComponentFactory* m_componentFactory{};
		std::string m_dataPath{};
	};
}
