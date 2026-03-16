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

		// Currently only supports binding one map per device, but this can be easily changed in the future if needed

		void BindMapToKeyboard(std::unique_ptr<InputMap> inputMap);
		void UnbindMapFromKeyboard();

		void BindMapToGamepad(int controllerIdx, std::unique_ptr<InputMap> inputMap);
		void UnbindMapFromGamepad(int controllerIdx);

		Keyboard* GetKeyboard() const { return m_keyboard.get(); }
		Gamepad* GetGamepad(int controllerIdx) const;

	private:

		std::unique_ptr<Keyboard> m_keyboard{};
		std::vector<std::unique_ptr<Gamepad>> m_gamepads;
	};

}
