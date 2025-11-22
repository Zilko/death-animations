#include "BaseAnimation.hpp"

const float FADE_IN = 0.23f;

class WiiBorder : public CCNode {

private:

    CCSprite* m_controller = nullptr;

    CCLayerColor* m_layer = nullptr;
    CCLayerColor* m_line = nullptr;

    CCLayerGradient* m_gradient = nullptr;

    CCSize m_size = {0, 0};

    bool m_hovering = false;

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

    void setHover(bool hover) {
        if (hover == m_hovering) {
            return;
        }

        m_hovering = hover;

        ccColor3B color = m_hovering ? ccc3(171, 227, 247) : ccc3(151, 151, 151);

        m_line->runAction(CCTintTo::create(0.12f, color.r, color.g, color.b));
        m_gradient->runAction(CCFadeTo::create(0.12f, m_hovering ? 255 : 0));
        
        if (m_controller)
            m_controller->runAction(CCMoveTo::create(0.12f, {m_controller->getPositionX(), m_hovering ? -1.5f : -15.f}));
    }

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

    void checkHover(const CCPoint& pos) {
        CCPoint localPos = m_layer->convertToNodeSpace(pos);
        CCRect rect = CCRect(0, 0, m_layer->getContentSize().width, m_layer->getContentSize().height);

        setHover(rect.containsPoint(localPos));
    }

};

class WiiButton : public CCNode {

private:

    CCSprite* m_buttonSprite = nullptr;

    bool m_hovering = false;

