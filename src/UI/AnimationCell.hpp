#pragma once

#include "../Includes.hpp"

class AnimationsLayer;

class AnimationCell : public CCNode {
    
private:

    CCSprite* m_selectSprite = nullptr;
    
    CCMenuItemSpriteExtra* m_button = nullptr;
    
    AnimationsLayer* m_parentLayer = nullptr;
    
    DeathAnimation m_animation;
    
    AnimationCell(const DeathAnimation&, AnimationsLayer*);

    bool init();
    
    void onSelect(CCObject*);
  
public:
    
    static AnimationCell* create(const DeathAnimation&, AnimationsLayer*);
    
    void setSelected(bool);
    
    const DeathAnimation& getAnimation();
    
};