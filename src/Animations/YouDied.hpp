#include "BaseAnimation.hpp"

class YouDied : public BaseAnimation {

private:

    CCLayerColor* m_bg = nullptr;
    CCSprite* m_banner = nullptr;
    CCSprite* m_title = nullptr;
    
public:

    DEFINE_CREATE(YouDied)
    
    void start() override {
        BaseAnimation::start();
        
        Utils::playSound(Anim::YouDied, "you-died.mp3", m_speed, 3000, 5000);
        
        CCSize winSize = CCDirector::get()->getWinSize();
        
        m_bg = CCLayerColor::create({0, 0, 0, 0}, winSize.width, winSize.height);
        m_bg->setOpacity(0);
        
        m_banner = CCSprite::create("bg-you-died.png"_spr);
        m_banner->setOpacity(0);
        m_banner->setPosition(winSize / 2.f + ccp(0, -7));
        m_banner->setScaleX(winSize.width / m_banner->getContentSize().width);
        
        m_title = CCSprite::create("text-you-died.png"_spr);
        m_title->setOpacity(0);
        m_title->setScale(1.14f);
        m_title->setPosition(m_banner->getPosition());
        
        addChild(m_bg);
        addChild(m_banner);
        addChild(m_title);
        
        scheduleOnce(schedule_selector(YouDied::firstStep), 0.7f / m_speed);
    }
    
    void firstStep(float) {
        m_banner->runAction(CCFadeTo::create(0.285f / m_speed, 220));
        m_bg->runAction(CCFadeTo::create(0.285f / m_speed, 50));
        m_title->runAction(CCEaseIn::create(CCFadeTo::create(0.3f / m_speed, 10), 2.f));
    
        scheduleOnce(schedule_selector(YouDied::secondStep), 0.31f / m_speed);
    }
    
    void secondStep(float) {
        m_title->setOpacity(25);
        m_title->runAction(CCEaseIn::create(CCFadeTo::create(0.35f / m_speed, 232), 1.f));
        m_title->runAction(CCScaleTo::create(1.f / m_speed, 1.24f));
        m_bg->runAction(CCEaseOut::create(CCFadeTo::create(1.25f / m_speed, 90), 1.f));
    
        scheduleOnce(schedule_selector(YouDied::thirdStep), 1.4f / m_speed);
    }
    
    void thirdStep(float) {
        m_title->runAction(CCEaseOut::create(CCScaleTo::create(1.9f / m_speed, 1.35f), 1.f));
    
        scheduleOnce(schedule_selector(YouDied::fourthStep), 0.55f / m_speed);
    }
    
    void fourthStep(float) {
        m_banner->runAction(CCFadeTo::create(1.15f / m_speed, 0));
        m_bg->runAction(CCFadeTo::create(1.15f / m_speed, 0));
        m_title->runAction(CCFadeTo::create(0.35f / m_speed, 0));
    }
    
    void end() override {
        BaseAnimation::end();
    }
    
};