    void init(CCSprite* label) {
        m_buttonSprite = CCSprite::create("button-wii.png"_spr);
        m_buttonSprite->setColor({0, 0, 0});
        m_buttonSprite->setOpacity(0);
        m_buttonSprite->setScale(0.91f);
        m_buttonSprite->setPosition({3.2f, -3.5f});
        m_buttonSprite->runAction(CCEaseSineOut::create(CCFadeTo::create(FADE_IN, 143)));

        addChild(m_buttonSprite);

        CCSprite* spr = CCSprite::create("button-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setOpacity(0);
        spr->runAction(CCEaseSineOut::create(CCFadeTo::create(FADE_IN, 255)));

        addChild(spr);

        label->setOpacity(0);
        label->runAction(CCEaseSineOut::create(CCFadeTo::create(FADE_IN, 255)));

        addChild(label);
    }

    void setHover(bool hover) {
        if (hover == m_hovering) {
            return;
        }

        m_hovering = hover;

        runAction(CCScaleTo::create(0.12f, m_hovering ? 1.042f : 1.f));
    }

public:

    static WiiButton* create(CCSprite* label) {
        WiiButton* ret = new WiiButton();

        ret->init(label);
        ret->autorelease();

        return ret;
    }

    void checkHover(const CCPoint& pos) {
        CCPoint localPos = m_buttonSprite->convertToNodeSpace(pos);
        CCRect rect = CCRect(0, 0, m_buttonSprite->getContentSize().width, m_buttonSprite->getContentSize().height);

        setHover(rect.containsPoint(localPos));
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

    WiiBorder* m_topBorder = nullptr;
    WiiBorder* m_bottomBorder = nullptr;

    WiiButton* m_menuButton = nullptr;
    WiiButton* m_resetButton = nullptr;

    void updatePointer(float) {
        CCPoint pos = getMousePos();

        m_pointer->setPosition(pos + ccp(5, -13));

        m_menuButton->checkHover(pos);
        m_resetButton->checkHover(pos);
        m_topBorder->checkHover(pos);
        m_bottomBorder->checkHover(pos);
    }

public:

    ANIMATION_CTOR_CREATE(Wii) {}

    void startEarly() override {
        BaseAnimation::start();

        Utils::setHighestZ(this);

        CCSprite* freezeSprite = CCSprite::createWithTexture(Utils::takeScreenshot());
        freezeSprite->setFlipY(true);
        freezeSprite->setAnchorPoint({0, 0});
        freezeSprite->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        
        addChild(freezeSprite);

        Utils::playSound(Anim::Wii, "wii-home.mp3", m_speed, 1.f);
        
        m_bottomBorder = WiiBorder::create({m_size.width, 65.5f}, false);
        m_bottomBorder->runAction(CCEaseSineOut::create(CCMoveTo::create(FADE_IN, {0, 0})));

        addChild(m_bottomBorder);

        CCSprite* spr = CCSprite::create("left-thing-border-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setPosition({171, 66.5f});
        spr->setOpacity(228);

        Utils::fixScaleTextureSizexd(spr);

        m_bottomBorder->addChild(spr);

        spr = CCSprite::create("left-thing-border-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setFlipX(true);
        spr->setPosition({456.5f, 66.5f});
        spr->setOpacity(228);

        Utils::fixScaleTextureSizexd(spr);

        m_bottomBorder->addChild(spr);

        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 228}, 277, 19);
        layer->setPosition({176, 57});

        m_bottomBorder->addChild(layer);

        layer = CCLayerColor::create({212, 212, 212, 228}, 277, 0.5f);
        layer->setPosition({176, 56.5f});

        m_bottomBorder->addChild(layer);

        layer = CCLayerColor::create({212, 212, 212, 228}, 277, 0.5f);
        layer->setPosition({176, 76});

        m_bottomBorder->addChild(layer);

        layer = CCLayerColor::create({161, 161, 161, 228}, 1.5f, 19);
        layer->setPosition({242, 57});

        m_bottomBorder->addChild(layer);

        layer = CCLayerColor::create({161, 161, 161, 228}, 1.5f, 19);
        layer->setPosition({313.1f, 57});

        m_bottomBorder->addChild(layer);

        layer = CCLayerColor::create({161, 161, 161, 228}, 1.5f, 19);
        layer->setPosition({384.3f, 57});

        m_bottomBorder->addChild(layer);

        WiiPoppy* poppy = WiiPoppy::create({255, 255, 255}, 1, true);
        poppy->setPosition({183.5f, 66.9f});

        m_bottomBorder->addChild(poppy);

        poppy = WiiPoppy::create({60, 60, 60}, 2, false);
        poppy->setPosition({256.55f, 66.9f});

        m_bottomBorder->addChild(poppy);

        poppy = WiiPoppy::create({60, 60, 60}, 3, false);
        poppy->setPosition({329.6f, 66.9f});

        m_bottomBorder->addChild(poppy);

        poppy = WiiPoppy::create({60, 60, 60}, 4, false);
        poppy->setPosition({402.8f, 66.9f});

        m_bottomBorder->addChild(poppy);

        spr = CCSprite::create("wii-remote-settings-text.png"_spr);
        spr->setScale(0.91f);
        spr->setPosition({303, 39.5f});

        m_bottomBorder->addChild(spr);

        spr = CCSprite::create("controller-wii.png"_spr);
        spr->setPosition({122, -15});
        spr->setScale(0.91f);
        spr->setAnchorPoint({0.5f, 0});
    
        m_bottomBorder->addChild(spr);
        m_bottomBorder->setController(spr);

        m_topBorder = WiiBorder::create({m_size.width, 57}, true);
        m_topBorder->runAction(CCEaseSineOut::create(CCMoveTo::create(FADE_IN, {0, m_size.height})));

        addChild(m_topBorder);

        spr = CCSprite::create("home-menu-text-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setPosition({97.5f, -38});
    
        m_topBorder->addChild(spr);

        spr = CCSprite::create("close-button-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setPosition({497, -39.2f});

        m_topBorder->addChild(spr);

        spr = CCSprite::create("wii-menu-text.png"_spr);
        spr->setScale(0.91f);
        spr->setColor({40, 53, 104});
        spr->setPositionX(-1);

        m_menuButton = WiiButton::create(spr);
        m_menuButton->setPosition({156, 165.5f});

        addChild(m_menuButton);

        spr = CCSprite::create("reset-text-wii.png"_spr);
        spr->setScale(0.91f);
        spr->setColor({40, 53, 104});

        m_resetButton = WiiButton::create(spr);
        m_resetButton->setPosition({414, 165.5f});

        addChild(m_resetButton);

        #ifndef GEODE_IS_MOBILE

        m_pointer = CCSprite::create("pointer-wii.png"_spr);

        addChild(m_pointer);

        schedule(schedule_selector(Wii::updatePointer));

        #endif
    }

    void start() override {
        disableRetry();
    }

};