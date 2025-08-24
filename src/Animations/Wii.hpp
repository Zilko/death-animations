#include "BaseAnimation.hpp"

const float FADE_IN = 0.23f;

class WiiBorder : public CCNode {

private:

    CCSprite* m_controller = nullptr;

    CCLayerColor* m_layer = nullptr;
    CCLayerColor* m_line = nullptr;

    CCLayerGradient* m_gradient = nullptr;

    CCSize m_size = {0, 0};

    WiiBorder(const CCSize& size)
        : m_size(size) {}

    void init(bool inverted) {
        CCNode::init();

        setPositionY(inverted ? CCDirector::get()->getWinSize().height + m_size.height : -m_size.height);
        setContentSize(m_size + CCSize(0, 1.5f));

        m_layer = CCLayerColor::create({0, 0, 0, 255}, m_size.width, m_size.height);
        m_layer->setPositionY(inverted ? -m_size.height : 0);

        addChild(m_layer);

        m_line = CCLayerColor::create({151, 151, 151, 255}, m_size.width, 1.5f);
        m_line->setPositionY(inverted ? -m_size.height - 1.5f : m_size.height);

        addChild(m_line);

        m_gradient = CCLayerGradient::create(
            inverted ? ccc4(0, 187, 255, 255) : ccc4(11, 38, 104, 255),
            inverted ? ccc4(11, 38, 104, 255) : ccc4(0, 187, 255, 255),
            {0, 1}
        );
        m_gradient->setPositionY(inverted ? -m_size.height : 0);
        m_gradient->setContentSize(m_size);
        m_gradient->setOpacity(0);

        addChild(m_gradient);        
    }

    // void

public:

    static WiiBorder* create(const CCSize& size, bool inverted) {
        WiiBorder* ret = new WiiBorder(size);

        ret->init(inverted);
        ret->autorelease();

        return ret;
    }

    void setController(CCSprite* controller) {
        m_controller = controller;
    }

};

class WiiButton : public CCNode {

private:

    void init(CCSprite* label) {
        CCSprite* spr = CCSprite::create("button-wii.png"_spr);
        spr->setColor({0, 0, 0});
        spr->setOpacity(0);
        spr->setScale(0.91f);
        spr->setPosition({3.2f, -3.5f});
        spr->runAction(CCEaseSineInOut::create(CCFadeTo::create(FADE_IN, 143)));

        addChild(spr);

        spr = CCSprite::create("button-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setOpacity(0);
        spr->runAction(CCEaseSineInOut::create(CCFadeTo::create(FADE_IN, 255)));

        addChild(spr);

        label->setOpacity(0);
        label->runAction(CCEaseSineInOut::create(CCFadeTo::create(FADE_IN, 255)));

        addChild(label);
    }

public:

    static WiiButton* create(CCSprite* label) {
        WiiButton* ret = new WiiButton();

        ret->init(label);
        ret->autorelease();

        return ret;
    }

};

class WiiPoppy : public CCNode {

private:

    void init(const ccColor3B& color, int num, bool pro) {
        int opacity = pro ? 255 : 235;
        
        CCSprite* spr = CCSprite::create("p-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setOpacity(opacity);
        spr->setColor(color);

        addChild(spr);

        spr = CCSprite::create(fmt::format("{}-wii.png"_spr, num).c_str());
        spr->setPositionX(8.5f);
        spr->setScale(0.91f);
        spr->setOpacity(opacity);
        spr->setColor(color);

        addChild(spr);

        spr = CCSprite::create("battery-wii.png"_spr);
        spr->setPosition({34.75f, -0.16f});
        spr->setScale(0.92f);
        spr->setOpacity(opacity);
        spr->setColor(color);

        addChild(spr);

        if (!pro) return;

        CCLayerColor* layer = CCLayerColor::create({color.r, color.g, color.b, 255}, 3.5f, 6.5f);
        layer->setPosition({25.65f, -3.5f});

        addChild(layer);

        layer = CCLayerColor::create({color.r, color.g, color.b, 255}, 3.5f, 6.5f);
        layer->setPosition({30.f, -3.5f});

        addChild(layer);

        layer = CCLayerColor::create({color.r, color.g, color.b, 255}, 3.5f, 6.5f);
        layer->setPosition({34.35f, -3.5f});

        addChild(layer);

        layer = CCLayerColor::create({color.r, color.g, color.b, 255}, 3.5f, 6.5f);
        layer->setPosition({38.7f, -3.5f});

        addChild(layer);
    }

public:

    static WiiPoppy* create(const ccColor3B& color, int num, bool pro) {
        WiiPoppy* ret = new WiiPoppy();

        ret->init(color, num, pro);
        ret->autorelease();

        return ret;
    }

};

class Wii : public BaseAnimation {

private:

