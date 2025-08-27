#include "BaseAnimation.hpp"

class Jumpscare : public BaseAnimation {
    
    ANIMATION_CTOR_CREATE(Jumpscare)
    
public:

    void start() override {
        BaseAnimation::start();

        std::filesystem::path sound = Utils::getRandomFile(Mod::get()->getSaveDir() / "jumpscare-sounds", { ".mp3", ".ogg", ".wav" });
        std::filesystem::path image = Utils::getRandomFile(Mod::get()->getSaveDir() / "jumpscare-images", { ".png", ".jpg" });

        if (sound.empty())
            sound = Mod::get()->getResourcesDir() / "jumpscare.mp3";

        if (image.empty())
            image = Mod::get()->getResourcesDir() / "jumpscare.png";

        if (Utils::getSettingBool(Anim::Jumpscare, "ignore-volume"))
            Utils::playSoundManual(Anim::Jumpscare, m_speed, 1.f, sound);
        else
            Utils::playSound(Anim::Jumpscare, m_speed, 1.f, sound);

        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255}, m_size.width, m_size.height);
        
        addChild(layer);
        
        CCSprite* spr = CCSprite::create(utils::string::pathToString(image).c_str());

        if (!spr) return layer->setVisible(false);

        float scale = Utils::getSettingBool(Anim::Jumpscare, "fill-screen")
            ? std::max(m_size.width / spr->getContentWidth(), m_size.height / spr->getContentHeight())
            : std::min(m_size.width / spr->getContentWidth(), m_size.height / spr->getContentHeight());
        
        spr->setScale(scale * 0.35f);
        spr->setPosition(m_size / 2.f);

        if (!Utils::getSettingBool(Anim::Jumpscare, "static"))
            spr->runAction(CCBlink::create(0.5f / m_speed, 10));
        else
            spr->setScale(scale);

        if (!Utils::getSettingBool(Anim::Jumpscare, "disable-flashing"))
            spr->runAction(CCScaleTo::create(0.2f / m_speed, scale));
        

        addChild(spr);
    }
};