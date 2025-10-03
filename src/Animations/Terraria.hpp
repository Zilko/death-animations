#include "BaseAnimation.hpp"

class Terraria : public BaseAnimation {
    
private:

    constexpr static std::array m_deathMessages = std::to_array<std::string_view>({
        "{} left a small crater.","{} discovered gravity.","{} didn't bounce.","{} faceplanted the ground.","{} fell victim to gravity.","{} fell to their death.","{} hit the ground face first.","{} is shark food.","{} forgot to breathe.","{} is sleeping with the fish.","{} drowned.","{} thought he was a fish.","{} likes to play in magma.","{} got melted.","{} tried to swim in lava.","{} was incinerated.","{} exploded.","{} had a mining accident.","{} forgot to run.","{} was destroyed.","{} was turned into pile of flesh.","{} got snapped in half.","{} was murdered.","{} was mangled.","{}'s vital organs were ruptured.","{}'s plead for death was answered.","{}'s innards were made outards.","{}'s skull was crushed.","{}'s body was mangled.","{} was licked.","{} tried to escape.","{} turned corrupt.","{} was torn in half.","{} was slain...","{} didn't materialize.","{}'s legs appeared where his/her head should be."
    });

    inline static const ParticleStruct m_particleStruct = {200,0.36,0.35,0,-1,90,180,67,27,17,17,0,0,0,0,0,0,4,0,258,0,0.37,0,0,0,0,0,1,0,0,0,0,0,0.37,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,2,false,false,false,false,0,false,0,0,0,0,false,false,false,false,false,0,0,0,0,"particle_00_001.png"};
    
    CCLabelBMFont* m_slainLabel = nullptr;
    CCLabelBMFont* m_dropsLabel = nullptr;
    CCLabelBMFont* m_countdownLabel = nullptr;
    CCLabelBMFont* m_deathMessageLabel1 = nullptr;
    CCLabelBMFont* m_deathMessageLabel2 = nullptr;
    CCLabelBMFont* m_deathMessageLabel3 = nullptr;
    CCLabelBMFont* m_deathMessageLabel4 = nullptr;
    CCLabelBMFont* m_deathMessageLabel5 = nullptr;
    
    int m_currentSecond = 10;
    int m_ogPlayerDeathEffect = 1;
    
    bool m_wasExplodeEnabled = false;
    
    void updateCountdown(float) {
        if (m_currentSecond == 1) return;
        
        m_currentSecond--;
        
        m_countdownLabel->setString(std::to_string(m_currentSecond).c_str());
    }
        
    ANIMATION_CTOR_CREATE(Terraria) {}
    
public:

    void startEarly() override {
        GameManager* gm = GameManager::get();
        
        m_wasExplodeEnabled = gm->getGameVariable("0153");
        m_ogPlayerDeathEffect = gm->getPlayerDeathEffect();
        
        gm->setGameVariable("0153", false);
        gm->setPlayerDeathEffect(1);
        
        Utils::setHookEnabled("cocos2d::CCParticleSystemQuad::create", true);
        Utils::setHookEnabled("CCCircleWave::create", true);
    }

