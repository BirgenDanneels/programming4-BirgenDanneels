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
		virtual sound_id LoadSound(const std::string& file) = 0;
	};
}
