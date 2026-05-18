#pragma once
#include "Singleton.h"
#include "Input/Keyboard.h"
#include "Input/Gamepad.h"
#include <vector>
#include <memory>
#include <string>

namespace dae
{

	class InputManager final : public Singleton<InputManager>
	{
	public:
		InputManager();

		bool ProcessInput();

		// Currently only supports binding one map per device, but this can be easily changed in the future if needed

		void BindMapToKeyboard(std::string mapName, std::unique_ptr<InputMap> inputMap);
		void UnbindMapFromKeyboard(std::string mapName);

		void BindMapToGamepad(int controllerIdx, std::string mapName, std::unique_ptr<InputMap> inputMap);
		void UnbindMapFromGamepad(int controllerIdx, std::string mapName);

		Keyboard* GetKeyboard() const { return m_keyboard.get(); }
		Gamepad* GetGamepad(int controllerIdx) const;

		InputDevice* GetDeviceByName(const std::string& name) const;

		void LoadDeviceMapsFromFile(const std::string& filePath);

	private:

		std::unique_ptr<Keyboard> m_keyboard{};
		std::vector<std::unique_ptr<Gamepad>> m_gamepads;
	};

}
