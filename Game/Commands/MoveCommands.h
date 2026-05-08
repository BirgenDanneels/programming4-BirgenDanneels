#pragma once
#include "Minigin/Commands/Command.h"

namespace dae
{
	class Rigidbody;
} 

class Move2DCommand : public dae::Axis2DCommand
{
private:
	dae::Rigidbody* m_rigidbody{ nullptr };
	float m_speed{ 100.0f };
	bool m_isMoving{ false };

public:
	Move2DCommand(dae::Rigidbody& rigidbody, float speed);
	void Execute() override;
};