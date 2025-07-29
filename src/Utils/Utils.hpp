#pragma once

#include "../Includes.hpp"
#include "../Animations/BaseAnimation.hpp"

class Utils {
    
public:

    static int getRandomInt(int, int);

    static void playSound(Anim, const std::string&, float, int, int);
    static void playSound(Anim, const std::string&, float, float);
    
    static BaseAnimation* createAnimation(Anim, CCNode*, PlayLayer*, AnimationDelegate*, float);
    
    static Anim getSelectedAnimationEnum();
    static const DeathAnimation& getSelectedAnimation();
    static const DeathAnimation& getSelectedAnimation(Anim anim);
    
    static float getSpeedValue(float);
    static float getSettingFloat(int, const std::string&);
    static bool getSettingBool(int, const std::string&);
    
    static void saveSetting(int, const std::string&, float);
    static void saveSetting(int, const std::string&, bool);
    static void setDefaults(int);
    
};
