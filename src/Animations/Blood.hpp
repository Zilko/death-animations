#include "BaseAnimation.hpp"

class Blood : public BaseAnimation {

    ANIMATION_CTOR_CREATE(Blood) {}
    
public:

    void start() override {
    }
    
    void end() override {
        BaseAnimation::end();

    }
};