#include "BaseAnimation.hpp"

class HollowKnight : public BaseAnimation {

public:
    
    DEFINE_CREATE(HollowKnight)

    void start() override {
        BaseAnimation::start();
        
    }
    
    void end() override {
        BaseAnimation::end();
    }
};