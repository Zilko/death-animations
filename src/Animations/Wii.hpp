#include "BaseAnimation.hpp"

class Wii : public BaseAnimation {

public:
    
    DEFINE_CREATE(Wii)

    void start() override {
        BaseAnimation::start();
        
    }
    
    void end() override {
        BaseAnimation::end();
    }
};