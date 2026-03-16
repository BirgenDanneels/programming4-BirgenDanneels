#include <SDL3/SDL.h>
#include <backends/imgui_impl_sdl3.h>
#include "InputManager.h"
#include "Input/InputMap.h"

dae::InputManager::InputManager()
{
	m_gamepads.reserve(4);

	for (int i = 0; i < 4; ++i)
	{
		m_gamepads.emplace_back(std::make_unique<Gamepad>(i));  // Creates Gamepad with controller index
	}

	m_keyboard = std::make_unique<Keyboard>();
}

bool dae::InputManager::ProcessInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) {
			return false;
		}
		
		/*
		if (e.type == SDL_EVENT_GAMEPAD_ADDED) {
			SDL_Log("Gamepad Connected! ID: %u", e.gdevice.which);
			SDL_Log("IsGamepad: %d", SDL_IsGamepad(e.gdevice.which));

			SDL_Gamepad* pad = SDL_OpenGamepad(e.gdevice.which);

			if (!pad)
			{
				SDL_Log("Failed to open gamepad");
				break;
			}

			SDL_JoystickID instance = SDL_GetGamepadID(pad);

			SDL_Log("Opened gamepad:");
			SDL_Log("Name: %s", SDL_GetGamepadName(pad));
			SDL_Log("Instance ID: %d", instance);
			SDL_Log("Player index: %d", SDL_GetGamepadPlayerIndex(pad));
			SDL_Log("Vendor: %04x", SDL_GetGamepadVendor(pad));
			SDL_Log("Product: %04x", SDL_GetGamepadProduct(pad));
			SDL_Log("Type: %s", SDL_GetGamepadStringForType(SDL_GetGamepadType(pad)));

		}
		else if (e.type == SDL_EVENT_JOYSTICK_ADDED) {
			SDL_Log("Joystick Connected! ID: %u (If a gamepad isn't assigned, SDL is missing its web mapping)", e.jdevice.which);
		}*/
		
		if (e.type == SDL_EVENT_KEY_DOWN) {
			
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			
		}
		// etc...

		//process event for IMGUI
		ImGui_ImplSDL3_ProcessEvent(&e);

	}

	m_keyboard->Update();

	for(const auto& gamepad : m_gamepads)
	{
		gamepad->Update();
	}

	return true;
}

void dae::InputManager::BindMapToKeyboard(std::unique_ptr<InputMap> inputMap)
{
	m_keyboard->SetInputMap(std::move(inputMap));
}

void dae::InputManager::UnbindMapFromKeyboard()
{
	m_keyboard->SetInputMap(nullptr);
}

void dae::InputManager::BindMapToGamepad(int controllerIdx, std::unique_ptr<InputMap> inputMap)
{
	assert(controllerIdx >= 0 && controllerIdx < static_cast<int>(m_gamepads.size()));

	m_gamepads[controllerIdx]->SetInputMap(std::move(inputMap));
}

void dae::InputManager::UnbindMapFromGamepad(int controllerIdx)
{
	m_gamepads[controllerIdx]->SetInputMap(nullptr);
}

dae::Gamepad* dae::InputManager::GetGamepad(int controllerIdx) const
{
	return m_gamepads[controllerIdx].get();
}
