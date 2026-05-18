#pragma once
#include <memory>
#include <unordered_map>
#include "Bindings/InputStates.h"
#include <nlohmann/json.hpp>
using Json = nlohmann::json;

namespace dae
{
	class InputMap;

	class InputDevice
	{
	public:
		InputDevice() = default;
		virtual ~InputDevice();
		virtual void Update() = 0;

		InputDevice(const InputDevice&) = delete;
		InputDevice& operator=(const InputDevice&) = delete;
		InputDevice(InputDevice&&) = delete;
		InputDevice& operator=(InputDevice&&) = delete;

		void AddInputMap(std::string name, std::unique_ptr<InputMap> inputMap);
		void RemoveInputMap(std::string name);

		void SetActiveInputMap(std::string name);

		virtual bool GetKeyState(int key, InputState state) const = 0;
		virtual float GetAxisValue(int key) const = 0;

		virtual void LoadInputMaps(const Json& mapsJson) = 0;

		InputMap* GetInputMap(std::string name) const;

	protected:
		InputMap* m_pActiveInputMap{nullptr};
		std::unordered_map<std::string, std::unique_ptr<InputMap>> m_inputMaps;
	};
}