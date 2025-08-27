#include "BaseAnimation.hpp"

class Wii : public BaseAnimation {

    ANIMATION_CTOR_CREATE(Wii)
    
public:

    void start() override {
        BaseAnimation::start();
        
    }
    
    void end() override {
        BaseAnimation::end();

    }
};