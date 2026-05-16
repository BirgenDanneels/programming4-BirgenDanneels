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
		
		void LoadSound(const std::string& file) override
		{	
			std::cout << "Loading sound from file: " << file << std::endl;
			m_realSoundSystem->LoadSound(file);
		}

		virtual void UnloadSound(const sound_id id) override
		{
			std::cout << "Unloading sound with ID: " << id << std::endl;

			m_realSoundSystem->UnloadSound(id);
		}

		virtual void UnloadAll() override
		{
			std::cout << "Unloading all sounds" << std::endl;

			m_realSoundSystem->UnloadAll();
		}

		virtual sound_id GetSoundId(const std::string& file) const override
		{
			std::cout << "Getting sound ID for file: " << file;

			sound_id id = m_realSoundSystem->GetSoundId(file);

			std::cout << " -> ID: " << id << std::endl;

			return id;
		}

		bool AreAllSoundsLoaded() const override
		{
			std::cout << "Checking if all sounds are loaded..." << std::endl;

			bool allLoaded = m_realSoundSystem->AreAllSoundsLoaded();
			std::cout << " -> " << (allLoaded ? "All sounds loaded" : "Some sounds still loading") << std::endl;

			return allLoaded;
		}

		Subject<>& OnSoundsLoaded() override
		{
			std::cout << "Accessing OnSoundsLoaded event" << std::endl;
			return m_realSoundSystem->OnSoundsLoaded();
		}

	private:
		std::unique_ptr<SoundSystem> m_realSoundSystem;
	};
}
