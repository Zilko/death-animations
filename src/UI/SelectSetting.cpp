#include "SelectSetting.hpp"

#include "../Other/Utils.hpp"

void SelectSetting::onSelectorArrow(CCObject* obj) {
    int currentElement = static_cast<int>(Utils::getSettingFloat(m_animation.id, m_setting.id));
    
    currentElement += static_cast<CCNode*>(obj)->getTag() == 11 ? 1 : -1;
    
    if (currentElement < 0) currentElement = m_setting.elements.size() - 1;
    if (currentElement >= static_cast<int>(m_setting.elements.size())) currentElement = 0;

    Utils::saveSetting(m_animation.id, m_setting.id, static_cast<float>(currentElement));
    
    m_label->setString(m_setting.elements[currentElement].c_str());
    m_label->limitLabelWidth(58.f, 0.35f, 0.0001f);
}

void SelectSetting::init(float y) {
    BaseSetting::init(y);

    float xOffset = m_nameLabel->getContentWidth() * m_nameLabel->getScale() - 60;
    
    CCScale9Sprite* bg = CCScale9Sprite::create("square02b_001.png");
    bg->setColor({0, 0, 0});
    bg->setOpacity(90);
    bg->setScale(0.275f);
    bg->setPosition({125 + xOffset, 10});
    bg->setContentSize({237, 69});
    
    addChild(bg);

    m_label = CCLabelBMFont::create(m_setting.elements[static_cast<int>(Utils::getSettingFloat(m_animation.id, m_setting.id))].c_str(), "bigFont.fnt");
    m_label->setPosition({125 + xOffset, 10});
    m_label->limitLabelWidth(58.f, 0.35f, 0.0001f);
    
    addChild(m_label);
    
    CCSprite* spr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    spr->setFlipX(true);
    spr->setScale(0.675f);
    
    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SelectSetting::onSelectorArrow));
    btn->setPosition({83 + xOffset, 10});
    
    m_menu->addChild(btn);
    
    spr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    spr->setScale(0.675f);
    
    m_rightArrow = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SelectSetting::onSelectorArrow));
    m_rightArrow->setTag(11);
    m_rightArrow->setPosition({167 + xOffset, 10});
    
    m_menu->addChild(m_rightArrow);
    
    setInfoButtonPos(m_rightArrow);
}