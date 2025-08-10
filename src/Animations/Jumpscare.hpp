#include "BaseAnimation.hpp"

class Jumpscare : public BaseAnimation {

public:
    
    DEFINE_CREATE(Jumpscare)

    void start() override {
        BaseAnimation::start();

        if (Utils::getSettingBool(Anim::Jumpscare, "ignore-volume"))
            Utils::playSoundManual(Anim::Jumpscare, "jumpscare.mp3", m_speed, 1.f);
        else
            Utils::playSound(Anim::Jumpscare, "jumpscare.mp3", m_speed, 1.f);

        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255}, m_size.width, m_size.height);
        layer->setAnchorPoint({0, 0});

        addChild(layer);
        
        CCSprite* spr = CCSprite::create("jumpscare.png"_spr);

        float scale = std::max(m_size.width / spr->getContentWidth(), m_size.height / spr->getContentHeight());
        
        spr->setScale(scale * 0.35f);
        spr->setPosition(m_size / 2.f);
        spr->runAction(CCBlink::create(0.5f / m_speed, 10));

        if (Utils::getSettingBool(Anim::Jumpscare, "flashing"))
            spr->runAction(CCScaleTo::create(0.2f / m_speed, scale));

        addChild(spr);
    }
    };