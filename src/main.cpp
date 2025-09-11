#include "Includes.hpp"

#include "Other/Utils.hpp"
#include "Other/SoundManager.hpp"

#include "UI/AnimationsLayer.hpp"

#include "Animations/BaseAnimation.hpp"

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCCircleWave.hpp>
#include <Geode/modify/CCParticleSystem.hpp>
#include <Geode/modify/ExplodeItemNode.hpp>
#include <Geode/modify/CCFadeOut.hpp>
#include <Geode/modify/CCAnimation.hpp>
#include <Geode/modify/ChannelControl.hpp>

class Vars {

public:

    inline static DeathAnimation selectedAnimation = {};

};

$on_mod(Loaded) {

    Utils::setHookEnabled("cocos2d::CCFadeOut::create", false);
    Utils::setHookEnabled("cocos2d::CCAnimation::createWithSpriteFrames", false);
    Utils::setHookEnabled("cocos2d::CCParticleSystem::update", false);
    Utils::setHookEnabled("CCCircleWave::updateTweenAction", false);
    Utils::setHookEnabled("ExplodeItemNode::update", false);
    Utils::setHookEnabled("GJBaseGameLayer::update", false);
    Utils::setHookEnabled("FMOD::ChannelControl::stop", false);
    Utils::setHookEnabled("FMOD::ChannelControl::setPaused", false);

    if (!Mod::get()->hasSavedValue("selected-animation"))
        Mod::get()->setSavedValue("selected-animation", 1);
    
    if (!Mod::get()->setSavedValue("created-jumpscare-folders", true)) {
        std::filesystem::path soundsFolder = Mod::get()->getSaveDir() / "jumpscare-sounds";
        std::filesystem::path imagesFolder = Mod::get()->getSaveDir() / "jumpscare-images";
        
        if (!utils::file::createDirectoryAll(soundsFolder).isErr())
            std::filesystem::copy_file(
                Mod::get()->getResourcesDir() / "jumpscare.mp3",
                soundsFolder / "jumpscare.mp3",
                std::filesystem::copy_options::overwrite_existing
            );

        if (!utils::file::createDirectoryAll(imagesFolder).isErr())
            std::filesystem::copy_file(
                Mod::get()->getResourcesDir() / "jumpscare.png",
                imagesFolder / "jumpscare.png",
                std::filesystem::copy_options::overwrite_existing
            );
    }

    if (!Mod::get()->setSavedValue("created-speech-bubble-folder", true)) {
        std::filesystem::path folder = Mod::get()->getSaveDir() / "speech-bubbles";
        
        if (!utils::file::createDirectoryAll(folder).isErr())
            for (int i = 1; i < 3; i++) {
                std::string name = fmt::format("speech-bubble-{}.png", i);
                std::error_code ec;
                
                std::filesystem::copy_file(
                    Mod::get()->getResourcesDir() / name,
                    folder / name,
                    std::filesystem::copy_options::overwrite_existing,
                    ec
                );
            }
    }
                
}

class $modify(MenuLayer) {
  
    void onMoreGames(CCObject* obj) {
        AnimationsLayer::create()->show();
    }
    
};

class $modify(ProPlayLayer, PlayLayer) {
    
    struct Fields {
        BaseAnimation* m_animation = nullptr;

        bool m_isNewBest = false;
        bool m_forceRestart = false;
        bool m_storedNewReward = false;
        bool m_storedDemonKey = false;
        bool m_storedNoTitle = false;

        int m_storedOrbs = 0;
        int m_storedDiamonds = 0;
    };
    
    static void onModify(auto& self) {
        (void)self.setHookPriorityPre("PlayLayer::destroyPlayer", Priority::Last + 21903809);
    }

    bool shouldReturn(Anim anim) {
        return anim == Anim::None
            || Utils::getRandomInt(1, 100) > static_cast<int>(Utils::getSettingFloat(anim, "probability"))
            || (m_isPracticeMode && !Utils::getSettingBool(anim, "play-in-practice"))
            || (m_isTestMode && !Utils::getSettingBool(anim, "play-in-testmode"))
            || getCurrentPercentInt() < Utils::getSettingFloat(anim, "only-after")
            || (getCurrentPercentInt() <= m_level->m_normalPercent && Utils::getSettingBool(anim, "only-on-new-best"));
    }

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        auto f = m_fields.self();

        Anim anim = Utils::getSelectedAnimationEnum();

        if (anim == Anim::Random)
            anim = static_cast<Anim>(animations[Utils::getRandomInt(2, static_cast<int>(animations.size()) - 1)].id);

        if (shouldReturn(anim) || p1 == m_anticheatSpike || f->m_animation)
            return PlayLayer::destroyPlayer(p0, p1);

        const DeathAnimation& animation = Utils::getSelectedAnimation(anim);

        Vars::selectedAnimation = animation;

        float speed = Utils::getSpeedValue(Utils::getSettingFloat(anim, "speed"));
        
        f->m_animation = Utils::createAnimation(anim, {this, this, nullptr, speed});      

        if (!f->m_animation)
            return PlayLayer::destroyPlayer(p0, p1);

        f->m_animation->startEarly();

        bool og = m_gameState.m_unkBool26;

        if (animation.isNoDeathEffect || animation.isNoDeathSound)
            m_gameState.m_unkBool26 = true;

        if (animation.isSlowDown) {
            Utils::setHookEnabled("cocos2d::CCFadeOut::create", true);
            Utils::setHookEnabled("cocos2d::CCAnimation::createWithSpriteFrames", true);
        }

        if (animation.isNoStopMusic) {
            Utils::setHookEnabled("FMOD::ChannelControl::stop", true);
            Utils::setHookEnabled("FMOD::ChannelControl::setPaused", true);
        }

