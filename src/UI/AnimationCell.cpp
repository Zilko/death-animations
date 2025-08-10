#include "AnimationCell.hpp"
#include "AnimationsLayer.hpp"

ProMenu::ProMenu(AnimationsLayer* layer) {
    m_layer = layer;
}

ProMenu* ProMenu::create(AnimationsLayer* layer) {
    ProMenu* ret = new ProMenu(layer);

    if (ret->init()) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool ProMenu::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    cocos2d::CCPoint pos = m_layer->getList()->convertToNodeSpace(touch->getLocation());

    if (pos.x < 0 || pos.y < 0 || pos.x > 289 || pos.y > 153) return false;

    return CCMenu::ccTouchBegan(touch, event);
}

AnimationCell::AnimationCell(const DeathAnimation& animation, AnimationsLayer* parentLayer) {
    m_animation = animation;
    m_parentLayer = parentLayer;
}

AnimationCell* AnimationCell::create(const DeathAnimation& animation, AnimationsLayer* parentLayer) {
    AnimationCell* ret = new AnimationCell(animation, parentLayer);
    
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}

void AnimationCell::onSelect(CCObject*) {
    m_parentLayer->selectAnimation(this);
}

void AnimationCell::setSelected(bool selected) {
    m_selectSprite->setVisible(selected);
    m_button->setEnabled(!selected);
}

const DeathAnimation& AnimationCell::getAnimation() {
    return m_animation;
}

bool AnimationCell::init() {
    CCNode::init();
    
    ProMenu* menu = ProMenu::create(m_parentLayer);
    menu->setPosition({0, 0});
    addChild(menu);
    
    CCNode* container = CCNode::create();
    container->setAnchorPoint({0.5f, 0.5f});
    
    CCScale9Sprite* border = CCScale9Sprite::create("border.png"_spr);
    border->setContentSize({61, 39.639f});
    container->addChild(border, 1);
    
    m_selectSprite = CCSprite::create("select.png"_spr);
    m_selectSprite->setScale(0.32f);
    m_selectSprite->setVisible(false);
    container->addChild(m_selectSprite, 1);
    
    CCSprite* thumbnail = CCSprite::create((""_spr + m_animation.thumbnail).c_str());
    container->addChild(thumbnail);
    
    setAnchorPoint({0.5f, 0.5f});
    setContentSize({61, 39.639f});
    
    container->setContentSize(getContentSize());
    
    m_button = CCMenuItemSpriteExtra::create(container, this, menu_selector(AnimationCell::onSelect));
    menu->addChild(m_button);
    
    CCPoint center = getContentSize() / 2.f;
    
    m_selectSprite->setPosition(center);
    m_button->setPosition(center);
    thumbnail->setPosition(center);
    border->setPosition(center);
    
    return true;
}