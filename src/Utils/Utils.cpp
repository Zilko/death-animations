#include "Utils.hpp"

#include "../Animations/YouDied.hpp"
#include "../Animations/Bsod.hpp"

#include <random>

int Utils::getRandomInt(int min, int max) {
    static std::mt19937 gen(std::random_device{}());

    std::uniform_int_distribution<> dis(min, max);
    
    return dis(gen);
}

void Utils::playSound(Anim anim, const std::string& sound, int fade, int duration, float speed) {
        if (!Utils::getSettingBool(getSelectedAnimation(anim).name, "play-sound-effects", true))
            return;
                
    duration = static_cast<int>(duration / speed / 1000.f) * 1000;
    fade /= speed;
    
    if (duration < 1000) duration = 1000;
            
    FMODAudioEngine::get()->playEffectAdvanced(
        (Mod::get()->getResourcesDir() / "you-died.mp3").string().c_str(),
        1.f, 1.f, 1.f, speed, 1.f, 1.f, 0, duration, fade, fade, false, 1, false, false, 1, 1, 0, 1
    );
}

BaseAnimation* Utils::createAnimation(Anim animation, CCNode* parentNode, PlayLayer* playLayer, AnimationDelegate* delegate, float speed) {
    switch (animation) {
        case Anim::YouDied: return YouDied::create(parentNode, playLayer, delegate, speed);
        case Anim::Bsod: return Bsod::create(parentNode, playLayer, delegate, speed);
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

float Utils::getSettingFloat(const std::string& animation, const std::string& setting, float defaultValue) {
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");
    
    if (object.contains(animation))
        return object[animation][setting].asDouble().unwrapOr(defaultValue);
        
    return defaultValue;
}

bool Utils::getSettingBool(const std::string& animation, const std::string& setting, bool defaultValue) {
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");
    
    if (object.contains(animation))
        return object[animation][setting].asBool().unwrapOr(defaultValue);
        
    return defaultValue;
}

void Utils::saveSetting(const std::string& animation, const std::string& setting, float value) {
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");
    matjson::Value newObject = object[animation];
    
    newObject[setting] = value;
    object[animation] = newObject;
    
    Mod::get()->setSavedValue("settings", object);
}

void Utils::saveSetting(const std::string& animation, const std::string& setting, bool value) {
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");
    matjson::Value newObject = object[animation];
    
    newObject[setting] = value;
    object[animation] = newObject;
    
    Mod::get()->setSavedValue("settings", object);
}

void Utils::setDefaults(const std::string& animation) {
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");

    object.erase(animation);
    
    Mod::get()->setSavedValue("settings", object);
}