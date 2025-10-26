#pragma once

#include "../Includes.hpp"

class Variables {

private:

    DeathAnimation m_selectedAnimation = {};

    float m_speedMultiplier = 1.f;
    
public:

    static Variables& get() {
        static Variables instance;
        return instance;
    }

    static void setSelectedAnimation(const DeathAnimation& selectedAnimation) {
        get().m_selectedAnimation = selectedAnimation;
    }

    static DeathAnimation getSelectedAnimation() {
        return get().m_selectedAnimation;
    }

    static void setSpeed(float speed) {
        get().m_speedMultiplier = speed;
    }

    static float getSpeed() {
        return get().m_speedMultiplier;
    }
    
};