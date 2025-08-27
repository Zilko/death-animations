#include "JumpscareSetting.hpp"

#include "../Other/Utils.hpp"

void JumpscareSetting::onFolder(CCObject* obj) {
    std::filesystem::path folder = Mod::get()->getSaveDir()
        / (static_cast<CCNode*>(obj)->getTag()
        ? "jumpscare-images" : "jumpscare-sounds");

    if (!std::filesystem::exists(folder))
        if (utils::file::createDirectoryAll(folder).isErr())
            return;

    file::openFolder(folder);
}

void JumpscareSetting::init(float y) {
    BaseSetting::init(y);
    
    ButtonSprite* spr = ButtonSprite::create("Open Folder");
    spr->setScale(0.5f);

    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(JumpscareSetting::onFolder));
    btn->setPosition({142, 10});
    btn->setTag(m_setting.id == "images-folder");

    m_menu->addChild(btn);

    setInfoButtonPos(btn);
}