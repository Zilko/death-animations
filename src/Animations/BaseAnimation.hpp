#pragma once

#include "../Includes.hpp"

class BaseAnimation : public CCLayer {

protected:

    CCNode* m_parentNode = nullptr;

    PlayLayer* m_playLayer = nullptr;
    
    AnimationDelegate* m_delegate = nullptr;
    
    CCSize m_size;
    
    bool m_isGoing = false;
    bool m_isPreview = false;
    
    float m_speed = 1.f;
    
    BaseAnimation(CCNode* parentNode, PlayLayer* playLayer, AnimationDelegate* delegate, float speed) {
        m_parentNode = parentNode;
        m_playLayer = playLayer;
        m_delegate = delegate;
        m_speed = speed;
        m_isPreview = delegate != nullptr;
        m_size = CCDirector::get()->getWinSize();
    }

public:

    virtual void start() {
        setContentSize(m_size);
        setAnchorPoint({0, 0});
        
        m_parentNode->addChild(this);
        m_isGoing = true;
    }

    virtual void end() {
        removeFromParentAndCleanup(true);
        m_isGoing = false;
    }
    
    bool isGoing() {
        return m_isGoing;
    }
  
};