    CCSprite* m_pointer = nullptr;

    void updatePointer(float) {
        m_pointer->setPosition(getMousePos());
    }

public:

    DEFINE_CREATE(Wii)

    void start() override {
        BaseAnimation::start();

        CCSprite* freezeSprite = CCSprite::createWithTexture(Utils::takeScreenshot());
        freezeSprite->setFlipY(true);
        freezeSprite->setAnchorPoint({0, 0});
        freezeSprite->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        
        addChild(freezeSprite);

        Utils::playSound(Anim::Wii, "wii-home.mp3", m_speed, 1.f);
        
        WiiBorder* border = WiiBorder::create({m_size.width, 65.5f}, false);
        border->runAction(CCEaseSineInOut::create(CCMoveTo::create(FADE_IN, {0, 0})));

        addChild(border);

        CCSprite* spr = CCSprite::create("left-thing-border-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setPosition({171, 66.5f});
        spr->setOpacity(228);

        border->addChild(spr);

        spr = CCSprite::create("left-thing-border-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setFlipX(true);
        spr->setPosition({456.5f, 66.5f});
        spr->setOpacity(228);

        border->addChild(spr);

        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 228}, 277, 19);
        layer->setPosition({176, 57});

        border->addChild(layer);

        layer = CCLayerColor::create({255, 255, 255, 228}, 277, 0.5f);
        layer->setPosition({176, 56.5f});

        border->addChild(layer);

        layer = CCLayerColor::create({255, 255, 255, 228}, 277, 0.5f);
        layer->setPosition({176, 76});

        border->addChild(layer);

        layer = CCLayerColor::create({161, 161, 161, 228}, 1.5f, 19);
        layer->setPosition({242, 57});

        border->addChild(layer);

        layer = CCLayerColor::create({161, 161, 161, 228}, 1.5f, 19);
        layer->setPosition({313.1f, 57});

        border->addChild(layer);

        layer = CCLayerColor::create({161, 161, 161, 228}, 1.5f, 19);
        layer->setPosition({384.3f, 57});

        border->addChild(layer);

        WiiPoppy* poppy = WiiPoppy::create({255, 255, 255}, 1, true);
        poppy->setPosition({183.5f, 66.9f});

        border->addChild(poppy);

        poppy = WiiPoppy::create({60, 60, 60}, 2, false);
        poppy->setPosition({256.55f, 66.9f});

        border->addChild(poppy);

        poppy = WiiPoppy::create({60, 60, 60}, 3, false);
        poppy->setPosition({329.6f, 66.9f});

        border->addChild(poppy);

        poppy = WiiPoppy::create({60, 60, 60}, 4, false);
        poppy->setPosition({402.8f, 66.9f});

        border->addChild(poppy);

        spr = CCSprite::create("wii-remote-settings-text.png"_spr);
        spr->setScale(0.91f);
        spr->setPosition({303, 39.5f});

        border->addChild(spr);

        spr = CCSprite::create("controller-wii.png"_spr);
        spr->setPosition({122, -15});
        spr->setScale(0.91f);
        spr->setAnchorPoint({0.5f, 0});
    
        border->addChild(spr);
        border->setController(spr);

        border = WiiBorder::create({m_size.width, 57}, true);
        border->runAction(CCEaseSineInOut::create(CCMoveTo::create(FADE_IN, {0, m_size.height})));

        addChild(border);

        spr = CCSprite::create("home-menu-text-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setPosition({97.5f, -38});
    
        border->addChild(spr);

        spr = CCSprite::create("close-button-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setPosition({497, -39.2f});

        border->addChild(spr);

        spr = CCSprite::create("wii-menu-text.png"_spr);
        spr->setScale(0.91f);
        spr->setColor({40, 53, 104});
        spr->setPositionX(-1);

        WiiButton* button = WiiButton::create(spr);
        button->setPosition({156, 165.5f});

        addChild(button);

        spr = CCSprite::create("reset-text-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setColor({40, 53, 104});

        button = WiiButton::create(spr);
        button->setPosition({414, 165.5f});

        addChild(button);

        #ifndef GEODE_IS_MOBILE

        m_pointer = CCSprite::create("pointer-wii.png"_spr);

        addChild(m_pointer);

        schedule(schedule_selector(Wii::updatePointer));

        #endif
    }

};