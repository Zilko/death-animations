#include "AmongUsSetting.hpp"

#include "../Other/Utils.hpp"

void AmongUsSetting::onColorPicker(CCObject* btn) {
    m_currentColor = static_cast<CCNode*>(btn)->getTag();
    
    ColorPickPopup* popup = ColorPickPopup::create(
        (m_currentColor == 1 ? m_colorSprite1 : m_colorSprite2)->getColor()
    );
    popup->setDelegate(this);
    popup->show();
}

void AmongUsSetting::updateColor(const ccColor4B& color) {
    (m_currentColor == 1 ? m_colorSprite1 : m_colorSprite2)->setColor({ color.r, color.g, color.b });
    
    std::string n = std::to_string(m_currentColor);
    
    Utils::saveSetting(m_animation.id, "r" + n, static_cast<float>(color.r));
    Utils::saveSetting(m_animation.id, "g" + n, static_cast<float>(color.g));
    Utils::saveSetting(m_animation.id, "b" + n, static_cast<float>(color.b));
}

void AmongUsSetting::onSelectorArrow(CCObject* obj) {
    SelectSetting::onSelectorArrow(obj);

    m_colorButton1->setVisible(static_cast<int>(Utils::getSettingFloat(m_animation.id, "colors")) == 1);
    m_colorButton2->setVisible(static_cast<int>(Utils::getSettingFloat(m_animation.id, "colors")) == 1);
    
    setInfoButtonPos(m_colorButton2->isVisible() ? m_colorButton2 : m_rightArrow);
}

void AmongUsSetting::init(float y) {
    SelectSetting::init(y);

    m_colorSprite1 = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    m_colorSprite1->setScale(0.5f);
    m_colorSprite1->setColor({
        static_cast<GLubyte>(Utils::getSettingFloat(m_animation.id, "r1")),
        static_cast<GLubyte>(Utils::getSettingFloat(m_animation.id, "g1")),
        static_cast<GLubyte>(Utils::getSettingFloat(m_animation.id, "b1"))
    });
    
    m_colorButton1 = CCMenuItemSpriteExtra::create(m_colorSprite1, this, menu_selector(AmongUsSetting::onColorPicker));
    m_colorButton1->setPosition({m_rightArrow->getPositionX() + 22, 10});
    m_colorButton1->setTag(1);
    
    m_menu->addChild(m_colorButton1);
    
    m_colorSprite2 = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    m_colorSprite2->setScale(0.5f);
    m_colorSprite2->setColor({
        static_cast<GLubyte>(Utils::getSettingFloat(m_animation.id, "r2")),
        static_cast<GLubyte>(Utils::getSettingFloat(m_animation.id, "g2")),
        static_cast<GLubyte>(Utils::getSettingFloat(m_animation.id, "b2"))
    });
    
    m_colorButton2 = CCMenuItemSpriteExtra::create(m_colorSprite2, this, menu_selector(AmongUsSetting::onColorPicker));
    m_colorButton2->setPosition({m_rightArrow->getPositionX() + 46, 10});
    m_colorButton2->setTag(2);
    
    m_menu->addChild(m_colorButton2);
    
    m_colorButton1->setVisible(static_cast<int>(Utils::getSettingFloat(m_animation.id, "colors")) == 1);
    m_colorButton2->setVisible(m_colorButton1->isVisible());

    setInfoButtonPos(m_colorButton2->isVisible() ? m_colorButton2 : m_rightArrow);
}