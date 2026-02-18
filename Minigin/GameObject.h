#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include "Components/Component.h"

namespace dae
{

	class Texture2D;
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

		//COMPONENT FUNCTIONS
		template<typename TComponent>
		TComponent* AddComponent()
		{
			auto component = std::shared_ptr<Component>(new TComponent{ *this });

			m_components[component->GetComponentName()] = component;

			return static_cast<TComponent*>(component.get());
		}

		template<typename TComponent>
		TComponent* GetComponent(const std::string& componentName) const
		{
			if (HasComponent(componentName))
			{
				return  static_cast<TComponent*>(m_components.find(componentName)->second.get());
			}
			return nullptr;
		}

		bool RemoveComponent(const std::string& componentName);
		bool HasComponent(const std::string& componentName) const;

	private:
		std::unique_ptr<TransformComponent> m_transformComponent;

		std::unordered_map<std::string, std::shared_ptr<Component>> m_components;
	};
}