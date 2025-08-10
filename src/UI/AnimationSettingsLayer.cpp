#include "../Other/Utils.hpp"

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

void AnimationSettingsLayer::onSlider(CCObject* obj) {
    SliderThumb* slider = static_cast<SliderThumb*>(obj);
    std::string id = m_sliderIDs.at(slider);
    
    Utils::saveSetting(m_animation.id, id, slider->getValue());
    
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
        Utils::saveSetting(m_animation.id, m_toggleIDs.at(toggle), !toggle->isToggled());
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

void AnimationSettingsLayer::onSelectorArrow(CCObject* obj) {
    SelectorStuff selector = m_selectors.at(obj);
    
    int currentElement = static_cast<int>(Utils::getSettingFloat(m_animation.id, selector.id));
    
    currentElement += static_cast<CCNode*>(obj)->getTag() == 11 ? 1 : -1;
    
    if (currentElement < 0) currentElement = selector.elements.size() - 1;
    if (currentElement >= static_cast<int>(selector.elements.size())) currentElement = 0;

    Utils::saveSetting(m_animation.id, selector.id, static_cast<float>(currentElement));
    
    selector.label->setString(selector.elements[currentElement].c_str());
    selector.label->limitLabelWidth(58.f, 0.35f, 0.0001f);
    
    if (m_colorButton1) {
        m_colorButton1->setVisible(static_cast<int>(Utils::getSettingFloat(m_animation.id, "colors")) == 1);
        m_colorButton2->setVisible(static_cast<int>(Utils::getSettingFloat(m_animation.id, "colors")) == 1);
        
        CCNode* node = m_colorButton2->isVisible() ? m_colorButton2 : m_amogusArrow;
        m_amogusInfo->setPositionX(node->getPositionX() + node->getContentWidth() * node->getScaleX() * (1 - node->getAnchorPoint().x) + 10);
    }
}

void AnimationSettingsLayer::onColorPicker(CCObject* btn) {
    m_currentColor = static_cast<CCNode*>(btn)->getTag();
    
    ColorPickPopup* popup = ColorPickPopup::create(
        (m_currentColor == 1 ? m_colorSprite1 : m_colorSprite2)->getColor()
    );
    popup->setDelegate(this);
    popup->show();
}

void AnimationSettingsLayer::onInfo(CCObject* sender) {
    FLAlertLayer::create(
        m_descriptions.at(sender).first.c_str(),
        m_descriptions.at(sender).second,
        "Ok"
    )->show();
}

void AnimationSettingsLayer::onFolder(CCObject* sender) {
    std::filesystem::path folder = Mod::get()->getSaveDir()
        / (static_cast<CCNode*>(sender)->getTag()
        ? "jumpscare-images" : "jumpscare-sounds");

    if (!std::filesystem::exists(folder))
        if (utils::file::createDirectoryAll(folder).isErr())
            return;

    file::openFolder(folder);
}

void AnimationSettingsLayer::textChanged(CCTextInputNode* node) {
    if (m_inputIDs.contains(node)) {
        std::string id = m_inputIDs.at(node);
        
        int number = numFromString<int>(node->getString()).unwrapOr(0);
        
        if (m_percentInputs.contains(node)) {
            number = std::clamp(number, 0, 100);
            node->setDelegate(nullptr);
            node->setString(std::to_string(number).c_str());
            node->setDelegate(this);
        }
        
        Utils::saveSetting(m_animation.id, id, number / 1.f);
    }
}

void AnimationSettingsLayer::updateColor(const ccColor4B& color) {
    (m_currentColor == 1 ? m_colorSprite1 : m_colorSprite2)->setColor({ color.r, color.g, color.b });
    
    std::string n = std::to_string(m_currentColor);
    
    Utils::saveSetting(Anim::AmongUs, "r" + n, static_cast<float>(color.r));
    Utils::saveSetting(Anim::AmongUs, "g" + n, static_cast<float>(color.g));
    Utils::saveSetting(Anim::AmongUs, "b" + n, static_cast<float>(color.b));
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
    float y = static_cast<int>(settings.size()) * separation + 15;
    
    scroll->m_contentLayer->setContentHeight(y);
    
    CCMenu* menu = CCMenu::create();
    menu->setAnchorPoint({0, 0});
    menu->setPosition({0, 0});
    menu->setContentSize(scroll->m_contentLayer->getContentSize());
    
    scroll->m_contentLayer->addChild(menu);
    
    y -= 15;
    
    for (const AnimationSetting& setting : settings) {
        bool isToggle = setting.type == SettingType::Toggle;
        
        CCLabelBMFont* lbl = CCLabelBMFont::create(setting.name.c_str(), "bigFont.fnt");
        lbl->setOpacity(220);
        lbl->setScale(0.325f);
        lbl->setAnchorPoint({0, 0.5f});
        lbl->setPosition({isToggle ? 31.f : 11.f, y});
        
        scroll->m_contentLayer->addChild(lbl);
        
        CCNode* fartestNode = nullptr;
        
        switch (setting.type) {
            case SettingType::Toggle: fartestNode = addToggleSetting(y, setting, menu); break;
            case SettingType::Speed: fartestNode = addSpeedSetting(y, setting, scroll); break;
            case SettingType::Percent: fartestNode = addPercentSetting(y, setting, scroll); break;
            case SettingType::Select: fartestNode = addSelectSetting(y, setting, menu, scroll, lbl); break;
            case SettingType::AmongUsColor: fartestNode = addAmongUsSetting(y, setting, menu, scroll, lbl); break;
            case SettingType::JumpscareFolder: fartestNode = addJumpscareSetting(y, setting, menu); break;
            default: break;
        }
                
        if (!setting.description.empty()) {
            if (!fartestNode) fartestNode = lbl;
        
            CCSprite* spr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
            spr->setScale(0.375f);
            
            CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AnimationSettingsLayer::onInfo));
            btn->setPosition({fartestNode->getPositionX() + fartestNode->getContentWidth() * fartestNode->getScaleX() * (1 - fartestNode->getAnchorPoint().x) + 10, y});
            
            menu->addChild(btn);
            
            m_descriptions[btn] = std::make_pair(setting.name, setting.description);
            
            if (setting.type == SettingType::AmongUsColor) m_amogusInfo = btn;
        }
        
        if (m_amogusInfo && m_colorButton2 && !m_colorButton2->isVisible())
            m_amogusInfo->setPositionX(m_amogusArrow->getPositionX() + m_amogusArrow->getContentWidth() * m_amogusArrow->getScaleX() * (1 - m_amogusArrow->getAnchorPoint().x) + 10);
        
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

CCNode* AnimationSettingsLayer::addToggleSetting(float y, const AnimationSetting& setting, CCMenu* menu) {
    CCMenuItemToggler* toggle = CCMenuItemToggler::create(
        CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
        CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
        this,
        menu_selector(AnimationSettingsLayer::onToggle)
    );
    toggle->setScale(0.5f);
    toggle->setPosition({19, y});
    toggle->toggle(Utils::getSettingBool(m_animation.id, setting.id));
    
    menu->addChild(toggle);
    
    m_toggleIDs[toggle] = setting.id;
    
    return nullptr;
}

CCNode* AnimationSettingsLayer::addSpeedSetting(float y, const AnimationSetting& setting, ScrollLayer* scroll) {
    Slider* slider = Slider::create(this, menu_selector(AnimationSettingsLayer::onSlider));
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
    
    slider->setValue(Utils::getSettingFloat(m_animation.id, setting.id));
    onSlider(slider->getThumb());
    
    return m_speedLabel;
}

CCNode* AnimationSettingsLayer::addPercentSetting(float y, const AnimationSetting& setting, ScrollLayer* scroll) {
    TextInput* input = TextInput::create(70, "%");
   	input->setPosition({96, y});
   	input->setScale(0.525f);
   	input->getInputNode()->setDelegate(this);
   	input->getInputNode()->setAllowedChars("0123456789");
   	input->setString(std::to_string(static_cast<int>(Utils::getSettingFloat(m_animation.id, setting.id))).c_str());
    
    scroll->m_contentLayer->addChild(input);
    
    CCLabelBMFont* lbl = CCLabelBMFont::create("%", "bigFont.fnt");
    lbl->setOpacity(220);
    lbl->setScale(0.325f);
    lbl->setPosition({125, y});
    
    scroll->m_contentLayer->addChild(lbl);
    
    m_inputIDs[input->getInputNode()] = setting.id;

    m_percentInputs.insert(input->getInputNode());
    
    return lbl;
}

CCNode* AnimationSettingsLayer::addSelectSetting(float y, const AnimationSetting& setting, CCMenu* menu, ScrollLayer* scroll, CCLabelBMFont* nameLabel) {
    float xOffset = nameLabel->getContentWidth() * nameLabel->getScale() - 60;
    
    CCScale9Sprite* bg = CCScale9Sprite::create("square02b_001.png");
    bg->setColor({0, 0, 0});
    bg->setOpacity(90);
    bg->setScale(0.275f);
    bg->setPosition({125 + xOffset, y});
    bg->setContentSize({237, 69});
    
    scroll->m_contentLayer->addChild(bg);
    
    CCLabelBMFont* lbl = CCLabelBMFont::create(setting.elements[static_cast<int>(Utils::getSettingFloat(m_animation.id, setting.id))].c_str(), "bigFont.fnt");
    lbl->setPosition({125 + xOffset, y});
    lbl->limitLabelWidth(58.f, 0.35f, 0.0001f);
    
    scroll->m_contentLayer->addChild(lbl);
    
    CCSprite* spr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    spr->setFlipX(true);
    spr->setScale(0.675f);
    
    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AnimationSettingsLayer::onSelectorArrow));
    btn->setPosition({83 + xOffset, y});
    
    menu->addChild(btn);
    
    m_selectors[btn] = {
        setting.id,
        lbl,
        setting.elements
    };
    
    spr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    spr->setScale(0.675f);
    
    btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AnimationSettingsLayer::onSelectorArrow));
    btn->setTag(11);
    btn->setPosition({167 + xOffset, y});
    
    menu->addChild(btn);
    
    m_selectors[btn] = {
        setting.id,
        lbl,
        setting.elements
    };
    
    return btn;
}

