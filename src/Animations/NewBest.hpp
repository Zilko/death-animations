#include "BaseAnimation.hpp"

class NewBest : public BaseAnimation {

    ANIMATION_CTOR_CREATE(NewBest)
    
public:

    void start() override {
    }
    
    void end() override {
        BaseAnimation::end();

    }
};