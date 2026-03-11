#include "GamepadImpl.h"
#include <Windows.h>
#include <Xinput.h>

namespace dae
{
	class XInputGamepadImpl : public GamepadImpl
	{
	public:
		explicit XInputGamepadImpl(int controllerIdx);
		~XInputGamepadImpl() override = default;

		void Update() override;
		bool GetKeyState(int key, InputState state) const override;
		float GetAxisValue(int key) const override;
		bool IsConnected() const override { return m_isConnected; }

		void SetDeadzone(float deadzone) override { m_deadzone = deadzone; }
		void SetAnalogToButtonThreshold(float threshold) override { m_analogToButtonThreshold = threshold; }

	private:
		int m_controllerIdx;

		XINPUT_STATE m_currentState{};
		XINPUT_STATE m_previousState{};
		bool m_isConnected{ false };

		bool IsAnalogInput(int key) const;
		bool GetAnalogButtonState(int key, bool usePrevious) const;
		float GetRawAxisValue(int key, bool usePrevious) const;

		// Convert platform-agnostic key to XInput button
		static int ToXInputButton(int key);
	};
}