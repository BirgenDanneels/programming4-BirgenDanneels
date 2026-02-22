#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Components/Component.h"

namespace dae
{
	class TransformComponent;

	class GameObject final
	{
	public:
		void Update(float deltaTime);
		void FixedUpdate(float fixedDeltaTime);
		void Render() const;

		GameObject();
		~GameObject();
		GameObject(const GameObject& other) = delete;
		GameObject(GameObject&& other) = delete;
		GameObject& operator=(const GameObject& other) = delete;
		GameObject& operator=(GameObject&& other) = delete;

		//POSITION FUNCTIONS
		const glm::vec3& GetLocalPosition();

		void SetLocalPosition(const glm::vec3& localPos);
		
		void SetLocalPosition(int x, int y);

		//MARK FOR DELETE
		void Delete() { m_markedForDelete = true; };
		bool IsMarkedForDelete() const { return m_markedForDelete; }

		//COMPONENT FUNCTIONS
		template<std::derived_from<Component> TComponent>
		TComponent* AddComponent()
		{
			auto component = std::make_shared<TComponent>(*this);
			m_components.push_back(component);
			return static_cast<TComponent*>(component.get());
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
		
	private:
		std::unique_ptr<TransformComponent> m_transformComponent;

		std::vector<std::shared_ptr<Component>> m_components;

		bool m_markedForDelete = false;
	};
}