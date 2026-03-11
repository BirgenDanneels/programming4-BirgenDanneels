#include "Singleton.h"
#include "Input/Keyboard.h"
#include "Input/Gamepad.h"
#include <vector>
#include <memory>

namespace dae
{

	class InputManager final : public Singleton<InputManager>
	{
	public:
		InputManager();

		bool ProcessInput();

		void BindMapToKeyboard(std::unique_ptr<InputMap> inputMap);
		void BindMapToGamepad(int controllerIdx, std::unique_ptr<InputMap> inputMap);

	private:

		Keyboard m_keyboard{};
		std::vector<std::unique_ptr<Gamepad>> m_gamepads;
	};

}
