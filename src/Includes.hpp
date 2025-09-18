#pragma once

using namespace geode::prelude;

#include "UI/PreviewDelegate.hpp"

inline const std::string vertexShader = R"(
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

enum SettingType {
    Empty,
    Toggle,
    Speed,
    Percent,
    AmongUsColor,
    Select,
    Folder,
    Scale,
    Text,
    PercentToggle
};

enum Anim {
    None = 1,
    Random = 2,
    YouDied = 3,
    Bsod = 4,
    AmongUs = 5,
    ToBeContinued = 6,
    Celeste = 7,
    Jumpscare = 9,
    Wasted = 10,
    Maro = 11,
    Ghost = 12,
    Undertale = 13,
    Minecraft = 14,
    Terraria = 15,
    CBFDetected = 16,
    Pop = 17,
    SpeechBubble = 18,
    What = 19,
    Congregation = 20,
    LevelComplete = 21,
    PracticeComplete = 22,
    NewBest = 23,
    Blood = 24,
    Poof = 25,
    FadeOut = 26,
};

struct NoobHSV {
    float hue = 0.f;
    float saturation = 1.f;
    float brightness = 1.f;
};

struct ExtraParams {
    int transition = 0;
    bool reverse = false;
    CCPoint position = {0, 0};
};

struct AnimationParams {
    CCNode* parentNode = nullptr;
    PlayLayer* playLayer = nullptr;
    PreviewDelegate* delegate = nullptr;
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
    bool isStopMusic = false;
    bool isNoStopMusic = false;
    bool isDelayNewBest = false;
    bool isNoSpawnEffect = false;
    bool isStopSoundsOnEnd = false;
};

struct AnimationSetting {
    std::string id;
    std::string name;
    std::string description;
    SettingType type;
    
    std::vector<std::string> elements;
    std::filesystem::path folder;
};

inline const std::unordered_map<std::string, float> globalFloatDefaults = {
    { "speed", 0.183673f },
    { "probability", 100.f }
};

inline const std::unordered_set<std::string> gloalBoolDefaults = {
    "play-sound-effects"
};

inline const std::unordered_map<int, std::unordered_map<std::string, std::string>> specificStringDefaults {
    { Anim::What, {
        { "top-text", "WHAT" },
        { "bottom-text", "what" }
    } }
};

inline const std::unordered_map<int, std::unordered_map<std::string, float>> specificFloatDefaults {
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
    } },
    { Anim::SpeechBubble, {
        { "scale-y", 1.f }
    } }
};

inline const std::unordered_map<int, std::unordered_map<std::string, bool>> specificBoolDefaults {
    { Anim::Celeste, { { "respawn-animation", true }, { "second-player", true }, { "shockwave", true } } },
    { Anim::CBFDetected, { { "use-level-name", true } } },
    { Anim::Maro, { { "second-player", true } } },
    { Anim::Undertale, { { "second-player", true } } },
    { Anim::Ghost, { { "second-player", true } } },
    { Anim::Pop, { { "second-player", true } } },
    { Anim::SpeechBubble, { { "stop-auto-restart", true } } },
    { Anim::ToBeContinued, { { "yellow-shader", true } } }
};

