#include "../Utils/Utils.hpp"

#include "PreviewLayer.hpp"

PreviewLayer* PreviewLayer::create() {
    PreviewLayer* ret = new PreviewLayer();
    
    CCSize winSize = CCDirector::get()->getWinSize();
    
    if (ret->initAnchored(winSize.width, winSize.height)) {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}

void PreviewLayer::updateCircle(float) {
    if (m_circleWave && m_player->isVisible())
        m_circleWave->setPosition(m_player->getPosition());
}

void PreviewLayer::spawnPlayer(float) {
    if (m_animation) {
        m_animation->end();
        m_animation = nullptr;
    }
    
    scheduleOnce(schedule_selector(PreviewLayer::playerDied), m_time);
    
    m_player->setPosition({m_size.width / 2.f - 100, 95.5f});
    m_player->runAction(CCMoveTo::create(m_time, {m_spike->getPositionX() - 17.5f, 95.5f}));
    m_player->runAction(CCSequence::create(CCBlink::create(0.4f, 4), CCShow::create(), nullptr));
    
    if (m_circleWave)
        m_circleWave->release();
    
    m_circleWave = CCCircleWave::create(70.0,2.0,0.3,true,true);
    m_circleWave->m_color.r = 255;
    m_circleWave->m_color.g = 255;
    m_circleWave->m_color.b = 255;
    m_circleWave->m_circleMode = CircleMode::Outline;
    m_circleWave->retain();
    m_circleWave->setPosition({m_player->getPosition()});
    
    m_mainLayer->addChild(m_circleWave);
}

void PreviewLayer::playerDied(float) {
    playDeathEffect();
    
    Anim anim = Utils::getSelectedAnimationEnum();
        
    const DeathAnimation& animation = Utils::getSelectedAnimation(anim);
        
    while (anim == Anim::Random || anim == Anim::None)
        anim = static_cast<Anim>(Utils::getRandomInt(1, animations.size()));
    
    m_speed = Utils::getSpeedValue(Utils::getSettingFloat(animation.id, "speed"));
    m_duration = Utils::getSelectedAnimation(anim).duration / m_speed;
    m_animation = Utils::createAnimation(anim, {m_mainLayer, nullptr, this, m_speed});
      
    if (!m_animation) return;
    
    m_animation->start();
    m_animation->setZOrder(10);
    
    scheduleOnce(schedule_selector(PreviewLayer::spawnPlayer), m_duration);
}

void PreviewLayer::playDeathEffect() {
    if (Utils::getSelectedAnimation().stopDeathEffect) return;
    
    FMODAudioEngine::get()->playEffect("explode_11.ogg", 1.f, 0.f, 0.5f);
    
    CCParticleSystemQuad* particles = CCParticleSystemQuad::create("explodeEffect.plist", false);
    particles->setScale(0.7f);    
    particles->setPosition(m_player->getPosition());
    particles->setStartColor(ccc4FFromccc3B(m_mainColor));
    particles->resetSystem();
    
    m_mainLayer->addChild(particles);
    
    CCCircleWave* circle = CCCircleWave::create(10.f, 90.f, 0.5f, false, true);
    circle->setPosition(m_player->getPosition());
    
    circle->m_color.r = m_mainColor.r;
    circle->m_color.g = m_mainColor.g;
    circle->m_color.b = m_mainColor.b;
    
    m_mainLayer->addChild(circle, 0);
    
    m_player->setVisible(false);
}

void PreviewLayer::onClose(CCObject*) {
    geode::Popup<>::onClose(nullptr);
    
    FMODAudioEngine::get()->stopAllEffects();
    
    if (m_circleWave)
        m_circleWave->release();
}

bool PreviewLayer::setup() {
    m_noElasticity = true;
    
    m_bgSprite->setVisible(false);
    m_closeBtn->setVisible(false);
    
    setOpacity(0);
    runAction(CCEaseInOut::create(CCFadeTo::create(0.22f, 200), 2.f));
    
    CCSprite* spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    spr->setScale(0.8f);
    
    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(PreviewLayer::onClose));
    btn->setPosition({20, m_size.height - 20});
    
    m_buttonMenu->addChild(btn);
    m_buttonMenu->setZOrder(11);
    
    CCSprite* line = CCSprite::createWithSpriteFrameName("floorLine_01_001.png");
    line->setOpacity(0);
    line->setPosition({m_size.width / 2.f, 80});
    line->runAction(CCEaseInOut::create(CCFadeTo::create(0.22f, 255), 2.f));
    
    m_mainLayer->addChild(line, 2);
    
    m_spike = CCSprite::createWithSpriteFrameName("spike_01_001.png");
    m_spike->setOpacity(0);
    m_spike->setPosition({m_size.width / 2.f + 100, 80});
    m_spike->setAnchorPoint({0.5f, 0});
    m_spike->runAction(CCEaseInOut::create(CCFadeTo::create(0.22f, 255), 2.f));
    
    m_mainLayer->addChild(m_spike, 1);
    
    GameManager* gm = GameManager::get();
    
    m_mainColor = gm->colorForIdx(gm->getPlayerColor());
    
    m_player = SimplePlayer::create(gm->getPlayerFrame());
    m_player->m_hasGlowOutline = gm->getPlayerGlow();
    m_player->setVisible(false);
    m_player->setPosition({m_size.width / 2.f - 100, 95.5f});
    m_player->setColors(m_mainColor, gm->colorForIdx(gm->getPlayerColor2()));
    
    if (m_player->m_hasGlowOutline)
        m_player->enableCustomGlowColor(gm->colorForIdx(gm->getPlayerGlowColor()));
    else
        m_player->disableCustomGlowColor();
        
    m_player->updateColors();
    
    m_mainLayer->addChild(m_player);
    
    m_time = (m_spike->getPositionX() - m_player->getPositionX()) * 0.003625f;
        
    schedule(schedule_selector(PreviewLayer::updateCircle), 0, kCCRepeatForever, 0);
    scheduleOnce(schedule_selector(PreviewLayer::spawnPlayer), 0.2f);
    
    return true;
}

CCNodeRGBA* PreviewLayer::getPlayer() {
    return m_player;
}