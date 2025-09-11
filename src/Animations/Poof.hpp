#include "BaseAnimation.hpp"

class Poof : public BaseAnimation {

    ANIMATION_CTOR_CREATE(Poof)
    
public:

    void start() override {
    }
    
    void end() override {
        BaseAnimation::end();

    }
};