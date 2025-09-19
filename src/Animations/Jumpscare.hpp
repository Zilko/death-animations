#include "BaseAnimation.hpp"

class Jumpscare : public BaseAnimation {

private:

    CCLayerColor* m_backgroundLayer = nullptr;

    CCSprite* m_jumpcare = nullptr;
    
    void fadeOut(float) {
        m_backgroundLayer->runAction(CCFadeOut::create(2.5f));

        if (m_jumpcare)
            m_jumpcare->runAction(CCFadeOut::create(2.5f));
    }

    ANIMATION_CTOR_CREATE(Jumpscare) {}
    
public:

    void start() override {
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

        m_backgroundLayer = CCLayerColor::create({0, 0, 0, 255});
        
        addChild(m_backgroundLayer);
        
        m_jumpcare = CCSprite::create(utils::string::pathToString(image).c_str());

        if (!m_jumpcare) return m_backgroundLayer->setVisible(false);

        float scale = Utils::getSettingBool(Anim::Jumpscare, "fill-screen")
            ? std::max(m_size.width / m_jumpcare->getContentWidth(), m_size.height / m_jumpcare->getContentHeight())
            : std::min(m_size.width / m_jumpcare->getContentWidth(), m_size.height / m_jumpcare->getContentHeight());
        
        m_jumpcare->setScale(scale * 0.35f);
        m_jumpcare->setPosition(m_size / 2.f);

        if (!Utils::getSettingBool(Anim::Jumpscare, "static"))
            m_jumpcare->runAction(CCBlink::create(0.5f / m_speed, 10));
        else
            m_jumpcare->setScale(scale);

        if (!Utils::getSettingBool(Anim::Jumpscare, "disable-flashing"))
            m_jumpcare->runAction(CCScaleTo::create(0.2f / m_speed, scale));
        

        addChild(m_jumpcare);

        scheduleOnce(schedule_selector(Jumpscare::fadeOut), 1.f);
    }
};