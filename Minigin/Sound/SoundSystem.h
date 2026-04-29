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
		virtual void SetDataPath(const std::string& path) { m_dataPath = path; }
		virtual sound_id LoadSound(const std::string& file) = 0;

	protected:
		std::string m_dataPath;
	};

	class DefaultSoundSystem final : public SoundSystem
	{
	public:
		void Play(const sound_id, const float) override {}
		sound_id LoadSound(const std::string&) override { return 0; }
	};
}
