#pragma once

#include "../Includes.hpp"

class PreviewDelegate {
  
public:

    virtual CCNodeRGBA* getPlayer() { return nullptr; }

    virtual CCNode* getBackButton() { return nullptr; }

    virtual void setBackgroundOpacity(int) {}
    
};