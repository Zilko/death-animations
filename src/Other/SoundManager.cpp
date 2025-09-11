#include "SoundManager.hpp"

void SoundUpdater::updateSound(float dt) {
    bool isPlaying = true;

    m_totalTime += dt;
    
    m_channel->isPlaying(&isPlaying);
    
    if ((m_channel && !isPlaying) || m_totalTime >= 60.f) {
        m_sound->release();
        this->release();
        m_channel = nullptr;
        SoundManager::remove(this);
        CCScheduler::get()->unscheduleSelector(schedule_selector(SoundUpdater::updateSound), this);
    }
}

void SoundUpdater::pause(bool paused) {
    m_channel->setPaused(paused);
}

void SoundUpdater::stop() {
    m_channel->stop();
}

SoundManager& SoundManager::get() {
    static SoundManager instance;
    return instance;
}

SoundUpdater* SoundManager::add(FMOD::Channel* channel, FMOD::Sound* sound) {
    SoundUpdater* updater = new SoundUpdater(channel, sound);
    updater->retain();
    CCScheduler::get()->scheduleSelector(schedule_selector(SoundUpdater::updateSound), updater, 0.2f, kCCRepeatForever, 0, false);
    get().m_sounds.push_back(updater);
    return updater;
}

void SoundManager::remove(SoundUpdater* updater) {
    std::vector<SoundUpdater*>& sounds = get().m_sounds;
    sounds.erase(std::remove(sounds.begin(), sounds.end(), updater), sounds.end());
}

void SoundManager::retain(SoundUpdater* sound) {
    if (sound)
        get().m_retainedSounds.insert(sound);
}

void SoundManager::release(SoundUpdater* sound) {
    if (sound)
        get().m_retainedSounds.erase(sound);
}

void SoundManager::pause(bool paused) {
    for (SoundUpdater* updater : get().m_sounds)
        updater->pause(paused);
}

void SoundManager::stop() {
    for (SoundUpdater* updater : get().m_sounds)
        if (!get().m_retainedSounds.contains(updater))
            updater->stop();
}