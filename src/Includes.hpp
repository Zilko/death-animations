#pragma once

using namespace geode::prelude;

class AnimationDelegate {
  
public:

    virtual CCNodeRGBA* getPlayer() { return nullptr; }
    
};

static const std::string vertexShader = R"(
    attribute vec4 a_position;
    attribute vec2 a_texCoord;
    attribute vec4 a_color;
    
    #ifdef GL_ES
    varying lowp vec4 v_fragmentColor;
    varying mediump vec2 v_texCoord;
    #else
    varying vec4 v_fragmentColor;
    varying vec2 v_texCoord;
    #endif
    
    void main() {
        gl_Position = CC_MVPMatrix * a_position;
        v_fragmentColor = a_color;
        v_texCoord = a_texCoord;
    }
)";

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
    Toggle,
    Speed,
    Percent,
    AmongUsColor,
    Select
};

enum Anim {
    None = 1,
    Random = 2,
    YouDied = 3,
    Bsod = 4,
    AmongUs = 5,
    ToBeContinued = 6,
    Celeste = 7,
    Wii = 8,
    HollowKnight = 9
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
    
    std::vector<std::string> elements;
};

static const std::unordered_map<std::string, float> globalFloatDefaults = {
    { "speed", 0.230769f },
    { "probability", 100.f }
};

static const std::unordered_set<std::string> gloalBoolDefaults = {
    "play-sound-effects"
};

static const std::unordered_map<int, std::unordered_map<std::string, float>> specificFloatDefaults {
    { Anim::AmongUs, {
        { "r1", 273.f },
        { "g1", 95.f },
        { "b1", 74.f },
        { "r2", 14.f },
        { "g2", 53.f },
        { "b2", 235.f }
    } }
};

static const std::unordered_map<int, std::unordered_map<std::string, bool>> specificBoolDefaults {
    { Anim::Celeste, { { "respawn-animation", true }, { "second-player", true } } }
};

static const std::array<DeathAnimation, 9> animations = {
    DeathAnimation{ .id = Anim::None, .thumbnail = "none-thumbnail.png", .name = "None" },
    DeathAnimation{ .id = Anim::Random, .thumbnail = "random-thumbnail.png", .name = "Random" },
    DeathAnimation{ .id = Anim::YouDied, .thumbnail = "you-died-thumbnail.png", .name = "Dark Souls - YOU DIED", .duration = 5.f},
    DeathAnimation{ .id = Anim::Bsod, .thumbnail = "bsod-thumbnail.png", .name = "Blue Screen of Death", .duration = 12.f },
    DeathAnimation{ .id = Anim::AmongUs, .thumbnail = "among-us-thumbnail.png", .name = "Among Us", .duration = 3.f },
    DeathAnimation{ .id = Anim::Celeste, .thumbnail = "among-us-thumbnail.png", .name = "Celeste", .duration = 2.f, .stopDeathEffect = true },
    DeathAnimation{ .id = Anim::ToBeContinued, .thumbnail = "among-us-thumbnail.png", .name = "To Be Continued", .duration = 5.f, .stopDeathEffect = true },
    DeathAnimation{ .id = Anim::Wii, .thumbnail = "among-us-thumbnail.png", .name = "Wii", .duration = 621.f },
    DeathAnimation{ .id = Anim::HollowKnight, .thumbnail = "among-us-thumbnail.png", .name = "Hollow Knight", .duration = 621.f }
};

static const std::array<AnimationSetting, 8> defaultSettings = {
    AnimationSetting{ .id = "speed", .name = "Speed", .type = SettingType::Speed },
    AnimationSetting{ .id = "only-after", .name = "Only After", .type = SettingType::Percent },
    AnimationSetting{ .id = "probability", .name = "Probability", .type = SettingType::Percent },
    AnimationSetting{ .id = "prevent-early-restart", .name = "Prevent Early Restart", .type = SettingType::Toggle },
    AnimationSetting{ .id = "stop-auto-restart", .name = "Stop Auto Restart", .type = SettingType::Toggle },
    AnimationSetting{ .id = "play-sound-effects", .name = "Play Sound Effects", .type = SettingType::Toggle },
    AnimationSetting{ .id = "play-on-practice", .name = "Play On Practice", .type = SettingType::Toggle },
    AnimationSetting{ .id = "only-on-new-best", .name = "Only On New Best ", .type = SettingType::Toggle }
};

static const std::unordered_map<int, std::vector<AnimationSetting>> extraSettings = {
    { Anim::AmongUs, {
        { .id = "animation", .name = "Kill Animation", .type = SettingType::Select, .elements = { "Random", "Gun", "Knife", "Neck", "Alien" } },
        { .id = "colors", .name = "Colors", .type = SettingType::AmongUsColor, .elements = { "Player Colors", "Custom" } }
    } },
    { Anim::Celeste, {
        { .id = "transition", .name = "Transition", .type = SettingType::Select, .elements = { "Random", "Chapter 1", "Chapter 2", "Chapter 3", "Chapter 4", "Chapter 5", "Chapter 6", "Chapter 7", "Chapter 8", "Chapter 9" } },
        { .id = "respawn-animation", .name = "Respawn Animation", .type = SettingType::Toggle },
        { .id = "second-player", .name = "Second Player", .type = SettingType::Toggle }
    } },
};
