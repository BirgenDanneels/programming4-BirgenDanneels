#pragma once
#include <glm/glm.hpp>

namespace dae
{
	class Command
	{
	public:
		virtual ~Command() = default;
		virtual void Execute() = 0;

	};


	// This command is here since it is used in engine. tbh it would be better to just us std::function<void(float)> for axis input (which i will do after the assignment)
	class Axis1DCommand : public Command
	{
	public:
		void SetAxisValue(float value) { m_axisValue = value; }

	protected:
		float GetAxisValue() const { return m_axisValue; }

	private:

		float m_axisValue{ 0.f };

	};

	class Axis2DCommand : public Command
	{
	public:
		void SetAxisValue(const glm::vec3& value) { m_axisValue = value; }

	protected:
		const glm::vec3& GetAxisValue() const { return m_axisValue; }

	private:
		glm::vec3 m_axisValue{ 0.f };
	};
}