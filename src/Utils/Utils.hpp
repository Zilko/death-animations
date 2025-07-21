#pragma once

#include "../Includes.hpp"
#include "../Animations/BaseAnimation.hpp"

class Utils {
    
public:

    static int getRandomInt(int, int);

    static void playSound(Anim, const std::string&, int, int, float);
    
    static BaseAnimation* createAnimation(Anim, CCNode*, PlayLayer*, AnimationDelegate*, float);
    
    static Anim getSelectedAnimationEnum();
    static const DeathAnimation& getSelectedAnimation();
    static const DeathAnimation& getSelectedAnimation(Anim anim);
    
    static float getSpeedValue(float);
    static float getSettingFloat(const std::string&, const std::string&, float);
    static bool getSettingBool(const std::string&, const std::string&, bool);
    
    static void saveSetting(const std::string&, const std::string&, float);
    static void saveSetting(const std::string&, const std::string&, bool);
    static void setDefaults(const std::string&);
    
};
