#include "BaseAnimation.hpp"

class CBFDetected : public BaseAnimation {

public:
    
    DEFINE_CREATE(CBFDetected)

    void start() override {
        BaseAnimation::start();
        
        // CCSprite::create();
    }

    };