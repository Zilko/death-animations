#include "CelesteColorSetting.hpp"

#include "../Other/Utils.hpp"

void CelesteColorSetting::onColorPicker(CCObject* btn) {
    ColorPickPopup* popup = ColorPickPopup::create(m_colorSprite->getColor());
    popup->setDelegate(this);
    popup->show();
}

void CelesteColorSetting::updateColor(const ccColor4B& color) {
    m_colorSprite->setColor({ color.r, color.g, color.b });

    Utils::saveSetting(m_animation.id, m_setting.id + "-r", static_cast<float>(color.r));
    Utils::saveSetting(m_animation.id, m_setting.id + "-g", static_cast<float>(color.g));
    Utils::saveSetting(m_animation.id, m_setting.id + "-b", static_cast<float>(color.b));
}

void CelesteColorSetting::init(float y) {
    BaseSetting::init(y);

    float xOffset = m_nameLabel->getContentWidth() * m_nameLabel->getScale();

    m_colorSprite = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    m_colorSprite->setScale(0.5f);
    m_colorSprite->setColor({
        static_cast<GLubyte>(Utils::getSettingFloat(m_animation.id, m_setting.id + "-r")),
        static_cast<GLubyte>(Utils::getSettingFloat(m_animation.id, m_setting.id + "-g")),
        static_cast<GLubyte>(Utils::getSettingFloat(m_animation.id, m_setting.id + "-b"))
    });
    
    m_colorButton = CCMenuItemSpriteExtra::create(m_colorSprite, this, menu_selector(CelesteColorSetting::onColorPicker));
    m_colorButton->setPosition({xOffset + 25, 10});
    
    m_menu->addChild(m_colorButton);

    setInfoButtonPos(m_colorButton);
}