inline const std::array<DeathAnimation, 25> animations = {{
    { .id = Anim::None, .thumbnail = "none-thumbnail.png", .name = "None" },
    { .id = Anim::Random, .thumbnail = "random-thumbnail.png", .name = "Random" },
    { .id = Anim::YouDied, .thumbnail = "you-died-thumbnail.png", .name = "Dark Souls - You Died", .duration = 5.f, .isStopMusic = true, .isDelayNewBest = true },
    { .id = Anim::Bsod, .thumbnail = "bsod-thumbnail.png", .name = "Blue Screen of Death", .duration = 12.f, .isStopMusic = true  },
    { .id = Anim::AmongUs, .thumbnail = "among-us-thumbnail.png", .name = "Among Us", .duration = 3.f, .isStopMusic = true, .isDelayNewBest = true },
    { .id = Anim::Celeste, .thumbnail = "celeste-thumbnail.png", .name = "Celeste", .duration = 1.35f, .isNoDeathEffect = true, .isNoSpawnEffect = true },
    { .id = Anim::ToBeContinued, .thumbnail = "to-be-continued-thumbnail.png", .name = "To Be Continued", .duration = 5.f, .isNoDeathEffect = true, .isStopMusic = true },
    { .id = Anim::Jumpscare, .thumbnail = "jumpscare-thumbnail.png", .name = "Jumpscare", .duration = 0.8f, .isNoDeathSound = true, .isStopMusic = true },
    { .id = Anim::Wasted, .thumbnail = "wasted-thumbnail.png", .name = "GTA V - Wasted", .duration = 8.f, .isNoDeathSound = true, .isNoShakeEffect = true, .isSlowDown = true, .isStopMusic = true, .isDelayNewBest = true },
    { .id = Anim::Maro, .thumbnail = "maro-thumbnail.png", .name = "Super Mario Bros.", .duration = 3.f, .isNoDeathEffect = true, .isFreezeGameLayer = true, .isStopMusic = true },
    { .id = Anim::Ghost, .thumbnail = "ghost-thumbnail.png", .name = "Ghost", .duration = 1.2f },
    { .id = Anim::Undertale, .thumbnail = "undertale-thumbnail.png", .name = "Undertale", .duration = 17.f, .isNoDeathEffect = true, .isStopMusic = true, .isDelayNewBest = true },
    { .id = Anim::Minecraft, .thumbnail = "none-thumbnail.png", .name = "Minecraft", .duration = 10.f },
    { .id = Anim::Terraria, .thumbnail = "none-thumbnail.png", .name = "Terraria", .duration = 10.f },
    { .id = Anim::CBFDetected, .thumbnail = "cbf-detected-thumbnail.png", .name = "CBF Detected, Loser!", .duration = 1.f },
    { .id = Anim::Pop, .thumbnail = "pop-thumbnail.png", .name = "Pop", .duration = 1.1f, .isNoDeathEffect = true },
    { .id = Anim::SpeechBubble, .thumbnail = "speech-bubble-thumbnail.png", .name = "Speech Bubble", .duration = 5.f, .isNoDeathEffect = true, .isDelayNewBest = true },
    { .id = Anim::What, .thumbnail = "none-thumbnail.png", .name = "What", .duration = 4.5f, .isNoDeathEffect = true, .isDelayNewBest = true },   
    { .id = Anim::Congregation, .thumbnail = "none-thumbnail.png", .name = "Congregation Jumpscare", .duration = 4.45f, .isNoDeathEffect = true, .isNoStopMusic = true, .isDelayNewBest = true, .isStopSoundsOnEnd = true },
    { .id = Anim::LevelComplete, .thumbnail = "none-thumbnail.png", .name = "Level Complete", .duration = 3.9f, .isNoDeathEffect = true, .isNoStopMusic = true },
    { .id = Anim::PracticeComplete, .thumbnail = "none-thumbnail.png", .name = "Practice Complete", .duration = 2.3f, .isNoDeathEffect = true, .isNoStopMusic = true },
    { .id = Anim::NewBest, .thumbnail = "none-thumbnail.png", .name = "New Best", .duration = 1.8f },
    { .id = Anim::Blood, .thumbnail = "none-thumbnail.png", .name = "Blood", .duration = 10.f },
    { .id = Anim::Poof, .thumbnail = "none-thumbnail.png", .name = "Poof", .duration = 10.f, .isNoDeathEffect = true },
    { .id = Anim::FadeOut, .thumbnail = "none-thumbnail.png", .name = "Fade Out", .duration = 10.f },
}};

inline const std::array<AnimationSetting, 9> defaultSettings = {{
    { .id = "speed", .name = "Speed", .description = "The speed at which the animation plays.", .type = SettingType::Speed },
    { .id = "only-after", .name = "Only After", .description = "Play the animation only if you die after a certain percentage.", .type = SettingType::Percent },
    { .id = "probability", .name = "Probability", .description = "The probability that the animation will play.", .type = SettingType::Percent },
    { .id = "prevent-early-restart", .name = "Prevent Early Restart", .description = "Prevent restarting the level while the animation is playing.", .type = SettingType::Toggle },
    { .id = "stop-auto-restart", .name = "Stop Auto Restart", .description = "Prevent automatic restart after you die.", .type = SettingType::Toggle },
    { .id = "play-sound-effects", .name = "Play Sound Effects", .description = "Play any sound effects associated with the animation.", .type = SettingType::Toggle },
    { .id = "play-in-practice", .name = "Play in Practice", .description = "Play the animation in practice mode.", .type = SettingType::Toggle },
    { .id = "play-in-testmode", .name = "Play in Testmode", .description = "Play the animation in test mode.", .type = SettingType::Toggle },
    { .id = "only-on-new-best", .name = "Only on New Best", .description = "Play the animation only when achieving a new best.", .type = SettingType::Toggle }
}};

