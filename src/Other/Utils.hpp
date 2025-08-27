#pragma once

#include "../Includes.hpp"
#include "../Animations/BaseAnimation.hpp"

class Utils {
    
public:

    static int getRandomInt(int, int);
    static std::filesystem::path getRandomFile(const std::filesystem::path&, const std::unordered_set<std::string>);

    static void playSound(Anim, const std::string&, float, float);
    static void playSound(Anim, float, float, const std::filesystem::path&);
    static void playSound(Anim, const std::string&, float, int, int, int, float = 1.f, bool = false);
    static void playSound(Anim, const std::string&, float, int, int, float = 1.f, bool = false);
    static void playSoundManual(Anim, const std::string&, float, float);
    static void playSoundManual(Anim, float, float, const std::filesystem::path&);
    
    static Anim getSelectedAnimationEnum();
    static const DeathAnimation& getSelectedAnimation();
    static const DeathAnimation& getSelectedAnimation(Anim anim);

    static float getSpeedValue(float);
    static float getSettingFloat(int, const std::string&);
    static bool getSettingBool(int, const std::string&);
    
    static void saveSetting(int, const std::string&, float);
    static void saveSetting(int, const std::string&, bool);
    static void setDefaults(int);

    static void setHookEnabled(const std::string&, bool);
    static void setHighestZ(CCNode*);

    static void fixScaleTextureSizexd(CCNode*);
    static CCPoint getPlayerScreenPos(PlayLayer*, CCNode*, bool);
    static std::vector<std::filesystem::path> getAllFilesFromFolder(const std::filesystem::path&, const std::unordered_set<std::string>);

    static BaseAnimation* createAnimation(Anim, const AnimationParams&);
    static CCGLProgram* createShader(const std::string&, bool);
    static CCTexture2D* takeScreenshot(CCRenderTexture* = nullptr);
    static CCRenderTexture* renderPlayerTexture(CCNodeRGBA*, bool);
    static CCSprite* renderPlayerSprite(CCNodeRGBA*, bool);
    
};
