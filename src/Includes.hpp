#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class PreviewDelegate;

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
    PercentToggle,
    MonologueSelect,
    SpeechBubbleSelect,
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
    MrHippo = 27,
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
    ExtraParams extras = {};
    float speed = 1.f;
    Anim id = Anim::None;
};

struct DeathAnimation {
    int id = Anim::None;
    std::string thumbnail = "";
    std::string name = "";
    float duration = 1.f;
    float retryLayerDelay = 0.f;
    bool isNoDeathEffect = false;
    bool isNoDeathSound = false;
    bool isNoShakeEffect = false;
    bool isFreezeGameLayer = false;
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
    { "probability", 100.f },
};

inline const std::unordered_set<std::string> gloalBoolDefaults = {
    "play-sound-effects",
    "play-in-testmode",
    "second-player",
};

inline const std::unordered_map<int, std::unordered_map<std::string, std::string>> specificStringDefaults {
    { Anim::What, {
        { "top-text", "WHAT" },
        { "bottom-text", "how" },
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
        { "scale-y", 0.81f },
        { "speech-bubble", 1.f }
    } },
};

inline const std::unordered_map<int, std::unordered_map<std::string, bool>> specificBoolDefaults {
    { Anim::Celeste, { { "respawn-animation", true }, { "second-player", true }, { "shockwave", true }, { "speed-affects-respawn", true } } },
    { Anim::CBFDetected, { { "use-level-name", true } } },
    { Anim::ToBeContinued, { { "yellow-shader", true } } },
    { Anim::Blood, { { "blood-spatter", true }, { "play-in-practice", true } } },
    { Anim::Poof, { { "play-in-practice", true } } },
    { Anim::Ghost, { { "play-in-practice", true } } },
    { Anim::Pop, { { "play-in-practice", true } } },
    { Anim::FadeOut, { { "desaturate", true }, { "slow-down", true } } },
    { Anim::SpeechBubble, { { "show-ui", true } } },
};

