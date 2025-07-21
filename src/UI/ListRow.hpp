#pragma once

#include "../Includes.hpp"

class AnimationsLayer;

class ListRow : public CCNode {
    
private:

    AnimationsLayer* m_parentLayer;
  
public:
    
    static ListRow* create(const std::vector<DeathAnimation>&, AnimationsLayer*);
    
    bool init(const std::vector<DeathAnimation>&, AnimationsLayer*);
    
};