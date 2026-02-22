#include "FPSCounter.h"
#include <sstream>
#include <iomanip>
#include "TextComponent.h"
#include "GameObject.h"

dae::FPSCounter::FPSCounter(dae::GameObject& refOwner)
	:Component(refOwner)
{
	m_ptrTextComponent = GetOwner()->GetComponent<TextComponent>();

	if (!m_ptrTextComponent)
		m_ptrTextComponent = GetOwner()->AddComponent<TextComponent>();
}

void dae::FPSCounter::Update(float deltaTime)
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

void dae::FPSCounter::FixedUpdate(float fixedDeltaTime)
{
	(void)fixedDeltaTime;
}

void dae::FPSCounter::Initialize()
{

}
