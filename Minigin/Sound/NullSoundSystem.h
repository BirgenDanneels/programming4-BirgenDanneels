#include "SoundSystem.h"

namespace dae
{

	class NullSoundSystem final : public SoundSystem
	{
	public:
		NullSoundSystem() = default;
		~NullSoundSystem() override = default;
		void Play(const sound_id , const float ) override
		{
		}
		void LoadSound(const std::string& ) override
		{
		}
		void SetDataPath(const std::string&) override
		{
		}

		virtual void UnloadSound(const sound_id)
		{
		}

		virtual void UnloadAll()
		{
		}

		virtual sound_id GetSoundId(const std::string&) const
		{
			return 0;
		}

		bool AreAllSoundsLoaded() const override
		{
			return true;
		}
	};
}
