#include "ListRow.hpp"
#include "AnimationCell.hpp"
#include "AnimationsLayer.hpp"

ListRow* ListRow::create(const std::vector<DeathAnimation>& animations, AnimationsLayer* parentLayer) {
    ListRow* ret = new ListRow();
    
    if (ret->init(animations, parentLayer)) {
        ret->autorelease();
        return ret;
    }
    
    delete ret;
    return nullptr;
}

bool ListRow::init(const std::vector<DeathAnimation>& animations, AnimationsLayer* parentLayer) {
    CCNode::init();
    
    for (int i = 0; i < animations.size(); i++) {
        AnimationCell* cell = AnimationCell::create(animations[i], parentLayer);
        cell->setPosition({70.f * i + 9 + cell->getContentSize().width * cell->getScale() / 2.f, 25});
        addChild(cell);
        
        parentLayer->createdCell(cell);
    }
    
    return true;
}