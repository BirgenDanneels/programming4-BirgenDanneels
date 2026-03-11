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

		void BindAction(const std::string& name, int key, InputState state, Command& command)
		{
			m_ActionBindings.emplace_back(std::make_unique<ActionBinding>(name, key, state, command));
		};

		void BindAxis(const std::string& name, int keyLeft, int keyRight, Axis1DCommand& command)
		{
			m_AxisBindings.emplace_back(std::make_unique<Axis1DBinding>(name, keyLeft, keyRight, command));
		};

		void BindAxis2D(const std::string& name, int keyLeft, int keyRight, int keyUp, int keyDown, Axis2DCommand& command)
		{
			m_Axis2DBindings.emplace_back(std::make_unique<Axis2DBinding>(name, keyLeft, keyRight, keyUp, keyDown, command));
		};


		//Unbinding should be just by name and its probably better if it just binded with a pointer or ref to an input action and then the user is responsible for the ownership and can unbind it using a string or the pointer. Still to be determined.
		void UnbindAction(const std::string& name);
		void UnbindAxis(const std::string& name);
		void UnbindAxis2D(const std::string& name);

		void SetDevice(InputDevice* device) { m_pDevice = device; }

		const std::vector<std::unique_ptr<ActionBinding>>& GetActionBindings() const { return m_ActionBindings; }
		const InputDevice* GetDevice() const { return m_pDevice; }

		void Evaluate();

	private:

		std::vector<std::unique_ptr<ActionBinding>> m_ActionBindings{};
		std::vector< std::unique_ptr<Axis1DBinding>> m_AxisBindings{};
		std::vector< std::unique_ptr<Axis2DBinding>> m_Axis2DBindings{};

		InputDevice* m_pDevice{ nullptr };

	};
}