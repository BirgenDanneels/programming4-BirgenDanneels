#pragma once
#include <memory>
#include "SoundSystem.h"
#include "Minigin/Physics/PhysicsManager.h"

namespace dae
{

	class ServiceLocator final
	{
		static std::unique_ptr<SoundSystem> m_soundInstance;
		static std::unique_ptr<PhysicsManager> m_physicsInstance;
	public:
		static SoundSystem& GetSoundSystem() { return *m_soundInstance; }
		static void RegisterSoundSystem(std::unique_ptr<SoundSystem>&& ss) { m_soundInstance = std::move(ss); }
		static void UnregisterSoundSystem() { m_soundInstance.reset(); }

		static PhysicsManager& GetPhysicsManager() { return *m_physicsInstance; }
		static void RegisterPhysicsManager(std::unique_ptr<PhysicsManager>&& pm) { m_physicsInstance = std::move(pm); }
		static void UnregisterPhysicsManager() { m_physicsInstance.reset(); }
	};
}
