#include "BaseAnimation.hpp"

class Undertale : public BaseAnimation {

public:
    
    DEFINE_CREATE(Undertale)

    void start() override {
        BaseAnimation::start();
        
        // CCSprite::create();
    }

    };