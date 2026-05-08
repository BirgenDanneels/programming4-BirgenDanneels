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

		sound_id LoadSound(const std::string& file) override
		{
			return m_pImpl->LoadSound(file);
		}

		virtual void SetDataPath(const std::string& path) override
		{
			m_pImpl->SetDataPath(path);
		}
	private:
		
		std::unique_ptr<SoundSystem> m_pImpl;
	};
}