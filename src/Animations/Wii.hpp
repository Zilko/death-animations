#include "BaseAnimation.hpp"

class Wii : public BaseAnimation {

    ANIMATION_CTOR_CREATE(Wii)
    
public:

    void start() override {
    }
    
    void end() override {
        BaseAnimation::end();

    }
};