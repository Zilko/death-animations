#pragma once

using namespace geode::prelude;

class AnimationDelegate {
  
public:

    virtual void event() {}
    
};

#define DEFINE_CREATE(CLASS_NAME)                                                 \
    CLASS_NAME(CCNode* parentNode, PlayLayer* playLayer, AnimationDelegate* delegate, float speed) \
        : BaseAnimation(parentNode, playLayer, delegate, speed) {}  \
                                                                    \
    static CLASS_NAME* create(CCNode* parentNode, PlayLayer* playLayer, AnimationDelegate* delegate, float speed) { \
        CLASS_NAME* ret = new CLASS_NAME(parentNode, playLayer, delegate, speed); \
        ret->init();                                                              \
        ret->autorelease();                                                       \
        return ret;                                                               \
    }

enum SettingType {
    Toggle = 1,
    Speed = 2,
    Percent = 3
};

enum Anim {
    None = 1,
    Random = 2,
    YouDied = 3,
    Bsod = 4,
    AmongUs = 5
};

struct DeathAnimation {
    int id = Anim::None;
    std::string thumbnail = "";
    std::string name = "";
    float duration = 1.f;
    bool stopDeathEffect = false;
};

struct AnimationSetting {
    std::string id;
    std::string name;
    SettingType type;
};

const std::vector<DeathAnimation> animations = {
    { .id = Anim::None, .thumbnail = "none-thumbnail.png", .name = "None" },
    { .id = Anim::Random, .thumbnail = "random-thumbnail.png", .name = "Random" },
    { .id = Anim::YouDied, .thumbnail = "you-died-thumbnail.png", .name = "Dark Souls - YOU DIED", .duration = 5.f},
    { .id = Anim::Bsod, .thumbnail = "bsod-thumbnail.png", .name = "Blue Screen of Death", .duration = 12.f },
    { .id = Anim::AmongUs, .thumbnail = "bsod-thumbnail.png", .name = "Among Us", .duration = 6.f }
};

const std::vector<AnimationSetting> defaultSettings = {
    { "speed", "Speed", SettingType::Speed },
    { "only-after", "Only After", SettingType::Percent },
    { "prevent-early-restart", "Prevent Early Restart", SettingType::Toggle },
    { "stop-auto-restart", "Stop Auto Restart", SettingType::Toggle },
    { "play-sound-effects", "Play Sound Effects", SettingType::Toggle },
    { "play-on-practice", "Play On Practice", SettingType::Toggle },
    // { "play-on-practice", "NEW BEST", SettingType::Toggle }
    // { "play-on-practice", "CHANCE OF HAPPENING", SettingType::Toggle }
};

const std::unordered_map<int, std::vector<AnimationSetting>> extraSettings = {
};
