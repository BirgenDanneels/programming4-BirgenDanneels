#include "InputDevice.h"
#include <Windows.h>
#include <Xinput.h>

namespace dae
{
	class Gamepad : public InputDevice
	{
	public:
		Gamepad(int controllerIdx);
		~Gamepad() = default;

		void Update() override;

		bool GetKeyState(int key, InputState state) const override;
		float GetAxisValue(int key) const override;

		// Unified input enum
		enum class Input
		{
			DPadUp = XINPUT_GAMEPAD_DPAD_UP,
			DPadDown = XINPUT_GAMEPAD_DPAD_DOWN,
			DPadLeft = XINPUT_GAMEPAD_DPAD_LEFT,
			DPadRight = XINPUT_GAMEPAD_DPAD_RIGHT,

			Start = XINPUT_GAMEPAD_START,
			Back = XINPUT_GAMEPAD_BACK,

			LeftThumb = XINPUT_GAMEPAD_LEFT_THUMB,
			RightThumb = XINPUT_GAMEPAD_RIGHT_THUMB,
			LeftShoulder = XINPUT_GAMEPAD_LEFT_SHOULDER,
			RightShoulder = XINPUT_GAMEPAD_RIGHT_SHOULDER,

			A = XINPUT_GAMEPAD_A,
			B = XINPUT_GAMEPAD_B,
			X = XINPUT_GAMEPAD_X,
			Y = XINPUT_GAMEPAD_Y,

			LeftStickUp,
			LeftStickDown,
			LeftStickLeft,
			LeftStickRight,

			RightStickUp,
			RightStickDown,
			RightStickLeft,
			RightStickRight,

			LeftTrigger,
			RightTrigger,
		};

	private:

		int m_controllerIdx;
		float m_deadzone{ 0.1f }; //Should be improved since it is now per axis and not a circular deadzone
		float m_analogToButtonThreshold{ 0.5f }; //Threshold for treating analog movement as button presses when used as an action instead of axis (ofc just dont use these ase non analog movement)

		XINPUT_STATE m_currentState{};
		XINPUT_STATE m_previousState{};

		bool m_isConnected{ false };

		bool IsAnalogInput(int key) const;
		bool GetAnalogButtonState(int key, bool usePrevious) const;
	};
}