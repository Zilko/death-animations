#include "BaseAnimation.hpp"

class Terraria : public BaseAnimation {

public:
    
    DEFINE_CREATE(Terraria)

    void start() override {
        BaseAnimation::start();
        
        // CCSprite::create();
    }

    };