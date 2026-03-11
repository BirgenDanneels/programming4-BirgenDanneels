#pragma once
#include "GamepadImpl.h"
#include <SDL3/SDL.h>

namespace dae
{
	class SDL3GamepadImpl : public GamepadImpl
	{
	public:
		explicit SDL3GamepadImpl(int controllerIdx);
		~SDL3GamepadImpl() override;

		void Update() override;
		bool GetKeyState(int key, InputState state) const override;
		float GetAxisValue(int key) const override;
		bool IsConnected() const override { return m_pGamepad != nullptr; }

		void SetDeadzone(float deadzone) override { m_deadzone = deadzone; }
		void SetAnalogToButtonThreshold(float threshold) override { m_analogToButtonThreshold = threshold; }

	private:
		SDL_Gamepad* m_pGamepad{ nullptr };
		int m_controllerIdx;

		struct GamepadState
		{
			Uint8 buttons[static_cast<int>(SDL_GAMEPAD_BUTTON_COUNT)]{};
			float axes[static_cast<int>(SDL_GAMEPAD_AXIS_COUNT)]{};
		};

		GamepadState m_currentState{};
		GamepadState m_previousState{};

		bool IsAnalogInput(int key) const;
		bool GetAnalogButtonState(int key, bool usePrevious) const;
		float GetRawAxisValue(int key, const GamepadState& state) const;

		// Convert platform-agnostic key to SDL button/axis
		static SDL_GamepadButton ToSDLButton(int key);
	};
}