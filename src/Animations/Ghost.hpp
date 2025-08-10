#include "BaseAnimation.hpp"

class Ghost : public BaseAnimation {

public:
    
    DEFINE_CREATE(Ghost)

    void start() override {
        BaseAnimation::start();
        
        // CCSprite::create();
    }

    };