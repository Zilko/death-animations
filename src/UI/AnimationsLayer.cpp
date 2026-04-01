#include "AnimationsLayer.hpp"
#include "AnimationSettingsLayer.hpp"
#include "PreviewLayer.hpp"
#include "ListRow.hpp"

AnimationsLayer* AnimationsLayer::create() {
    AnimationsLayer* ret = new AnimationsLayer();
    
    if (ret->init()) {
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

Border* AnimationsLayer::getBorder() {
    return m_border;
}

void AnimationsLayer::onPreview(CCObject*) {
    PreviewLayer::create()->show();
}

void AnimationsLayer::onSettings(CCObject*) {
    AnimationSettingsLayer::create(m_selectedAnimation->getAnimation())->show();
}

bool AnimationsLayer::init() {
    Popup::init(337, 240);
    
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

    auto scroll = ScrollLayer::create({289, 153}, true, true);
    scroll->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout());

    m_border = Border::create(scroll, {138, 77, 46, 255}, {289, 153}, {0, 0});
    m_border->ignoreAnchorPointForPosition(false);
    m_border->setPosition(m_size / 2.f + CCPoint{0, 3.5f});

    m_mainLayer->addChild(m_border);

    std::vector<DeathAnimation> row;
    
    for (const DeathAnimation& animation : animations) {
        row.push_back(animation);
        
        if (row.size() == 4) {
            scroll->m_contentLayer->addChild(ListRow::create(row, this));
            row.clear();
        }
    }
    
    if (row.size() != 4 && !row.empty())
        scroll->m_contentLayer->addChild(ListRow::create(row, this));

    scroll->m_contentLayer->updateLayout();

    auto scrollbar = Scrollbar::create(scroll);
    scrollbar->setPosition({323, m_border->getPositionY()});

    m_mainLayer->addChild(scrollbar, 5);
    
    scroll->scrollToTop();
        
    int selected = Mod::get()->getSavedValue<int>("selected-animation");
    
    if (m_animationCells.contains(selected))
        selectAnimation(m_animationCells.at(selected));

    queueInMainThread([scroll = Ref(scroll)] {
        if (auto handler = CCTouchDispatcher::get()->findHandler(scroll)) {
            CCTouchDispatcher::get()->setPriority(handler->getPriority() - 1, handler->getDelegate());
        }
    });
    
    return true;
}   