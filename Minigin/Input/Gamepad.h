#pragma once
#include "InputDevice.h"
#include "GamepadInput.h"
#include <memory>

namespace dae
{
	class GamepadImpl;

	class Gamepad : public InputDevice
	{
	public:
		Gamepad(int controllerIdx);
		~Gamepad() = default;

		void Update() override;

		bool GetKeyState(int key, InputState state) const override;
		float GetAxisValue(int key) const override;

		void SetDeadzone(float deadzone);
		void SetAnalogToButtonThreshold(float threshold);

	private:
		std::unique_ptr<GamepadImpl> m_pImpl;
	};
}