#include "Utils.hpp"
#include "SoundManager.hpp"

#include "../Animations/YouDied.hpp"
#include "../Animations/Bsod.hpp"
#include "../Animations/AmongUs.hpp"
#include "../Animations/Celeste.hpp"
#include "../Animations/ToBeContinued.hpp"
#include "../Animations/Jumpscare.hpp"
#include "../Animations/Wasted.hpp"
#include "../Animations/Maro.hpp"
#include "../Animations/Ghost.hpp"
#include "../Animations/Undertale.hpp"
#include "../Animations/Minecraft.hpp"
#include "../Animations/Terraria.hpp"
#include "../Animations/CBFDetected.hpp"
#include "../Animations/Pop.hpp"
#include "../Animations/SpeechBubble.hpp"
#include "../Animations/What.hpp"
#include "../Animations/Congregation.hpp"
#include "../Animations/LevelComplete.hpp"
#include "../Animations/PracticeComplete.hpp"
#include "../Animations/NewBest.hpp"
#include "../Animations/Blood.hpp"
#include "../Animations/Poof.hpp"
#include "../Animations/FadeOut.hpp"
#include "../Animations/MrHippo.hpp"

#include <random>

int Utils::getRandomInt(int min, int max) {
    static std::mt19937 gen(std::random_device{}());

    std::uniform_int_distribution<> dis(min, max);
    
    return dis(gen);
}

std::filesystem::path Utils::getRandomFile(const std::filesystem::path& folder, const std::unordered_set<std::string> validExtensions) {
    const std::vector<std::filesystem::path>& files = getAllFilesFromFolder(folder, validExtensions);

    if (files.empty()) return std::filesystem::path{};

    return files[getRandomInt(0, static_cast<int>(files.size()) - 1)];
}

void Utils::playSound(Anim anim, const std::string& sound, float speed, int fade, int duration, float volume, bool loop) {
    playSound(anim, sound, speed, fade, fade, duration, volume, loop);
}

void Utils::playSound(Anim anim, const std::string& sound, float speed, int fadeIn, int fadeOut, int duration, float volume, bool loop, int start) {
    if (!Utils::getSettingBool(anim, "play-sound-effects"))
        return;
                
    duration = static_cast<int>(duration / speed / 1000.f) * 1000;
    fadeIn /= speed;
    fadeOut /= speed;
    
    if (duration < 1000) duration = 1000;
            
    FMODAudioEngine::get()->playEffectAdvanced(
        utils::string::pathToString(Mod::get()->getResourcesDir() / sound).c_str(),
        0.f, 0.f, volume, speed, false, false, start, duration, fadeIn, fadeOut, loop, 0, false, false, 0, 0, 0, 0
    );
}

void Utils::playSound(Anim anim, const std::string& sound, float speed, float volume) {
    playSound(anim, speed, volume, Mod::get()->getResourcesDir() / sound);
}

void Utils::playSound(Anim anim, float speed, float volume, const std::filesystem::path& sound) {
    if (Utils::getSettingBool(anim, "play-sound-effects"))
        FMODAudioEngine::get()->playEffect(utils::string::pathToString(sound).c_str(), speed, 1.f, volume);
}

SoundUpdater* Utils::playSoundManual(Anim anim, const std::string& sound, float speed, float volume) {
    return playSoundManual(anim, speed, volume, Mod::get()->getResourcesDir() / sound);
}

SoundUpdater* Utils::playSoundManual(Anim anim, float speed, float volume, const std::filesystem::path& sound) {
    if (!Utils::getSettingBool(anim, "play-sound-effects"))
        return nullptr;
            
    FMOD::System* system = FMODAudioEngine::get()->m_system;
    FMOD::Sound* souwnd;
    FMOD::Channel* channel;
    
    system->createSound(utils::string::pathToString(sound).c_str(), FMOD_DEFAULT, nullptr, &souwnd);
    system->playSound(souwnd, nullptr, false, &channel);
    channel->setVolume(volume);
    channel->setPitch(speed);
    
    return SoundManager::add(channel, souwnd);
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
    return 4.9f * std::clamp(value, 0.f, 1.f) + 0.1f;
}

