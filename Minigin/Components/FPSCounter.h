#pragma once
#include "Component.h"

namespace dae
{

	class TextComponent;

	class FPSCounter final : public Component
	{
	public:

		FPSCounter() = delete;
		FPSCounter(dae::GameObject& refOwner);

		FPSCounter(const FPSCounter& other) = delete;
		FPSCounter(FPSCounter&& other) = delete;
		FPSCounter& operator=(const FPSCounter& other) = delete;
		FPSCounter& operator=(FPSCounter&& other) = delete;

		~FPSCounter() = default;

		void Update(float deltaTime) override;
		void FixedUpdate(float fixedDeltaTime) override;
		void Render() const override {};

		void Initialize();

	private:

		TextComponent* m_ptrTextComponent;

		float m_Elapsed{ 0.0f };
		int m_FrameCount{ 0 };
		const float m_UpdateInterval{ 0.5f }; // seconds between updates
		static constexpr float EPSILON{ 1e-6f };
	};

}

