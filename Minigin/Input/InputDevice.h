#pragma once
#include <memory>
#include "Input/Bindings/InputStates.h"


namespace dae
{
	class InputMap;

	class InputDevice
	{
	public:
		virtual ~InputDevice();
		virtual void Update() = 0;

		void SetInputMap(std::unique_ptr<InputMap> inputMap);

		virtual bool GetKeyState(int key, InputState state) const = 0;
		virtual float GetAxisValue(int key) const = 0;

		InputMap* GetInputMap() const {return m_pInputMap.get(); }

	protected:

		std::unique_ptr<InputMap> m_pInputMap{}; //Can be made compatible with holding multiple input maps, but for now we will just use one
	};
}