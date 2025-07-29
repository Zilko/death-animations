#include "Utils.hpp"

#include "../Animations/YouDied.hpp"
#include "../Animations/Bsod.hpp"
#include "../Animations/AmongUs.hpp"
#include "../Animations/Celeste.hpp"
#include "../Animations/ToBeContinued.hpp"
#include "../Animations/Wii.hpp"
#include "../Animations/HollowKnight.hpp"

#include <random>

int Utils::getRandomInt(int min, int max) {
    static std::mt19937 gen(std::random_device{}());

    std::uniform_int_distribution<> dis(min, max);
    
    return dis(gen);
}

void Utils::playSound(Anim anim, const std::string& sound, float speed, int fade, int duration) {
        if (!Utils::getSettingBool(anim, "play-sound-effects"))
            return;
                
    duration = static_cast<int>(duration / speed / 1000.f) * 1000;
    fade /= speed;
    
    if (duration < 1000) duration = 1000;
            
    FMODAudioEngine::get()->playEffectAdvanced(
        (Mod::get()->getResourcesDir() / sound).string().c_str(),
        1.f, 1.f, 1.f, speed, 1.f, 1.f, 0, duration, fade, fade, false, 1, false, false, 1, 1, 0, 1
    );
}

void Utils::playSound(Anim anim, const std::string& sound, float speed, float volume) {
    FMODAudioEngine::get()->playEffect((Mod::get()->getResourcesDir() / sound).string().c_str(), speed, 1.f, volume);
}

BaseAnimation* Utils::createAnimation(Anim animation, CCNode* parentNode, PlayLayer* playLayer, AnimationDelegate* delegate, float speed) {
    switch (animation) {
        case Anim::YouDied: return YouDied::create(parentNode, playLayer, delegate, speed);
        case Anim::Bsod: return Bsod::create(parentNode, playLayer, delegate, speed);
        case Anim::AmongUs: return AmongUs::create(parentNode, playLayer, delegate, speed);
        case Anim::Celeste: return Celeste::create(parentNode, playLayer, delegate, speed);
        case Anim::ToBeContinued: return ToBeContinued::create(parentNode, playLayer, delegate, speed);
        case Anim::Wii: return Wii::create(parentNode, playLayer, delegate, speed);
        case Anim::HollowKnight: return HollowKnight::create(parentNode, playLayer, delegate, speed);
        default: return nullptr;
    };
}

Anim Utils::getSelectedAnimationEnum() {
    return static_cast<Anim>(Mod::get()->getSavedValue<int>("selected-animation"));
}

const DeathAnimation& Utils::getSelectedAnimation() {
    return getSelectedAnimation(getSelectedAnimationEnum());
}

const DeathAnimation& Utils::getSelectedAnimation(Anim anim) {
    for (const DeathAnimation& animation : animations)
        if (anim == animation.id)
            return animation;
           
    return animations.front();
}

float Utils::getSpeedValue(float value) {
    return 3.9f * std::clamp(value, 0.f, 1.f) + 0.1f;
}

float Utils::getSettingFloat(int id, const std::string& setting) {
    std::string animation = std::to_string(id);
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");
    
    float defaultValue = globalFloatDefaults.contains(setting) ? globalFloatDefaults.at(setting) : 0.f;
    
    if (specificFloatDefaults.contains(id))
        if (specificFloatDefaults.at(id).contains(setting))
            defaultValue = specificFloatDefaults.at(id).at(setting);
    
    if (object.contains(animation))
        return object[animation][setting].asDouble().unwrapOr(defaultValue);
        
    return defaultValue;
}

bool Utils::getSettingBool(int id, const std::string& setting) {
    std::string animation = std::to_string(id);
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");
    
    bool defaultValue = gloalBoolDefaults.contains(setting);
    
    if (specificBoolDefaults.contains(id))
        if (specificBoolDefaults.at(id).contains(setting))
            defaultValue = specificBoolDefaults.at(id).at(setting);
    
    if (object.contains(animation))
        return object[animation][setting].asBool().unwrapOr(defaultValue);
        
    return defaultValue;
}

void Utils::saveSetting(int id, const std::string& setting, float value) {
    std::string animation = std::to_string(id);
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");
    matjson::Value newObject = object[animation];
    
    newObject[setting] = value;
    object[animation] = newObject;
    
    Mod::get()->setSavedValue("settings", object);
}

void Utils::saveSetting(int id, const std::string& setting, bool value) {
    std::string animation = std::to_string(id);
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");
    matjson::Value newObject = object[animation];
    
    newObject[setting] = value;
    object[animation] = newObject;
    
    Mod::get()->setSavedValue("settings", object);
}

void Utils::setDefaults(int id) {
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");

    object.erase(std::to_string(id));
    
    Mod::get()->setSavedValue("settings", object);
}

CCSprite* Utils::takeScreenshot() {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    
    int width = viewport[2];
    int height = viewport[3];
    GLubyte* buffer = new GLubyte[width * height * 3];
    
    glReadBuffer(GL_BACK);
    glFinish();
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    
    for (int y = 0; y < height / 2; ++y)
        for (int x = 0; x < width * 3; ++x)
            std::swap(buffer[y * width * 3 + x], buffer[(height - 1 - y) * width * 3 + x]);
    
    CCTexture2D* texture = new CCTexture2D();
    texture->initWithData(buffer, kCCTexture2DPixelFormat_RGB888, width, height, ccp(width, height));
    texture->autorelease();
    
    delete[] buffer;
    
    return CCSprite::createWithTexture(texture);
}