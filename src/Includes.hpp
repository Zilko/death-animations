#pragma once

using namespace geode::prelude;

class PreviewEvents {
  
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

#define ANIMATION_CHECK(ANIMATION) \
    if (animation == Anim::ANIMATION) \
        return ANIMATION::create(params);


#define DEFINE_CREATE(CLASS)                                                 \
    CLASS(const AnimationParams& params) \
        : BaseAnimation(params) {}  \
                                                                    \
    static CLASS* create(const AnimationParams& params) { \
        CLASS* ret = new CLASS(params); \
        ret->init();                                                              \
        ret->autorelease();                                                       \
        return ret;                                                               \
    }

enum SettingType {
    Toggle,
    Speed,
    Percent,
    AmongUsColor,
    Select,
    JumpscareFolder
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
    Jumpscare = 9,
    Wasted = 10,
    Maro = 11,
    Ghost = 12,
    Undertale = 13,
    Minecraft = 14,
    Terraria = 15,
    CBFDetected = 16
};

struct ExtraParams {
    int transition = 0;
    bool reverse = false;
};

struct AnimationParams {
    CCNode* parentNode = nullptr;
    PlayLayer* playLayer = nullptr;
    PreviewEvents* delegate = nullptr;
    float speed = 1.f;
    ExtraParams extras = {};
};

struct DeathAnimation {
    int id = Anim::None;
    std::string thumbnail = "";
    std::string name = "";
    float duration = 1.f;
    bool isNoDeathEffect = false;
    bool isNoDeathSound = false;
};

struct AnimationSetting {
    std::string id;
    std::string name;
    std::string description;
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
        { "r1", 243.f },
        { "g1", 65.f },
        { "b1", 44.f },
        { "r2", 14.f },
        { "g2", 53.f },
        { "b2", 235.f }
    } },
    { Anim::Celeste, {
        { "transition", 1.f }
    } }
};

static const std::unordered_map<int, std::unordered_map<std::string, bool>> specificBoolDefaults {
    { Anim::Celeste, { { "respawn-animation", true }, { "second-player", true } } },
    { Anim::Jumpscare, { { "flashing", true } } }
};

static const std::array<DeathAnimation, 16> animations = {
    DeathAnimation{ .id = Anim::None, .thumbnail = "none-thumbnail.png", .name = "None" },
    DeathAnimation{ .id = Anim::Random, .thumbnail = "random-thumbnail.png", .name = "Random" },
    DeathAnimation{ .id = Anim::YouDied, .thumbnail = "you-died-thumbnail.png", .name = "Dark Souls - You Died", .duration = 5.f},
    DeathAnimation{ .id = Anim::Bsod, .thumbnail = "bsod-thumbnail.png", .name = "Blue Screen of Death", .duration = 12.f },
    DeathAnimation{ .id = Anim::AmongUs, .thumbnail = "among-us-thumbnail.png", .name = "Among Us", .duration = 3.f },
    DeathAnimation{ .id = Anim::Celeste, .thumbnail = "among-us-thumbnail.png", .name = "Celeste", .duration = 1.35f, .isNoDeathEffect = true },
    DeathAnimation{ .id = Anim::ToBeContinued, .thumbnail = "among-us-thumbnail.png", .name = "To Be Continued", .duration = 5.f, .isNoDeathEffect = true },
    DeathAnimation{ .id = Anim::Wii, .thumbnail = "among-us-thumbnail.png", .name = "Wii", .duration = 621.f },
    DeathAnimation{ .id = Anim::Jumpscare, .thumbnail = "jumpscare-thumbnail.png", .name = "Jumpscare", .duration = 0.8f, .isNoDeathSound = true },
    DeathAnimation{ .id = Anim::Wasted, .thumbnail = "you-died-thumbnail.png", .name = "GTA V - Wasted", .duration = 10.f, .isNoDeathSound = true },
    DeathAnimation{ .id = Anim::Maro, .thumbnail = "among-us-thumbnail.png", .name = "GTA V - Wasted", .duration = 10.f, .isNoDeathSound = true },
    DeathAnimation{ .id = Anim::Ghost, .thumbnail = "you-died-thumbnail.png", .name = "GTA V - Wasted", .duration = 10.f, .isNoDeathSound = true },
    DeathAnimation{ .id = Anim::Undertale, .thumbnail = "among-us-thumbnail.png", .name = "GTA V - Wasted", .duration = 10.f, .isNoDeathSound = true },
    DeathAnimation{ .id = Anim::Minecraft, .thumbnail = "among-us-thumbnail.png", .name = "GTA V - Wasted", .duration = 10.f, .isNoDeathSound = true },
    DeathAnimation{ .id = Anim::Terraria, .thumbnail = "among-us-thumbnail.png", .name = "GTA V - Wasted", .duration = 10.f, .isNoDeathSound = true },
    DeathAnimation{ .id = Anim::CBFDetected, .thumbnail = "among-us-thumbnail.png", .name = "GTA V - Wasted", .duration = 10.f, .isNoDeathSound = true }
};

