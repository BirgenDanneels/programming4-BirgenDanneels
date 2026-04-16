#pragma once
#include <memory>
#include <string>
#include <vector>
#include "GameObject.h"
#include "Events/GameEventQueue.h"

namespace dae
{
	struct ParentChange final
	{
		GameObject* child{};
		GameObject* newParent{};
		bool keepWorldPos{ true };
	};


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
		void ApplyPendingHierarchyChanges();

		~Scene() = default;
		Scene(const Scene& other) = delete;
		Scene(Scene&& other) = delete;
		Scene& operator=(const Scene& other) = delete;
		Scene& operator=(Scene&& other) = delete;

		GameEventQueue& GetGameEventQueue() { return m_gameEventQueue; }
		bool GetIsIteratingObjects() const { return m_isIteratingObjects; }

		void AddPendingHierarchyChange(GameObject* child, GameObject* newParent, bool keepWorldPos = true);

	private:
		friend class SceneManager;
		explicit Scene() = default;

		void FlushPendingObjects();
		std::unique_ptr<GameObject> RemoveFromContainer(std::vector<std::unique_ptr<GameObject>>& container, const GameObject& object);
		void DestroyMarkedObjects();

		std::vector<std::unique_ptr<GameObject>> m_objects{};
		std::vector<std::unique_ptr<GameObject>> m_pendingObjects{};
		std::vector<ParentChange> m_pendingHierarchyChanges{};
		
		bool m_isIteratingObjects{ false };

		GameEventQueue m_gameEventQueue{};
	};

}
