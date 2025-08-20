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
    CBFDetected = 16,
    Pop = 17
};

struct ExtraParams {
    int transition = 0;
    bool reverse = false;
    CCPoint position = {0, 0};
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
    bool isNoShakeEffect = false;
    bool isFreezeGameLayer = false;
    bool isSlowDown = false;
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
    } },
    { Anim::Jumpscare, {
        { "probability", 10.f }
    } }
};

static const std::unordered_map<int, std::unordered_map<std::string, bool>> specificBoolDefaults {
    { Anim::Celeste, { { "respawn-animation", true }, { "second-player", true } } },
    { Anim::CBFDetected, { { "use-level-name", true } } },
    { Anim::Maro, { { "second-player", true } } },
    { Anim::Undertale, { { "second-player", true } } },
    { Anim::Ghost, { { "second-player", true } } },
    { Anim::Pop, { { "second-player", true } } }
};

static const std::array<DeathAnimation, 17> animations = {
    DeathAnimation{ .id = Anim::None, .thumbnail = "none-thumbnail.png", .name = "None" },
    DeathAnimation{ .id = Anim::Random, .thumbnail = "random-thumbnail.png", .name = "Random" },
    DeathAnimation{ .id = Anim::YouDied, .thumbnail = "you-died-thumbnail.png", .name = "Dark Souls - You Died", .duration = 5.f},
    DeathAnimation{ .id = Anim::Bsod, .thumbnail = "bsod-thumbnail.png", .name = "Blue Screen of Death", .duration = 12.f },
    DeathAnimation{ .id = Anim::AmongUs, .thumbnail = "among-us-thumbnail.png", .name = "Among Us", .duration = 3.f },
    DeathAnimation{ .id = Anim::Celeste, .thumbnail = "celeste-thumbnail.png", .name = "Celeste", .duration = 1.35f, .isNoDeathEffect = true },
    DeathAnimation{ .id = Anim::ToBeContinued, .thumbnail = "to-be-continued-thumbnail.png", .name = "To Be Continued", .duration = 5.f, .isNoDeathEffect = true },
    DeathAnimation{ .id = Anim::Wii, .thumbnail = "none-thumbnail.png", .name = "Wii", .duration = 621.f },
    DeathAnimation{ .id = Anim::Jumpscare, .thumbnail = "jumpscare-thumbnail.png", .name = "Jumpscare", .duration = 0.8f, .isNoDeathSound = true },
    DeathAnimation{ .id = Anim::Wasted, .thumbnail = "wasted-thumbnail.png", .name = "GTA V - Wasted", .duration = 8.f, .isNoDeathSound = true, .isNoShakeEffect = true, .isSlowDown = true },
    DeathAnimation{ .id = Anim::Maro, .thumbnail = "maro-thumbnail.png", .name = "Super Mario Bros.", .duration = 3.f, .isNoDeathEffect = true, .isFreezeGameLayer = true },
    DeathAnimation{ .id = Anim::Ghost, .thumbnail = "ghost-thumbnail.png", .name = "Ghost", .duration = 1.2f },
    DeathAnimation{ .id = Anim::Undertale, .thumbnail = "undertale-thumbnail.png", .name = "Undertale", .duration = 17.f, .isNoDeathEffect = true },
    DeathAnimation{ .id = Anim::Minecraft, .thumbnail = "none-thumbnail.png", .name = "Minecraft", .duration = 10.f },
    DeathAnimation{ .id = Anim::Terraria, .thumbnail = "none-thumbnail.png", .name = "Terraria", .duration = 10.f },
    DeathAnimation{ .id = Anim::CBFDetected, .thumbnail = "cbf-detected-thumbnail.png", .name = "CBF Detected, Loser!", .duration = 1.f },
    DeathAnimation{ .id = Anim::Pop, .thumbnail = "none-thumbnail.png", .name = "Pop", .duration = 1.f, .isNoDeathEffect = true }
};

