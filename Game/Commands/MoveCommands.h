#pragma once
#include "Minigin/Commands/Command.h"

namespace dae
{
	class Rigidbody;

	class Move2DCommand : public Axis2DCommand
	{
	private:
		Rigidbody* m_rigidbody{ nullptr };
		float m_speed{ 100.0f };
		bool m_isMoving{ false };

	public:
		Move2DCommand(Rigidbody& rigidbody, float speed);
		void Execute() override;
	};

}