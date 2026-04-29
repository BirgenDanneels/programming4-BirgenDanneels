#include "SoundSystem.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <unordered_map>
#include <vector>
#include <SDL3_mixer/SDL_mixer.h>

namespace dae
{
    struct SoundCommand
    {
        sound_id id;
        float volume;
    };

    class SdlSoundSystem final : public SoundSystem
    {
    public:
        SdlSoundSystem()
        {
            MIX_Init();
            m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

            // Pre-allocate a pool of 16 tracks for overlapping sounds
            if (m_mixer)
            {
                for (int i = 0; i < 16; ++i)
                {
                    m_tracks.push_back(MIX_CreateTrack(m_mixer));
                }
            }

            m_worker = std::jthread([this](std::stop_token st) { ProcessQueue(st); });
        }

        ~SdlSoundSystem() override
        {
            m_condition.notify_all();
            if (m_worker.joinable())
            {
                m_worker.request_stop();
                m_worker.join();
            }

            // Clean up the track pool
            for (MIX_Track* track : m_tracks)
            {
                if (track)
                    MIX_DestroyTrack(track);
            }
            m_tracks.clear();

            // Clean up all loaded sounds
            for (auto& [id, audio] : m_sounds)
            {
                if (audio)
                    MIX_DestroyAudio(audio);
            }
            m_sounds.clear();
            m_pathToId.clear();

            if (m_mixer)
                MIX_DestroyMixer(m_mixer);

            MIX_Quit();
        }

        void Play(const sound_id id, const float volume) override
        {
            std::unique_lock lock(m_mutex);
            m_queue.push({ id, volume });
            lock.unlock();

            m_condition.notify_one();
        }

        sound_id LoadSound(const std::string& file) override
        {
			std::string fullPath = m_dataPath + file;

            auto it = m_pathToId.find(fullPath);
            if (it != m_pathToId.end())
                return it->second;

            MIX_Audio* audio = MIX_LoadAudio(m_mixer, (fullPath).c_str(), false);
            if (!audio)
                return 0;

            sound_id id = GenerateId();
            m_pathToId[fullPath] = id;
            m_sounds[id] = audio;

            return id;
        }

    private:
        void ProcessQueue(std::stop_token stopToken)
        {
            while (true)
            {
                std::unique_lock lock(m_mutex);

                m_condition.wait(lock, stopToken, [this]
                    {
                        return !m_queue.empty();
                    });

                if (stopToken.stop_requested() && m_queue.empty())
                    break;

                auto command = m_queue.front();
                m_queue.pop();
                lock.unlock();

                PlayInternal(command.id, command.volume);
            }
        }

        void PlayInternal(sound_id id, float volume)
        {
            auto it = m_sounds.find(id);
            if (it == m_sounds.end())
                return;

            if (m_mixer)
            {
                // Find an available track that is not currently playing anything
                for (MIX_Track* track : m_tracks)
                {
                    if (!MIX_TrackPlaying(track))
                    {
                        // Apply the specific volume (0.0f = silence, 1.0f = full volume)
                        MIX_SetTrackGain(track, volume);
                        
                        // Assign the audio segment and play immediately (0 uses default options)
                        MIX_SetTrackAudio(track, it->second);
                        MIX_PlayTrack(track, 0);
                        
                        break; // Successfully played, break out of search
                    }
                }
            }
        }

        sound_id GenerateId()
        {
            static sound_id nextId = 1;
            return nextId++;
        }

        std::queue<SoundCommand> m_queue;
        std::mutex m_mutex;
        std::condition_variable_any m_condition;
        std::jthread m_worker;

        MIX_Mixer* m_mixer = nullptr;
        std::vector<MIX_Track*> m_tracks; // Track pool
        std::unordered_map<std::string, sound_id> m_pathToId;
        std::unordered_map<sound_id, MIX_Audio*> m_sounds;
    };
}
