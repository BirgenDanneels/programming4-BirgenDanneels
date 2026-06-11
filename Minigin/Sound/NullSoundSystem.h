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

		void UnloadSound(const sound_id) override
		{
		}

		void UnloadAll() override
		{
		}

		sound_id GetSoundId(const std::string&) const override
		{
			return 0;
		}

		bool AreAllSoundsLoaded() const override
		{
			return true;
		}

		Subject<>& OnSoundsLoaded() override
		{
			static Subject<> dummySubject;
			return dummySubject;
		}
	};
}