inline const std::unordered_map<int, std::vector<AnimationSetting>> extraSettings = {
    { Anim::AmongUs, {
        { .id = "animation", .name = "Kill Animation", .description = "The Among Us kill animation that plays on death.", .type = SettingType::Select, .elements = { "Random", "Gun", "Knife", "Neck", "Alien" } },
        { .id = "colors", .name = "Colors", .description = "The colors of the impostor and victim in the animation.", .type = SettingType::AmongUsColor, .elements = { "Player Colors", "Custom" } }
    } },
    { Anim::Celeste, {
        { .id = "second-player", .name = "Second Player", .description = "Play the death/respawn animation on the second player as well.", .type = SettingType::Toggle },
        { .id = "respawn-animation", .name = "Respawn Animation", .description = "Play the Celeste respawn animation.", .type = SettingType::Toggle },
        { .id = "shockwave", .name = "Shockwave", .description = "Play the shockwave effect.", .type = SettingType::Toggle },
        { .id = "transition", .name = "Transition", .description = "The Celeste transition to use in the animation.", .type = SettingType::Select, .elements = { "None", "Random", "Chapter 1", "Chapter 2", "Chapter 3", "Chapter 4" } }
    } },
    { Anim::Jumpscare, {
        { .id = "static", .name = "Static", .description = "Stop the image from zooming in.", .type = SettingType::Toggle },
        { .id = "disable-flashing", .name = "Disable Flashing", .description = "Stop the image from flashing repeatedly.", .type = SettingType::Toggle },
        { .id = "ignore-volume", .name = "Ignore Volume", .description = "Ignore the game's SFX volume.", .type = SettingType::Toggle },
        { .id = "fill-screen", .name = "Fill Screen", .description = "Make the image fill the screen (may crop the image).", .type = SettingType::Toggle },
        { .id = "images-folder", .name = "Images Folder", .description = "Folder containing the jumpscare images; one will be chosen at random.", .type = SettingType::Folder, .folder = Mod::get()->getSaveDir() / "jumpscare-images" },
        { .id = "sounds-folder", .name = "Sounds Folder", .description = "Folder containing the jumpscare sounds; one will be chosen at random.", .type = SettingType::Folder, .folder = Mod::get()->getSaveDir() / "jumpscare-sounds" }
    } },
    { Anim::Maro, {
        { .id = "use-nes-colors", .name = "Use NES Colors", .description = "", .type = SettingType::Toggle },
        { .id = "use-maro-sprite", .name = "Use Mario Sprite", .description = "", .type = SettingType::Toggle },
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle }
    } },
    { Anim::CBFDetected, {
        { .id = "use-level-name", .name = "Use Level Name", .description = "",. type = SettingType::Toggle },
        { .id = "include-loser", .name = "Include Loser! text", .description = "",. type = SettingType::Toggle },
        { .id = "yellow-color", .name = "Yellow Color", .description = "",. type = SettingType::Toggle }
    } },
    { Anim::Undertale, {
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle }
    } },
    { Anim::Ghost, {
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle }
    } },
    { Anim::Pop, {
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle }
    } },
    { Anim::ToBeContinued, {
        { .id = "yellow-shader", .name = "Yellow Shader", .description = "", .type = SettingType::Toggle }
    } },
    { Anim::SpeechBubble, {
        { .id = "scale-y", .name = "Scale Y", .description = "", .type = SettingType::Scale },
        { .id = "flip-x", .name = "Flip X", .description = "", .type = SettingType::Toggle },
        { .id = "flip-y", .name = "Flip Y", .description = "", .type = SettingType::Toggle },
        { .id = "hide-arrows", .name = "Hide Arrows", .description = "", .type = SettingType::Toggle },
        { .id = "hide-ui", .name = "Hide UI", .description = "", .type = SettingType::Toggle },
        { .id = "images-folder", .name = "Images Folder", .description = "Folder containing the images you have saved.", .type = SettingType::Folder, .folder = Mod::get()->getSaveDir() / "screenshots" }
    } },
    { Anim::What, {
        { .id = "top-text", .name = "Top Text", .description = "", .type = SettingType::Text },
        { .id = "bottom-text", .name = "Bottom Text", .description = "", .type = SettingType::Text }
    } },
    { Anim::NewBest, {
        { .id = "use-custom-percent", .name = "Use Custom Percent", .description = "", .type = SettingType::PercentToggle },
        { .type = SettingType::Empty },
    } },
};

inline const std::unordered_map<int, std::unordered_set<std::string>> blockedSettings = {
    { Anim::Congregation, { "stop-auto-restart", "play-sound-effects" } },
    { Anim::NewBest, { "only-on-new-best", "speed", "play-sound-effects" } },
    { Anim::PracticeComplete, { "play-sound-effects" } },
};