    void start() override {
        m_playLayer->m_player1->setVisible(false);
        
        CCParticleSystemQuad* particle = GameToolbox::particleFromStruct(m_particleStruct, nullptr, false);
        particle->setID("terraria-blood"_spr);
        particle->setPosition(m_playLayer->m_player1->getPosition());
        particle->resetSystem();
        particle->setAutoRemoveOnFinish(true);
        
        m_playLayer->m_player1->getParent()->addChild(particle);
        
        Utils::setHighestZ(particle);
        
        Utils::playSound(Anim::Terraria, fmt::format("terraria-hit-{}.wav", Utils::getRandomInt(1, 2)), 1.f, 1.f);
        Utils::playSound(Anim::Terraria, "terraria-death.wav", 1.f, 1.f);
        
        GameManager* gm = GameManager::get();
        
        gm->setGameVariable("0153", m_wasExplodeEnabled);
        gm->setPlayerDeathEffect(m_ogPlayerDeathEffect);
        
        Utils::setHookEnabled("cocos2d::CCParticleSystemQuad::create", false);
        Utils::setHookEnabled("CCCircleWave::create", false);
        
        float duration = 10.f / m_speed;
        int copper = 0;
        int silver = 0;
        
        std::string username = GJAccountManager::get()->m_username;
        if (username.empty())
            username = "Player";

        int index = Utils::getRandomInt(0, static_cast<int>(m_deathMessages.size()) - 1);
        std::string deathMessage = fmt::format(fmt::runtime(m_deathMessages[index]), username);
        
        if (std::floor(duration) == duration)
            m_currentSecond = static_cast<int>(duration);
        else
            m_currentSecond = static_cast<int>(std::ceil(duration));
        
        if ((!m_isPreview && m_playLayer->m_level->m_stars.value() > 0) || m_isPreview) { 
            copper = Utils::getRandomInt(0, 10) <= 2 ? 0 : Utils::getRandomInt(20, 60);
            silver = Utils::getRandomInt(0, 10) <= 4 ? 0 : Utils::getRandomInt(2, 16);
        }

        m_slainLabel = CCLabelBMFont::create("You were slain...", "terraria.fnt"_spr);
        m_slainLabel->setPosition(m_size / 2.f + ccp(-0.2f, 14));
        m_slainLabel->setColor({255, 205, 205});
        m_slainLabel->setScale(0.875f);
        m_slainLabel->setOpacity(0);
        m_slainLabel->runAction(CCFadeTo::create(2.5f / (m_speed > 1.f ? m_speed : 1.f), 189));
        
        addChild(m_slainLabel);
        
        if (silver != 0 || copper != 0) {
            std::string dropString = "dropped";
            
            if (silver != 0)
                dropString += fmt::format(" {} silver", silver);
            
            if (copper != 0)
                dropString += fmt::format(" {} copper", copper);
                    
            m_dropsLabel = CCLabelBMFont::create(dropString.c_str(), "terraria.fnt"_spr);
            m_dropsLabel->setPosition(m_size / 2.f + ccp(-0.2f, 1));
            m_dropsLabel->setColor({255, 205, 205});
            m_dropsLabel->setScale(0.342f);
            m_dropsLabel->setOpacity(0);
            m_dropsLabel->runAction(CCFadeTo::create(2.5f / (m_speed > 1.f ? m_speed : 1.f), 189));
         
            addChild(m_dropsLabel);
        }
        
        m_countdownLabel = CCLabelBMFont::create(std::to_string(m_currentSecond).c_str(), "terraria.fnt"_spr);
        m_countdownLabel->setPosition(m_size / 2.f + ccp(1.5f, -15));
        m_countdownLabel->setColor({255, 205, 205});
        m_countdownLabel->setScale(0.62f);
        m_countdownLabel->setOpacity(0);
        m_countdownLabel->runAction(CCFadeTo::create(2.5f / (m_speed > 1.f ? m_speed : 1.f), 189));
        
        addChild(m_countdownLabel);
        
        m_deathMessageLabel1 = CCLabelBMFont::create(deathMessage.c_str(), "terraria.fnt"_spr);
        m_deathMessageLabel1->setPosition({27.7f, 17});
        m_deathMessageLabel1->setColor({0, 0, 0});
        m_deathMessageLabel1->setAnchorPoint({0, 0.5f});
        m_deathMessageLabel1->setScale(0.35f);
        m_deathMessageLabel1->runAction(CCRepeatForever::create(
            CCSequence::create(
                CCDelayTime::create(0.5f),
                CCFadeTo::create(1.f, 215),
                CCFadeTo::create(1.f, 255),
                nullptr
            )
        ));
        
        addChild(m_deathMessageLabel1);
        
        m_deathMessageLabel2 = CCLabelBMFont::create(deathMessage.c_str(), "terraria.fnt"_spr);
        m_deathMessageLabel2->setPosition({28.3f, 17});
        m_deathMessageLabel2->setColor({0, 0, 0});
        m_deathMessageLabel2->setAnchorPoint({0, 0.5f});
        m_deathMessageLabel2->setScale(0.35f);
        m_deathMessageLabel2->runAction(CCRepeatForever::create(
            CCSequence::create(
                CCDelayTime::create(0.5f),
                CCFadeTo::create(1.f, 215),
                CCFadeTo::create(1.f, 255),
                nullptr
            )
        ));
        
        addChild(m_deathMessageLabel2);
        
        m_deathMessageLabel3 = CCLabelBMFont::create(deathMessage.c_str(), "terraria.fnt"_spr);
        m_deathMessageLabel3->setPosition({28, 16.7f});
        m_deathMessageLabel3->setColor({0, 0, 0});
        m_deathMessageLabel3->setAnchorPoint({0, 0.5f});
        m_deathMessageLabel3->setScale(0.35f);
        m_deathMessageLabel3->runAction(CCRepeatForever::create(
            CCSequence::create(
                CCDelayTime::create(0.5f),
                CCFadeTo::create(1.f, 215),
                CCFadeTo::create(1.f, 255),
                nullptr
            )
        ));
        
        addChild(m_deathMessageLabel3);
        
        m_deathMessageLabel4 = CCLabelBMFont::create(deathMessage.c_str(), "terraria.fnt"_spr);
        m_deathMessageLabel4->setPosition({28, 17.3f});
        m_deathMessageLabel4->setColor({0, 0, 0});
        m_deathMessageLabel4->setAnchorPoint({0, 0.5f});
        m_deathMessageLabel4->setScale(0.35f);
        m_deathMessageLabel4->runAction(CCRepeatForever::create(
            CCSequence::create(
                CCDelayTime::create(0.5f),
                CCFadeTo::create(1.f, 215),
                CCFadeTo::create(1.f, 255),
                nullptr
            )
        ));
        
        addChild(m_deathMessageLabel4);
        
        m_deathMessageLabel5 = CCLabelBMFont::create(deathMessage.c_str(), "terraria.fnt"_spr);
        m_deathMessageLabel5->setPosition({28, 17});
        m_deathMessageLabel5->setColor({165, 29, 17});
        m_deathMessageLabel5->setAnchorPoint({0, 0.5f});
        m_deathMessageLabel5->setScale(0.35f);
        m_deathMessageLabel5->runAction(CCRepeatForever::create(
            CCSequence::create(
                CCDelayTime::create(0.5f),
                CCFadeTo::create(1.f, 215),
                CCFadeTo::create(1.f, 255),
                nullptr
            )
        ));
        
        addChild(m_deathMessageLabel5);
        
        schedule(schedule_selector(Terraria::updateCountdown), 1.f, kCCRepeatForever, duration - static_cast<int>(duration));
    }
    
    void end() override {
        if (!m_isPreview)
            m_playLayer->m_player1->setVisible(true);
            
        BaseAnimation::end();
    }

};