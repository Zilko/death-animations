#include "../Utils/Utils.hpp"

#include "AnimationSettingsLayer.hpp"

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

void AnimationSettingsLayer::updateDuration(CCObject* obj) {
    SliderThumb* slider = static_cast<SliderThumb*>(obj);
    std::string id = m_sliderIDs.at(slider);
    
    Utils::saveSetting(m_animation.name, id, slider->getValue());
    
    if (id == "speed") {
        float value = Utils::getSpeedValue(slider->getValue());

        m_speedLabel->setString(fmt::format("{:.2f}x", value).c_str());
        m_durationLabel->setString(fmt::format("{:.2f}s", m_animation.duration / value).c_str());
        m_durationLabel->limitLabelWidth(25, m_durationLabel->getScale(), 0.0001f);
        m_durationSprite->setPositionX(
            m_durationLabel->getPositionX()
            - m_durationLabel->getContentWidth()
            * m_durationLabel->getScale()
            - m_durationSprite->getContentWidth()
            * m_durationSprite->getScale() / 2.f - 1.f
        );
    }
}

void AnimationSettingsLayer::onToggle(CCObject* obj) {
    CCMenuItemToggler* toggle = static_cast<CCMenuItemToggler*>(obj);
    if (m_toggleIDs.contains(toggle))
        Utils::saveSetting(m_animation.name, m_toggleIDs.at(toggle), !toggle->isToggled());
}

void AnimationSettingsLayer::onRestore(CCObject*) {
    geode::createQuickPopup(
        "Warning",
        "Restore all settings?",
        "Cancel", "Ok",
        [this](auto, bool btn2) {
            if (!btn2) return;
            
            Utils::setDefaults(m_animation.name);
            
            onClose(nullptr);
            
            AnimationSettingsLayer* layer = AnimationSettingsLayer::create(m_animation);
            layer->m_noElasticity = true;
            layer->show();
        }
    );
}

void AnimationSettingsLayer::textChanged(CCTextInputNode* node) {
    if (m_inputIDs.contains(node)) {
        std::string id = m_inputIDs.at(node);
        
        int number = numFromString<int>(node->getString()).unwrapOr(0);
        
        if (id == "only-after") {
            number = std::clamp(number, 0, 100);
            node->setDelegate(nullptr);
            node->setString(std::to_string(number).c_str());
            node->setDelegate(this);
        }
        
        Utils::saveSetting(m_animation.name, id, number / 1.f);
    }
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
    
    float separation = 25.f;
    float y = static_cast<int>(defaultSettings.size()) * separation + 15;
    
    std::vector<AnimationSetting> extras;
    
    if (extraSettings.contains(m_animation.id)) {
        extras = extraSettings.at(m_animation.id);
        y += static_cast<int>(extras.size()) * 22.f;
    }
    
    scroll->m_contentLayer->setContentHeight(y);
    
    CCMenu* menu = CCMenu::create();
    menu->setAnchorPoint({0, 0});
    menu->setPosition({0, 0});
    menu->setContentSize(scroll->m_contentLayer->getContentSize());
    
    scroll->m_contentLayer->addChild(menu);
    
    y -= 15;
    
    for (const AnimationSetting& setting : defaultSettings) {
        bool isToggle = setting.type == SettingType::Toggle;
        
        CCLabelBMFont* lbl = CCLabelBMFont::create(setting.name.c_str(), "bigFont.fnt");
        lbl->setOpacity(220);
        lbl->setScale(0.325f);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({isToggle ? 31.f : 11.f, y});
        
        scroll->m_contentLayer->addChild(lbl);
        
        switch (setting.type) {
            case SettingType::Toggle: {
                CCMenuItemToggler* toggle = CCMenuItemToggler::create(
                    CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
                    CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
                    this,
                    menu_selector(AnimationSettingsLayer::onToggle)
                );
                toggle->setScale(0.5f);
                toggle->setPosition({19, y});
                toggle->toggle(Utils::getSettingBool(m_animation.name, setting.id, setting.id == "play-sound-effects"));
                
                menu->addChild(toggle);
                
                m_toggleIDs[toggle] = setting.id;
                break;
            }
            case SettingType::Speed: {
                Slider* slider = Slider::create(this, menu_selector(AnimationSettingsLayer::updateDuration));
               	slider->setPosition({87, y});
               	slider->setAnchorPoint({ 0.f, 0.f });
               	slider->setScale(0.37f);
               	slider->setValue(1.f);
               	scroll->m_contentLayer->addChild(slider);
                
                m_speedLabel = CCLabelBMFont::create("1.2x", "bigFont.fnt");
                m_speedLabel->setOpacity(190);
                m_speedLabel->setScale(0.25f);
                m_speedLabel->setAnchorPoint({0, 0.5f});
                m_speedLabel->setPosition({134, y});
                
                scroll->m_contentLayer->addChild(m_speedLabel);
                
                m_sliderIDs[slider->getThumb()] = setting.id;
                
                slider->setValue(Utils::getSettingFloat(m_animation.name, setting.id, 0.230769f));
                updateDuration(slider->getThumb());
                
                break;
            }
            case SettingType::Percent: {
                TextInput* input = TextInput::create(70, "%");
               	input->setPosition({96, y});
               	input->setScale(0.525f);
               	input->getInputNode()->setDelegate(this);
               	input->getInputNode()->setAllowedChars("0123456789");
               	input->setString(std::to_string(static_cast<int>(Utils::getSettingFloat(m_animation.name, setting.id, 0.f))).c_str());
                
                scroll->m_contentLayer->addChild(input);
                
                CCLabelBMFont* lbl = CCLabelBMFont::create("%", "bigFont.fnt");
                lbl->setOpacity(220);
                lbl->setScale(0.325f);
                lbl->setPosition({125, y});
                
                scroll->m_contentLayer->addChild(lbl);
                
                m_inputIDs[input->getInputNode()] = setting.id;
                
                break;
            }
            default: {
                y += separation;
                break;
            };
        }
        
        y -= separation;
    }
    
    for (const AnimationSetting& setting : extras) {
        CCLabelBMFont* lbl = CCLabelBMFont::create(setting.name.c_str(), "bigFont.fnt");
        lbl->setOpacity(220);
        lbl->setScale(0.325f);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({31, y});
        
        scroll->m_contentLayer->addChild(lbl);
        
        if (setting.type == SettingType::Toggle) {
            CCMenuItemToggler* toggle = CCMenuItemToggler::create(
                CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
                CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
                this,
                nullptr
            );
            toggle->setScale(0.5f);
            toggle->setPosition({19, y});
            
            menu->addChild(toggle);
        }
        
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