static const std::array<AnimationSetting, 8> defaultSettings = {
    AnimationSetting{ .id = "speed", .name = "Speed", .description = "The speed at which the animation will play.", .type = SettingType::Speed },
    AnimationSetting{ .id = "only-after", .name = "Only After", .description = "Only play the animation if you die after a certain percent.", .type = SettingType::Percent },
    AnimationSetting{ .id = "probability", .name = "Probability", .description = "The chance the animation has to play.", .type = SettingType::Percent },
    AnimationSetting{ .id = "prevent-early-restart", .name = "Prevent Early Restart", .description = "Stop you from restarting with R while the animation is playing.", .type = SettingType::Toggle },
    AnimationSetting{ .id = "stop-auto-restart", .name = "Stop Auto Restart", .description = "Don't restart automatically after you die.", .type = SettingType::Toggle },
    AnimationSetting{ .id = "play-sound-effects", .name = "Play Sound Effects", .description = "Play any sound effects the animation might have.", .type = SettingType::Toggle },
    AnimationSetting{ .id = "play-on-practice", .name = "Play On Practice", .description = "Whether to play the animation on practice or not.", .type = SettingType::Toggle },
    AnimationSetting{ .id = "only-on-new-best", .name = "Only On New Best ", .description = "Only play the animation on a new best.", .type = SettingType::Toggle }
};

static const std::unordered_map<int, std::vector<AnimationSetting>> extraSettings = {
    { Anim::AmongUs, {
        { .id = "animation", .name = "Kill Animation", .description = "The among us kill animation that will play on death.", .type = SettingType::Select, .elements = { "Random", "Gun", "Knife", "Neck", "Alien" } },
        { .id = "colors", .name = "Colors", .description = "The colors that the impostor and victim will be in the animation.", .type = SettingType::AmongUsColor, .elements = { "Player Colors", "Custom" } }
    } },
    { Anim::Celeste, {
        { .id = "transition", .name = "Transition", .description = "The celeste transition to use in the animation.", .type = SettingType::Select, .elements = { "None", "Random", "Chapter 1", "Chapter 2", "Chapter 3", "Chapter 4" } },
        { .id = "respawn-animation", .name = "Respawn Animation", .description = "Play the celeste respawn animation.", .type = SettingType::Toggle },
        { .id = "second-player", .name = "Second Player", .description = "Play the death/respawn animation on the second player too.", .type = SettingType::Toggle }
    } },
    { Anim::Jumpscare, {
        { .id = "flashing", .name = "Flashing", .description = "Make the image flash repeatedly.", .type = SettingType::Toggle },
        { .id = "ignore-volume", .name = "Ignore Volume", .description = "Ignores the game's SFX volume.", .type = SettingType::Toggle },
        { .id = "images-folder", .name = "Images Folder", .description = "Ignores the game's SFX volume.", .type = SettingType::JumpscareFolder },
        { .id = "sounds-folder", .name = "Sounds Folder", .description = "Ignores the game's SFX volume.", .type = SettingType::JumpscareFolder }
    } }
};
