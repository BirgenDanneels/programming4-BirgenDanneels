#pragma once
#include <string>
#include <vector>
#include "Minigin/Loading/LoadingStructs.h"

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

		GameObject* GetOwner() const;

		//Loading functions
		virtual std::vector<ParamDefinition> GetExpectedParams() const = 0;
		virtual void Load(const ParamMap& params) = 0;

	private:

		GameObject* m_ptrOwnerObject;

	};
}