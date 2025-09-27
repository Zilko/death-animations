#pragma once

#include "../Includes.hpp"

class PreviewDelegate {
  
public:

    virtual CCNodeRGBA* getPlayer() { return nullptr; }

    virtual CCMenuItemSpriteExtra* getBackButton() { return nullptr; }

    virtual void setBackgroundOpacity(int) {}

    virtual bool isDead() { return false; }

    virtual void reset() {}
    
};