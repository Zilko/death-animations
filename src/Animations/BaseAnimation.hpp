#pragma once

#include "../Includes.hpp"

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

public:

    virtual void start() {
        setContentSize(m_size);
        setAnchorPoint({0, 0});
        setTouchEnabled(true);
        registerWithTouchDispatcher();
        setTouchMode(kCCTouchesOneByOne);

        if (!m_isPreview)
            setID("death-animation"_spr);
        
        m_parentNode->addChild(this);
    }

    virtual void end() {
        removeFromParentAndCleanup(true);
    }
    
    virtual bool ccTouchBegan(CCTouch*, CCEvent*) override { return false; }
  
};