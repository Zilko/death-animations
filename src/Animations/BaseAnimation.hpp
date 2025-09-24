#pragma once

#include "../Includes.hpp"

#define ANIMATION_CTOR(Class) \
    protected: \
        Class(const AnimationParams& params) \
            : BaseAnimation(params) \

#define ANIMATION_CREATE(Class) \
    public: \
        static Class* create(const AnimationParams& params) { \
            Class* ret = new Class(params); \
            ret->autorelease(); \
            ret->init(); \
            return ret; \
        } \

#define ANIMATION_CTOR_CREATE(Class) \
    ANIMATION_CREATE(Class) \
    ANIMATION_CTOR(Class) \

class BaseAnimation : public CCLayer {

protected:

    CCNode* m_parentNode = nullptr;

    PlayLayer* m_playLayer = nullptr;
    
    PreviewDelegate* m_delegate = nullptr;
        
    CCSize m_size;
  
    ExtraParams m_extras;
    
    bool m_isPreview = false;
    bool m_wasQuickKeysDisabled = false;
    
    float m_speed = 1.f;
    float m_duration = 1.f;
    
    BaseAnimation(const AnimationParams& params)
        : m_extras(params.extras),
          m_parentNode(params.parentNode),
          m_playLayer(params.playLayer),
          m_delegate(params.delegate),
          m_speed(params.speed),
          m_isPreview(params.delegate != nullptr),
          m_size(CCDirector::get()->getWinSize()),
          m_duration(params.duration) {}

    ~BaseAnimation() {
        if (m_wasQuickKeysDisabled)
            GameManager::get()->setGameVariable("0163", false);
    }

    void enableTouch() {
        setTouchEnabled(true);
        registerWithTouchDispatcher();
        setTouchMode(kCCTouchesOneByOne);
    }

    virtual void setAnimationID() {
        setID("death-animation"_spr);
    }

    virtual bool init() override {
        CCLayer::init();

        setAnimationID();
        setContentSize(m_size);
        setAnchorPoint({0, 0});
        
        if (m_parentNode)
            m_parentNode->addChild(this);

        if (m_duration >= 5.f && !GameManager::get()->getGameVariable("0163")) {
            GameManager::get()->setGameVariable("0163", true);
            m_wasQuickKeysDisabled = true;
        }

        return true;
    }

    float getCurrentZoom() {
        return m_isPreview ? 1.f : m_playLayer->m_gameState.m_cameraZoom;
    }

    CCPoint getNodeScreenPos(CCNode* node) {
        if (!node) return {0, 0};

        if (m_isPreview)
            return node->getPosition();

        CCPoint pos = node->convertToWorldSpaceAR({0, 0});
        CCPoint cameraCenter = m_playLayer->m_cameraObb2->m_center;

        float angle = CC_DEGREES_TO_RADIANS(-m_playLayer->m_gameState.m_cameraAngle);
        float cos = cosf(angle);
        float isn = sinf(angle);
        float offsetX = pos.x - cameraCenter.x;
        float offsetY = pos.y - cameraCenter.y;
        float rotatedX = offsetX * cos - offsetY * isn;
        float rotatedY = offsetX * isn + offsetY * cos;

        pos = ccp(cameraCenter.x + rotatedX, cameraCenter.y + rotatedY);

        return pos;
    }

public:

    float getDuration() {
        return m_duration;
    }

    virtual void startEarly() {}

    virtual void start() {}

    virtual void end() {
        removeFromParentAndCleanup(true);
    }

    virtual void onPause() {}

    virtual void onResume() {}
  
};