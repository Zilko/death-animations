#include "BaseSetting.hpp"

void BaseSetting::onInfo(CCObject*) {
    FLAlertLayer::create(
        m_animation.name.c_str(),
        m_setting.description,
        "Ok"
    )->show();
}

void BaseSetting::setInfoButtonPos(CCNode* node) {
    m_infoBtn->setPositionX(node->getPositionX() + node->getContentWidth() * node->getScaleX() * (1 - node->getAnchorPoint().x) + 10);
}

void BaseSetting::init(float y) {
    setContentSize({270, 20});
    setAnchorPoint({0, 0.5f});
    setPositionY(y);

    m_menu = CCMenu::create();
    m_menu->setPosition({0, 0});
    
    addChild(m_menu);

    m_nameLabel = CCLabelBMFont::create(m_setting.name.c_str(), "bigFont.fnt");
    m_nameLabel->setOpacity(220);
    m_nameLabel->setScale(0.325f);
    m_nameLabel->setAnchorPoint({0, 0.5f});
    m_nameLabel->setPosition({11, 10});
    
    addChild(m_nameLabel);

    CCSprite* spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
    spr->setScale(0.375f);
    
    m_infoBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(BaseSetting::onInfo));
    m_infoBtn->setPositionY(10);
    m_infoBtn->setVisible(!m_setting.description.empty());
    
    m_menu->addChild(m_infoBtn);
}