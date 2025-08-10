#include "Utils.hpp"

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
            
    FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;
    FMOD::Sound* souwnd;
    FMOD::Channel* channel;
    
    system->createSound(sound.string().c_str(), FMOD_DEFAULT, nullptr, &souwnd);
    system->playSound(souwnd, nullptr, false, &channel);
    channel->setVolume(volume);
    channel->setPitch(speed);
    
    class SoundUpdater : public CCObject {
    
    private:
    
        FMOD::Channel* m_channel = nullptr;
        FMOD::Sound* m_sound = nullptr;
        
        SoundUpdater(FMOD::Channel* channel, FMOD::Sound* sound)
            : m_channel(channel), m_sound(sound) {}
        
        void updateSound(float) {
            bool isPlaying = true;
            
            m_channel->isPlaying(&isPlaying);
            
            if (m_channel && !isPlaying) {
                m_sound->release();
                this->release();
                m_channel = nullptr;
                CCScheduler::get()->unscheduleSelector(schedule_selector(SoundUpdater::updateSound), this);
            }
        }
        
    public:
            
        static void add(FMOD::Channel* channel, FMOD::Sound* sound) {
            SoundUpdater* updater = new SoundUpdater(channel, sound);
            updater->retain();
            CCScheduler::get()->scheduleSelector(schedule_selector(SoundUpdater::updateSound), updater, 0.2f, kCCRepeatForever, 0, false);
        }
        
    };
    
    SoundUpdater::add(channel, souwnd);
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

CCTexture2D* Utils::takeScreenshot(CCRenderTexture* renderTexture) { // theres bug with non 16:9 aspect ratios lets hope no one notices
    CCEGLView* view = CCEGLView::get();
    CCDirector* director = CCDirector::get();

    CCSize winSize = CCDirector::get()->getWinSize();
    CCSize ogRes = view->getDesignResolutionSize();
    CCSize ogScale = { view->m_fScaleX, view->m_fScaleY };
    CCSize newSize = { roundf(320.f * (winSize.width / winSize.height)), 320.f };
    CCSize newScale = { winSize.width / newSize.width, winSize.height / newSize.height };

    director->m_obWinSizeInPoints = newSize;
    view->setDesignResolutionSize(newSize.width, newSize.height, ResolutionPolicy::kResolutionExactFit);

    float scale = director->getContentScaleFactor() / utils::getDisplayFactor();
    
    view->m_fScaleX = scale * newScale.width;
    view->m_fScaleY = scale * newScale.height;

    if (!renderTexture)
        renderTexture = CCRenderTexture::create(winSize.width, winSize.height);

    renderTexture->beginWithClear(0, 0, 0, 1);
    CCScene::get()->visit();
    renderTexture->end();

    director->m_obWinSizeInPoints = ogRes;
    view->setDesignResolutionSize(ogRes.width, ogRes.height, ResolutionPolicy::kResolutionExactFit);
    view->m_fScaleX = ogScale.width;
    view->m_fScaleY = ogScale.height;

    renderTexture->getSprite()->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
    
    return renderTexture->getSprite()->getTexture();
}