#include "FPSCounter.h"
#include <sstream>
#include <iomanip>
#include "Minigin/Components/TextComponent.h"
#include "Minigin/GameObject.h"

FPSCounter::FPSCounter(dae::GameObject& refOwner)
	:Component(refOwner)
{
	m_ptrTextComponent = GetOwner()->GetComponent<dae::TextComponent>();

	if (!m_ptrTextComponent)
		m_ptrTextComponent = GetOwner()->AddComponent<dae::TextComponent>();
}

void FPSCounter::Update(float deltaTime)
{
	if (deltaTime <= EPSILON)
		return;

	//Accumulate frames and time so it jitters less
	m_FrameCount++;
	m_Elapsed += deltaTime;

	if (m_Elapsed >= m_UpdateInterval)
	{
		const float instantFPS = static_cast<float>(m_FrameCount) / m_Elapsed;

		std::ostringstream ss;
		ss << std::fixed << std::setprecision(1) << instantFPS;
		const std::string fpsText = ss.str();

		if (m_ptrTextComponent)
			m_ptrTextComponent->SetText(fpsText);

		m_FrameCount = 0;
		m_Elapsed = 0.0f;
	}
}

void FPSCounter::Initialize()
{
}
