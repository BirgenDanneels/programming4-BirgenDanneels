#include "Minigin/Components/Component.h"
#include "ButtonComponent.h"
#include <memory>

namespace dae
{
	class InputDevice;
	class Axis2DCommand;
	class Command;
}

//This class does not manager any UI elements itself, it just manages the navigation and selection of buttons.
class UINavigator final : public dae::Component
{
public:
	UINavigator(dae::GameObject& owner);
	~UINavigator() override;
	void Update(float) override {};
	void Render() const override {};
	void FixedUpdate(float) override {};

	void Initialize(std::string inputDeviceName, dae::GameObject* firstFocused);
	std::vector<dae::ParamDefinition> GetExpectedParams() const override;
	void Load(const dae::ParamMap& params) override;

	void SetCurrentFocusedButton(ButtonComponent* button);
	void Navigate(ButtonComponent::ButtonDirection direction);
	void PressButton();

private:

	std::unique_ptr<dae::Axis2DCommand> m_navigateCommand;
	std::unique_ptr<dae::Command> m_pressCommand;

	dae::InputDevice* m_pInputDevice = nullptr;

	ButtonComponent* m_pCurrentFocusedButton = nullptr;
};