static const std::array<AnimationSetting, 9> defaultSettings = {
    AnimationSetting{ .id = "speed", .name = "Speed", .description = "The speed at which the animation plays.", .type = SettingType::Speed },
    AnimationSetting{ .id = "only-after", .name = "Only After", .description = "Play the animation only if you die after a certain percentage.", .type = SettingType::Percent },
    AnimationSetting{ .id = "probability", .name = "Probability", .description = "The probability that the animation will play.", .type = SettingType::Percent },
    AnimationSetting{ .id = "prevent-early-restart", .name = "Prevent Early Restart", .description = "Prevent restarting the level while the animation is playing.", .type = SettingType::Toggle },
    AnimationSetting{ .id = "stop-auto-restart", .name = "Stop Auto Restart", .description = "Prevent automatic restart after you die.", .type = SettingType::Toggle },
    AnimationSetting{ .id = "play-sound-effects", .name = "Play Sound Effects", .description = "Play any sound effects associated with the animation.", .type = SettingType::Toggle },
    AnimationSetting{ .id = "play-in-practice", .name = "Play in Practice", .description = "Play the animation in practice mode.", .type = SettingType::Toggle },
    AnimationSetting{ .id = "play-in-testmode", .name = "Play in Testmode", .description = "Play the animation in test mode.", .type = SettingType::Toggle },
    AnimationSetting{ .id = "only-on-new-best", .name = "Only on New Best", .description = "Play the animation only when achieving a new best.", .type = SettingType::Toggle }
};

static const std::unordered_map<int, std::vector<AnimationSetting>> extraSettings = {
    { Anim::AmongUs, {
        { .id = "animation", .name = "Kill Animation", .description = "The Among Us kill animation that plays on death.", .type = SettingType::Select, .elements = { "Random", "Gun", "Knife", "Neck", "Alien" } },
        { .id = "colors", .name = "Colors", .description = "The colors of the impostor and victim in the animation.", .type = SettingType::AmongUsColor, .elements = { "Player Colors", "Custom" } }
    } },
    { Anim::Celeste, {
        { .id = "transition", .name = "Transition", .description = "The Celeste transition to use in the animation.", .type = SettingType::Select, .elements = { "None", "Random", "Chapter 1", "Chapter 2", "Chapter 3", "Chapter 4" } },
        { .id = "respawn-animation", .name = "Respawn Animation", .description = "Play the Celeste respawn animation.", .type = SettingType::Toggle },
        { .id = "second-player", .name = "Second Player", .description = "Play the death/respawn animation on the second player as well.", .type = SettingType::Toggle }
    } },
    { Anim::Jumpscare, {
        { .id = "static", .name = "Static", .description = "Stop the image from zooming in.", .type = SettingType::Toggle },
        { .id = "disable-flashing", .name = "Disable Flashing", .description = "Stop the image from flashing repeatedly.", .type = SettingType::Toggle },
        { .id = "ignore-volume", .name = "Ignore Volume", .description = "Ignore the game's SFX volume.", .type = SettingType::Toggle },
        { .id = "fill-screen", .name = "Fill Screen", .description = "Make the image fill the screen (may crop the image).", .type = SettingType::Toggle },
        { .id = "images-folder", .name = "Images Folder", .description = "Folder containing the jumpscare images; one will be chosen at random.", .type = SettingType::JumpscareFolder },
        { .id = "sounds-folder", .name = "Sounds Folder", .description = "Folder containing the jumpscare sounds; one will be chosen at random.", .type = SettingType::JumpscareFolder }
    } },
    { Anim::Maro, {
        { .id = "use-nes-colors", .name = "Use NES Colors", .description = "", .type = SettingType::Toggle },
        { .id = "use-maro-sprite", .name = "Use Mario Sprite", .description = "", .type = SettingType::Toggle },
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle }
    } },
    { Anim::CBFDetected, {
        { .id = "use-level-name", .name = "Use Level Name", .description = "",. type = SettingType::Toggle }
    } },
    { Anim::Undertale, {
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle }
    } },
    { Anim::Ghost, {
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle }
    } },
    { Anim::Pop, {
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle }
    } }
};