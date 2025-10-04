#include "BaseAnimation.hpp"

class SpeechBubble : public BaseAnimation, public FLAlertLayerProtocol {

private:

    CCMenu* m_buttonMenu = nullptr;

    CCMenuItemSpriteExtra* m_leftArrow = nullptr;
    CCMenuItemSpriteExtra* m_rightArrow = nullptr;

    CCSprite* m_freezeSprite = nullptr;
    CCSprite* m_currentSprite = nullptr;

    FLAlertLayer* m_layer = nullptr;

    std::unordered_map<int, CCSprite*> m_sprites;
    std::vector<std::filesystem::path> m_speechBubbles;
    
    int m_currentSpeechBubble = 0;

    bool m_canChangeSpeechBubble = true;
    bool m_uiEnabled = false;

    void loadSpeechBubble(int speechBubble) {
        if (speechBubble >= static_cast<int>(m_speechBubbles.size()))
            speechBubble = 0;

        if (speechBubble < 0)
            speechBubble = static_cast<int>(m_speechBubbles.size()) - 1;

        m_currentSpeechBubble = speechBubble;
        
        if (m_uiEnabled) {
            m_leftArrow->setOpacity(static_cast<int>(m_speechBubbles.size()) == 1 ? 140 : 255);
            m_rightArrow->setOpacity(static_cast<int>(m_speechBubbles.size()) == 1 ? 140 : 255);
            
            m_leftArrow->setEnabled(static_cast<int>(m_speechBubbles.size()) != 1);
            m_rightArrow->setEnabled(static_cast<int>(m_speechBubbles.size()) != 1);
        }

        if (m_canChangeSpeechBubble)
            Utils::saveSetting(Anim::SpeechBubble, "speech-bubble", m_currentSpeechBubble + 1);

        if (m_currentSprite)
            m_currentSprite->setVisible(false);

        if (m_sprites.contains(speechBubble)) {
            m_currentSprite = m_sprites.at(speechBubble);
            
            m_currentSprite->setVisible(true);

            updateSprite();

            return;
        }

        CCSprite* spr = CCSprite::create(utils::string::pathToString(m_speechBubbles[speechBubble]).c_str());
        spr->setAnchorPoint({0.5f, 1});
        spr->setPosition({m_size.width / 2.f, m_size.height});

        Utils::fixScaleTextureSizexd(spr);

        spr->setScale(m_size.width / spr->getContentWidth());

        addChild(spr, -1);

        m_sprites[speechBubble] = spr;

        m_currentSprite = spr;

        updateSprite();
    }

    void updateSprite() {
        if (!m_currentSprite) return;

        bool flipY = Utils::getSettingBool(Anim::SpeechBubble, "flip-y");

        m_currentSprite->setScaleY(m_currentSprite->getScaleX() * Utils::getSettingFloat(Anim::SpeechBubble, "scale-y"));
        m_currentSprite->setFlipX(Utils::getSettingBool(Anim::SpeechBubble, "flip-x"));
        m_currentSprite->setFlipY(flipY);
        m_currentSprite->setAnchorPoint({0.5f, flipY ? 0.f : 1.f});
        m_currentSprite->setPositionY(flipY ? 0.f : m_size.height);
    }

    void onArrowRight(CCObject*) {
        loadSpeechBubble(m_currentSpeechBubble + 1);
    }

    void onArrowLeft(CCObject*) {
        loadSpeechBubble(m_currentSpeechBubble - 1);
    }

    void update(float) override {
        if (m_uiEnabled && !m_isPreview)
            PlatformToolbox::showCursor();
    }

    void onFlipX(CCObject*) {
        Utils::saveSetting(Anim::SpeechBubble, "flip-x", !Utils::getSettingBool(Anim::SpeechBubble, "flip-x"));

        updateSprite();
    }

    void onFlipY(CCObject*) {
        Utils::saveSetting(Anim::SpeechBubble, "flip-y", !Utils::getSettingBool(Anim::SpeechBubble, "flip-y"));

        updateSprite();
    }

