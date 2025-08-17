#include "../Includes.hpp"

class SoundUpdater : public CCObject {
    
private:

    FMOD::Channel* m_channel = nullptr;
    FMOD::Sound* m_sound = nullptr;

    float m_totalTime = 0.f;
    
public:

    SoundUpdater(FMOD::Channel* channel, FMOD::Sound* sound)
        : m_channel(channel), m_sound(sound) {}
    
    void updateSound(float);

    void pause(bool);

    void stop();
    
};

class SoundManager {

private:

    std::vector<SoundUpdater*> m_sounds;

    static SoundManager& get();

public:

    static void add(FMOD::Channel*, FMOD::Sound*);

    static void remove(SoundUpdater*);

    static void pause(bool);

    static void stop();

};