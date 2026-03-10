#include <SDL3/SDL.h>
#include "InputDevice.h"

namespace dae
{
	class Keyboard final: public InputDevice
	{
	public:
		Keyboard();
		~Keyboard() = default;

		void Update() override;

		bool GetKeyState(int key, InputState state) const override;
		float GetAxisValue(int key) const override;

	private:
		const bool* m_pCurrentState{ nullptr };
		bool  m_previousState[SDL_SCANCODE_COUNT]{ 0 };

		bool IsKeyDown(int key) const;
		bool IsKeyUp(int key) const;
		bool IsKeyPressed(int key) const;
		bool IsKeyReleased(int key) const;
	};
}