inline const std::array<DeathAnimation, 26> animations = {{
    { .id = Anim::None, .thumbnail = "none-thumbnail.png", .name = "None" },
    { .id = Anim::Random, .thumbnail = "random-thumbnail.png", .name = "Random" },
    { .id = Anim::Poof, .thumbnail = "poof-thumbnail.png", .name = "Poof", .duration = 1.2f, .retryLayerDelay = 0.3f, .isNoDeathEffect = true },
    { .id = Anim::Pop, .thumbnail = "pop-thumbnail.png", .name = "Pop", .duration = 1.1f, .retryLayerDelay = 0.5f, .isNoDeathEffect = true },
    { .id = Anim::Blood, .thumbnail = "blood-thumbnail.png", .name = "Blood", .duration = 1.2f, .retryLayerDelay = 0.3f, .isNoDeathEffect = true },
    { .id = Anim::Ghost, .thumbnail = "ghost-thumbnail.png", .name = "Ghost", .duration = 1.2f, .retryLayerDelay = 0.3f },
    { .id = Anim::FadeOut, .thumbnail = "fade-out-thumbnail.png", .name = "Song Fade Out", .duration = 1.2f, .retryLayerDelay = 0.75f, .isNoStopMusic = true },
    { .id = Anim::YouDied, .thumbnail = "you-died-thumbnail.png", .name = "Dark Souls - You Died", .duration = 4.8f, .retryLayerDelay = 3.9f, .isStopMusic = true },
    { .id = Anim::Maro, .thumbnail = "maro-thumbnail.png", .name = "Super Mario Bros.", .duration = 3.f, .retryLayerDelay = 1.9f, .isNoDeathEffect = true, .isFreezeGameLayer = true, .isStopMusic = true },
    { .id = Anim::What, .thumbnail = "what-thumbnail.png", .name = "What", .duration = 4.5f, .retryLayerDelay = 4.1f, .isNoDeathEffect = true, .isStopMusic = true, .isDelayNewBest = true },   
    { .id = Anim::Wasted, .thumbnail = "wasted-thumbnail.png", .name = "GTA V - Wasted", .duration = 8.f, .retryLayerDelay = 7.7f, .isNoDeathSound = true, .isNoShakeEffect = true, .isStopMusic = true, .isDelayNewBest = true },
    { .id = Anim::AmongUs, .thumbnail = "among-us-thumbnail.png", .name = "Among Us", .duration = 3.f, .retryLayerDelay = 2.4f, .isStopMusic = true, .isDelayNewBest = true },
    { .id = Anim::Celeste, .thumbnail = "celeste-thumbnail.png", .name = "Celeste", .duration = 1.35f, .retryLayerDelay = 0.94f, .isNoDeathEffect = true, .isNoSpawnEffect = true },
    { .id = Anim::LevelComplete, .thumbnail = "level-complete-thumbnail.png", .name = "Level Complete", .duration = 3.9f, .retryLayerDelay = 3.1f, .isNoDeathEffect = true, .isNoStopMusic = true, .isDelayNewBest = true },
    { .id = Anim::PracticeComplete, .thumbnail = "practice-complete-thumbnail.png", .name = "Practice Complete", .duration = 2.3f, .retryLayerDelay = 1.5f, .isNoDeathEffect = true, .isNoStopMusic = true, .isDelayNewBest = true },
    { .id = Anim::NewBest, .thumbnail = "new-best-thumbnail.png", .name = "New Best", .duration = 2.f, .retryLayerDelay = 1.85f },
    { .id = Anim::Congregation, .thumbnail = "congregation-thumbnail.png", .name = "Congregation Jumpscare", .duration = 4.45f, .retryLayerDelay = 3.95f, .isNoDeathEffect = true, .isNoStopMusic = true, .isDelayNewBest = true, .isStopSoundsOnEnd = true },
    { .id = Anim::Bsod, .thumbnail = "bsod-thumbnail.png", .name = "Blue Screen of Death", .duration = 12.f, .retryLayerDelay = 11.5f, .isStopMusic = true  },
    { .id = Anim::Undertale, .thumbnail = "undertale-thumbnail.png", .name = "Undertale", .duration = 17.f, .isNoDeathEffect = true, .isStopMusic = true, .isDelayNewBest = true },
    { .id = Anim::Minecraft, .thumbnail = "minecraft-thumbnail.png", .name = "Minecraft", .duration = 1.2f, .retryLayerDelay = 0.4f, .isNoDeathEffect = true },
    { .id = Anim::Terraria, .thumbnail = "terraria-thumbnail.png", .name = "Terraria", .duration = 10.f, .retryLayerDelay = 9.2f, .isNoDeathSound = true, .isNoShakeEffect = true },
    { .id = Anim::MrHippo, .thumbnail = "mr-hippo-thumbnail.png", .name = "Mr. Hippo", .isStopMusic = true },
    { .id = Anim::ToBeContinued, .thumbnail = "to-be-continued-thumbnail.png", .name = "To Be Continued", .duration = 5.f, .retryLayerDelay = 4.7f, .isNoDeathEffect = true, .isStopMusic = true },
    { .id = Anim::Jumpscare, .thumbnail = "jumpscare-thumbnail.png", .name = "Jumpscare", .duration = 0.8f, .retryLayerDelay = 0.85f, .isNoDeathSound = true, .isStopMusic = true },
    { .id = Anim::CBFDetected, .thumbnail = "cbf-detected-thumbnail.png", .name = "CBF Detected, Loser!", .duration = 1.f },
    { .id = Anim::SpeechBubble, .thumbnail = "speech-bubble-thumbnail.png", .name = "Speech Bubble", .duration = 1.f, .retryLayerDelay = 0.4f, .isNoDeathEffect = true, .isDelayNewBest = true },
}};

