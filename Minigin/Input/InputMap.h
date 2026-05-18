#include <vector>
#include "Bindings/ActionBinding.h"

namespace dae
{
	class InputDevice;

	class InputMap
	{
	public:
		InputMap();
		~InputMap();

		InputMap(const InputMap&) = delete;
		InputMap& operator=(const InputMap&) = delete;
		InputMap(InputMap&&) = delete;
		InputMap& operator=(InputMap&&) = delete;

		void CreateActionBinding(const std::string& name, int key, InputState state)
		{
			m_ActionBindings.emplace_back(std::make_unique<ActionBinding>(name, key, state));
		}

		void CreateAxisBinding(const std::string& name, int keyLeft, int keyRight)
		{
			m_AxisBindings.emplace_back(std::make_unique<Axis1DBinding>(name, keyLeft, keyRight));
		}

		void CreateAxis2DBinding(const std::string& name, int keyLeft, int keyRight, int keyDown, int keyUp)
		{
			m_Axis2DBindings.emplace_back(std::make_unique<Axis2DBinding>(name, keyLeft, keyRight, keyDown, keyUp));
		}

		void RemoveActionBinding(const std::string& name)
		{
			m_ActionBindings.erase(
				std::remove_if(m_ActionBindings.begin(), m_ActionBindings.end(),
					[&name](const std::unique_ptr<ActionBinding>& binding) { return binding->m_name == name; }),
				m_ActionBindings.end()
			);
		}

		void RemoveAxisBinding(const std::string& name)
		{
			m_AxisBindings.erase(
				std::remove_if(m_AxisBindings.begin(), m_AxisBindings.end(),
					[&name](const std::unique_ptr<Axis1DBinding>& binding) { return binding->m_name == name; }),
				m_AxisBindings.end()
			);
		}

		void RemoveAxis2DBinding(const std::string& name)
		{
			m_Axis2DBindings.erase(
				std::remove_if(m_Axis2DBindings.begin(), m_Axis2DBindings.end(),
					[&name](const std::unique_ptr<Axis2DBinding>& binding) { return binding->m_name == name; }),
				m_Axis2DBindings.end()
			);
		}
		void AddCommandToActionBinding(const std::string& bindingName, Command& action)
		{
			auto it = std::find_if(m_ActionBindings.begin(), m_ActionBindings.end(),
				[&bindingName](const std::unique_ptr<ActionBinding>& binding) { return binding->m_name == bindingName; });

			if (it != m_ActionBindings.end())
			{
				(*it)->AddCommand(action);
			}
		}

		void AddCommandToAxisBinding(const std::string& bindingName, Axis1DCommand& action)
		{
			auto it = std::find_if(m_AxisBindings.begin(), m_AxisBindings.end(),
				[&bindingName](const std::unique_ptr<Axis1DBinding>& binding) { return binding->m_name == bindingName; });
			if (it != m_AxisBindings.end())
			{
				(*it)->AddCommand(action);
			}
		}

		void AddCommandToAxis2DBinding(const std::string& bindingName, Axis2DCommand& action)
		{
			auto it = std::find_if(m_Axis2DBindings.begin(), m_Axis2DBindings.end(),
				[&bindingName](const std::unique_ptr<Axis2DBinding>& binding) { return binding->m_name == bindingName; });
			if (it != m_Axis2DBindings.end())
			{
				(*it)->AddCommand(action);
			}
		}

		void RemoveCommandFromActionBinding(const std::string& bindingName, Command& action)
		{
			auto it = std::find_if(m_ActionBindings.begin(), m_ActionBindings.end(),
				[&bindingName](const std::unique_ptr<ActionBinding>& binding) { return binding->m_name == bindingName; });
			if (it != m_ActionBindings.end())
			{
				auto& commands = (*it)->m_commands;
				commands.erase(std::remove(commands.begin(), commands.end(), &action), commands.end());
			}
		}

		void RemoveCommandFromAxisBinding(const std::string& bindingName, Axis1DCommand& action)
		{
			auto it = std::find_if(m_AxisBindings.begin(), m_AxisBindings.end(),
				[&bindingName](const std::unique_ptr<Axis1DBinding>& binding) { return binding->m_name == bindingName; });
			if (it != m_AxisBindings.end())
			{
				auto& commands = (*it)->m_commands;
				commands.erase(std::remove(commands.begin(), commands.end(), &action), commands.end());
			}
		}

		void RemoveCommandFromAxis2DBinding(const std::string& bindingName, Axis2DCommand& action)
		{
			auto it = std::find_if(m_Axis2DBindings.begin(), m_Axis2DBindings.end(),
				[&bindingName](const std::unique_ptr<Axis2DBinding>& binding) { return binding->m_name == bindingName; });
			if (it != m_Axis2DBindings.end())
			{
				auto& commands = (*it)->m_commands;
				commands.erase(std::remove(commands.begin(), commands.end(), &action), commands.end());
			}
		}

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