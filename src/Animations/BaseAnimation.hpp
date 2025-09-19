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

        return true;
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