#include "Includes.hpp"
#include "Utils/Utils.hpp"

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>

#include "UI/AnimationsLayer.hpp"

#include "Animations/BaseAnimation.hpp"
#include "Animations/Celeste.hpp"

$on_mod(Loaded) {
    
    if (!Mod::get()->hasSavedValue("selected-animation"))
        Mod::get()->setSavedValue("selected-animation", 1);
        
}

class $modify(MenuLayer) {
  
    void onMoreGames(CCObject* obj) {
        AnimationsLayer::create()->show();
    }
    
};

class $modify(FMODAudioEngine) {

    int playEffect(gd::string path, float speed, float p2, float volume) {
        if (path == "explode_11.ogg" && Utils::getSelectedAnimation().stopDeathEffect)
            return 0;
        
        return FMODAudioEngine::playEffect(path, speed, p2, volume);
    }

};

class $modify(ProPlayLayer, PlayLayer) {
    
    struct Fields {
        BaseAnimation* m_animation = nullptr;
        BaseAnimation* m_transition = nullptr;
    };
    
    static void onModify(auto& self) {
        (void)self.setHookPriorityPost("PlayLayer::destroyPlayer", Priority::Last);
    }
  
    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        PlayLayer::destroyPlayer(p0, p1);
        
        if (p1 == m_anticheatSpike) return;
            
        Anim anim = Utils::getSelectedAnimationEnum();
        
        if (anim == Anim::None)
            return;
            
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
        
        f->m_animation = Utils::createAnimation(anim, this, this, nullptr, speed);      
      
        if (!f->m_animation) return;
        
        f->m_animation->start();
        
        stopActionByTag(16);
        
        if (Utils::getSettingBool(anim, "stop-auto-restart"))
            return;
        
        CCSequence* seq = CCSequence::create(
            CCDelayTime::create(Utils::getSelectedAnimation(anim).duration / speed),
            CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
            nullptr
        );
        
        seq->setTag(16);
        runAction(seq);
    }
    
    void resetLevel() {
        PlayLayer::resetLevel();
        
        auto f = m_fields.self();
        
        if (f->m_animation) {
            f->m_animation->end();
            f->m_animation = nullptr;
        }
    }
    
    void delayedResetLevel() {
        auto f = m_fields.self();
        
        if (f->m_transition) {
            f->m_transition->end();
            f->m_transition = nullptr;
        }
        
        if (Utils::getSelectedAnimationEnum() == Anim::Celeste && Utils::getSettingBool(Anim::Celeste, "respawn-animation")) {
            f->m_transition = CelesteRevive::create(this, this, nullptr, Utils::getSpeedValue(Utils::getSettingFloat(Anim::Celeste, "speed")));
            f->m_transition->start();
        }
        
        PlayLayer::delayedResetLevel();
    }
    
};

class $modify(PlayerObject) {
  
    void playDeathEffect() {
        if (!m_gameLayer)
            return PlayerObject::playDeathEffect();
        
        if (this != m_gameLayer->m_player1 && this != m_gameLayer->m_player2)
            return PlayerObject::playDeathEffect();
            
        if (!Utils::getSelectedAnimation().stopDeathEffect)
            return PlayerObject::playDeathEffect();
            
        stopActionByTag(11);
    }
    
    void playSpawnEffect() {
        if (this != m_gameLayer->m_player1 && this != m_gameLayer->m_player2)
            return PlayerObject::playDeathEffect();
        
        if (Utils::getSelectedAnimationEnum() != Anim::Celeste || !Utils::getSettingBool(Anim::Celeste, "respawn-animation"))
            return PlayerObject::playDeathEffect();
            
        stopActionByTag(11);
    }
    
};

class $modify(UILayer) {
    
    void handleKeypress(enumKeyCodes key, bool p) {
        if ((m_gameLayer && !m_gameLayer->m_player1->m_isDead) || key != enumKeyCodes::KEY_R || !m_gameLayer)
            return UILayer::handleKeypress(key, p);
            
        auto f = static_cast<ProPlayLayer*>(static_cast<PlayLayer*>(m_gameLayer))->m_fields.self();
        
        if (
            f->m_animation
            && Utils::getSettingBool(Utils::getSelectedAnimation().id, "prevent-early-restart")
        ) {
            return;
        }
                
        UILayer::handleKeypress(key, p);
    }
    
};