CCNode* AnimationSettingsLayer::addAmongUsSetting(float y, const AnimationSetting& setting, CCMenu* menu, ScrollLayer* scroll, CCLabelBMFont* nameLabel) {
    CCNode* node = addSelectSetting(y, setting, menu, scroll, nameLabel);
    
    m_colorSprite1 = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    m_colorSprite1->setScale(0.5f);
    m_colorSprite1->setColor({
        static_cast<GLubyte>(Utils::getSettingFloat(Anim::AmongUs, "r1")),
        static_cast<GLubyte>(Utils::getSettingFloat(Anim::AmongUs, "g1")),
        static_cast<GLubyte>(Utils::getSettingFloat(Anim::AmongUs, "b1"))
    });
    
    m_colorButton1 = CCMenuItemSpriteExtra::create(m_colorSprite1, this, menu_selector(AnimationSettingsLayer::onColorPicker));
    m_colorButton1->setPosition({node->getPositionX() + 22, y});
    m_colorButton1->setTag(1);
    
    menu->addChild(m_colorButton1);
    
    m_colorSprite2 = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png");
    m_colorSprite2->setScale(0.5f);
    m_colorSprite2->setColor({
        static_cast<GLubyte>(Utils::getSettingFloat(Anim::AmongUs, "r2")),
        static_cast<GLubyte>(Utils::getSettingFloat(Anim::AmongUs, "g2")),
        static_cast<GLubyte>(Utils::getSettingFloat(Anim::AmongUs, "b2"))
    });
    
    m_colorButton2 = CCMenuItemSpriteExtra::create(m_colorSprite2, this, menu_selector(AnimationSettingsLayer::onColorPicker));
    m_colorButton2->setPosition({node->getPositionX() + 46, y});
    m_colorButton2->setTag(2);
    
    menu->addChild(m_colorButton2);
    
    m_colorButton1->setVisible(static_cast<int>(Utils::getSettingFloat(m_animation.id, "colors")) == 1);
    m_colorButton2->setVisible(static_cast<int>(Utils::getSettingFloat(m_animation.id, "colors")) == 1);
    
    m_amogusArrow = node;
    
    return m_colorButton2;
}

CCNode* AnimationSettingsLayer::addJumpscareSetting(float y, const AnimationSetting& setting, CCMenu* menu) {
    ButtonSprite* spr = ButtonSprite::create("Open Folder");
    spr->setScale(0.5f);

    CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AnimationSettingsLayer::onFolder));
    btn->setPosition({142, y});
    btn->setTag(setting.id == "images-folder");

    menu->addChild(btn);

    return btn;
}