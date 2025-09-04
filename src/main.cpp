#include "Includes.hpp"

#include "Other/Utils.hpp"
#include "Other/SoundManager.hpp"

#include "UI/AnimationsLayer.hpp"

#include "Animations/BaseAnimation.hpp"

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCCircleWave.hpp>
#include <Geode/modify/CCParticleSystem.hpp>
#include <Geode/modify/ExplodeItemNode.hpp>
#include <Geode/modify/CCFadeOut.hpp>
#include <Geode/modify/CCAnimation.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/ChannelControl.hpp>

class Vars {

public:

    inline static DeathAnimation selectedAnimation = {};

};

std::vector<std::string> split(const std::string &s, char delim, bool keep_empty = true) {
    std::vector<std::string> out;
    size_t start = 0;
    while (true) {
        size_t pos = s.find(delim, start);
        if (pos == std::string::npos) {
            std::string token = s.substr(start);
            if (keep_empty || !token.empty()) out.push_back(std::move(token));
            break;
        }
        std::string token = s.substr(start, pos - start);
        if (keep_empty || !token.empty()) out.push_back(std::move(token));
        start = pos + 1;
    }
    return out;
}

class $modify(EditorUI) {

    void selectObjects(CCArray* a, bool b) {
        EditorUI::selectObjects(a, b);

        for (GameObject* o : CCArrayExt<GameObject*>(a)) {
            std::unordered_map<int, std::string> p;

auto v = split(o->getSaveString(nullptr), ',');
for (size_t i = 0; i + 1 < v.size(); i += 2) {
        int pa = numFromString<int>(v[i]).unwrapOr(0);
        if (pa != 0)
        p[pa] = v[i + 1];
}

std::string vecPlap;

auto it = p.find(57);
if (it != p.end()) {
    auto parts = split(it->second, '.');
    for (const auto& k : parts) {
        vecPlap += k + ", ";
    }
    if (!vecPlap.empty()) {
        vecPlap.pop_back();
        vecPlap.pop_back();
    }
}

auto safeAt = [&](int key, std::string ddef) -> std::string {
    auto it = p.find(key);
    return (it != p.end()) ? it->second : ddef;
};

;

std::vector plappy = split(safeAt(43, "0a1a1a0a0"), 'a');
std::vector plappy2 = split(safeAt(44, "0a1a1a0a0"), 'a');

log::debug("xdObjectPLAP22{},PLAP22{},{}PLAP23,PLAP22{}PLAP23,{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{},{}PLAP23,",
           safeAt(1, "0"), safeAt(2, "0"), safeAt(3, "0"), vecPlap, safeAt(21, "0"), safeAt(22, "0"), safeAt(128, "1.f"), safeAt(6, "0"), safeAt(24, "0"), safeAt(25, "0"), safeAt(4, "0"), safeAt(5, "0"),
            plappy[0], plappy[3], plappy[4], plappy[1], plappy[2],
            plappy2[0], plappy2[3], plappy2[4], plappy2[1], plappy2[2]
        );
        }
    }

    void selectObject(GameObject* o, bool p1) {
        EditorUI::selectObject(o, p1);
log::debug("{}", o->getSaveString(nullptr));
        
        // xdObject{ 67, CCPoint{277.82343, 379.19995}, std::vector<int>{ 2 }, 0, 0, 1.f, 0.f },
        
    }

};

