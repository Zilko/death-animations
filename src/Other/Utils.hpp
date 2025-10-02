#pragma once

#include "../Includes.hpp"

class BaseAnimation;
class SoundUpdater;

class Utils {
    
public:

    static int getRandomInt(int, int);
    static std::filesystem::path getRandomFile(const std::filesystem::path&, const std::unordered_set<std::string>);

    static void playSound(Anim, const std::string&, float, float);
    static void playSound(Anim, float, float, const std::filesystem::path&);
    static void playSound(Anim, const std::string&, float, int, int, int, float = 1.f, bool = false, int = 0);
    static void playSound(Anim, const std::string&, float, int, int, float = 1.f, bool = false);
    static SoundUpdater* playSoundManual(Anim, const std::string&, float, float);
    static SoundUpdater* playSoundManual(Anim, float, float, const std::filesystem::path&);
    
    static Anim getSelectedAnimationEnum();
    static const DeathAnimation& getSelectedAnimation();
    static const DeathAnimation& getAnimationByID(Anim anim);

    static float getSpeedValue(float);
    static std::string getSettingString(int, const std::string&);
    static float getSettingFloat(int, const std::string&);
    static bool getSettingBool(int, const std::string&);
    
    template <typename T>
    static void saveSetting(int, const std::string&, const T&);
    static void saveSetting(int, const std::string&, const std::string&);
    static void setDefaults(int);

    static void setHookEnabled(const std::string&, bool);
    static void setHighestZ(CCNode*);

    static void fixScaleTextureSizexd(CCNode*);
    static std::vector<std::filesystem::path> getAllFilesFromFolder(const std::filesystem::path&, const std::unordered_set<std::string>);
    static ccColor3B applyHSV(const ccColor3B&, const NoobHSV&);
    static std::vector<std::string> split(const std::string&, char, size_t = 0);

    static BaseAnimation* createAnimation(Anim, const AnimationParams&);
    static CCGLProgram* createShader(const std::string&, bool);
    static CCTexture2D* takeScreenshot(CCRenderTexture* = nullptr);
    static CCSprite* renderPlayerSprite(CCNodeRGBA*, bool);
    
};
