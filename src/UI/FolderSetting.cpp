#include "FolderSetting.hpp"

#include "../Other/Utils.hpp"

void FolderSetting::onFolder(CCObject* obj) {
    if (!std::filesystem::exists(m_setting.folder))
        if (utils::file::createDirectoryAll(m_setting.folder).isErr())
            return;

    file::openFolder(m_setting.folder);
}

void FolderSetting::init(float y) {
    BaseSetting::init(y);
    
    ButtonSprite* spr = ButtonSprite::create("Open Folder");
    spr->setScale(0.5f);

    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FolderSetting::onFolder));
    btn->setPosition({
        m_nameLabel->getPositionX() + m_nameLabel->getScale() * m_nameLabel->getContentWidth() + 7.5f + btn->getContentWidth() / 2.f,
        10
    });

    m_menu->addChild(btn);

    setInfoButtonPos(btn);
}