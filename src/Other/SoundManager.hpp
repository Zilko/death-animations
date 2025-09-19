#include "../Includes.hpp"

class SoundUpdater : public CCObject {
    
private:

    FMOD::Channel* m_channel = nullptr;
    FMOD::Sound* m_sound = nullptr;

    float m_timeLimit = 0.f;
    float m_totalTime = 0.f;
    
public:

    SoundUpdater(FMOD::Channel* channel, FMOD::Sound* sound, float timeLimit)
        : m_channel(channel), m_sound(sound), m_timeLimit(timeLimit) {}
    
    void updateSound(float);

    void pause(bool);

    void stop();
    
};

class SoundManager {

private:

    std::vector<SoundUpdater*> m_sounds;

    std::unordered_set<SoundUpdater*> m_retainedSounds;

    static SoundManager& get();

public:

    static SoundUpdater* add(FMOD::Channel*, FMOD::Sound*, float = 60.f);

    static void remove(SoundUpdater*);

    static void retain(SoundUpdater*);

    static void release(SoundUpdater*);

    static void pause(bool);

    static void stop();

};