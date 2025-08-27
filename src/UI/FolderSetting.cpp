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
    btn->setPosition({142, 10});
    btn->setTag(m_setting.id == "images-folder");

    m_menu->addChild(btn);

    setInfoButtonPos(btn);
}