#include "Includes.hpp"
#include "Utils/Utils.hpp"

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/UILayer.hpp>

#include "UI/AnimationsLayer.hpp"

#include "Animations/BaseAnimation.hpp"

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
    };
    
    static void onModify(auto& self) {
        (void)self.setHookPriorityPost("PlayLayer::destroyPlayer", Priority::Last);
    }
  
    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        PlayLayer::destroyPlayer(p0, p1);
        
        if (p1 == m_anticheatSpike) return;
            
        Anim anim = Utils::getSelectedAnimationEnum();
        
        if (anim == Anim::None) return;
        
        const DeathAnimation& animation = Utils::getSelectedAnimation(anim);
        
        if (m_isPracticeMode && !Utils::getSettingBool(animation.id, "play-on-practice"))
            return;

        if (getCurrentPercentInt() < Utils::getSettingFloat(animation.id, "only-after"))
            return;
        
        while (anim == Anim::Random)
            anim = static_cast<Anim>(Utils::getRandomInt(1, animations.size()));
        
        auto f = m_fields.self();
        
        float speed = Utils::getSpeedValue(Utils::getSettingFloat(animation.id, "speed"));
        
        f->m_animation = Utils::createAnimation(anim, this, this, nullptr, speed);      
      
        if (!f->m_animation) return;
        
        f->m_animation->start();
        
        stopActionByTag(16);
        
        if (Utils::getSettingBool(animation.id, "stop-auto-restart"))
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
    
};

class $modify(PlayerObject) {
  
    void playDeathEffect() {
        if (!m_gameLayer)
            return PlayerObject::playDeathEffect();
        
        if (this != m_gameLayer->m_player1 && this != m_gameLayer->m_player2)
            return PlayerObject::playDeathEffect();
            
        auto f = static_cast<ProPlayLayer*>(m_gameLayer)->m_fields.self();
        
        if (!Utils::getSelectedAnimation().stopDeathEffect)
            PlayerObject::playDeathEffect();
    }
    
};

class $modify(UILayer) {
    
    void handleKeypress(enumKeyCodes key, bool p) {
        if ((m_gameLayer && !m_gameLayer->m_player1->m_isDead) || key != enumKeyCodes::KEY_R || !m_gameLayer)
            return UILayer::handleKeypress(key, p);
            
        auto f = static_cast<ProPlayLayer*>(static_cast<PlayLayer*>(m_gameLayer))->m_fields.self();
        
        if (
            f->m_animation
            && f->m_animation->isGoing()
            && Utils::getSettingBool(Utils::getSelectedAnimation().id, "prevent-early-restart")
        ) {
            return;
        }
                
        UILayer::handleKeypress(key, p);
    }
    
};