$on_mod(Loaded) {

    Utils::setHookEnabled("cocos2d::CCFadeOut::create", false);
    Utils::setHookEnabled("cocos2d::CCAnimation::createWithSpriteFrames", false);
    Utils::setHookEnabled("cocos2d::CCParticleSystem::update", false);
    Utils::setHookEnabled("CCCircleWave::updateTweenAction", false);
    Utils::setHookEnabled("GJBaseGameLayer::update", false);
    Utils::setHookEnabled("FMOD::ChannelControl::stop", false);
    Utils::setHookEnabled("FMOD::ChannelControl::setPaused", false);

    if (!Mod::get()->hasSavedValue("selected-animation"))
        Mod::get()->setSavedValue("selected-animation", 1);
    
    if (!Mod::get()->setSavedValue("created-jumpscare-folders", true)) {
        std::filesystem::path soundsFolder = Mod::get()->getSaveDir() / "jumpscare-sounds";
        std::filesystem::path imagesFolder = Mod::get()->getSaveDir() / "jumpscare-images";
        
        if (!utils::file::createDirectoryAll(soundsFolder).isErr())
            std::filesystem::copy_file(
                Mod::get()->getResourcesDir() / "jumpscare.mp3",
                soundsFolder / "jumpscare.mp3",
                std::filesystem::copy_options::overwrite_existing
            );

        if (!utils::file::createDirectoryAll(imagesFolder).isErr())
            std::filesystem::copy_file(
                Mod::get()->getResourcesDir() / "jumpscare.png",
                imagesFolder / "jumpscare.png",
                std::filesystem::copy_options::overwrite_existing
            );
    }

    if (!Mod::get()->setSavedValue("created-speech-bubble-folder", true)) {
        std::filesystem::path folder = Mod::get()->getSaveDir() / "speech-bubbles";
        
        if (!utils::file::createDirectoryAll(folder).isErr())
            for (int i = 1; i < 3; i++) {
                std::string name = fmt::format("speech-bubble-{}.png", i);
                std::error_code ec;
                
                std::filesystem::copy_file(
                    Mod::get()->getResourcesDir() / name,
                    folder / name,
                    std::filesystem::copy_options::overwrite_existing,
                    ec
                );
            }
    }
                
}

class $modify(MenuLayer) {
  
    void onMoreGames(CCObject* obj) {
        AnimationsLayer::create()->show();
    }
    
};

class $modify(ProPlayLayer, PlayLayer) {
    
    struct Fields {
        BaseAnimation* m_animation = nullptr;

        bool m_forceRestart = false;
    };
    
    static void onModify(auto& self) {
        (void)self.setHookPriorityPre("PlayLayer::destroyPlayer", Priority::Last + 21903809);
    }

    bool shouldReturn(Anim anim) {
        return anim == Anim::None
            || Utils::getRandomInt(1, 100) > static_cast<int>(Utils::getSettingFloat(anim, "probability"))
            || (m_isPracticeMode && !Utils::getSettingBool(anim, "play-in-practice"))
            || (m_isTestMode && !Utils::getSettingBool(anim, "play-in-testmode"))
            || getCurrentPercentInt() < Utils::getSettingFloat(anim, "only-after")
            || (getCurrentPercentInt() <= m_level->m_normalPercent && Utils::getSettingBool(anim, "only-on-new-best"));
    }

    void destroyPlayer(PlayerObject* p0, GameObject* p1) {
        auto f = m_fields.self();

        Anim anim = Utils::getSelectedAnimationEnum();

        if (anim == Anim::Random)
            anim = static_cast<Anim>(animations[Utils::getRandomInt(2, static_cast<int>(animations.size()) - 1)].id);

        if (shouldReturn(anim) || p1 == m_anticheatSpike || f->m_animation)
            return PlayLayer::destroyPlayer(p0, p1);

        const DeathAnimation& animation = Utils::getSelectedAnimation(anim);

        Vars::selectedAnimation = animation;

        bool og = m_gameState.m_unkBool26;
        DashRingObject* dashOrb1 = m_player1->m_isDashing ? m_player1->m_dashRing : nullptr;
        DashRingObject* dashOrb2 = m_player2->m_isDashing && m_gameState.m_isDualMode ? m_player2->m_dashRing : nullptr;

        if (animation.isNoDeathEffect || animation.isNoDeathSound)
            m_gameState.m_unkBool26 = true;

        if (animation.isSlowDown) {
            Utils::setHookEnabled("cocos2d::CCFadeOut::create", true);
            Utils::setHookEnabled("cocos2d::CCAnimation::createWithSpriteFrames", true);
        }

        if (animation.isNoStopMusic) {
            Utils::setHookEnabled("FMOD::ChannelControl::stop", true);
            Utils::setHookEnabled("FMOD::ChannelControl::setPaused", true);
        }

        PlayLayer::destroyPlayer(p0, p1);

        if (animation.isSlowDown) {
            Utils::setHookEnabled("cocos2d::CCFadeOut::create", false);
            Utils::setHookEnabled("cocos2d::CCAnimation::createWithSpriteFrames", false);
        }
        
        if (animation.isNoStopMusic) {
            Utils::setHookEnabled("FMOD::ChannelControl::stop", false);
            Utils::setHookEnabled("FMOD::ChannelControl::setPaused", false);
        }

        m_gameState.m_unkBool26 = og;

        float speed = Utils::getSpeedValue(Utils::getSettingFloat(anim, "speed"));
        
        f->m_animation = Utils::createAnimation(anim, {this, this, nullptr, speed, { .dashOrb1 = dashOrb1, .dashOrb2 = dashOrb2 }});      
      
        if (!f->m_animation) return;
        
        f->m_animation->start();
        
        stopActionByTag(16);
        
        if (Utils::getSettingBool(anim, "stop-auto-restart"))
            return;
        
        CCSequence* seq = CCSequence::create(
            CCDelayTime::create(animation.duration / speed + 0.05f),
            CCCallFunc::create(this, callfunc_selector(ProPlayLayer::delayedResetLevelReal)),
            nullptr
        );
        
        seq->setTag(16);
        runAction(seq);
    }

