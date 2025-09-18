#include "BaseAnimation.hpp"

class Poof : public BaseAnimation {

    ANIMATION_CTOR_CREATE(Poof)
    
public:

    void start() override {
        // Utils::playSound(Anim::Poof, "poof-1.mp3", m_speed, 0, 0, 9999, 0.7f, false, 115);
        // Utils::playSound(Anim::Poof, "poof-2.mp3", m_speed, 0, 0, 9999, 0.1f, false, 165);
    }

};