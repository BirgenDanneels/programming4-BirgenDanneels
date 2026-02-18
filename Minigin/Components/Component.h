#pragma once
#include "GameObject.h"

namespace dae
{

	class Component
	{
	public:

		Component() = delete;
		Component(GameObject& refOwner, const std::string& componentName);

		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;

		virtual ~Component() = default;

		virtual void FixedUpdate(float fixedDeltaTime) = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Render() const = 0;

		std::string GetComponentName() const;

	protected:

		GameObject* GetOwner() const;

	private:

		GameObject* m_ptrOwnerObject;
		std::string m_componentName{};

	};
}