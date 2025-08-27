#pragma once

#include "../Includes.hpp"

#define ANIMATION_CTOR_CREATE(CLASS) \
    private: \
        CLASS(const AnimationParams& params) \
            : BaseAnimation(params) {} \
    public:\
        static CLASS* create(const AnimationParams& params) { \
            CLASS* ret = new CLASS(params); \
            ret->init(); \
            ret->autorelease(); \
            return ret; \
        } \
    private:

class BaseAnimation : public CCLayer {

protected:

    CCNode* m_parentNode = nullptr;

    PlayLayer* m_playLayer = nullptr;
    
    PreviewEvents* m_delegate = nullptr;
    
    ExtraParams m_extras;
    
    CCSize m_size;
    
    bool m_isPreview = false;
    
    float m_speed = 1.f;
    
    BaseAnimation(const AnimationParams& params) {
        m_extras = params.extras;
        m_parentNode = params.parentNode;
        m_playLayer = params.playLayer;
        m_delegate = params.delegate;
        m_speed = params.speed;
        m_isPreview = m_delegate != nullptr;
        m_size = CCDirector::get()->getWinSize();
    }

    virtual void setAnimationID() {
        setID("death-animation"_spr);
    }

    void enableTouch() {
        setTouchEnabled(true);
        registerWithTouchDispatcher();
        setTouchMode(kCCTouchesOneByOne);
    }

public:

    virtual void start() {
        setAnimationID();
        setContentSize(m_size);
        setAnchorPoint({0, 0});
        
        if (m_parentNode)
            m_parentNode->addChild(this);
    }

    virtual void end() {
        removeFromParentAndCleanup(true);
    }
  
};