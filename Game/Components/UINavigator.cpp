#include "UINavigator.h"
#include "Minigin/GameObject.h"
#include "Minigin/InputManager.h"
#include "Minigin/Input/InputMap.h"
#include "Minigin/Commands/Command.h"
#include "Minigin/Loading/LoadingHelpers.h"


class NavigateUICommand final : public dae::Axis2DCommand
{
public:
	NavigateUICommand(UINavigator& manager)
		: m_manager(manager)
	{
	}

	void Execute() override
	{
		ButtonComponent::ButtonDirection direction;

		auto axisValue = GetAxisValue();

		// Very arbitrary thresholds to determine direction, can be tweaked or made configurable if needed

		if (axisValue.y < -0.5f)
			direction = ButtonComponent::ButtonDirection::Down;
		else if (axisValue.y > 0.5f)
			direction = ButtonComponent::ButtonDirection::Up;
		else if (axisValue.x < -0.5f)
			direction = ButtonComponent::ButtonDirection::Left;
		else if (axisValue.x > 0.5f)
			direction = ButtonComponent::ButtonDirection::Right;
		else
			return; // No significant input, do not navigate

		m_manager.Navigate(direction);
	}

private:
	UINavigator& m_manager;
};

class PressUICommand final : public dae::Command
{
public:
	PressUICommand(UINavigator& manager)
		: m_manager(manager)
	{
	}

	void Execute() override
	{
		m_manager.PressButton();
	}

private:
	UINavigator& m_manager;
};

UINavigator::UINavigator(dae::GameObject& owner)
	: Component(owner)
{
}

UINavigator::~UINavigator()
{

}

void UINavigator::Initialize(std::string inputDeviceName, dae::GameObject* firstFocused)
{
	m_navigateCommand = std::make_unique<NavigateUICommand>(*this);
	m_pressCommand = std::make_unique<PressUICommand>(*this);

	m_pCurrentFocusedButton = firstFocused->GetComponent<ButtonComponent>();
	m_pCurrentFocusedButton->OnFocus();

	dae::InputManager& inputManager = dae::InputManager::GetInstance();
	auto device = inputManager.GetDeviceByName(inputDeviceName);

	//TODO: SHOULDNT BE CREATING MAPS HERE THIS IS JUST FOR TESTING TILL FILE LOADING MAPS IS A THING

	auto* map = device->GetInputMap();

	if (!map)
	{
		auto tempMapPtr = std::make_unique<dae::InputMap>();
		map = tempMapPtr.get();
		device->SetInputMap(std::move(tempMapPtr));
	}

	map->BindAxis2D("NavigateUI",
		SDL_SCANCODE_A, SDL_SCANCODE_D, SDL_SCANCODE_S, SDL_SCANCODE_W, *m_navigateCommand);

	map->BindAction("PressButton", SDL_SCANCODE_RETURN, dae::InputState::Pressed, *m_pressCommand);
}

std::vector<dae::ParamDefinition> UINavigator::GetExpectedParams() const
{
	return { { "inputDeviceName", std::string{} },
			{ "firstFocusedButton", (dae::GameObject*) nullptr } };
}

void UINavigator::Load(const dae::ParamMap& params)
{
	Initialize(dae::GetRequiredParam<std::string>(params, "inputDeviceName"), dae::GetRequiredParam<dae::GameObject*>(params, "firstFocusedButton"));
}


void UINavigator::SetCurrentFocusedButton(ButtonComponent* button)
{
	if (m_pCurrentFocusedButton)
		m_pCurrentFocusedButton->OnUnfocus();
	m_pCurrentFocusedButton = button;
	if (m_pCurrentFocusedButton)
		m_pCurrentFocusedButton->OnFocus();
}

void UINavigator::Navigate(ButtonComponent::ButtonDirection direction)
{
	if (!m_pCurrentFocusedButton)
		return;

	auto nextButton = m_pCurrentFocusedButton->GetNeighbor(direction);
	if (nextButton)
	{
		m_pCurrentFocusedButton->OnUnfocus();
		m_pCurrentFocusedButton = nextButton;
		m_pCurrentFocusedButton->OnFocus();
	}
}

void UINavigator::PressButton()
{
	if (m_pCurrentFocusedButton)
		m_pCurrentFocusedButton->Press();
}
