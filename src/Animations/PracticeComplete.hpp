#include "BaseAnimation.hpp"

class PracticeComplete : public BaseAnimation {

    ANIMATION_CTOR_CREATE(PracticeComplete)
    
public:

    void start() override {
    }
    
    void end() override {
        BaseAnimation::end();

    }
};