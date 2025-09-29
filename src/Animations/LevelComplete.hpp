#pragma once

#include "BaseAnimation.hpp"

#include "../Other/SpeedCCParticleSystemQuad.hpp"

class Shake : public CCActionInterval {

private:

    CCPoint m_initialPosition = {0, 0};

    float m_strength = 0.f;

    Shake(float strength)
        : m_strength(strength) {}

    void startWithTarget(CCNode* target) override {
        CCActionInterval::startWithTarget(target);

        m_initialPosition = target->getPosition();
    }

    void update(float time) override {
        float randX = (CCRANDOM_0_1() * 2 - 1) * m_strength;
        float randY = (CCRANDOM_0_1() * 2 - 1) * m_strength;

        m_pTarget->setPosition(m_initialPosition + ccp(randX, randY));
    }

public:

    static Shake* create(float duration, float strength) {
        Shake* ret = new Shake(strength);

        if (ret->initWithDuration(duration)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

};

class LevelComplete : public BaseAnimation {

protected:

    CCNode* m_parentLayer = nullptr;
    CCNode* m_effectContainer = nullptr;
    
    ccColor3B m_color1;
    ccColor3B m_color2;

    CCPoint m_position;

    bool m_isPractice = false;
    bool m_playerWasVisible = false;

    uint64_t m_ogUnkUint64_1 = 0;
    CCPoint m_ogUnkPoint34 = {0, 0};
    float m_ogCameraShakeFactor = 0.f;
    float m_ogUnkUint15 = 0.f;
    float m_ogUnkUint16 = 0.f;
    bool m_ogStaticCameraShake = false;

    void spawnCircle() {
        CCCircleWave* circleWave = CCCircleWave::create(10.f, m_size.width, 0.5f / m_speed, true, true);
        circleWave->m_lineWidth = 4;
        circleWave->m_color = m_color1;
        circleWave->m_circleMode = CircleMode::Outline;

        m_effectContainer->addChild(circleWave);
    }

    void spawnFirework() {
        float rand1 = static_cast<float>(rand()) / RAND_MAX;
        float rand2 = static_cast<float>(rand()) / RAND_MAX;
                
        CCCircleWave* circleWave = CCCircleWave::create(10.f, rand1 * 15.f + 35.f, 0.5f / m_speed, true, true);
        circleWave->m_lineWidth = 4;
        circleWave->m_color = m_color2;

        rand1 = static_cast<float>(rand()) / RAND_MAX;

        CCPoint pos = (m_size - ccp(50, 50)) * ccp(rand2, rand1) + ccp(50, 50);

        circleWave->setPosition(pos);

        addChild(circleWave, 10);

        SpeedCCParticleSystemQuad* particle = SpeedCCParticleSystemQuad::create("firework.plist", false);
        particle->setUpdateSpeed(m_speed);
        particle->setAutoRemoveOnFinish(true);
        particle->setPosition(pos);
        particle->setStartColor(ccc4FFromccc3B(m_color1));
        particle->setEndColor(ccc4FFromccc3B(m_color1));

        addChild(particle, 13);
    }

    void showCompleteText() {
        CCSprite* spr = CCSprite::createWithSpriteFrameName(m_isPractice ? "GJ_practiceComplete_001.png" : "GJ_levelComplete_001.png");
        spr->setPosition(m_size / 2.f + ccp(0, 35.f));
        spr->setScale(0.01f);

        addChild(spr, 15);

        spr->runAction(CCSequence::create(
            CCEaseElasticOut::create(CCScaleTo::create(0.66f / m_speed, 1.1f), 0.6f),
            CCDelayTime::create(0.88f / m_speed),
            CCEaseIn::create(CCScaleTo::create(0.22f / m_speed, 0.01f, 0.01f), 2.f),
            CCHide::create(),
            nullptr
        ));


        SpeedCCParticleSystemQuad* particle = SpeedCCParticleSystemQuad::create("levelComplete01.plist", false);
        particle->setUpdateSpeed(m_speed);
        particle->setAutoRemoveOnFinish(true);
        particle->setPosition(spr->getPosition());
        particle->setStartColor(ccc4FFromccc3B(m_color1));
        particle->setEndColor(ccc4FFromccc3B(m_color1));

        addChild(particle, 13);

        particle = SpeedCCParticleSystemQuad::create("levelComplete01.plist",false);
        particle->setUpdateSpeed(m_speed);
        particle->setAutoRemoveOnFinish(true);
        particle->setPosition(spr->getPosition());

        addChild(particle, 13);

        if (m_isPractice) return;

        CCCircleWave* circleWave = CCCircleWave::create(10.f, m_size.width, 0.8f / m_speed, false, true);
        circleWave->m_lineWidth = 4;
        circleWave->m_color = m_color1;
        
        m_effectContainer->addChild(circleWave, 0);

        circleWave = CCCircleWave::create(10.f, 250.f, 0.8f / m_speed, false, true);
        circleWave->m_lineWidth = 4;
        circleWave->m_color = m_color1;
        circleWave->setPosition(spr->getPosition());
        
        addChild(circleWave, 9);

        for (int i = 0; i < 5; ++i)
            runAction(CCSequence::create(
                CCDelayTime::create(i * 0.05f / m_speed),
                CCCallFunc::create(this, callfunc_selector(LevelComplete::spawnCircle)),
                nullptr
            ));

        for (int i = 0; i < 10; ++i)
            CCNode::runAction(CCSequence::create(
                CCDelayTime::create((static_cast<float>(rand()) / RAND_MAX * 1.6f - 0.8f + 0.15f * i) / m_speed),
                CCCallFunc::create(this, callfunc_selector(LevelComplete::spawnFirework)),
                nullptr
            ));
    }

    ANIMATION_CTOR_CREATE(LevelComplete) {}
    
public:

    virtual void start() override {
        GameManager* gm = GameManager::get();

        m_color1 = gm->colorForIdx(gm->getPlayerColor());
        m_color2 = gm->colorForIdx(gm->getPlayerColor2());

        if (!m_isPreview) {
            m_playerWasVisible = m_playLayer->m_player1->isVisible();
            m_playLayer->m_player1->setVisible(false);
            m_position = m_playLayer->m_player1->getPosition();
            m_parentLayer = m_playLayer->m_player1->getParent();
        } else {
            m_delegate->getPlayer()->setVisible(false);
            m_position = m_delegate->getPlayer()->getPosition();
            m_parentLayer = this;
        }

        m_effectContainer = CCNode::create();
        m_effectContainer->setPosition(m_position);

        m_parentLayer->addChild(m_effectContainer);

        SpeedCCParticleSystemQuad* particle = SpeedCCParticleSystemQuad::create("explodeEffectVortex.plist", false);
        particle->setPosition({0, 0});
        particle->setUpdateSpeed(m_speed);
        particle->setPositionType((tCCPositionType)2);
        particle->setAutoRemoveOnFinish(true);
        particle->setStartColor(ccc4FFromccc3B(m_color1));
        particle->setEndColor(ccc4FFromccc3B(m_color1));
        particle->setBlendAdditive(true);
        particle->setStartColorVar(ccc4FFromccc3B(m_color1));
        particle->setEndColorVar(ccc4FFromccc3B(m_color1));
        particle->setLife(particle->getLife() * 3.f);
        particle->setPosVar(particle->getPosVar() * 1.2f);
        particle->setStartSize(particle->getStartSize() * 0.8f);
        particle->setEndSize(particle->getEndSize() * 0.4f);
        particle->resetSystem();

        m_effectContainer->addChild(particle, 99);

        CCCircleWave* circleWave = CCCircleWave::create(20.f, 80.f, 0.72f / m_speed, false, true);
        circleWave->m_color = m_color1;

        m_effectContainer->addChild(circleWave, 99);

        circleWave = CCCircleWave::create(30.f, 50.f, 0.84f / m_speed, false, true);
        circleWave->m_color = m_color2;
        circleWave->m_opacityMod = 0.8f;

        m_effectContainer->addChild(circleWave, 1000);

        circleWave = CCCircleWave::create(30.f, 20.f, 0.96f / m_speed, false, true);
        circleWave->m_color = m_color1;
        circleWave->m_opacityMod = 0.8;

        m_effectContainer->addChild(circleWave, 1000);

        for (int i = 0; i < 5; ++i)
            runAction(CCSequence::create(
                CCDelayTime::create(i * 0.05f / m_speed),
                CCCallFunc::create(this, callfunc_selector(LevelComplete::spawnCircle)),
                nullptr
            ));

        circleWave = CCCircleWave::create(10.f, 250.f, 0.5f / m_speed, false, true);
        circleWave->m_lineWidth = 4;
        circleWave->m_color.r = m_color1.r;
        circleWave->m_color.g = m_color1.g;
        circleWave->m_color.b = m_color1.b;
        
        m_effectContainer->addChild(circleWave);

        if (m_isPractice) {
            runAction(CCSequence::create(
                CCDelayTime::create(0.1f / m_speed),
                CCCallFunc::create(this, callfunc_selector(LevelComplete::showCompleteText)),
                nullptr
            ));

            return;
        }

        if (Utils::getSettingBool(Anim::LevelComplete, "play-sound-effects"))
            FMODAudioEngine::get()->playEffect("endStart_02.ogg", m_speed, 0.f, 1.f);

        int stripCount = 16;
        float stripRotation = 0.f;
        float stripRotationVariation = 180.f;
        float duration = 0.5f / m_speed;

        // CCLightFlash* lightFlash = CCLightFlash::create();
        // lightFlash->setPosition({0, 0});
        // lightFlash->m_mainLayer = m_parentLayer;
        // lightFlash->m_layerColorZOrder = 100;

        // m_effectContainer->addChild(lightFlash);

        // float zoom = getCurrentZoom();

        // lightFlash->playEffect(
        //     {0, 0},
        //     m_color1,
        //     1.f,
        //     0.f,
        //     30.f,
        //     20.f,
        //     std::max(m_size.width * zoom, m_size.height * zoom) * 2.f,
        //     duration * 0.18f,
        //     duration * 0.04f,
        //     duration * 0.195f,
        //     duration * 0.04f,
        //     duration * 0.04f,
        //     stripRotation,
        //     stripRotationVariation,
        //     155.f,
        //     100.f,
        //     stripCount,
        //     true,
        //     true,
        //     0.4f / m_speed
        // );

        float shakeDuration = ((stripCount + 2.f) * 0.195f * duration);

        if (!m_isPreview) {
            bool ogSkipCameraShake = m_playLayer->m_skipCameraShake;

            m_playLayer->m_skipCameraShake = false;

            m_ogUnkUint64_1 = m_playLayer->m_gameState.m_unkUint64_1;
            m_ogUnkPoint34 = m_playLayer->m_gameState.m_unkPoint34;
            m_ogCameraShakeFactor = m_playLayer->m_gameState.m_cameraShakeFactor;
            m_ogUnkUint15 = m_playLayer->m_gameState.m_unkUint15;
            m_ogUnkUint16 = m_playLayer->m_gameState.m_unkUint16;
            m_ogStaticCameraShake = m_playLayer->m_staticCameraShake;

            m_playLayer->m_gameState.m_unkUint64_1 = 0;
            m_playLayer->m_gameState.m_unkPoint34 = CCPoint{0, 0};
            m_playLayer->m_gameState.m_cameraShakeFactor = shakeDuration;
            m_playLayer->m_gameState.m_unkUint15 = 3.f;
            m_playLayer->m_gameState.m_unkUint16 = 0.f;
            m_playLayer->m_staticCameraShake = false;

            m_playLayer->m_skipCameraShake = ogSkipCameraShake;
        } else {
            CCActionInterval* shake = Shake::create(shakeDuration, 3);

            shake->setTag(10);

            m_delegate->getPlayer()->getParent()->runAction(shake);
        }
        
        runAction(CCSequence::create(
            CCDelayTime::create(shakeDuration),
            CCCallFunc::create(this, callfunc_selector(LevelComplete::showCompleteText)),
            nullptr
        ));
    }

    void end() override {
        if (!m_isPreview) {
            m_playLayer->m_player1->setVisible(true);
            
            if (!m_isPractice) {
                m_playLayer->m_gameState.m_unkUint64_1 = m_ogUnkUint64_1;
                m_playLayer->m_gameState.m_unkPoint34 = m_ogUnkPoint34;
                m_playLayer->m_gameState.m_cameraShakeFactor = m_ogCameraShakeFactor;
                m_playLayer->m_gameState.m_unkUint15 = m_ogUnkUint15;
                m_playLayer->m_gameState.m_unkUint16 = m_ogUnkUint16;
                m_playLayer->m_staticCameraShake = m_ogStaticCameraShake;
            }
        } else
            m_delegate->getPlayer()->getParent()->stopActionByTag(10);

        if (m_effectContainer)
            m_effectContainer->removeFromParentAndCleanup(true);

        BaseAnimation::end();
    }

};