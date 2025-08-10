#include "BaseAnimation.hpp"

class Maro : public BaseAnimation {

public:
    
    DEFINE_CREATE(Maro)

    void start() override {
        BaseAnimation::start();
        
        // CCSprite::create();
    }

    };