#include "SoundSystem.h"
#include "memory.h"

namespace dae
{
	class SdlSoundSystem final : public SoundSystem
	{
	public:
		SdlSoundSystem();
		~SdlSoundSystem() override = default;
		void Play(const sound_id id, const float volume) override
		{
			m_pImpl->Play(id, volume);
		}

		void LoadSound(const std::string& file) override
		{
			m_pImpl->LoadSound(file);
		}

		virtual void SetDataPath(const std::string& path) override
		{
			m_pImpl->SetDataPath(path);
		}

		virtual void UnloadSound(const sound_id id) override
		{
			m_pImpl->UnloadSound(id);
		}

		virtual void UnloadAll() override
		{
			m_pImpl->UnloadAll();
		}

		virtual sound_id GetSoundId(const std::string& file) const override
		{
			return m_pImpl->GetSoundId(file);
		}

		bool AreAllSoundsLoaded() const override
		{
			return m_pImpl->AreAllSoundsLoaded();
		}

		Subject<>& OnSoundsLoaded() override
		{
			return m_pImpl->OnSoundsLoaded();
		}

	private:
		
		std::unique_ptr<SoundSystem> m_pImpl;
	};
}