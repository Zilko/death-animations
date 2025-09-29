#include "../Other/Utils.hpp"

#include "AnimationSettingsLayer.hpp"
#include "ToggleSetting.hpp"
#include "SpeedSetting.hpp"
#include "PercentSetting.hpp"
#include "SelectSetting.hpp"
#include "AmongUsSetting.hpp"
#include "FolderSetting.hpp"
#include "ScaleSetting.hpp"
#include "TextSetting.hpp"
#include "PercentToggleSetting.hpp"
#include "MonologueSetting.hpp"
#include "SpeechBubbleSetting.hpp"

AnimationSettingsLayer::AnimationSettingsLayer(const DeathAnimation& animation) {
    m_animation = animation;
}

AnimationSettingsLayer* AnimationSettingsLayer::create(const DeathAnimation& animation) {
    AnimationSettingsLayer* ret = new AnimationSettingsLayer(animation);
    
    if (ret->initAnchored(300, 200)) {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}

void AnimationSettingsLayer::onRestore(CCObject*) {
    geode::createQuickPopup(
        "Warning",
        "Restore all settings?",
        "Cancel", "Ok",
        [this](auto, bool btn2) {
            if (!btn2) return;
            
            Utils::setDefaults(m_animation.id);
            
            onClose(nullptr);
            
            AnimationSettingsLayer* layer = AnimationSettingsLayer::create(m_animation);
            layer->m_noElasticity = true;
            layer->show();
        }
    );
}

bool AnimationSettingsLayer::setup() {
    setTitle("Animation Settings");
    
    m_title->setPositionY(m_title->getPositionY() + 2);
    
    CCScale9Sprite* bg = CCScale9Sprite::create("square02b_001.png");
    bg->setColor({0, 0, 0});
    bg->setOpacity(70);
    bg->setPosition({150, 153});
    bg->setScale(1.f / 2.15f);
    bg->setContentSize(ccp(270, 25) * 2.15f);
    
    m_mainLayer->addChild(bg);
    
    bg = CCScale9Sprite::create("square02b_001.png");
    bg->setColor({0, 0, 0});
    bg->setOpacity(70);
    bg->setPosition({150, 85});
    bg->setContentSize({270, 102});
    
    m_mainLayer->addChild(bg);
    
    ButtonSprite* btnSpr = ButtonSprite::create("Ok");
    btnSpr->setScale(0.6f);
    
    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(btnSpr, this, menu_selector(AnimationSettingsLayer::onClose));
    btn->setPosition({274, 19.5f});
    
    m_buttonMenu->addChild(btn);
    
    btnSpr = ButtonSprite::create("Restore");
    btnSpr->setScale(0.6f);
    
    btn = CCMenuItemSpriteExtra::create(btnSpr, this, menu_selector(AnimationSettingsLayer::onRestore));
    btn->setPosition({47, 19.5f});
    
    m_buttonMenu->addChild(btn);
    
    CCLabelBMFont* lbl = CCLabelBMFont::create(m_animation.name.c_str(), "bigFont.fnt");
    lbl->setOpacity(220);
    lbl->setScale(0.325f);
    lbl->setAnchorPoint({0, 0.5f});
    lbl->setPosition({23, 153});
    lbl->limitLabelWidth(200, lbl->getScale(), 0.0001f);
    
    m_mainLayer->addChild(lbl);
    
    m_durationLabel = CCLabelBMFont::create("0.5s", "bigFont.fnt");
    m_durationLabel->setOpacity(220);
    m_durationLabel->setScale(0.325f);
    m_durationLabel->setAnchorPoint({1, 0.5f});
    m_durationLabel->setPosition({277, 153});
    
    m_mainLayer->addChild(m_durationLabel);
    
    m_durationSprite = CCSprite::createWithSpriteFrameName("GJ_timeIcon_001.png");
    m_durationSprite->setScale(0.55f);
    m_durationSprite->setPosition({245, 153});
    
    m_mainLayer->addChild(m_durationSprite);
    
    ScrollLayer* scroll = ScrollLayer::create({270, 102});
	scroll->setPosition(ccp(150, 85) - ccp(270, 102) / 2.f);
    m_mainLayer->addChild(scroll);
    
    std::vector<AnimationSetting> settings;
    
    for (const AnimationSetting& setting : defaultSettings)
        settings.push_back(setting);
    
    if (extraSettings.contains(m_animation.id))
        for (const AnimationSetting& setting : extraSettings.at(m_animation.id))
            settings.push_back(setting);
    
    float separation = 25.f;
    float y = (static_cast<int>(settings.size())
        - (blockedSettings.contains(m_animation.id) ? static_cast<int>(blockedSettings.at(m_animation.id).size()) : 0))
        * separation + 15;
    
    scroll->m_contentLayer->setContentHeight(y);
    
    CCMenu* menu = CCMenu::create();
    menu->setAnchorPoint({0, 0});
    menu->setPosition({0, 0});
    menu->setContentSize(scroll->m_contentLayer->getContentSize());
    
    scroll->m_contentLayer->addChild(menu);
    
    y -= 15;
    
    for (const AnimationSetting& setting : settings) {
        BaseSetting* settingNode = nullptr;

        if (blockedSettings.contains(m_animation.id))
            if (blockedSettings.at(m_animation.id).contains(setting.id))
                continue;

        switch (setting.type) {
            case SettingType::Toggle: settingNode = ToggleSetting::create(this, setting, m_animation, y); break;
            case SettingType::Speed: settingNode = SpeedSetting::create(this, setting, m_animation, y); break;
            case SettingType::Percent: settingNode = PercentSetting::create(this, setting, m_animation, y); break;
            case SettingType::Select: settingNode = SelectSetting::create(this, setting, m_animation, y); break;
            case SettingType::AmongUsColor: settingNode = AmongUsSetting::create(this, setting, m_animation, y); break;
            case SettingType::Folder: settingNode = FolderSetting::create(this, setting, m_animation, y); break;
            case SettingType::Scale: settingNode = ScaleSetting::create(this, setting, m_animation, y); break;
            case SettingType::Text: settingNode = TextSetting::create(this, setting, m_animation, y); break;
            case SettingType::PercentToggle: settingNode = PercentToggleSetting::create(this, setting, m_animation, y); break;
            case SettingType::MonologueSelect: settingNode = MonologueSetting::create(this, setting, m_animation, y); break;
            case SettingType::SpeechBubbleSelect: settingNode = SpeechBubbleSetting::create(this, setting, m_animation, y); break;
            default: break;
        }

        if (settingNode)
            scroll->m_contentLayer->addChild(settingNode);
        
        y -= separation;
    }
    
    scroll->scrollToTop();
    
    Scrollbar* scrollbar = Scrollbar::create(scroll);
    scrollbar->setScale(0.85f);
    scrollbar->setAnchorPoint({0, 0});
    scrollbar->setPosition({273, 42.5f});
    
    m_mainLayer->addChild(scrollbar);
        
    return true;
}

CCLabelBMFont* AnimationSettingsLayer::getDurationLabel() {
    return m_durationLabel;
}

CCSprite* AnimationSettingsLayer::getDurationSprite() {
    return m_durationSprite;
}