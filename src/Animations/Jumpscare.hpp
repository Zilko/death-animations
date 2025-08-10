#include "BaseAnimation.hpp"

class Jumpscare : public BaseAnimation {

public:
    
    DEFINE_CREATE(Jumpscare)

    void start() override {
        BaseAnimation::start();

        std::filesystem::path sound = Utils::getRandomFile(Mod::get()->getSaveDir() / "jumpscare-sounds", { ".mp3", ".ogg", ".wav" });

        if (Utils::getSettingBool(Anim::Jumpscare, "ignore-volume"))
            Utils::playSoundManual(Anim::Jumpscare, m_speed, 1.f, sound);
        else
            Utils::playSound(Anim::Jumpscare, m_speed, 1.f, sound);

        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255}, m_size.width, m_size.height);
        layer->setAnchorPoint({0, 0});

        addChild(layer);
        
        CCSprite* spr = CCSprite::create(Utils::getRandomFile(Mod::get()->getSaveDir() / "jumpscare-images", { ".png", ".jpg" }).string().c_str());

        float scale = std::max(m_size.width / spr->getContentWidth(), m_size.height / spr->getContentHeight());
        
        spr->setScale(scale * 0.35f);
        spr->setPosition(m_size / 2.f);

        if (!Utils::getSettingBool(Anim::Jumpscare, "static")) {
            spr->runAction(CCBlink::create(0.5f / m_speed, 10));
            spr->runAction(CCScaleTo::create(0.2f / m_speed, scale));
        } else
            spr->setScale(scale);

        addChild(spr);
    }
    };