    void onSlider(CCObject* obj) {
        Utils::saveSetting(Anim::SpeechBubble, "scale-y", static_cast<SliderThumb*>(obj)->getValue());

        updateSprite();
    }

    void onSaveImage(CCObject*) {
        if (!m_currentSprite) return;

        std::filesystem::path folder = Mod::get()->getSaveDir() / "screenshots";

        if (!std::filesystem::exists(folder))
            if (utils::file::createDirectoryAll(folder).isErr())
                return Notification::create("Fail", NotificationIcon::Error)->show();

        CCRenderTexture* renderTexture = CCRenderTexture::create(m_size.width, m_size.height);

        renderTexture->begin();

        m_freezeSprite->visit();
        m_currentSprite->visit();

        renderTexture->end();

        auto now = std::chrono::system_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        std::string filepath = utils::string::pathToString(
            folder / fmt::format("{}_{}.png", m_isPreview ? "speech_bubble" : m_playLayer->m_level->m_levelName, std::to_string(timestamp))
        );

        CCImage* image = renderTexture->newCCImage();
        image->saveToFile(
            filepath.c_str(),
            false
        );
        image->release();

        m_layer = FLAlertLayer::create(
            this,
            "Speech Bubble",
            fmt::format("Saved to: {}", filepath),
            "Ok",
            nullptr
        );
        
        m_layer->show();
    }

    void onBack(CCObject*) {
        resetLevel();
    }

    void onAnimationEnd() override {
        m_didFinish = true;
    }
    
    ~SpeechBubble() {
        if (m_layer)
            m_layer->keyBackClicked();
    }
    
    ANIMATION_CTOR_CREATE(SpeechBubble) {}

public:

    void FLAlert_Clicked(FLAlertLayer*, bool p1) override {
        m_layer = nullptr;
    }

