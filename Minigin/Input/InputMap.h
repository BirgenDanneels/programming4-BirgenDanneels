#include <vector>
#include "Bindings/ActionBinding.h"

namespace dae
{
	class InputDevice;

	class InputMap
	{
	public:
		InputMap() = default;
		~InputMap() = default;

		InputMap(const InputMap&) = delete;
		InputMap& operator=(const InputMap&) = delete;
		InputMap(InputMap&&) = delete;
		InputMap& operator=(InputMap&&) = delete;

		void BindAction(int key, InputState state, std::unique_ptr<Command> command)
		{
			m_ActionBindings.emplace_back(key, state, std::move(command));
		};

		void BindAxis(int keyLeft, int keyRight, std::unique_ptr<Axis1DCommand> command)
		{
			m_AxisBindings.emplace_back(keyLeft, keyRight, std::move(command));
		};

		void BindAxis2D(int keyLeft, int keyRight, int keyUp, int keyDown, std::unique_ptr<Axis2DCommand> command)
		{
			m_Axis2DBindings.emplace_back(keyLeft, keyRight, keyUp, keyDown, std::move(command));
		};

		void SetDevice(InputDevice* device) { m_pDevice = device; }

		const std::vector<ActionBinding>& GetActionBindings() const { return m_ActionBindings; }
		const InputDevice* GetDevice() const { return m_pDevice; }

		void Evaluate();

	private:

		std::vector<ActionBinding> m_ActionBindings{};
		std::vector<Axis1DBinding> m_AxisBindings{};
		std::vector<Axis2DBinding> m_Axis2DBindings{};

		InputDevice* m_pDevice{ nullptr };

	};
}