#include "BaseAnimation.hpp"

constexpr std::array audioDelays = { 0.22f, 0.15f };

class AmongUs : public BaseAnimation {

private:

    CCLayerColor* m_redLayer = nullptr;
    
    CCSprite* m_bg = nullptr;
    CCSprite* m_animationSprite = nullptr;
    
    int m_currentFrame = 0;
    int m_animation = 2;
    
public:

    DEFINE_CREATE(AmongUs)

    void start() override {
        BaseAnimation::start();
        
        setZOrder(1000);
        
        Utils::playSound(Anim::AmongUs, "scary-among-us.ogg", m_speed, 0.5f);

        CCSize winSize = CCDirector::get()->getWinSize();
        
        m_bg = CCSprite::create("bg-among-us.png"_spr);
        m_bg->setScaleX(2.98f);
        m_bg->setScaleY(0.88f);
        m_bg->setRotation(-25);
        m_bg->setPosition(winSize / 2.f);
        
        addChild(m_bg);
        
        m_redLayer = CCLayerColor::create({255, 1, 0, 255}, winSize.width, winSize.height);
        
        addChild(m_redLayer);
        
        scheduleOnce(schedule_selector(AmongUs::firstStep), 0.09f / m_speed);
    }
    
    void firstStep(float) {
        m_redLayer->setVisible(false);
        
        scheduleOnce(schedule_selector(AmongUs::secondStep), 0.09f / m_speed);
    }
    
    void secondStep(float) {
        m_bg->setScaleY(1.47f);
        m_bg->setRotation(15);
        
        scheduleOnce(schedule_selector(AmongUs::thirdStep), 0.09f / m_speed);
    }
    
    void thirdStep(float) {
        m_bg->setScaleY(2.98f);
        m_bg->setRotation(0);
        
        
        // m_animation = Utils::getRandomInt(1, 2);
        
        CCSize winSize = CCDirector::get()->getWinSize();
        CCArray* animFrames = CCArray::create();
        CCSpriteFrameCache* cache = CCSpriteFrameCache::get();
        CCSpriteFrame* lastValidFrame = nullptr;
        cache->addSpriteFramesWithFile(fmt::format("among-us-death-{}.plist"_spr, m_animation).c_str());
        
        for (int i = 1; i <= 115; ++i) {
            CCSpriteFrame* frame = cache->spriteFrameByName((fmt::format("among-us-death-{}-{}.png"_spr, m_animation, i)).c_str());
    
            if (frame)
                lastValidFrame = frame;
            
            if (lastValidFrame)
                animFrames->addObject(lastValidFrame);
        }
        
        m_animationSprite = CCSprite::createWithSpriteFrameName(fmt::format("among-us-death-{}-1.png"_spr, m_animation).c_str());
        m_animationSprite->setAnchorPoint({0, 0});
        m_animationSprite->setScale(winSize.width / m_animationSprite->getContentWidth());
        m_animationSprite->runAction(
            CCSequence::create(
                CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, 1.f / 60.f / m_speed)),
                CCDelayTime::create(0.09f / m_speed),
                CCCallFunc::create(this, callfunc_selector(AmongUs::fourthStep)),
                nullptr
            )
        );
        
        addChild(m_animationSprite);
        scheduleOnce(schedule_selector(AmongUs::playSound), audioDelays[m_animation - 1] / m_speed);
    }
    
    void fourthStep() {
        m_animationSprite->setVisible(false);
        m_bg->runAction(CCScaleTo::create(0.23333f / m_speed, 2.9f, 0.f));
    }
    
    void playSound(float) {
        Utils::playSound(Anim::AmongUs, fmt::format("death-{}-among-us.ogg", m_animation), m_speed, 0.5f);
    }
    
    void end() override {
        BaseAnimation::end();
    }

};