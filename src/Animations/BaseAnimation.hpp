#pragma once

#include "../Includes.hpp"
#include "../Other/PreviewDelegate.hpp"

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

    Anim m_id = Anim::None;
    
    bool m_isPreview = false;
    bool m_forceRestart = false;
    bool m_dontRestart = false;
    bool m_isDelayRestart = false;
    bool m_isNoRetryLayer = false;
    bool m_didFinish = false;
    
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
          m_id(params.id)
          {
            for (const DeathAnimation& animation : animations)
                if (params.id == animation.id) {
                    m_duration = animation.duration;
                    break;
                }
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

        runAction(CCSequence::create(
            CCDelayTime::create(m_duration / m_speed + 0.1f),
            CallFuncExt::create([this]{
                setVisible(false);
                m_didFinish = true;
            }),
            nullptr
        ));

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

    void resetLevel() {
        if (m_isPreview)
            return m_delegate->reset();

        m_forceRestart = true;
        
        m_playLayer->m_inResetDelay = true;
        m_playLayer->stopActionByTag(16);
        m_playLayer->delayedResetLevel();
    }

public:

    virtual void startEarly() {}

    virtual void startWithObject(GameObject*) {}

    virtual void start() {}

    virtual void onPause() {}

    virtual void onResume() {}

    virtual void onRestart() {}

    virtual void end() {
        removeFromParentAndCleanup(true);
    }

    float getDuration() {
        return m_duration;
    }

    bool isForceRestart() {
        return m_forceRestart;
    }

    bool isDelayRestart() {
        return m_isDelayRestart;
    }

    bool isDontRestart() {
        return m_dontRestart;
    }

    bool isNoRetryLayer() {
        return m_isNoRetryLayer;
    }

    bool didFinish() {
        return m_didFinish;
    }
  
};