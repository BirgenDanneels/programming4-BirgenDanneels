#pragma once
#include <glm/glm.hpp>

namespace dae
{
	class GameObject;

	class Transform final
	{
	public:
		explicit Transform(GameObject* pOwner);

		// Local Setters
		void SetLocalPosition(float x, float y, float z = 0.0f);
		void SetLocalPosition(const glm::vec3& position);
		
		void SetLocalRotation(float angle);

		void SetLocalScale(float x, float y);
		void SetLocalScale(const glm::vec2& scale);
		void SetLocalScale(float uniformScale);

		// World Setters
		void SetWorldPosition(float x, float y, float z = 0.0f);
		void SetWorldPosition(const glm::vec3& position);
		
		void SetWorldRotation(float angle);

		void SetWorldScale(float x, float y);
		void SetWorldScale(const glm::vec2& scale);
		void SetWorldScale(float uniformScale);

		// Relative Mutators
		void Translate(float x, float y);
		void Translate(const glm::vec2& translation);
		
		void Rotate(float angle);

		// Local Getters
		const glm::vec3& GetLocalPosition() const { return m_localPosition; }
		float GetLocalRotation() const { return m_localRotation; }
		const glm::vec2& GetLocalScale() const { return m_localScale; }

		// World Getters
		const glm::vec3& GetWorldPosition() const;
		float GetWorldRotation() const;
		const glm::vec2& GetWorldScale() const;

	private:
		friend class GameObject;


		// Internal helper to recalculate world caches if dirty
		void UpdateWorldTransform() const;

		void SetDirty();

		GameObject* m_pOwner{ nullptr };

		// Local spatial data
		glm::vec3 m_localPosition{ 0.0f, 0.0f, 0.0f };
		float m_localRotation{ 0.0f }; 
		glm::vec2 m_localScale{ 1.0f, 1.0f };

		// World spatial caches
		mutable glm::vec3 m_worldPosition{ 0.0f, 0.0f, 0.0f };
		mutable float m_worldRotation{ 0.0f }; 
		mutable glm::vec2 m_worldScale{ 1.0f, 1.0f };

		mutable bool m_isDirty{ true };
	};
}
