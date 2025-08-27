#include "BaseAnimation.hpp"

class SpeechBubble : public BaseAnimation {

private:

    CCSprite* m_pointer = nullptr;

    CCSprite* m_currentSprite = nullptr;

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

            return;
        }

        CCSprite* spr = CCSprite::create(utils::string::pathToString(m_speechBubbles[speechBubble]).c_str());
        spr->setAnchorPoint({0.5f, 1});
        spr->setPosition({m_size.width / 2.f, m_size.height});

        Utils::fixScaleTextureSizexd(spr);

        spr->setScale(m_size.width / spr->getContentWidth());

        addChild(spr);

        m_currentSprite = spr;
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
    
    ANIMATION_CTOR_CREATE(SpeechBubble)
    
public:

    void start() override {
        BaseAnimation::start();

        m_speechBubbles = Utils::getAllFilesFromFolder(Mod::get()->getSaveDir() / "speech-bubbles", { ".png", ".jpg" });

        if (m_speechBubbles.empty()) return; // fill me up

        CCSprite* spr = CCSprite::createWithTexture(Utils::takeScreenshot());
        spr->setFlipY(true);
        spr->setScale(m_size.width / spr->getContentWidth());
        spr->setAnchorPoint({0, 0});
        spr->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        
        addChild(spr);

        loadSpeechBubble(0);

        CCMenu* menu = CCMenu::create();
        menu->setPosition({0, 0});

        addChild(menu, 10);

        spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        spr->setScale(1.1f);

        CCMenuItemSpriteExtra* btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SpeechBubble::onArrowLeft));
        btn->setPosition({29, m_size.height / 2.f});

        menu->addChild(btn);

        spr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        spr->setScale(1.1f);
        spr->setFlipX(true);

        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SpeechBubble::onArrowRight));
        btn->setPosition({m_size.width - 29, m_size.height / 2.f});

        menu->addChild(btn);

        #ifndef GEODE_IS_MOBILE

        m_pointer = CCSprite::create("smallDot.png");
        m_pointer->setScale(0.57f);
        m_pointer->setColor({0, 0, 0});

        spr = CCSprite::create("smallDot.png");
        spr->setScale(0.6f);
        spr->setPosition(m_pointer->getContentSize() / 2.f);

        m_pointer->addChild(spr);
        addChild(m_pointer, 20);

        schedule(schedule_selector(SpeechBubble::updatePointer));

        #endif
    }

};