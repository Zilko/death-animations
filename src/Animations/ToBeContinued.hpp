#include "BaseAnimation.hpp"

class ToBeContinued : public BaseAnimation {
  
private:

    

public:
    
    DEFINE_CREATE(ToBeContinued)

    void start() override {
        BaseAnimation::start();
        
    }
    
    void end() override {
        BaseAnimation::end();
    }
};