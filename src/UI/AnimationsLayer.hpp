#pragma once

#include "../Includes.hpp"

#include "AnimationCell.hpp"

class AnimationsLayer : public geode::Popup<> {  
  
private:

    GJCommentListLayer* m_list = nullptr;

    CCNode* m_contentLayer = nullptr;

    Scrollbar* m_scrollbar = nullptr;

    TableView* m_tableView = nullptr;

    AnimationCell* m_selectedAnimation = nullptr;
    
    CCLabelBMFont* m_nameLabel = nullptr;

    CCMenuItemSpriteExtra* m_previewBtn = nullptr;
    CCMenuItemSpriteExtra* m_settingsBtn = nullptr;
    
    std::unordered_map<int, AnimationCell*> m_animationCells;

    bool setup() override;
    
    void onPreview(CCObject*);
    void onSettings(CCObject*);

    void updateTableView(float);
    
public:

    static AnimationsLayer* create();
    
    void selectAnimation(AnimationCell*);
    
    void createdCell(AnimationCell*);

    GJCommentListLayer* getList();
        
};