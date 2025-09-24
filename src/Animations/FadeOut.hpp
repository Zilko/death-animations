#include "BaseAnimation.hpp"

#include "../Other/Variables.hpp"

class FadeOut : public BaseAnimation {

private:

    float m_time = 0.f;
    float m_ogMusicPitch = 1.f;
    float m_ogSfxPitch = 1.f;
    float m_ogMusicVolume = 1.f;
    float m_ogSfxVolume = 1.f;

    bool m_audioOnly = false;

    void update(float dt) {
        m_time += dt * m_speed;

        FMODAudioEngine* fmod = FMODAudioEngine::get();

        float pitchProgress = 1.f - std::min(m_time / 1.f, 1.f);
        float volumeProgress = 1.f - std::min(m_time / 0.95f, 1.f);
        float speedProgress = 1.f - std::min(m_time / 1.f, 1.f);

        fmod->m_backgroundMusicChannel->setPitch(m_ogMusicPitch * pitchProgress);
        fmod->m_globalChannel->setPitch(m_ogSfxPitch * pitchProgress);

        fmod->m_backgroundMusicChannel->setVolume(m_ogMusicVolume * volumeProgress);
        fmod->m_globalChannel->setVolume(m_ogSfxVolume * volumeProgress);

        Variables::setSpeed(std::max(speedProgress, 0.0001f));
    }

    void toggleHooks(bool toggled) {
        if (m_audioOnly) return;
        
        Utils::setHookEnabled("cocos2d::CCParticleSystem::update", toggled);
        Utils::setHookEnabled("CCCircleWave::updateTweenAction", toggled);
        Utils::setHookEnabled("GJBaseGameLayer::update", toggled);
        Utils::setHookEnabled("ExplodeItemNode::update", toggled);
    }

    ~FadeOut() {
        FMODAudioEngine* fmod = FMODAudioEngine::get();

        fmod->m_backgroundMusicChannel->setPitch(m_ogMusicPitch);
        fmod->m_globalChannel->setPitch(m_ogSfxPitch);

        fmod->m_backgroundMusicChannel->setVolume(m_ogMusicVolume);
        fmod->m_globalChannel->setVolume(m_ogSfxVolume);

        Variables::setSpeed(1.f);

        toggleHooks(false);
    }

    ANIMATION_CTOR_CREATE(FadeOut) {}
    
public:

    void start() override {
        m_audioOnly = Utils::getSettingBool(Anim::FadeOut, "audio-only");

        Variables::setSpeed(1.f);

        toggleHooks(true);

        FMODAudioEngine* fmod = FMODAudioEngine::get();

        fmod->m_backgroundMusicChannel->getPitch(&m_ogMusicPitch);
        fmod->m_globalChannel->getPitch(&m_ogSfxPitch);
        
        m_ogMusicVolume = fmod->m_musicVolume;
        m_ogSfxVolume = fmod->m_sfxVolume;

        schedule(schedule_selector(FadeOut::update));
    }

    void onPause() override {
        toggleHooks(false);

        FMODAudioEngine* fmod = FMODAudioEngine::get();

        fmod->m_backgroundMusicChannel->setVolume(m_ogMusicVolume);
        fmod->m_globalChannel->setVolume(m_ogSfxVolume);
    }

    void onResume() override {
        toggleHooks(true);

        FMODAudioEngine* fmod = FMODAudioEngine::get();

        m_ogMusicVolume = fmod->m_musicVolume;
        m_ogSfxVolume = fmod->m_sfxVolume;
    }
    
};