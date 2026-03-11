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
}

bool dae::InputManager::ProcessInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if (e.type == SDL_EVENT_QUIT) {
			return false;
		}
		if (e.type == SDL_EVENT_KEY_DOWN) {
			
		}
		if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
			
		}
		// etc...

		//process event for IMGUI
		ImGui_ImplSDL3_ProcessEvent(&e);

	}

	m_keyboard.Update();

	for(const auto& gamepad : m_gamepads)
	{
		gamepad->Update();
	}

	return true;
}

void dae::InputManager::BindMapToKeyboard(std::unique_ptr<InputMap> inputMap)
{
	m_keyboard.SetInputMap(std::move(inputMap));
}

void dae::InputManager::BindMapToGamepad(int controllerIdx, std::unique_ptr<InputMap> inputMap)
{
	assert(controllerIdx >= 0 && controllerIdx < static_cast<int>(m_gamepads.size()));

	m_gamepads[controllerIdx]->SetInputMap(std::move(inputMap));
}
