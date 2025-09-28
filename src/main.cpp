#include "Includes.hpp"

#include "Other/Utils.hpp"
#include "Other/SoundManager.hpp"
#include "Other/Variables.hpp"

#include "UI/AnimationsLayer.hpp"

#include "Animations/BaseAnimation.hpp"

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCCircleWave.hpp>
#include <Geode/modify/CCParticleSystem.hpp>
#include <Geode/modify/ExplodeItemNode.hpp>
#include <Geode/modify/CCFadeOut.hpp>
#include <Geode/modify/CCAnimation.hpp>
#include <Geode/modify/ChannelControl.hpp>

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
        std::error_code ec;
        
        if (!utils::file::createDirectoryAll(soundsFolder).isErr())
            std::filesystem::copy_file(
                Mod::get()->getResourcesDir() / "jumpscare.mp3",
                soundsFolder / "jumpscare.mp3",
                std::filesystem::copy_options::overwrite_existing,
                ec
            );

        if (!utils::file::createDirectoryAll(imagesFolder).isErr())
            std::filesystem::copy_file(
                Mod::get()->getResourcesDir() / "jumpscare.png",
                imagesFolder / "jumpscare.png",
                std::filesystem::copy_options::overwrite_existing,
                ec
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

        bool m_forceRetryLayer = false;
        bool m_keyPressHookEnabled = false;
        bool m_didShowRetryLayer = false;

        int m_accumulatedRetryLayers = false;

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
            || (getCurrentPercentInt() <= m_level->m_normalPercent.value() && Utils::getSettingBool(anim, "only-on-new-best"))
            || (getCurrentPercentInt() > m_level->m_normalPercent.value() && anim == Anim::NewBest);
    }

    void delayedResetLevelReal() {
        m_fields->m_forceRestart = true;
        delayedResetLevel();
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) {
        auto f = m_fields.self();

        Anim anim = Utils::getSelectedAnimationEnum();

        if (anim == Anim::Random)
            anim = static_cast<Anim>(animations[Utils::getRandomInt(2, static_cast<int>(animations.size()) - 1)].id);

        if (shouldReturn(anim) || obj == m_anticheatSpike || f->m_animation) {
            Variables::setSelectedAnimation({});
            return PlayLayer::destroyPlayer(player, obj);
        }

        const DeathAnimation& animation = Utils::getAnimationByID(anim);

        Variables::setSelectedAnimation(animation);

        float speed = Utils::getSpeedValue(Utils::getSettingFloat(anim, "speed"));
        
        f->m_animation = Utils::createAnimation(
            anim, 
            {
                .parentNode = this,
                .playLayer = this,
                .delegate = nullptr,
                .speed = speed,
                .id = anim
            }
        );      

        if (!f->m_animation)
            return PlayLayer::destroyPlayer(player, obj);

        f->m_animation->startEarly();

        bool og = m_gameState.m_unkBool26;

        if (animation.isNoDeathEffect || animation.isNoDeathSound)
            m_gameState.m_unkBool26 = true;

        if (animation.isNoStopMusic) {
            Utils::setHookEnabled("FMOD::ChannelControl::stop", true);
            Utils::setHookEnabled("FMOD::ChannelControl::setPaused", true);
        }

        PlayLayer::destroyPlayer(player, obj);
        
        if (animation.isNoStopMusic) {
            Utils::setHookEnabled("FMOD::ChannelControl::stop", false);
            Utils::setHookEnabled("FMOD::ChannelControl::setPaused", false);
        }

        m_gameState.m_unkBool26 = og;
        m_uiLayer->m_pauseBtn->setEnabled(true);
        f->m_didShowRetryLayer = false;
        
        f->m_animation->startWithObject(obj);
        f->m_animation->start();

        if (!getActionByTag(16))
            return;
        
        stopActionByTag(16);

        CCSequence* seq = CCSequence::create(
            CCDelayTime::create(f->m_animation->getDuration() / speed + 0.05f),
            CCCallFunc::create(this, callfunc_selector(ProPlayLayer::delayedResetLevelReal)),
            nullptr
        );
        
        seq->setTag(16);
        runAction(seq);
    }

    void delayedShowRetryLayer() {
        auto f = m_fields.self();

        f->m_forceRetryLayer = true;

        showRetryLayer();

        f->m_forceRetryLayer = false;
    }

    void showRetryLayer() {
        const DeathAnimation& anim = Variables::getSelectedAnimation();
        auto f = m_fields.self();

        if (f->m_accumulatedRetryLayers > 0) {
            f->m_accumulatedRetryLayers--;
            return;
        }

        f->m_didShowRetryLayer = true;

        if (!f->m_animation)
            return PlayLayer::showRetryLayer();

        if (
            (anim.retryLayerDelay <= 0.f && !f->m_animation->isNoRetryLayer())
            || f->m_forceRetryLayer
        ) {
            return PlayLayer::showRetryLayer();
        }

        if (anim.retryLayerDelay > 0.f && !f->m_animation->isNoRetryLayer()) {
            CCAction* action = CCSequence::create(
                CCDelayTime::create(anim.retryLayerDelay / Utils::getSpeedValue(Utils::getSettingFloat(anim.id, "speed"))),
                CCCallFunc::create(this, callfunc_selector(ProPlayLayer::delayedShowRetryLayer)),
                nullptr
            );

            action->setTag(472);

            runAction(action);
        }
    }
    
    void resetLevel() {
        auto f = m_fields.self();

        if (
            f->m_animation
            && !f->m_forceRestart
            && !f->m_animation->isForceRestart()
            && !f->m_animation->didFinish()
            && Utils::getSettingBool(Variables::getSelectedAnimation().id, "prevent-early-restart")
        ) {
            return;
        }

        if (f->m_animation) {
            if (f->m_animation->isDontRestart())
                return;

            if (f->m_animation->isDelayRestart())
                return f->m_animation->onRestart();
        }

        f->m_forceRestart = false;

        bool endedAnimation = f->m_animation != nullptr;

        if (f->m_animation) {
            f->m_animation->end();
            f->m_animation = nullptr;

            if (!f->m_didShowRetryLayer)
                f->m_accumulatedRetryLayers++;
        }

        PlayLayer::resetLevel();

        stopActionByTag(472);

        if (endedAnimation && f->m_isNewBest && Variables::getSelectedAnimation().isDelayNewBest) {
            Variables::setSelectedAnimation({});
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

        if (f->m_animation && Variables::getSelectedAnimation().isDelayNewBest) {
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
        SoundManager::pause(false);

        auto f = m_fields.self();

        if (f->m_animation)
            f->m_animation->onResume();

        PlayLayer::resume();
    }

    void onQuit() {
        PlayLayer::onQuit();

        SoundManager::stop();
    }
    
};

class $modify(CCFadeOut) {

    static CCFadeOut* create(float time) { // disabled by defolt
        return CCFadeOut::create(time / Variables::getSpeed());
    }

};

class $modify(CCAnimation) {

    static CCAnimation* createWithSpriteFrames(CCArray* array, float time) { // disabled by defolt
        return CCAnimation::createWithSpriteFrames(array, time / Variables::getSpeed());
    }

};

class $modify(ExplodeItemNode) {

    void update(float dt) { // disabled by defolt
        ExplodeItemNode::update(dt * Variables::getSpeed());
    }

};

class $modify(CCParticleSystem) {

    void update(float dt) { // disabled by defolt
        CCParticleSystem::update(dt * Variables::getSpeed());
    }

};

class $modify(CCCircleWave) {

    void updateTweenAction(float dt, const char* p1) { // disabled by defolt
        CCCircleWave::updateTweenAction(dt * Variables::getSpeed(), p1);
    }

};

class $modify(GJBaseGameLayer) {
    
    void update(float dt) { // disabled by defolt
        if (!Variables::getSelectedAnimation().isFreezeGameLayer)
            GJBaseGameLayer::update(dt * Variables::getSpeed());
    }

    void shakeCamera(float duration, float strength, float interval) {
        if (!Variables::getSelectedAnimation().isNoDeathEffect && !Variables::getSelectedAnimation().isNoShakeEffect)
            GJBaseGameLayer::shakeCamera(duration, strength, interval);
    }

};

class $modify(PlayerObject) {
  
    void playDeathEffect() {
        if (!m_gameLayer)
            return PlayerObject::playDeathEffect();
        
        if (this != m_gameLayer->m_player1 && this != m_gameLayer->m_player2)
            return PlayerObject::playDeathEffect();
            
        if (!Variables::getSelectedAnimation().isNoDeathEffect)
            return PlayerObject::playDeathEffect();
            
        stopActionByTag(11);
    }

    void playSpawnEffect() {
        if (!Variables::getSelectedAnimation().isNoSpawnEffect)
            PlayerObject::playSpawnEffect();
    }
    
};

class $modify(FMOD::ChannelControl) {

    FMOD_RESULT stop() { // disabled by defolt
        return (FMOD_RESULT)0;
    }

    FMOD_RESULT setPaused(bool paused) { // disabled by defolt
        return FMOD::ChannelControl::setPaused(false);
    }

};