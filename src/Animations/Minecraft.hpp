#include "BaseAnimation.hpp"

class Minecraft : public BaseAnimation {

public:
    
    DEFINE_CREATE(Minecraft)

    void start() override {
        BaseAnimation::start();
        
        // CCSprite::create();
    }

    };