    void start() override {
        m_speechBubbles = Utils::getAllFilesFromFolder(Mod::get()->getSaveDir() / "speech-bubbles", { ".png", ".jpg" });

        if (m_speechBubbles.empty()) {
            m_speechBubbles.push_back(
                Mod::get()->getResourcesDir() / "speech-bubble-1.png"
            );

            if (!std::filesystem::exists(m_speechBubbles.front()))
                return log::error("What did you do boy");
        }

        int id = static_cast<int>(Utils::getSettingFloat(Anim::SpeechBubble, "speech-bubble"));

        if (id == 0) {
            m_currentSpeechBubble = Utils::getRandomInt(0, m_speechBubbles.size() - 1);
            m_canChangeSpeechBubble = false;
        } else
            m_currentSpeechBubble = id - 1;


        if (m_currentSpeechBubble >= static_cast<int>(m_speechBubbles.size()))
            m_currentSpeechBubble = static_cast<int>(m_speechBubbles.size()) - 1;
        
        bool ogVisible = false;
        
        if (!m_isPreview) {
            ogVisible = m_playLayer->m_uiLayer->m_pauseBtn->isVisible();
            m_playLayer->m_uiLayer->m_pauseBtn->setVisible(false);
        }

        m_freezeSprite = CCSprite::createWithTexture(Utils::takeScreenshot());
        m_freezeSprite->setFlipY(true);
        m_freezeSprite->setScale(m_size.width / m_freezeSprite->getContentWidth());
        m_freezeSprite->setAnchorPoint({0, 0});
        m_freezeSprite->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        
        if (!m_isPreview)
            m_playLayer->m_uiLayer->m_pauseBtn->setVisible(ogVisible);
        
        addChild(m_freezeSprite, -2);

        Utils::getSettingBool(Anim::SpeechBubble, "show-ui");

        m_uiEnabled = Utils::getSettingBool(Anim::SpeechBubble, "show-ui");

        if (!m_uiEnabled) return loadSpeechBubble(m_currentSpeechBubble);;

        #ifndef GEODE_IS_MOBILE
        
        schedule(schedule_selector(SpeechBubble::update));

        #endif
        
        CCNode* parent = m_isPreview ? static_cast<CCNode*>(this) : static_cast<CCNode*>(m_playLayer->m_uiLayer);

        CCSprite* spr = nullptr;
        CCMenuItemSpriteExtra* btn = nullptr;

        disableRetryLayer();

        m_buttonMenu = CCMenu::create();
        m_buttonMenu->setID("speech-bubble-menu"_spr);
        m_buttonMenu->setPosition({0, 0});

        parent->addChild(m_buttonMenu, 10);

        spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        spr->setScale(1.1f);

        m_leftArrow = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SpeechBubble::onArrowLeft));
        m_leftArrow->setPosition({29, m_size.height / 2.f});
        m_leftArrow->setCascadeOpacityEnabled(true);

        m_buttonMenu->addChild(m_leftArrow);

        spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        spr->setScale(1.1f);
        spr->setFlipX(true);

        m_rightArrow = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SpeechBubble::onArrowRight));
        m_rightArrow->setPosition({m_size.width - 29, m_size.height / 2.f});
        m_rightArrow->setCascadeOpacityEnabled(true);

        m_buttonMenu->addChild(m_rightArrow);

        if (!m_isPreview) {
            spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
            spr->setScale(0.8f);
            
            btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SpeechBubble::onBack));
            btn->setPosition({20, m_size.height - 20});

            m_buttonMenu->addChild(btn);
        }

        m_buttonMenu->addChild(CCLayerColor::create({0, 0, 0, 122}, m_size.width, 46.2f));

        CCLabelBMFont* lbl = CCLabelBMFont::create("Flip X", "bigFont.fnt");
        lbl->setPosition({59, 23});
        lbl->setScale(0.55f);

        m_buttonMenu->addChild(lbl);

        lbl = CCLabelBMFont::create("Flip Y", "bigFont.fnt");
        lbl->setPosition({181, 23});
        lbl->setScale(0.55f);

        m_buttonMenu->addChild(lbl);

        lbl = CCLabelBMFont::create("Scale Y", "bigFont.fnt");
        lbl->setPosition({334, 31});
        lbl->setScale(0.47f);

        m_buttonMenu->addChild(lbl);

        Slider* slider = Slider::create(this, menu_selector(SpeechBubble::onSlider));
        slider->setPosition({334, 15});
        slider->setAnchorPoint({ 0.f, 0.f });
        slider->setScale(0.55f);
        slider->setValue(Utils::getSettingFloat(Anim::SpeechBubble, "scale-y"));
        
        m_buttonMenu->addChild(slider);

        CCMenuItemToggler* toggle = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
            this,
            menu_selector(SpeechBubble::onFlipX)
        );
        toggle->setScale(0.6f);
        toggle->setPosition({113, 23});
        toggle->toggle(Utils::getSettingBool(Anim::SpeechBubble, "flip-x"));

        m_buttonMenu->addChild(toggle);

        toggle = CCMenuItemToggler::create(
            CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
            this,
            menu_selector(SpeechBubble::onFlipY)
        );
        toggle->setScale(0.6f);
        toggle->setPosition({235, 23});
        toggle->toggle(Utils::getSettingBool(Anim::SpeechBubble, "flip-y"));

        m_buttonMenu->addChild(toggle);

        ButtonSprite* btnSpr = ButtonSprite::create("Save Image");

        btnSpr->setScale(0.7f);

        btn = CCMenuItemSpriteExtra::create(btnSpr, this, menu_selector(SpeechBubble::onSaveImage));
        btn->setPosition({486, 23});

        m_buttonMenu->addChild(btn);
        
        loadSpeechBubble(m_currentSpeechBubble);
    }

    void end() override {
        #ifndef GEODE_IS_MOBILE

        if (m_uiEnabled && !GameManager::get()->getGameVariable("0024") && !m_isPreview)
            PlatformToolbox::hideCursor();
        
        #endif
        
        if (m_buttonMenu)
            m_buttonMenu->removeFromParentAndCleanup(true);
            
        BaseAnimation::end();
    }

};