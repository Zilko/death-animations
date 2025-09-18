#pragma once

#include "../Includes.hpp"

#define ANIMATION_CTOR(CLASS) \
    protected: \
        CLASS(const AnimationParams& params) \
            : BaseAnimation(params) {}

#define ANIMATION_CREATE(CLASS) \
    public: \
        static CLASS* create(const AnimationParams& params) { \
            CLASS* ret = new CLASS(params); \
            ret->autorelease(); \
            ret->init(); \
            return ret; \
        }

#define ANIMATION_CTOR_CREATE(CLASS) \
    ANIMATION_CTOR(CLASS) \
    ANIMATION_CREATE(CLASS)

class BaseAnimation : public CCLayer {

protected:

    CCNode* m_parentNode = nullptr;

    PlayLayer* m_playLayer = nullptr;
    
    PreviewDelegate* m_delegate = nullptr;
    
    ExtraParams m_extras;
    
    CCSize m_size;
    
    bool m_isPreview = false;
    
    float m_speed = 1.f;
    
    BaseAnimation(const AnimationParams& params)
        : m_extras(params.extras),
          m_parentNode(params.parentNode),
          m_playLayer(params.playLayer),
          m_delegate(params.delegate),
          m_speed(params.speed),
          m_isPreview(params.delegate != nullptr),
          m_size(CCDirector::get()->getWinSize()) {}

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

    virtual void startEarly() {}

    virtual void start() {}

    virtual void end() {
        removeFromParentAndCleanup(true);
    }

    virtual void onPause() {}

    virtual void onResume() {}
  
};