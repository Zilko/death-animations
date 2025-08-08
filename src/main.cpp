#include "Includes.hpp"

#include "Other/Utils.hpp"

#include "UI/AnimationsLayer.hpp"

#include "Animations/BaseAnimation.hpp"
#include "Animations/Celeste.hpp"

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCCircleWave.hpp>
#include <Geode/modify/CCParticleSystem.hpp>
#include <Geode/modify/ExplodeItemNode.hpp>

$on_mod(Loaded) {
    
    if (!Mod::get()->hasSavedValue("selected-animation"))
        Mod::get()->setSavedValue("selected-animation", 1);
        
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
        (void)self.setHookPriorityPost("PlayLayer::destroyPlayer", Priority::Last);
    }

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        Anim anim = Utils::getSelectedAnimationEnum();

        if (anim == Anim::None)
            return PlayLayer::destroyPlayer(p0, p1);

        bool og = m_gameState.m_unkBool26;

        if (Utils::getSelectedAnimation(anim).isNoDeathEffect)
            m_gameState.m_unkBool26 = true;

        PlayLayer::destroyPlayer(p0, p1);

        if (Utils::getSelectedAnimation(anim).isNoDeathEffect)
            m_gameState.m_unkBool26 = og;
        
        if (p1 == m_anticheatSpike) return;
            
        if (Utils::getRandomInt(1, 100) > static_cast<int>(Utils::getSettingFloat(anim, "probability")))
            return;
        
        if (m_isPracticeMode && !Utils::getSettingBool(anim, "play-on-practice"))
            return;

        if (getCurrentPercentInt() < Utils::getSettingFloat(anim, "only-after"))
            return;
        
        while (anim == Anim::Random)
            anim = static_cast<Anim>(Utils::getRandomInt(1, animations.size()));
        
        auto f = m_fields.self();
        
        float speed = Utils::getSpeedValue(Utils::getSettingFloat(anim, "speed"));
        
        f->m_animation = Utils::createAnimation(anim, {this, this, nullptr, speed});      
      
        if (!f->m_animation) return;
        
        f->m_animation->start();
        
        stopActionByTag(16);
        
        if (Utils::getSettingBool(anim, "stop-auto-restart"))
            return;
        
        CCSequence* seq = CCSequence::create(
            CCDelayTime::create(Utils::getSelectedAnimation(anim).duration / speed + 0.05f),
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
    
};

// class $modify(ExplodeItemNode) {

//     void update(float dt) {
//         if (PlayLayer::get() && PlayLayer::get()->m_player1->m_isDead)
//         return ExplodeItemNode::update(dt * 0.025f);
//         ExplodeItemNode::update(dt);
//     }

// };

// class $modify(CCParticleSystem) {

//     void update(float dt) {
//         if (PlayLayer::get() && PlayLayer::get()->m_player1->m_isDead)
//         return CCParticleSystem::update(dt * 0.025f);
//         CCParticleSystem::update(dt);
//     }

// };

// class $modify(CCCircleWave) {

//     void updateTweenAction(float dt, const char* p1) {
//         if (PlayLayer::get() && PlayLayer::get()->m_player1->m_isDead)
//         return CCCircleWave::updateTweenAction(dt * 0.025f, p1);
//         CCCircleWave::updateTweenAction(dt, p1);
//     }

// };

class $modify(GJBaseGameLayer) {

    // void update(float dt) {
    //     if (m_player1->m_isDead)
    //     return GJBaseGameLayer::update(dt * 0.025f);
    //     GJBaseGameLayer::update(dt);
    // }

    void shakeCamera(float duration, float strength, float interval) {
        if (!Utils::getSelectedAnimation().isNoDeathEffect)
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