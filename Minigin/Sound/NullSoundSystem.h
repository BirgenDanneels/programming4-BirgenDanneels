#include "SoundSystem.h"

namespace dae
{

	class NullSoundSystem final : public SoundSystem
	{
	public:
		NullSoundSystem() = default;
		~NullSoundSystem() override = default;
		void Play(const sound_id /*id*/, const float /*volume*/) override
		{
		}
		sound_id LoadSound(const std::string& /*file*/) override
		{
			return 0;
		}
		void SetDataPath(const std::string& /*path*/) override
		{
		}
	};
}
