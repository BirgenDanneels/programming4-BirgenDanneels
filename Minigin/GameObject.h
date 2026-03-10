#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Components/Component.h"

namespace dae
{
	class TransformComponent;
	class Scene;

	class GameObject final
	{
	public:
		void Update(float deltaTime);
		void FixedUpdate(float fixedDeltaTime);
		void Render() const;
		void RenderUI() const;

		GameObject();
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		//POSITION FUNCTIONS
		const glm::vec3& GetLocalPosition();
		const glm::vec3& GetWorldPosition();

		void SetLocalPosition(const glm::vec3& localPos);
		void SetLocalPosition(int x, int y);
		void SetWorldPosition(float x, float y);
		void SetWorldPosition(const glm::vec3& worldPos);

		//MARK FOR DELETE
		void Delete() { m_markedForDelete = true; };
		bool IsMarkedForDelete() const { return m_markedForDelete; }

		//COMPONENT FUNCTIONS
		template<std::derived_from<Component> TComponent>
		TComponent* AddComponent()
		{
			auto component = std::make_unique<TComponent>(*this);
			TComponent* ptr = static_cast<TComponent*>(component.get());
			m_components.push_back(std::move(component));
			return static_cast<TComponent*>(ptr);
		}

		template<std::derived_from<Component> TComponent>
		TComponent* GetComponent() const
		{
			for (const auto& comp : m_components)
			{
				if(auto ptr = dynamic_cast<TComponent*>(comp.get()))
					return ptr;
			}

			return nullptr;
		}

		template<std::derived_from<Component> TComponent>
		bool RemoveComponent()
		{
			for (size_t i = 0; i < m_components.size(); ++i)
			{
				if (dynamic_cast<TComponent*>(m_components[i].get()) != nullptr)
				{
					std::swap(m_components[i], m_components.back());
					m_components.pop_back();
					return true;
				}
			}
			return false;
		}

		template<std::derived_from<Component> TComponent>
		bool HasComponent() const
		{
			return GetComponent<TComponent>() != nullptr;
		}

		//HIERARCHY FUNCTIONS
		void SetParent(GameObject* ptrParent, bool keepWorldPos = true);
		const std::vector<std::unique_ptr<dae::GameObject>>& GetChildren() const;
		GameObject* GetParent() const { return m_ptrParent; }

		void SetScene(Scene* scene);
		Scene* GetScene() const;
		
	private:

		//COMPONENT MEMBER VARIABLES
		std::vector<std::unique_ptr<Component>> m_components;

		//POSITION MEMBER VARIABLES
		std::unique_ptr<TransformComponent> m_transformComponent;
		glm::vec3 m_worldPosition;
		bool m_isPosDirty{ true };

		//HIERARCHY MEMBER VARIABLES
		GameObject* m_ptrParent;
		std::vector<std::unique_ptr<GameObject>> m_vectChildren;

		Scene* m_pScene = nullptr;

		//HIERARCHY FUNCTIONS
		std::unique_ptr<dae::GameObject> RemoveChild(GameObject* ptrChild);
		void AddChild(std::unique_ptr<dae::GameObject> child);
		bool CheckIfParentIsValid(GameObject* ptrParent) const;
		bool HasChild(GameObject* ptrChild) const;
		bool IsDescendant(GameObject* ptrChild) const; //Checks deeper in the hierarchy

		//POSITION FUNCTIONS
		void SetPositionDirty();

		bool m_markedForDelete = false;
	};
}