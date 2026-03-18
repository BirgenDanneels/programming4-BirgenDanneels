#pragma once
#include <memory>
#include <string>
#include <vector>
#include "GameObject.h"
#include "Events/GameEventQueue.h"

namespace dae
{
	class Scene final
	{
	public:
		void Add(std::unique_ptr<GameObject> object);
		GameObject* CreateGameObject();
		std::unique_ptr<GameObject> Remove(const GameObject& object);
		void RemoveAll();

		void Update(float deltaTime);
		void FixedUpdate(float fixedDeltaTime);
		void Render() const;
		void RenderUI() const;
		void DispatchGameEvents();

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		GameEventQueue& GetGameEventQueue() { return m_gameEventQueue; }

	private:
		friend class SceneManager;
		explicit Scene() = default;

		std::vector < std::unique_ptr<GameObject>> m_objects{};
		GameEventQueue m_gameEventQueue{};
	};

}
