#include "ToggleSetting.hpp"

#include "../Other/Utils.hpp"

void ToggleSetting::onToggle(CCObject*) {
    Utils::saveSetting(m_animation.id, m_setting.id, !m_toggle->isToggled());
}

void ToggleSetting::init(float y) {
    BaseSetting::init(y);

    m_nameLabel->setPositionX(31);
    
    setInfoButtonPos(m_nameLabel);

    m_toggle = CCMenuItemToggler::create(
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
        this,
        menu_selector(ToggleSetting::onToggle)
    );
    m_toggle->setScale(0.5f);
    m_toggle->setPosition({19, 10});
    m_toggle->toggle(Utils::getSettingBool(m_animation.id, m_setting.id));

    m_menu->addChild(m_toggle);
}