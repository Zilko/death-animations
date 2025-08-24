#pragma once

#include "../Includes.hpp"

class SelectedAnimation {

private:

    DeathAnimation m_animation;

public:

    static SelectedAnimation& instance() {
        static SelectedAnimation instance;
        return instance;
    }

    static DeathAnimation get() {
        return instance().m_animation;
    }

    static void set(const DeathAnimation& animation) {
        instance().m_animation = animation;
    }

};