#include "BaseAnimation.hpp"

class FadeOut : public BaseAnimation {

    ANIMATION_CTOR_CREATE(FadeOut)
    
public:

    void start() override {
    }
    
    void end() override {
        BaseAnimation::end();

    }
};