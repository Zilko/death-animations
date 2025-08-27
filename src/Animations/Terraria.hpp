#include "BaseAnimation.hpp"

class Terraria : public BaseAnimation {
    
    ANIMATION_CTOR_CREATE(Terraria)
    
public:

    void start() override {
        BaseAnimation::start();
        
        // CCSprite::create();
    }

    };