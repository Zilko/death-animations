#include "BaseAnimation.hpp"

class LevelComplete : public BaseAnimation {

    ANIMATION_CTOR_CREATE(LevelComplete)
    
public:

    void start() override {
    }
    
    void end() override {
        BaseAnimation::end();

    }
};