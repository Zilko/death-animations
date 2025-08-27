#include "BaseAnimation.hpp"

class SpeechBubble : public BaseAnimation, public FLAlertLayerProtocol {

private:

    CCSprite* m_freezeSprite = nullptr;
    CCSprite* m_currentSprite = nullptr;
    CCSprite* m_pointer = nullptr;

    FLAlertLayer* m_layer = nullptr;

    std::unordered_map<int, CCSprite*> m_sprites;
    std::vector<std::filesystem::path> m_speechBubbles;
    
    int m_currentSpeechBubble = 0;

    void loadSpeechBubble(int speechBubble) {
        if (speechBubble >= static_cast<int>(m_speechBubbles.size()))
            speechBubble = 0;

        if (speechBubble < 0)
            speechBubble = static_cast<int>(m_speechBubbles.size()) - 1;

        m_currentSpeechBubble = speechBubble;

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

    void updatePointer(float) {
        m_pointer->setPosition(getMousePos());
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

        if (m_pointer)
            Utils::setHighestZ(m_pointer);
    }
    
    ANIMATION_CTOR_CREATE(SpeechBubble)

    ~SpeechBubble() {
        if (m_pointer) {
            m_pointer->removeFromParentAndCleanup(true);
            m_pointer = nullptr;
        }

        if (m_layer)
            m_layer->keyBackClicked();
    }
    
public:

    void FLAlert_Clicked(FLAlertLayer*, bool p1) override {
        m_layer = nullptr;
    }

    void start() override {
        BaseAnimation::start();

        m_speechBubbles = Utils::getAllFilesFromFolder(Mod::get()->getSaveDir() / "speech-bubbles", { ".png", ".jpg" });

        if (m_speechBubbles.empty()) {
            m_speechBubbles.push_back(
                Mod::get()->getResourcesDir() / "speech-bubble-1.png"
            );

            if (!std::filesystem::exists(m_speechBubbles.front()))
                return;
        }

        m_freezeSprite = CCSprite::createWithTexture(Utils::takeScreenshot());
        m_freezeSprite->setFlipY(true);
        m_freezeSprite->setScale(m_size.width / m_freezeSprite->getContentWidth());
        m_freezeSprite->setAnchorPoint({0, 0});
        m_freezeSprite->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        
        addChild(m_freezeSprite, -2);

        loadSpeechBubble(0);

        CCMenu* menu = CCMenu::create();
        menu->setPosition({0, 0});

        addChild(menu, 10);

        CCSprite* spr = nullptr;
        CCMenuItemSpriteExtra* btn = nullptr;

        if (!Utils::getSettingBool(Anim::SpeechBubble, "hide-arrows")) {
            spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
            spr->setScale(1.1f);

            btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SpeechBubble::onArrowLeft));
            btn->setPosition({29, m_size.height / 2.f});

            menu->addChild(btn);

            spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
            spr->setScale(1.1f);
            spr->setFlipX(true);

            btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SpeechBubble::onArrowRight));
            btn->setPosition({m_size.width - 29, m_size.height / 2.f});

            menu->addChild(btn);
        }

        if (!Utils::getSettingBool(Anim::SpeechBubble, "hide-ui")) {
            addChild(CCLayerColor::create({0, 0, 0, 122}, m_size.width, 46.2f));

            CCLabelBMFont* lbl = CCLabelBMFont::create("Flip X", "bigFont.fnt");
            lbl->setPosition({59, 23});
            lbl->setScale(0.55f);

            addChild(lbl);

            lbl = CCLabelBMFont::create("Flip Y", "bigFont.fnt");
            lbl->setPosition({181, 23});
            lbl->setScale(0.55f);

            addChild(lbl);

            lbl = CCLabelBMFont::create("Scale Y", "bigFont.fnt");
            lbl->setPosition({334, 31});
            lbl->setScale(0.47f);

            addChild(lbl);

            Slider* slider = Slider::create(this, menu_selector(SpeechBubble::onSlider));
            slider->setPosition({334, 15});
            slider->setAnchorPoint({ 0.f, 0.f });
            slider->setScale(0.55f);
            slider->setValue(Utils::getSettingFloat(Anim::SpeechBubble, "scale-y"));
            
            addChild(slider);

            CCMenuItemToggler* toggle = CCMenuItemToggler::create(
                CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
                CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
                this,
                menu_selector(SpeechBubble::onFlipX)
            );
            toggle->setScale(0.6f);
            toggle->setPosition({113, 23});
            toggle->toggle(Utils::getSettingBool(Anim::SpeechBubble, "flip-x"));

            menu->addChild(toggle);

            toggle = CCMenuItemToggler::create(
                CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
                CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"),
                this,
                menu_selector(SpeechBubble::onFlipY)
            );
            toggle->setScale(0.6f);
            toggle->setPosition({235, 23});
            toggle->toggle(Utils::getSettingBool(Anim::SpeechBubble, "flip-y"));

            menu->addChild(toggle);

            ButtonSprite* btnSpr = ButtonSprite::create("Save Image");
            btnSpr->setScale(0.7f);

            btn = CCMenuItemSpriteExtra::create(btnSpr, this, menu_selector(SpeechBubble::onSaveImage));
            btn->setPosition({486, 23});

            menu->addChild(btn);
        }        

        #ifndef GEODE_IS_MOBILE

        if (!Utils::getSettingBool(Anim::SpeechBubble, "hide-arrows") || !Utils::getSettingBool(Anim::SpeechBubble, "hide-ui")) {
            m_pointer = CCSprite::create("smallDot.png");
            m_pointer->setScale(0.57f);
            m_pointer->setColor({0, 0, 0});
            m_pointer->retain();

            spr = CCSprite::create("smallDot.png");
            spr->setScale(0.6f);
            spr->setPosition(m_pointer->getContentSize() / 2.f);

            m_pointer->addChild(spr);
            CCScene::get()->addChild(m_pointer, 20);

            schedule(schedule_selector(SpeechBubble::updatePointer));
        }

        #endif
    }

    void onPause() override {
        if (m_pointer)
            m_pointer->setVisible(false);
    }

    void onResume() override {
        if (m_pointer)
            m_pointer->setVisible(true);
    }

};