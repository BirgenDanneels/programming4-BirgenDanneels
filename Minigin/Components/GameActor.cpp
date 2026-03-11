#include "GameActor.h"
#include "GameObject.h"
#include "Input/InputMap.h"
#include <SDL3/SDL_keycode.h>
#include "Commands/MoveCommands.h"
#include "InputManager.h"

dae::GameActor::GameActor(dae::GameObject& refOwner)
	: Component(refOwner)
{
	//Bind movement this is temp and will be removed
	auto map = std::make_unique<InputMap>();
	
	//map->BindAxis2D(SDL_SCANCODE_A, SDL_SCANCODE_D,SDL_SCANCODE_W, SDL_SCANCODE_S, std::make_unique<Move2DCommand>(*this));
	map->BindAxis2D((int)GamepadInput::LeftThumb, (int)GamepadInput::LeftStickRight, (int)GamepadInput::LeftStickUp, (int)GamepadInput::LeftStickDown, std::make_unique<Move2DCommand>(*this));

	InputManager::GetInstance().BindMapToGamepad(0, std::move(map));
}

void dae::GameActor::Update(float deltaTime)
{
	if(!(glm::length(m_direction) > 0.f))
		return;
		
	m_direction = glm::normalize(m_direction);

	glm::vec3 currentPos = GetOwner()->GetLocalPosition();
	currentPos += glm::vec3(m_direction.x * deltaTime * m_speed, m_direction.y * deltaTime * m_speed, 0);
	GetOwner()->SetLocalPosition(currentPos); 

	//Reset direction after moving
	m_direction = glm::vec2(0.0f, 0.0f);
}

void dae::GameActor::Move(const glm::vec2& direction)
{
	m_direction += direction;
}