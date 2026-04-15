#pragma once
#include <string>

namespace dae
{
	class GameObject;

	class Component
	{
	public:

		Component() = delete;
		Component(GameObject& refOwner);

		Component(const Component& other) = delete;
		Component(Component&& other) = delete;
		Component& operator=(const Component& other) = delete;
		Component& operator=(Component&& other) = delete;

		virtual ~Component() = default;

		virtual void Start() {};
		virtual void FixedUpdate(float fixedDeltaTime) = 0;
		virtual void Update(float deltaTime) = 0;
		virtual void Render() const = 0;
		virtual void RenderUI() {};

	protected:

		GameObject* GetOwner() const;

	private:

		GameObject* m_ptrOwnerObject;

	};
}