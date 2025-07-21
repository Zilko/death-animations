#include "AnimationsLayer.hpp"
#include "AnimationSettingsLayer.hpp"
#include "PreviewLayer.hpp"
#include "ListRow.hpp"

AnimationsLayer* AnimationsLayer::create() {
    AnimationsLayer* ret = new AnimationsLayer();
    
    if (ret->initAnchored(337, 240)) {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}

void AnimationsLayer::selectAnimation(AnimationCell* cell) {
    if (m_selectedAnimation)
        m_selectedAnimation->setSelected(false);
        
    m_selectedAnimation = cell;
    m_selectedAnimation->setSelected(true);
    
    const DeathAnimation& animation = m_selectedAnimation->getAnimation();
    
    m_nameLabel->setString(animation.name.c_str());
    m_nameLabel->limitLabelWidth(165, m_nameLabel->getScale(), 0.0001f);
    Mod::get()->setSavedValue("selected-animation", animation.id);
    
    m_settingsBtn->setEnabled(animation.id != Anim::None && animation.id != Anim::Random);
    m_settingsBtn->setOpacity(animation.id != Anim::None && animation.id != Anim::Random ? 255 : 90);
}

void AnimationsLayer::createdCell(AnimationCell* cell) {
    m_animationCells[cell->getAnimation().id] = cell;
}

void AnimationsLayer::onPreview(CCObject*) {
    PreviewLayer::create()->show();
}

void AnimationsLayer::onSettings(CCObject*) {
    AnimationSettingsLayer::create(m_selectedAnimation->getAnimation())->show();
}

bool AnimationsLayer::setup() {
    setTitle("Death Animations");
    
    ButtonSprite* btnSpr = ButtonSprite::create("Preview");
    btnSpr->setCascadeOpacityEnabled(true);
    btnSpr->setScale(0.675f);
    
    m_previewBtn = CCMenuItemSpriteExtra::create(btnSpr, this, menu_selector(AnimationsLayer::onPreview));
    m_previewBtn->setPosition({285, 24});
    m_previewBtn->setCascadeOpacityEnabled(true);
    
    m_buttonMenu->addChild(m_previewBtn);
    
    CCSprite* spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
    spr->setScale(0.445f);

    m_settingsBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AnimationsLayer::onSettings));
    m_settingsBtn->setCascadeOpacityEnabled(true);
    m_settingsBtn->setPosition({226, 24});
    
    m_buttonMenu->addChild(m_settingsBtn);
    
    m_nameLabel = CCLabelBMFont::create("", "goldFont.fnt");
    m_nameLabel->setAnchorPoint({0, 0.5f});
    m_nameLabel->setPosition({18, 24});
    m_nameLabel->setScale(0.475f);
    
    m_mainLayer->addChild(m_nameLabel);
    
    CCScale9Sprite* bg = CCScale9Sprite::create("square02b_001.png");
    bg->setColor({0, 0, 0});
    bg->setOpacity(24);
    bg->setAnchorPoint({0, 0});
    bg->setContentSize({ 289, 153 });
    
    m_mainLayer->addChild(bg);
    
    CCArray* array = CCArray::create();
    
    std::vector<DeathAnimation> row;
    
    for (const DeathAnimation& animation : animations) {
        row.push_back(animation);
        
        if (row.size() == 4) {
            array->addObject(ListRow::create(row, this));
            row.clear();
        }
    }
    
    if (row.size() != 4 && !row.empty())
        array->addObject(ListRow::create(row, this));
        
    ListView* listView = ListView::create(array, 50, 289, 153);
    CCNode* m_contentLayer = static_cast<CCNode*>(listView->m_tableView->getChildren()->objectAtIndex(0));
    
    listView->setPrimaryCellColor(ccc3(138, 77, 46));
    listView->setSecondaryCellColor(ccc3(138, 77, 46));
    listView->setCellBorderColor(ccc4(0, 0, 0, 0));
    
    GJCommentListLayer* m_list = GJCommentListLayer::create(listView, "", ccc4(255, 255, 255, 0), 289, 153, false);
    m_list->setPosition((m_size - m_list->getContentSize()) / 2.f);
    m_mainLayer->addChild(m_list, 2);
    
    bg->setPosition(m_list->getPosition());
    
    CCSprite* topBorder = m_list->getChildByType<CCSprite>(1);
    CCSprite* bottomBorder = m_list->getChildByType<CCSprite>(0);
    
    topBorder->setScale(0.85f);
    bottomBorder->setScale(0.85f);
    
    if (m_contentLayer->getContentSize().height >= 175) {
        Scrollbar* scrollbar = Scrollbar::create(listView->m_tableView);
        scrollbar->setPosition(m_list->getPosition() + ccp(m_list->getContentSize().width + 6, 0));
        scrollbar->setAnchorPoint({0, 0});
        m_mainLayer->addChild(scrollbar);
    }
    
    int selected = Mod::get()->getSavedValue<int>("selected-animation");
    
    if (m_animationCells.contains(selected))
        selectAnimation(m_animationCells.at(selected));
    
    return true;
}   