#include "Transform.h"
#include "GameObject.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/rotate_vector.hpp> // Needed for glm::rotateZ, but is experimental so might need to be replaced with a custom implementation

namespace dae
{
	Transform::Transform(GameObject* pOwner)
		: m_pOwner{ pOwner }
	{
	}

	// Local Setters
	void Transform::SetLocalPosition(const glm::vec3& position)
	{
		m_localPosition = position;
		SetDirty();
	}

	void Transform::SetLocalPosition(float x, float y, float z)
	{
		SetLocalPosition(glm::vec3{ x, y, z });
	}

	void Transform::SetLocalRotation(float angle)
	{
		m_localRotation = angle;
		SetDirty();
	}

	void Transform::SetLocalScale(const glm::vec2& scale)
	{
		m_localScale = scale;
		SetDirty();
	}

	void Transform::SetLocalScale(float x, float y)
	{
		SetLocalScale(glm::vec2{ x, y });
	}

	void Transform::SetLocalScale(float uniformScale)
	{
		SetLocalScale(glm::vec2{ uniformScale, uniformScale });
	}


	// World Setters
	void Transform::SetWorldPosition(const glm::vec3& position)
	{
		if (m_pOwner && m_pOwner->GetParent())
		{
			SetLocalPosition(position - m_pOwner->GetParent()->GetTransform().GetWorldPosition());
		}
		else
		{
			SetLocalPosition(position);
		}
	}

	void Transform::SetWorldPosition(float x, float y, float z)
	{
		SetWorldPosition(glm::vec3{ x, y, z });
	}

	void Transform::SetWorldRotation(float angle)
	{
		if (m_pOwner && m_pOwner->GetParent())
		{
			SetLocalRotation(angle - m_pOwner->GetParent()->GetTransform().GetWorldRotation());
		}
		else
		{
			SetLocalRotation(angle);
		}
	}

	void Transform::SetWorldScale(const glm::vec2& scale)
	{
		if (m_pOwner && m_pOwner->GetParent())
		{
			glm::vec2 parentScale = m_pOwner->GetParent()->GetTransform().GetWorldScale();
			// Prevent division by zero
			glm::vec2 localScale{};
			localScale.x = (parentScale.x != 0.0f) ? (scale.x / parentScale.x) : 0.0f;
			localScale.y = (parentScale.y != 0.0f) ? (scale.y / parentScale.y) : 0.0f;

			SetLocalScale(localScale);
		}
		else
		{
			SetLocalScale(scale);
		}
	}

	void Transform::SetWorldScale(float x, float y)
	{
		SetWorldScale(glm::vec2{ x, y });
	}

	void Transform::SetWorldScale(float uniformScale)
	{
		SetWorldScale(glm::vec2{ uniformScale, uniformScale });
	}


	// Relative Mutators
	void Transform::Translate(const glm::vec2& translation)
	{
		SetLocalPosition(m_localPosition + glm::vec3(translation, 0.0f));
	}

	void Transform::Translate(float x, float y)
	{
		Translate(glm::vec2{ x, y });
	}

	void Transform::Rotate(float angle)
	{
		SetLocalRotation(m_localRotation + angle);
	}


	// World Getters
	const glm::vec3& Transform::GetWorldPosition() const
	{
		if (m_isDirty) UpdateWorldTransform();
		return m_worldPosition;
	}

	float Transform::GetWorldRotation() const
	{
		if (m_isDirty) UpdateWorldTransform();
		return m_worldRotation;
	}

	const glm::vec2& Transform::GetWorldScale() const
	{
		if (m_isDirty) UpdateWorldTransform();
		return m_worldScale;
	}

	void Transform::SetDirty()
	{
		m_isDirty = true;
		
		if (m_pOwner)
		{
			for (const auto& pChild : m_pOwner->GetChildren())
			{
				pChild->GetTransform().SetDirty();
			}
		}
	}

	void Transform::UpdateWorldTransform() const
	{
		if (m_pOwner && m_pOwner->GetParent())
		{
			const auto& parentTransform = m_pOwner->GetParent()->GetTransform();

			// Scale the local position
			glm::vec3 scaledLocalPos = m_localPosition * glm::vec3(parentTransform.GetWorldScale(), 1.0f);

			// Rotate the scaled local position
			glm::vec3 rotatedScaledLocalPos = glm::rotateZ(scaledLocalPos, glm::radians(parentTransform.GetWorldRotation()));

			// Add the rotated & scaled offset to the parent's world position
			m_worldPosition = parentTransform.GetWorldPosition() + rotatedScaledLocalPos;

			// Rotation and Scale cascade additively/multiplicatively
			m_worldRotation = parentTransform.GetWorldRotation() + m_localRotation;
			m_worldScale = parentTransform.GetWorldScale() * m_localScale;
		}
		else
		{
			m_worldPosition = m_localPosition;
			m_worldRotation = m_localRotation;
			m_worldScale = m_localScale;
		}

		m_isDirty = false;
	}
}