    void delayedResetLevelReal() {
        m_fields->m_forceRestart = true;
        delayedResetLevel();
    }
    
    void resetLevel() {
        auto f = m_fields.self();

        if (
            f->m_animation
            && !f->m_forceRestart
            && Utils::getSettingBool(Vars::selectedAnimation.id, "prevent-early-restart")
        ) {
            return;
        }

        f->m_forceRestart = false;

        if (f->m_animation) {
            f->m_animation->end();
            f->m_animation = nullptr;
        }

        PlayLayer::resetLevel();
    }

    void dialogClosed(DialogLayer* wa) {
        if (!m_fields->m_animation)
            PlayLayer::dialogClosed(wa);
    }

    void currencyWillExit(CurrencyRewardLayer* wa) {
        if (!m_fields->m_animation)
            PlayLayer::currencyWillExit(wa);
    }

    void pauseGame(bool p0) {
        PlayLayer::pauseGame(p0);

        SoundManager::pause(true);

        auto f = m_fields.self();

        if (f->m_animation)
            f->m_animation->onPause();
    }

    void resume() {
        PlayLayer::resume();

        SoundManager::pause(false);

        auto f = m_fields.self();

        if (f->m_animation)
            f->m_animation->onResume();
    }
    
};

class $modify(CCFadeOut) {

    static CCFadeOut* create(float time) { // disabled by defolt
        return CCFadeOut::create(time / 0.07f);
    }

};

class $modify(CCAnimation) {

    static CCAnimation* createWithSpriteFrames(CCArray* array, float time) { // disabled by defolt
        return CCAnimation::createWithSpriteFrames(array, time / 0.07f);
    }

};

class $modify(ExplodeItemNode) {

    void update(float dt) { // disabled by defolt
        ExplodeItemNode::update(dt * 0.025f);
    }

};

class $modify(CCParticleSystem) {

    void update(float dt) { // disabled by defolt
        CCParticleSystem::update(dt * 0.025f);
    }

};

class $modify(CCCircleWave) {

    void updateTweenAction(float dt, const char* p1) { // disabled by defolt
        CCCircleWave::updateTweenAction(dt * 0.025f, p1);
    }

};

class $modify(GJBaseGameLayer) {
    
    void update(float dt) { // disabled by defolt
        if (!Vars::selectedAnimation.isFreezeGameLayer)
            GJBaseGameLayer::update(dt * 0.025f);
    }

    void shakeCamera(float duration, float strength, float interval) {
        if (!Vars::selectedAnimation.isNoDeathEffect && !Vars::selectedAnimation.isNoShakeEffect)
            GJBaseGameLayer::shakeCamera(duration, strength, interval);
    }

};

class $modify(PlayerObject) {
  
    void playDeathEffect() {
        if (!m_gameLayer)
            return PlayerObject::playDeathEffect();
        
        if (this != m_gameLayer->m_player1 && this != m_gameLayer->m_player2)
            return PlayerObject::playDeathEffect();
            
        if (!Vars::selectedAnimation.isNoDeathEffect)
            return PlayerObject::playDeathEffect();
            
        stopActionByTag(11);
    }
    
};

class $modify(FMOD::ChannelControl) {

    FMOD_RESULT stop() { // disabled by defolt
        return (FMOD_RESULT)0;
    }

    FMOD_RESULT setPaused(bool paused) { // disabled by defolt
        paused = false;

        return FMOD::ChannelControl::setPaused(paused);
    }

};