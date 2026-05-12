#pragma once
#include "Minigin/Components/Component.h"

namespace dae
{
	class TextComponent;
}

class FPSCounter final : public dae::Component
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
	void FixedUpdate(float) override {};
	void Render() const override {};

	void Initialize();
	//Loading functions
	virtual std::vector<dae::ParamDefinition> GetExpectedParams() const override { return {}; };
	virtual void Load(const dae::ParamMap&) override {};

private:

	dae::TextComponent* m_ptrTextComponent;

	float m_Elapsed{ 0.0f };
	int m_FrameCount{ 0 };
	const float m_UpdateInterval{ 0.5f }; // seconds between updates
	static constexpr float EPSILON{ 1e-6f };
};
