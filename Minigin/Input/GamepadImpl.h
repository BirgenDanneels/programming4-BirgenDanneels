#pragma once

namespace dae
{
	enum class InputState;

	// Abstract interface for gamepad implementations
	class GamepadImpl
	{
	public:
		virtual ~GamepadImpl() = default;

		virtual void Update() = 0;
		virtual bool GetKeyState(int key, InputState state) const = 0;
		virtual float GetAxisValue(int key) const = 0;
		virtual bool IsConnected() const = 0;

		virtual void SetDeadzone(float deadzone) { m_deadzone = deadzone; }
		virtual void SetAnalogToButtonThreshold(float threshold) { m_analogToButtonThreshold = threshold; }

	protected:
		
		float m_deadzone{ 0.1f };
		float m_analogToButtonThreshold{ 0.5f };
	};
}