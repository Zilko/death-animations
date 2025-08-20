#include "Utils.hpp"
#include "SoundManager.hpp"

#include "../Animations/YouDied.hpp"
#include "../Animations/Bsod.hpp"
#include "../Animations/AmongUs.hpp"
#include "../Animations/Celeste.hpp"
#include "../Animations/ToBeContinued.hpp"
#include "../Animations/Wii.hpp"
#include "../Animations/Jumpscare.hpp"
#include "../Animations/Wasted.hpp"
#include "../Animations/Maro.hpp"
#include "../Animations/Ghost.hpp"
#include "../Animations/Undertale.hpp"
#include "../Animations/Minecraft.hpp"
#include "../Animations/Terraria.hpp"
#include "../Animations/CBFDetected.hpp"
#include "../Animations/Pop.hpp"

#include <random>

int Utils::getRandomInt(int min, int max) {
    static std::mt19937 gen(std::random_device{}());

    std::uniform_int_distribution<> dis(min, max);
    
    return dis(gen);
}

std::filesystem::path Utils::getRandomFile(const std::filesystem::path& folder, const std::unordered_set<std::string> validExtensions) {
    std::vector<std::filesystem::path> files;
    std::error_code ec;

    for (const auto& entry : std::filesystem::directory_iterator(folder, ec)) {
        if (ec) return std::filesystem::path{};

        if (std::filesystem::is_regular_file(entry.status(ec)) && !ec)
            if (validExtensions.contains(entry.path().extension().string()))
                files.push_back(entry.path());
    }

    if (files.empty()) return std::filesystem::path{};

    return files[getRandomInt(0, static_cast<int>(files.size()) - 1)];
}

void Utils::playSound(Anim anim, const std::string& sound, float speed, int fade, int duration, float volume, bool loop) {
    playSound(anim, sound, speed, fade, fade, duration, volume, loop);
}

void Utils::playSound(Anim anim, const std::string& sound, float speed, int fadeIn, int fadeOut, int duration, float volume, bool loop) {
    if (!Utils::getSettingBool(anim, "play-sound-effects"))
        return;
                
    duration = static_cast<int>(duration / speed / 1000.f) * 1000;
    fadeIn /= speed;
    fadeOut /= speed;
    
    if (duration < 1000) duration = 1000;
            
    FMODAudioEngine::get()->playEffectAdvanced(
        (Mod::get()->getResourcesDir() / sound).string().c_str(),
        1.f, 1.f, volume, speed, 1.f, 1.f, 0, duration, fadeIn, fadeOut, loop, 0, false, false, 0, 0, 0, 0
    );
}

void Utils::playSound(Anim anim, const std::string& sound, float speed, float volume) {
    playSound(anim, speed, volume, Mod::get()->getResourcesDir() / sound);
}

void Utils::playSound(Anim anim, float speed, float volume, const std::filesystem::path& sound) {
    if (Utils::getSettingBool(anim, "play-sound-effects"))
        FMODAudioEngine::get()->playEffect(sound.string().c_str(), speed, 1.f, volume);
}

void Utils::playSoundManual(Anim anim, const std::string& sound, float speed, float volume) {
    playSoundManual(anim, speed, volume, Mod::get()->getResourcesDir() / sound);
}

void Utils::playSoundManual(Anim anim, float speed, float volume, const std::filesystem::path& sound) {
    if (!Utils::getSettingBool(anim, "play-sound-effects"))
        return;
            
    FMOD::System* system = FMODAudioEngine::get()->m_system;
    FMOD::Sound* souwnd;
    FMOD::Channel* channel;
    
    system->createSound(sound.string().c_str(), FMOD_DEFAULT, nullptr, &souwnd);
    system->playSound(souwnd, nullptr, false, &channel);
    channel->setVolume(volume);
    channel->setPitch(speed);
    
    SoundManager::add(channel, souwnd);
}

BaseAnimation* Utils::createAnimation(Anim animation, const AnimationParams& params) {
    ANIMATION_CHECK(YouDied)
    ANIMATION_CHECK(Bsod)
    ANIMATION_CHECK(AmongUs)
    ANIMATION_CHECK(Celeste)
    ANIMATION_CHECK(ToBeContinued)
    ANIMATION_CHECK(Wii)
    ANIMATION_CHECK(Wasted)
    ANIMATION_CHECK(Jumpscare)
    ANIMATION_CHECK(Maro)
    ANIMATION_CHECK(Ghost)
    ANIMATION_CHECK(Undertale)
    ANIMATION_CHECK(Minecraft)
    ANIMATION_CHECK(Terraria)
    ANIMATION_CHECK(CBFDetected)
    ANIMATION_CHECK(Pop)
    return nullptr;
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

    int highest = 0;

    for (CCNode* brother : CCArrayExt<CCNode*>(node->getParent()->getChildren()))
        if (brother->getZOrder() > highest && brother != node)
            highest = brother->getZOrder();

    if (highest < std::numeric_limits<int>::max())
        highest++;

    node->setZOrder(highest);
}

void Utils::fixScaleTextureSizexd(CCNode* sprite) {
    float mult = 1.f;

    switch (GameManager::get()->m_texQuality) {
        case 1: mult = 0.25f; break;
        case 2: mult = 0.5f; break;
        default: mult = 1.f;
    };

    sprite->setScale(sprite->getScale() * mult);
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

CCSprite* Utils::renderPlayer(CCNodeRGBA* player, bool rotation0) {
    CCRenderTexture* texture = CCRenderTexture::create(100, 100);

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


    CCSprite* spr = CCSprite::createWithTexture(texture->getSprite()->getTexture());
    spr->setFlipY(true);

    return spr;
}