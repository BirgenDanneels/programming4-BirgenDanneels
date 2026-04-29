#include "SoundSystem.h"
#include <memory>
#include <iostream>

namespace dae
{
	class LoggingSoundSystem final : public SoundSystem
	{
	public:
		LoggingSoundSystem(std::unique_ptr<SoundSystem> realSoundSystem) : m_realSoundSystem(std::move(realSoundSystem)) {}
		
		void SetDataPath(const std::string& path) override
		{
			m_realSoundSystem->SetDataPath(path);
		}
		
		void Play(const sound_id id, const float volume) override
		{
			std::cout << "Playing sound with ID " << id << " at volume " << volume << std::endl;
			m_realSoundSystem->Play(id, volume);
		}
		
		sound_id LoadSound(const std::string& file) override
		{	
			std::cout << "Loading sound from file: " << file << std::endl;
			return m_realSoundSystem->LoadSound(file);
		}

	private:
		std::unique_ptr<SoundSystem> m_realSoundSystem;
	};
}
