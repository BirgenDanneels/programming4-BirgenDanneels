#pragma once
#include "Singleton.h"
#include "Input/Keyboard.h"

namespace dae
{

	class InputManager final : public Singleton<InputManager>
	{
	public:
		bool ProcessInput();

		void BindMapToKeyboard(std::unique_ptr<InputMap> inputMap);

	private:

		Keyboard m_keyboard{};
	};

}
