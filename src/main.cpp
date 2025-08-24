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

$on_mod(Loaded) {

    Utils::setHookEnabled("cocos2d::CCFadeOut::create", false);
    Utils::setHookEnabled("cocos2d::CCAnimation::createWithSpriteFrames", false);
    Utils::setHookEnabled("cocos2d::CCParticleSystem::update", false);
    Utils::setHookEnabled("CCCircleWave::updateTweenAction", false);
    Utils::setHookEnabled("GJBaseGameLayer::update", false);
    Utils::setHookEnabled("ExplodeItemNode::update", false);

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
                
}

class $modify(MenuLayer) {
  
    void onMoreGames(CCObject* obj) {
        AnimationsLayer::create()->show();
    }
    
};

class $modify(ProPlayLayer, PlayLayer) {
    
    struct Fields {
        BaseAnimation* m_animation = nullptr;

        bool m_forceRestart = false;
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

        if (shouldReturn(anim) || p1 == m_anticheatSpike || f->m_animation)
            return PlayLayer::destroyPlayer(p0, p1);

        const DeathAnimation& animation = Utils::getSelectedAnimation(anim);
        bool og = m_gameState.m_unkBool26;
        DashRingObject* dashOrb1 = m_player1->m_isDashing ? m_player1->m_dashRing : nullptr;
        DashRingObject* dashOrb2 = m_player2->m_isDashing && m_gameState.m_isDualMode ? m_player2->m_dashRing : nullptr;

        if (animation.isNoDeathEffect || animation.isNoDeathSound)
            m_gameState.m_unkBool26 = true;

        if (animation.isSlowDown) {
            Utils::setHookEnabled("cocos2d::CCFadeOut::create", true);
            Utils::setHookEnabled("cocos2d::CCAnimation::createWithSpriteFrames", true);
        }

        PlayLayer::destroyPlayer(p0, p1);

        if (animation.isSlowDown) {
            Utils::setHookEnabled("cocos2d::CCFadeOut::create", false);
            Utils::setHookEnabled("cocos2d::CCAnimation::createWithSpriteFrames", false);
        }

        m_gameState.m_unkBool26 = og;
        
        while (anim == Anim::Random)
            anim = static_cast<Anim>(Utils::getRandomInt(1, animations.size()));

        float speed = Utils::getSpeedValue(Utils::getSettingFloat(anim, "speed"));
        
        f->m_animation = Utils::createAnimation(anim, {this, this, nullptr, speed, { .dashOrb1 = dashOrb1, .dashOrb2 = dashOrb2 }});      
      
        if (!f->m_animation) return;
        
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
            && Utils::getSettingBool(Utils::getSelectedAnimation().id, "prevent-early-restart")
        ) {
            return;
        }

        f->m_forceRestart = false;

        PlayLayer::resetLevel();
        
        if (f->m_animation) {
            f->m_animation->end();
            f->m_animation = nullptr;
        }
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
    }

    void resume() {
        PlayLayer::resume();
        SoundManager::pause(false);
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
        if (!Utils::getSelectedAnimation().isFreezeGameLayer)
            GJBaseGameLayer::update(dt * 0.025f);
    }

    void shakeCamera(float duration, float strength, float interval) {
        if (!Utils::getSelectedAnimation().isNoDeathEffect && !Utils::getSelectedAnimation().isNoShakeEffect)
            GJBaseGameLayer::shakeCamera(duration, strength, interval);
    }

};

class $modify(PlayerObject) {
  
    void playDeathEffect() {
        if (!m_gameLayer)
            return PlayerObject::playDeathEffect();
        
        if (this != m_gameLayer->m_player1 && this != m_gameLayer->m_player2)
            return PlayerObject::playDeathEffect();
            
        if (!Utils::getSelectedAnimation().isNoDeathEffect)
            return PlayerObject::playDeathEffect();
            
        stopActionByTag(11);
    }
    
};