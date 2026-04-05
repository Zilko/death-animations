#pragma once

#include "../Includes.hpp"

#include "AnimationCell.hpp"

class AnimationsLayer : public Popup {  
  
private:

    ScrollLayer* m_scroll = nullptr;

    Border* m_border = nullptr;

    AnimationCell* m_selectedAnimation = nullptr;
    
    CCLabelBMFont* m_nameLabel = nullptr;

    CCMenuItemSpriteExtra* m_previewBtn = nullptr;
    CCMenuItemSpriteExtra* m_settingsBtn = nullptr;
    
    std::unordered_map<int, AnimationCell*> m_animationCells;

    bool init() override;
    
    void onPreview(CCObject*);
    void onSettings(CCObject*);

    void updateTableView(float);
    
public:

    static AnimationsLayer* create();
    
    void selectAnimation(AnimationCell*);
    
    void createdCell(AnimationCell*);

    Border* getBorder();
        
};