std::string Utils::getSettingString(int id, const std::string& setting) {
    std::string animation = std::to_string(id);
    matjson::Value object = Mod::get()->getSavedValue<matjson::Value>("settings");
    
    std::string defaultValue;
    
    if (specificStringDefaults.contains(id))
        if (specificStringDefaults.at(id).contains(setting))
            defaultValue = specificStringDefaults.at(id).at(setting);
    
    if (object.contains(animation))
        return object[animation][setting].asString().unwrapOr(defaultValue);
        
    return defaultValue;
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

void Utils::saveSetting(int id, const std::string& setting, const std::string& value) {
    saveSetting<std::string>(id, setting, value);
}

template <typename T>
void Utils::saveSetting(int id, const std::string& setting, const T& value) {
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

void Utils::setHookEnabled(const std::string& name, bool enabled) {
    for (Hook* hook : Mod::get()->getHooks()) {
        if (hook->getDisplayName() == name) {
            (void)(enabled ? hook->enable() : hook->disable());
            break;
        }
    }
}

void Utils::setHighestZ(CCNode* node) {
    if (!node->getParent()) return;

    int highest = static_cast<CCScene*>(node->getParent())->getHighestChildZ();

    if (highest < std::numeric_limits<int>::max())
        highest++;

    node->setZOrder(highest);
}

void Utils::fixScaleTextureSizexd(CCNode* sprite) {
    float mult = 1.f;

    switch (GameManager::get()->m_texQuality) {
        case 1: mult = 0.25f; break;
        case 2: mult = 0.5f; break;
    };

    sprite->setScale(sprite->getScale() * mult);
}

std::vector<std::filesystem::path> Utils::getAllFilesFromFolder(const std::filesystem::path& folder, const std::unordered_set<std::string> validExtensions) {
    std::vector<std::filesystem::path> files;
    std::error_code ec;

    for (const auto& file : std::filesystem::directory_iterator(folder, ec)) {
        if (ec) return std::vector<std::filesystem::path>{};

        if (std::filesystem::is_regular_file(file.status(ec)) && !ec)
            if (validExtensions.contains(utils::string::pathToString(file.path().extension())))
                files.push_back(file.path());
    }

    return files;
}

ccColor3B Utils::applyHSV(const ccColor3B& color, const NoobHSV& hsv) {
    float r = color.r / 255.f;
    float g = color.g / 255.f;
    float b = color.b / 255.f;

    float maxc = std::max(r, std::max(g, b));
    float minc = std::min(r, std::min(g, b));
    float v = maxc;

    float s = (maxc == 0.f) ? 0.f : (maxc - minc) / maxc;
    float h = 0.f;

    if (maxc != minc) {
        if (r == maxc)       h = (g - b) / (maxc - minc);
        else if (g == maxc)  h = 2 + (b - r) / (maxc - minc);
        else                 h = 4 + (r - g) / (maxc - minc);
        h *= 60.f;
        if (h < 0) h += 360.f;
    }

    h += hsv.hue;
    if (h < 0) h += 360.f;
    if (h >= 360.f) h -= 360.f;

    int i = (int)(h / 60.f) % 6;
    float f = (h / 60.f) - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    switch (i) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }

    ccColor3B ret = ccc3(std::clamp(r, 0.f, 1.f) * 255, std::clamp(g, 0.f, 1.f) * 255, std::clamp(b, 0.f, 1.f) * 255);

    ret.r = static_cast<int>(std::min(static_cast<int>(ret.r) * hsv.brightness, 255.f));
    ret.g = static_cast<int>(std::min(static_cast<int>(ret.g) * hsv.brightness, 255.f));
    ret.b = static_cast<int>(std::min(static_cast<int>(ret.b) * hsv.brightness, 255.f));

    return ret;
}

std::vector<std::string> Utils::split(const std::string& str, char c, size_t size) {
    size_t tokens = size ? size : (std::count(str.begin(), str.end(), c) + 1);

    std::vector<std::string> ret;

    if (tokens)
        ret.reserve(tokens);

    size_t start = 0;

    while (true) {
        size_t pos = str.find(c, start);

        if (pos == std::string::npos) {
            ret.emplace_back(str.data() + start, str.size() - start);
            break;
        }

        ret.emplace_back(str.data() + start, pos - start);

        start = pos + 1;
    }

    return ret;
}

BaseAnimation* Utils::createAnimation(Anim animation, const AnimationParams& params) {
    ANIMATION_CHECK(YouDied)
    ANIMATION_CHECK(Bsod)
    ANIMATION_CHECK(AmongUs)
    ANIMATION_CHECK(Celeste)
    ANIMATION_CHECK(ToBeContinued)
    ANIMATION_CHECK(Wasted)
    ANIMATION_CHECK(Jumpscare)
    ANIMATION_CHECK(Maro)
    ANIMATION_CHECK(Ghost)
    ANIMATION_CHECK(Undertale)
    ANIMATION_CHECK(Minecraft)
    ANIMATION_CHECK(Terraria)
    ANIMATION_CHECK(CBFDetected)
    ANIMATION_CHECK(Pop)
    ANIMATION_CHECK(SpeechBubble)
    ANIMATION_CHECK(What)
    ANIMATION_CHECK(Congregation)
    ANIMATION_CHECK(LevelComplete)
    ANIMATION_CHECK(PracticeComplete)
    ANIMATION_CHECK(NewBest)
    ANIMATION_CHECK(Blood)
    ANIMATION_CHECK(Poof)
    ANIMATION_CHECK(FadeOut)
    ANIMATION_CHECK(MrHippo)
    return nullptr;
}

CCGLProgram* Utils::createShader(const std::string& shader, bool autorelease) {
    CCGLProgram* ret = new CCGLProgram();
    ret->initWithVertexShaderByteArray(vertexShader.c_str(), shader.c_str());
    ret->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
    ret->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
    ret->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
    ret->link();
    ret->updateUniforms();

    if (autorelease)
        ret->autorelease();

    return ret;
}

CCTexture2D* Utils::takeScreenshot(CCRenderTexture* renderTexture) { // theres bug with non 16:9 aspect ratios lets hope no one notices
    CCEGLView* view = CCEGLView::get();
    CCDirector* director = CCDirector::get();

    CCSize winSize = director->getWinSize();
    CCSize ogRes = view->getDesignResolutionSize();
    CCSize size = { roundf(320.f * (winSize.width / winSize.height)), 320.f };
    CCSize newScale = { winSize.width / size.width, winSize.height / size.height };
    CCPoint ogScale = { view->m_fScaleX, view->m_fScaleY };

    float scale = director->getContentScaleFactor() / utils::getDisplayFactor();

    director->m_obWinSizeInPoints = size;
    view->setDesignResolutionSize(size.width, size.height, ResolutionPolicy::kResolutionExactFit);
    view->m_fScaleX = scale * newScale.width;
    view->m_fScaleY = scale * newScale.height;

    if (!renderTexture)
        renderTexture = CCRenderTexture::create(winSize.width, winSize.height);

    renderTexture->beginWithClear(0, 0, 0, 1);
    CCScene::get()->visit();
    renderTexture->end();

    director->m_obWinSizeInPoints = ogRes;
    view->setDesignResolutionSize(ogRes.width, ogRes.height, ResolutionPolicy::kResolutionExactFit);
    view->m_fScaleX = ogScale.x;
    view->m_fScaleY = ogScale.y;

    return renderTexture->getSprite()->getTexture();
}

CCSprite* Utils::renderPlayerSprite(CCNodeRGBA* player, bool rotation0) {
    CCSize size = ccp(100, 100) * player->getScale();

    CCRenderTexture* texture = CCRenderTexture::create(size.width, size.height);

    CCPoint ogPosition = player->getPosition();
    GLubyte ogOpacity = player->getOpacity();
    float ogRotation = player->getRotation();
    bool ogVisibility = player->isVisible();
    
    texture->begin();
    
    player->setPosition({50, 50});
    player->setOpacity(255);
    player->setVisible(true);
    
    if (rotation0)
        player->setRotation(0);

    player->visit();

    player->setPosition(ogPosition);
    player->setVisible(ogVisibility);
    player->setRotation(ogRotation);
    
    texture->end();

    CCSprite* ret = CCSprite::createWithTexture(texture->getSprite()->getTexture());
    ret->setFlipY(true);

    return ret;
}