#include "BaseAnimation.hpp"

#include "../Other/SpeedCCParticleSystemQuad.hpp"

class PoofEffect : public CCNode {

private:

    void init(float speed) {
        CCNode::init();

        setID("poof-effect"_spr);
        
        static const std::array<ParticleStruct, 4> m_particles = {{
            {10,1.2,1.2,0,-1,90,0,26,0,13,8,0,0,0,0,0,0,22,24,50,50,1,0,1,0,1,0,0.35,0,30,24,50,50,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,true,false,false,false,180,false,0,0,0,0,false,false,false,false,false,0,0,0,0,"poof-particle.png"_spr},
            {14,1.4,1.4,0,-1,90,0,138,0,20,13,0,0,0,0,0,0,40,40,50,50,1,0,1,0,1,0,0.5,0,40,40,50,50,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,true,false,false,false,180,false,1.55,0,0,0,false,false,false,false,false,0,0,0,0,"poof-particle.png"_spr},
            {17,1.7,1.7,0,-1,90,0,40,0,8,12,0,0,0,0,0,0,20,6,130,50,1,0,1,0,1,0,0.09,0,11,8,50,50,1,0,1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,true,false,false,false,180,false,0.74,0,0,0,false,false,false,false,false,0,0,0,0,"poof-particle.png"_spr},
            {4,1,1,0,-1,90,14,256,40,11,11,0,-652,63,0,0,0,2,1,0,0,1,0,1,0,1,0,0.5,0,0,0,0,0,1,0,1,0,1,0,0.5,0,0,0,0,0,0,0,0,0,0,0,0,2,true,false,false,false,0,false,0,0,0,0,false,false,false,false,false,0,0,0,0,"particle_00_001.png"},
        }};

        for (const ParticleStruct& struct_ : m_particles) {
            SpeedCCParticleSystemQuad* particle = SpeedCCParticleSystemQuad::create(struct_);
            particle->setUpdateSpeed(speed);
            particle->setAutoRemoveOnFinish(true);
            particle->resetSystem();

            addChild(particle);
        }

        scheduleOnce(schedule_selector(PoofEffect::killMyself), 2.f / speed);
    }

    void killMyself(float) {
        removeFromParentAndCleanup(true);
    }

public:

    static PoofEffect* create(float speed) {
        PoofEffect* ret = new PoofEffect();

        ret->init(speed);
        ret->autorelease();

        return ret;
    }

};

class Poof : public BaseAnimation {

private:

    std::vector<CCNode*> m_visiblePlayers;
    
    void playPoof(CCNode* player) {
        PoofEffect* poof = PoofEffect::create(m_speed);
        poof->setPosition(player->getPosition());
        
        player->getParent()->addChild(poof);
        
        if (player->isVisible()) {
            m_visiblePlayers.push_back(player);
            player->setVisible(false); 
        }
        
        Utils::setHighestZ(poof);
    }
    
    ANIMATION_CTOR_CREATE(Poof) {}
    
public:

    void start() override {
        Utils::playSound(Anim::Poof, "poof-1.mp3", m_speed, 0, 0, 9999, 0.7f, false, 115);
        Utils::playSound(Anim::Poof, "poof-1.mp3", m_speed * 2.f, 0, 0, 9999, 1.f, false, 115);
        Utils::playSound(Anim::Poof, "poof-2.mp3", m_speed, 0, 0, 9999, 0.05f, false, 165);

        CCLayerColor* layer = CCLayerColor::create({255, 255, 255, 12});
        layer->runAction(CCEaseSineOut::create(CCFadeTo::create(1.6f / m_speed, 0)));

        addChild(layer);
        
        if (m_isPreview)
            playPoof(m_delegate->getPlayer());
        else {
            playPoof(m_playLayer->m_player1);
            
            if (Utils::getSettingBool(Anim::Poof, "second-player") && m_playLayer->m_gameState.m_isDualMode)
                playPoof(m_playLayer->m_player2);
        }
    }
    
    void end() override {
        for (CCNode* player : m_visiblePlayers)
            player->setVisible(true);
            
        BaseAnimation::end();
    }

};