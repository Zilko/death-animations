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
    m_previewBtn->setEnabled(animation.id != Anim::None);
    m_previewBtn->setOpacity(animation.id != Anim::None ? 255 : 90);
}

void AnimationsLayer::createdCell(AnimationCell* cell) {
    m_animationCells[cell->getAnimation().id] = cell;
}

GJCommentListLayer* AnimationsLayer::getList() {
    return m_list;
}

void AnimationsLayer::onPreview(CCObject*) {
    PreviewLayer::create()->show();
}

void AnimationsLayer::onSettings(CCObject*) {
    AnimationSettingsLayer::create(m_selectedAnimation->getAnimation())->show();
}

void AnimationsLayer::updateTableView(float) {
	if (!m_tableView || !m_contentLayer || !m_scrollbar) return;

	float scale = m_mainLayer->getScale();
	
	if (scale >= 1.f) {
		m_tableView->setContentSize(m_list->getContentSize() * scale);
		m_contentLayer->setPosition(m_contentLayer->getPosition());
		m_scrollbar->setScaleY(1.f / scale);
		m_scrollbar->getChildByType<CCScale9Sprite>(1)->setScaleY(1.f / scale * 0.4f);
	}

	if (m_mainLayer->numberOfRunningActions() == 0) {
        unschedule(schedule_selector(AnimationsLayer::updateTableView));

		m_tableView->setContentSize(m_list->getContentSize());
		m_contentLayer->setPosition(m_contentLayer->getPosition());
		m_scrollbar->setScaleY(1);
		m_scrollbar->getChildByType<CCScale9Sprite>(1)->setScaleY(0.4f);
	}
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
    
    listView->setPrimaryCellColor(ccc3(138, 77, 46));
    listView->setSecondaryCellColor(ccc3(138, 77, 46));
    listView->setCellBorderColor(ccc4(0, 0, 0, 0));
    
    m_list = GJCommentListLayer::create(listView, "", ccc4(255, 255, 255, 0), 289, 153, false);
    m_list->setPosition((m_size - m_list->getContentSize()) / 2.f);
    m_list->setUserObject("dont-correct-borders", CCBool::create(true));

    m_mainLayer->addChild(m_list, 2);

    m_tableView = listView->m_tableView;
    m_contentLayer = m_tableView->m_contentLayer;
    
    bg->setPosition(m_list->getPosition());
    
    CCSprite* topBorder = m_list->getChildByType<CCSprite>(1);
    CCSprite* bottomBorder = m_list->getChildByType<CCSprite>(0);
    
    topBorder->setScale(0.85f);
    bottomBorder->setScale(0.85f);
    
    m_scrollbar = Scrollbar::create(listView->m_tableView);
    m_scrollbar->setPosition(m_list->getPosition() + ccp(m_list->getContentSize().width + 6, 0));
    m_scrollbar->setAnchorPoint({0, 0});
    m_mainLayer->addChild(m_scrollbar);
    
    int selected = Mod::get()->getSavedValue<int>("selected-animation");
    
    if (m_animationCells.contains(selected))
        selectAnimation(m_animationCells.at(selected));

    schedule(schedule_selector(AnimationsLayer::updateTableView), 0, kCCRepeatForever, 1.f / 240.f);
    
    return true;
}   