        PlayLayer::destroyPlayer(p0, p1);

        if (animation.isSlowDown) {
            Utils::setHookEnabled("cocos2d::CCFadeOut::create", false);
            Utils::setHookEnabled("cocos2d::CCAnimation::createWithSpriteFrames", false);
        }
        
        if (animation.isNoStopMusic) {
            Utils::setHookEnabled("FMOD::ChannelControl::stop", false);
            Utils::setHookEnabled("FMOD::ChannelControl::setPaused", false);
        }

        m_gameState.m_unkBool26 = og;
        
        f->m_animation->start();
        
        stopActionByTag(16);
        
        if (Utils::getSettingBool(anim, "stop-auto-restart"))
            return;
        
        CCSequence* seq = CCSequence::create(
            CCDelayTime::create(animation.duration / speed + 0.05f),
            CCCallFunc::create(this, callfunc_selector(ProPlayLayer::delayedResetLevelReal)),
            nullptr
        );
        
        seq->setTag(16);
        runAction(seq);
    }

    void delayedResetLevelReal() {
        m_fields->m_forceRestart = true;
        delayedResetLevel();
    }
    
    void resetLevel() {
        auto f = m_fields.self();

        if (
            f->m_animation
            && !f->m_forceRestart
            && Utils::getSettingBool(Vars::selectedAnimation.id, "prevent-early-restart")
        ) {
            return;
        }

        f->m_forceRestart = false;

        bool endedAnimation = f->m_animation != nullptr;

        if (f->m_animation) {
            f->m_animation->end();
            f->m_animation = nullptr;
        }

        PlayLayer::resetLevel();

        if (endedAnimation && f->m_isNewBest && Vars::selectedAnimation.isDelayNewBest) {
            Vars::selectedAnimation = {};
            f->m_isNewBest = false;
            
            showNewBest(
                f->m_storedNewReward,
                f->m_storedOrbs,
                f->m_storedDiamonds,
                f->m_storedDemonKey,
                false,
                f->m_storedNoTitle
            );
        }
    }

    void showNewBest(bool newReward, int orbs, int diamonds, bool demonKey, bool noRetry, bool noTitle) {
        auto f = m_fields.self();

        if (f->m_animation && Vars::selectedAnimation.isDelayNewBest) {
            f->m_storedNewReward = newReward;
            f->m_storedOrbs = orbs;
            f->m_storedDiamonds = diamonds;
            f->m_storedDemonKey = demonKey;
            f->m_storedNoTitle = noTitle;
            f->m_isNewBest = true;
            
            return;
        }
            
        PlayLayer::showNewBest(newReward, orbs, diamonds, demonKey, noRetry, noTitle);
    }

    void dialogClosed(DialogLayer* wa) {
        if (!m_fields->m_animation)
            PlayLayer::dialogClosed(wa);
    }

    void currencyWillExit(CurrencyRewardLayer* wa) {
        if (!m_fields->m_animation)
            PlayLayer::currencyWillExit(wa);
    }

    void pauseGame(bool p0) {
        PlayLayer::pauseGame(p0);

        SoundManager::pause(true);

        auto f = m_fields.self();

        if (f->m_animation)
            f->m_animation->onPause();
    }

    void resume() {
        PlayLayer::resume();

        SoundManager::pause(false);

        auto f = m_fields.self();

        if (f->m_animation)
            f->m_animation->onResume();
    }
    
};

class $modify(CCFadeOut) {

    static CCFadeOut* create(float time) { // disabled by defolt
        return CCFadeOut::create(time / 0.07f);
    }

};

class $modify(CCAnimation) {

    static CCAnimation* createWithSpriteFrames(CCArray* array, float time) { // disabled by defolt
        return CCAnimation::createWithSpriteFrames(array, time / 0.07f);
    }

};

class $modify(ExplodeItemNode) {

    void update(float dt) { // disabled by defolt
        ExplodeItemNode::update(dt * 0.025f);
    }

};

class $modify(CCParticleSystem) {

    void update(float dt) { // disabled by defolt
        CCParticleSystem::update(dt * 0.025f);
    }

};

class $modify(CCCircleWave) {

    void updateTweenAction(float dt, const char* p1) { // disabled by defolt
        CCCircleWave::updateTweenAction(dt * 0.025f, p1);
    }

};

class $modify(GJBaseGameLayer) {
    
    void update(float dt) { // disabled by defolt
        if (!Vars::selectedAnimation.isFreezeGameLayer)
            GJBaseGameLayer::update(dt * 0.025f);
    }

    void shakeCamera(float duration, float strength, float interval) {
        if (!Vars::selectedAnimation.isNoDeathEffect && !Vars::selectedAnimation.isNoShakeEffect)
            GJBaseGameLayer::shakeCamera(duration, strength, interval);
    }

};

class $modify(PlayerObject) {
  
    void playDeathEffect() {
        if (!m_gameLayer)
            return PlayerObject::playDeathEffect();
        
        if (this != m_gameLayer->m_player1 && this != m_gameLayer->m_player2)
            return PlayerObject::playDeathEffect();
            
        if (!Vars::selectedAnimation.isNoDeathEffect)
            return PlayerObject::playDeathEffect();
            
        stopActionByTag(11);
    }

    void playSpawnEffect() {
        if (!Vars::selectedAnimation.isNoSpawnEffect)
            PlayerObject::playSpawnEffect();
    }
    
};

class $modify(FMOD::ChannelControl) {

    FMOD_RESULT stop() { // disabled by defolt
        return (FMOD_RESULT)0;
    }

    FMOD_RESULT setPaused(bool paused) { // disabled by defolt
        paused = false;

        return FMOD::ChannelControl::setPaused(paused);
    }

};