inline const std::array<AnimationSetting, 8> defaultSettings = {{
    { .id = "speed", .name = "Speed", .description = "The speed at which the animation plays.", .type = SettingType::Speed },
    { .id = "only-after", .name = "Only After", .description = "Play the animation only if you die after a certain percentage.", .type = SettingType::Percent },
    { .id = "probability", .name = "Probability", .description = "The probability that the animation will play.", .type = SettingType::Percent },
    { .id = "prevent-early-restart", .name = "Prevent Early Restart", .description = "Prevent restarting the level while the animation is playing.", .type = SettingType::Toggle },
    { .id = "play-sound-effects", .name = "Play Sound Effects", .description = "Play any sound effects associated with the animation.", .type = SettingType::Toggle },
    { .id = "play-in-practice", .name = "Play in Practice", .description = "Play the animation in practice mode.", .type = SettingType::Toggle },
    { .id = "play-in-testmode", .name = "Play in Testmode", .description = "Play the animation in test mode.", .type = SettingType::Toggle },
    { .id = "only-on-new-best", .name = "Only on New Best", .description = "Play the animation only when achieving a new best.", .type = SettingType::Toggle },
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
        { .id = "speed-affects-respawn", .name = "Speed Affects Respawn", .description = "The Speed setting will affect the respawn animation (if enabled) as well.", .type = SettingType::Toggle },
        { .id = "transition", .name = "Transition", .description = "The Celeste transition to use in the animation.", .type = SettingType::Select, .elements = { "None", "Random", "Chapter 1", "Chapter 2", "Chapter 3", "Chapter 4", "Chapter 5", "Chapter 6", "Chapter 7", "Chapter 8" } }
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
        { .id = "use-nes-colors", .name = "Use NES Colors", .description = "Uses the colors from the original NES.", .type = SettingType::Toggle },
        { .id = "use-maro-sprite", .name = "Use Mario Sprite", .description = "Uses the sprite of Mario instead of a pixel-art version of the player.", .type = SettingType::Toggle },
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle }
    } },
    { Anim::CBFDetected, {
        { .id = "use-level-name", .name = "Use Level Name", .description = "Use the name of the level you're currently playing instead of Nullscapes.",. type = SettingType::Toggle },
        { .id = "include-loser", .name = "Include Loser! text", .description = "In later versions of the level, the text got changed to remove \"Loser\", this setting brings it back.",. type = SettingType::Toggle },
        { .id = "yellow-color", .name = "Yellow Color", .description = "The red text was yellow at some point, this setting brings it back.",. type = SettingType::Toggle }
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
        { .id = "yellow-shader", .name = "Yellow Shader", .description = "Enables the yellow filter you usually see in these memes.", .type = SettingType::Toggle }
    } },
    { Anim::SpeechBubble, {
        { .id = "show-ui", .name = "Show UI", .description = "", .type = SettingType::Toggle },
        { .id = "scale-y", .name = "Scale Y", .description = "", .type = SettingType::Scale },
        { .id = "flip-x", .name = "Flip X", .description = "", .type = SettingType::Toggle },
        { .id = "flip-y", .name = "Flip Y", .description = "", .type = SettingType::Toggle },
        { .id = "speech-bubble", .name = "Speech Bubble", .description = "The speech bubble image that will be used by default when dying.", .type = SettingType::SpeechBubbleSelect, .elements = { "Random" } },
        { .id = "", .name = "Speech Bubbles Folder", .description = "Folder containing the speech bubble images.", .type = SettingType::Folder, .folder = Mod::get()->getSaveDir() / "speech-bubbles" },
        { .id = "", .name = "Images Folder", .description = "Folder containing the images you have saved.", .type = SettingType::Folder, .folder = Mod::get()->getSaveDir() / "screenshots" }
    } },
    { Anim::What, {
        { .id = "target-p2", .name = "Target Player 2", .description = "Zoom into the second player if possible.", .type = SettingType::Toggle },
        { .id = "top-text", .name = "Top Text", .description = "", .type = SettingType::Text },
        { .id = "bottom-text", .name = "Bottom Text", .description = "", .type = SettingType::Text }
    } },
    { Anim::NewBest, {
        { .id = "play-in-platformer", .name = "Play in platformer", .description = "New best in platformer (?).", .type = SettingType::Toggle },
        { .id = "use-custom-percent", .name = "Use Custom Percent", .description = "Show a constant % value instead of where you actually died.", .type = SettingType::PercentToggle },
        { .type = SettingType::Empty },
    } },
    { Anim::Poof, {
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle }
    } },
    { Anim::Blood, {
        { .id = "second-player", .name = "Second Player", .description = "Play the animation on the second player as well.", .type = SettingType::Toggle },
        { .id = "blood-spatter", .name = "Blood Spatter", .description = "Show the blood spatter on screen.", .type = SettingType::Toggle }
    } },
    { Anim::MrHippo, {
        { .id = "monologue", .name = "Monologue", .description = "What Mr. Hippo will yap about.", .type = SettingType::MonologueSelect, .elements = { "Random", "Monologue 1", "Monologue 2", "Monologue 3", "Monologue 4" } }
    } },
    { Anim::FadeOut, {
        { .id = "slow-down", .name = "Slow Down", .description = "Slow down the game along with the sound.", .type = SettingType::Toggle },
        { .id = "desaturate", .name = "Desaturate", .description = "Slightly desaturate the colors for more immersion.", .type = SettingType::Toggle },
    } },
    { Anim::Minecraft, {
        { .id = "no-overlay", .name = "No Overlay", .description = "Disables the \"You Died!\" overlay, leaving only the player animation.", .type = SettingType::Toggle },
        { .id = "no-cooldown", .name = "No Cooldown", .description = "Disables the 1s button cooldown in overlay.", .type = SettingType::Toggle },
    } },
};

inline const std::unordered_map<int, std::unordered_set<std::string>> blockedSettings = {
    { Anim::Congregation, { "play-sound-effects" } },
    { Anim::NewBest, { "only-on-new-best", "speed", "play-sound-effects" } },
    { Anim::PracticeComplete, { "play-sound-effects" } },
    { Anim::MrHippo, { "play-sound-effects", "speed" } },
    { Anim::Minecraft, { "speed" } },
    { Anim::Undertale, { "speed" } },
    { Anim::SpeechBubble, { "speed", "play-sound-effects" } },
};