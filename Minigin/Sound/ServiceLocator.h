#pragma once
#include <memory>
#include "SoundSystem.h"
#include "Minigin/Loading/PrefabManager.h"
#include "Minigin/Physics/PhysicsManager.h"
#include "Minigin/Sound/NullSoundSystem.h"

namespace dae
{

	class ServiceLocator final
	{
		static std::unique_ptr<SoundSystem> m_soundInstance;
		static std::unique_ptr<PhysicsManager> m_physicsInstance;
		static std::unique_ptr<PrefabManager> m_prefabInstance;
	public:
		static SoundSystem& GetSoundSystem() { return *m_soundInstance; }
		static void RegisterSoundSystem(std::unique_ptr<SoundSystem>&& ss) 
		{ 
			m_soundInstance = ss == nullptr ? std::make_unique<NullSoundSystem>() : std::move(ss);
		}
		static void UnregisterSoundSystem() { m_soundInstance.reset(); }

		static PhysicsManager& GetPhysicsManager() { return *m_physicsInstance; }
		static void RegisterPhysicsManager(std::unique_ptr<PhysicsManager>&& pm) { m_physicsInstance = std::move(pm); }
		static void UnregisterPhysicsManager() { m_physicsInstance.reset(); }

		static PrefabManager& GetPrefabManager() { return *m_prefabInstance; }
		static void RegisterPrefabManager(std::unique_ptr<PrefabManager>&& pm) { m_prefabInstance = std::move(pm); }
		static void UnregisterPrefabManager() { m_prefabInstance.reset(); }
	};
}
