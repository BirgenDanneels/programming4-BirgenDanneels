#pragma once
#include <vector>
#include <string>
#include <memory>
#include "Scene.h"
#include "Singleton.h"
#include <thread>
#include <mutex>
#include "Loading/ComponentFactory.h"
#include "Events/Subject.h"

namespace dae
{
	class SceneManager final : public Singleton<SceneManager>
	{
	public:
		Scene& CreateScene();
		void LoadScene(const std::string& sceneFile);

		void Update(float deltaTime);
		void FixedUpdate(float fixedDeltaTime);
		void Render() const;
		void RenderUI() const;
		void DestroyAllScenes();

		Subject<>& GetSceneLoadedSubject() { return m_sceneLoadedSubject; }

		void SetComponentFactory(ComponentFactory& factory) { m_componentFactory = &factory; }

	private:
		friend class Singleton<SceneManager>;
		SceneManager() = default;
		void LoadSceneAsync(const std::string& sceneFile);

		std::vector<std::unique_ptr<Scene>> m_scenes{};

		std::unique_ptr<Scene>m_pendingScene{};
		std::mutex m_pendingMutex{};
		std::jthread m_loadingThread;

		Subject<> m_sceneLoadedSubject{};

		std::atomic<bool> m_sceneReady{ false };

		ComponentFactory* m_componentFactory{};
	};
}
