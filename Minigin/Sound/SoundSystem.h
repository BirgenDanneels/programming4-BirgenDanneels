#pragma once
#include <memory>
#include <string>

namespace dae
{
	using sound_id = unsigned short;
	class SoundSystem
	{
	public:
		virtual ~SoundSystem() = default;
		virtual void Play(const sound_id id, const float volume) = 0;
		virtual void SetDataPath(const std::string& path) = 0;
		virtual void LoadSound(const std::string& file) = 0;

		virtual void UnloadSound(const sound_id id) = 0;
		virtual void UnloadSound(const std::string& file)
		{
			UnloadSound(GetSoundId(file));
		}

		virtual void UnloadAll() = 0;

		virtual sound_id GetSoundId(const std::string& file) const = 0;
		virtual bool AreAllSoundsLoaded() const = 0;
	};
}
