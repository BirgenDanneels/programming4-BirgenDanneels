#pragma once
#include <memory>
#include "SoundSystem.h"

namespace dae
{

	class ServiceLocator final
	{
		static std::unique_ptr<SoundSystem> _ss_instance;
	public:
		static SoundSystem& GetSoundSystem() { return *_ss_instance; }
		static void RegisterSoundSystem(std::unique_ptr<SoundSystem>&& ss) { _ss_instance = std::move(ss); }
		static void UnregisterSoundSystem() { _ss_instance.reset(); }
	};
}
