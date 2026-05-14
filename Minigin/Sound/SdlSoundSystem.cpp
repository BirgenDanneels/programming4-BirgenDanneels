#include "SdlSoundSystem.h"
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

    struct LoadCommand
    {
        std::string filePath;
    };

    class SdlSoundSystemImpl final : public SoundSystem
    {
    public:
        SdlSoundSystemImpl()
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

            m_playWorker = std::jthread([this](std::stop_token st) { ProcessPlayQueue(st); });
            m_loadWorker = std::jthread([this](std::stop_token st) { ProcessLoadQueue(st); });
        }

        ~SdlSoundSystemImpl() override
        {
            m_playCondition.notify_all();
            m_loadCondition.notify_all();

            if (m_playWorker.joinable())
            {
                m_playWorker.request_stop();
                m_playWorker.join();
            }

            if (m_loadWorker.joinable())
            {
                m_loadWorker.request_stop();
                m_loadWorker.join();
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
            std::unique_lock lock(m_playMutex);
            m_playQueue.push({ id, volume });
            lock.unlock();

            m_playCondition.notify_one();
        }

        void LoadSound(const std::string& file) override
        {
            std::string fullPath = m_dataPath + file;

            {
                std::unique_lock lock(m_loadMutex);
                auto it = m_pathToId.find(fullPath);
                if (it != m_pathToId.end())
                    return; // Already loaded
            }

            std::unique_lock lock(m_loadMutex);
            m_loadQueue.push({ fullPath });
            lock.unlock();

            m_loadCondition.notify_one();
        }

        void SetDataPath(const std::string& path) override
        {
            m_dataPath = path;
        }

        void UnloadSound(const sound_id id) override
        {
            std::unique_lock lock(m_loadMutex);

            auto it = m_sounds.find(id);
            if (it != m_sounds.end())
            {
                if (it->second)
                    MIX_DestroyAudio(it->second);
                m_sounds.erase(it);
            }

            // Remove from path-to-id mapping
            for (auto pathIt = m_pathToId.begin(); pathIt != m_pathToId.end(); ++pathIt)
            {
                if (pathIt->second == id)
                {
                    m_pathToId.erase(pathIt);
                    break;
                }
            }
        }

        void UnloadAll() override
        {
            std::unique_lock lock(m_loadMutex);

            // Clean up all loaded sounds
            for (auto& [id, audio] : m_sounds)
            {
                if (audio)
                    MIX_DestroyAudio(audio);
            }
            m_sounds.clear();
            m_pathToId.clear();
        }

        sound_id GetSoundId(const std::string& file) const override
        {
            std::string fullPath = m_dataPath + file;

            std::unique_lock lock(m_loadMutex);
            auto it = m_pathToId.find(fullPath);
            if (it != m_pathToId.end())
                return it->second;

            return 0; // Return 0 as invalid sound_id
        }

        bool AreAllSoundsLoaded() const override
        {
            std::unique_lock lock(m_loadMutex);
            return m_loadQueue.empty();
        }

    private:
        void ProcessPlayQueue(std::stop_token stopToken)
        {
            while (true)
            {
                std::unique_lock lock(m_playMutex);

                m_playCondition.wait(lock, stopToken, [this]
                    {
                        return !m_playQueue.empty();
                    });

                if (stopToken.stop_requested() && m_playQueue.empty())
                    break;

                auto command = m_playQueue.front();
                m_playQueue.pop();
                lock.unlock();

                PlayInternal(command.id, command.volume);
            }
        }

        void ProcessLoadQueue(std::stop_token stopToken)
        {
            while (true)
            {
                std::unique_lock lock(m_loadMutex);

                m_loadCondition.wait(lock, stopToken, [this]
                    {
                        return !m_loadQueue.empty();
                    });

                if (stopToken.stop_requested() && m_loadQueue.empty())
                    break;

                auto command = m_loadQueue.front();
                m_loadQueue.pop();
                lock.unlock();

                LoadInternal(command.filePath);
            }
        }

        void LoadInternal(const std::string& fullPath)
        {
            std::unique_lock lock(m_loadMutex);

            auto it = m_pathToId.find(fullPath);
            if (it != m_pathToId.end())
                return;

            lock.unlock();

            MIX_Audio* audio = MIX_LoadAudio(m_mixer, fullPath.c_str(), false);
            if (!audio)
                return;

            lock.lock();
            sound_id id = GenerateId();
            m_pathToId[fullPath] = id;
            m_sounds[id] = audio;
        }

        void PlayInternal(sound_id id, float volume)
        {
            std::unique_lock lock(m_loadMutex);
            auto it = m_sounds.find(id);
            if (it == m_sounds.end())
                return;

            MIX_Audio* audio = it->second;
            lock.unlock();

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
                        MIX_SetTrackAudio(track, audio);
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

        std::queue<SoundCommand> m_playQueue;
        std::queue<LoadCommand> m_loadQueue;
        std::mutex m_playMutex;
        mutable std::mutex m_loadMutex;
        std::condition_variable_any m_playCondition;
        std::condition_variable_any m_loadCondition;
        std::jthread m_playWorker;
        std::jthread m_loadWorker;

        MIX_Mixer* m_mixer = nullptr;
        std::vector<MIX_Track*> m_tracks; // Track pool
        std::unordered_map<std::string, sound_id> m_pathToId;
        std::unordered_map<sound_id, MIX_Audio*> m_sounds;

        std::string m_dataPath;
    };


    dae::SdlSoundSystem::SdlSoundSystem()
        :m_pImpl(std::make_unique<SdlSoundSystemImpl>())
    {
    }
}
