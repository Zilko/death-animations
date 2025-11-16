#include "BaseAnimation.hpp"

class UndertaleHeart : public CCNode {

private:

    ccColor3B m_color;

    float m_speed = 1.f;
    float m_scale = 1.f;
    bool m_isSecondPlayer = false;

    UndertaleHeart(float speed, float scale, bool isSecondPlayer)
        : m_speed(speed), m_scale(scale), m_isSecondPlayer(isSecondPlayer) {}

    bool init() override {
        CCNode::init();

        m_color = !m_isSecondPlayer ? ccc3(255, 0, 0) : ccc3(0, 0, 255);

        CCSprite* spr = CCSprite::create("heart-undertale.png"_spr);
        spr->setColor(m_color);
        spr->setScale(3.f * m_scale);
        spr->getTexture()->setAliasTexParameters();

        Utils::fixScaleTextureSizexd(spr);

        addChild(spr);

        CCSprite* spr2 = CCSprite::create("broken-heart-undertale.png"_spr);
        spr2->setColor(m_color);
        spr2->setScale(3.f * m_scale);
        spr2->getTexture()->setAliasTexParameters();
        spr2->setVisible(false);

        Utils::fixScaleTextureSizexd(spr2);

        addChild(spr2);

        spr->runAction(CCSequence::create(CCDelayTime::create(0.35f / m_speed), CCHide::create(), nullptr));
        spr2->runAction(CCSequence::create(CCDelayTime::create(0.35f / m_speed), CCShow::create(), CCDelayTime::create(1.4f / m_speed), CCHide::create(), nullptr));

        scheduleOnce(schedule_selector(UndertaleHeart::shattter), 1.75f / m_speed);

        return true;
    }

    void shattter(float) {
        createShard({70, 120});
        createShard({-70, 120});
        createShard({325, 70});
        createShard({-325, 70});
        createShard({550, 0});
        createShard({-550, -5});
    }

    void createShard(CCPoint velocity) {
        CCArray* animFrames = CCArray::create();
        CCSpriteFrameCache* cache = CCSpriteFrameCache::get();
        cache->addSpriteFramesWithFile("heart-shard-undertale.plist"_spr);
        
        for (int i = 1; i <= 3; i++)
            if (CCSpriteFrame* frame = cache->spriteFrameByName((fmt::format("heart-shard-{}-undertale.png"_spr, i)).c_str()))
                animFrames->addObject(frame);
        
        CCSprite* shard = CCSprite::createWithSpriteFrameName("heart-shard-1-undertale.png"_spr);
        shard->setColor(m_color);
        shard->setScale(3.225f * m_scale);
        shard->getTexture()->setAliasTexParameters();

        Utils::fixScaleTextureSizexd(shard);

        shard->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, 1.f / 15.f / m_speed))));
        shard->runAction(CCJumpBy::create(2.3f / m_speed, {0, -320}, velocity.y + Utils::getRandomInt(-30, 30) * m_scale, 1));
        shard->runAction(CCMoveBy::create(2.f / m_speed, ccp(velocity.x + Utils::getRandomInt(-100, 100) * m_scale, 0)));

        addChild(shard);
    }

public:

    static UndertaleHeart* create(float speed, float scale, bool isSecondPlayer) {
        UndertaleHeart* ret = new UndertaleHeart(speed, scale, isSecondPlayer);

        ret->init();
        ret->autorelease();
        
        return ret;
    }    

};

class Undertale : public BaseAnimation {

private:

    CCSprite* m_gameOver = nullptr;

    std::vector<CCNode*> m_letters;

    CCLabelBMFont* m_label1 = nullptr;
    CCLabelBMFont* m_label2 = nullptr;

    bool m_canGoNext = false;
    bool m_isOnCooldown = false;
    
    int m_currentLetter = 0;
    int m_currentStep = 0;
    
    float m_ogVolume = 1.f;
    float m_fadeTime = 0.f;

    void addHeart(CCNodeRGBA* player, bool isSecondPlayer = false) {
        UndertaleHeart* heart = UndertaleHeart::create(m_speed, getCurrentZoom(), isSecondPlayer);
        heart->setPosition(getNodeScreenPos(player));

        addChild(heart);
    }

    void playSound(float) {
        Utils::playSound(Anim::Undertale, "soul-shatter.mp3", m_speed, 1.f);
    }

    void firstStep(float) {
        m_gameOver = CCSprite::create("game-over-undertale.png"_spr);
        m_gameOver->setPosition({m_size.width / 2.f + 1.5f, m_size.height - 83});
        m_gameOver->setScale(21.35f);
        m_gameOver->getTexture()->setAliasTexParameters();
        m_gameOver->setOpacity(0);
        m_gameOver->runAction(CCFadeTo::create(1.f, 255));

        Utils::fixScaleTextureSizexd(m_gameOver);

        addChild(m_gameOver);

        Utils::playSound(Anim::Undertale, "determination.mp3", m_speed, 200, 0, 13000, 0.22f, true);

        scheduleOnce(schedule_selector(Undertale::secondStep), 2.f / m_speed);
    }

    CCLabelBMFont* createLabel(const char* string, float y) {
        CCLabelBMFont* lbl = CCLabelBMFont::create(string, "8bitoperator.fnt"_spr);
        lbl->setPosition({178, y});
        lbl->setScale(0.875f);
        lbl->setAnchorPoint({0, 0.5f});

        Utils::fixScaleTextureSizexd(lbl);
        
        addChild(lbl);
        
        for (CCNode* letter : CCArrayExt<CCNode*>(lbl->getChildren()))
            letter->setVisible(false);

        return lbl;
    }

    void loadLabel(CCLabelBMFont* lbl) {
        if (!lbl) return;
        
        CCArrayExt<CCNode*> children = CCArrayExt<CCNode*>(lbl->getChildren());
        std::string string = lbl->getString();

        m_letters.clear();
        m_currentLetter = 0;

        for (int i = 0; i < children.size(); i++)
            if (string[i] != ' ')
                m_letters.push_back(children[i]);
    }

    void updateLetters(float dt) {
        if (m_currentLetter >= static_cast<int>(m_letters.size())) {
            if (m_currentStep > 2)
                m_canGoNext = true;
                
            return;
        }

        CCNode* letter = m_letters[m_currentLetter++];

        if (!letter) return;

        if (dt > 0.f)
            Utils::playSound(Anim::Undertale, "text-undertale.wav", m_speed, 1.f);

        letter->setVisible(true);
    }

    void finishLabel(CCLabelBMFont* lbl) {
        for (CCNode* letter : CCArrayExt<CCNode*>(lbl->getChildren()))
            letter->setVisible(true);
    }
    
    void removeCooldown(float) {
        m_isOnCooldown = false;
    }
    
    void skip() {
        if (m_isOnCooldown) return;
        
        if (m_currentStep == 2 || m_currentStep == 3 || m_currentStep == 4)
            finishLabel(m_label1);
        
        if (m_currentStep == 2 || m_currentStep == 3 || m_currentStep == 5)
            finishLabel(m_label2);
        
        CCScheduler::get()->unscheduleSelector(schedule_selector(Undertale::thirdStep), this);
        
        m_canGoNext = true;
        m_currentLetter = 999;
        m_letters.clear();
        
        m_isOnCooldown = true;
        scheduleOnce(schedule_selector(Undertale::removeCooldown), 0.1f);
    }
    
    void next() {
        if (m_isOnCooldown || !m_canGoNext || m_currentStep > 5) return;
        
        m_canGoNext = false;
        
        switch (m_currentStep) {
            case 2:
            case 3: fourthStep(0.f); break;
            case 4: fifthStep(0.f); break;
            case 5: sixthStep(0.f); break;
        };
        
        m_isOnCooldown = true;
        scheduleOnce(schedule_selector(Undertale::removeCooldown), 0.1f);
    }
    
    void both() {
        if (m_canGoNext)
            next();
        else
            skip();
    }
    
    void secondStep(float) {
        m_currentStep = 2;
        
        m_label1 = createLabel("Y o u    c a n n o t    g i v e", 100);
        m_label2 = createLabel("u p    j u s t    y e t .  .  .", 76);

        loadLabel(m_label1);

        schedule(schedule_selector(Undertale::updateLetters), 4.3f / 60.f / m_speed, kCCRepeatForever, 0.f);
        scheduleOnce(schedule_selector(Undertale::thirdStep), 66.f / 60.f / m_speed);
    }

    void thirdStep(float) {
        m_currentStep = 3;
        
        finishLabel(m_label1);
        loadLabel(m_label2);
    }

    void fourthStep(float) {
        m_currentStep = 4;
        
        m_letters.clear();

        m_label1->removeFromParentAndCleanup(true);
        m_label2->removeFromParentAndCleanup(true);
        
        m_label1 = nullptr;
        m_label2 = nullptr;

        std::string name = GJAccountManager::get()->m_username;

        if (name.empty())
            name = "Player";

        std::string newName;

        for (char c : name) {
            newName += c;
            newName += " ";
        }
        
        newName += "!";

        m_label1 = createLabel(newName.c_str(), 100);
        m_label2 = createLabel("S t a y    d e t e r m i n e d .  .  .", 76);

        loadLabel(m_label1);
    }

    void fifthStep(float) {
        m_currentStep = 5;
        
        finishLabel(m_label1);
        loadLabel(m_label2);
    }

    void sixthStep(float) {
        m_currentStep = 6;
        
        m_letters.clear();

        m_label1->removeFromParentAndCleanup(true);
        m_label2->removeFromParentAndCleanup(true);
        
        m_label1 = nullptr;
        m_label2 = nullptr;
        
        scheduleOnce(schedule_selector(Undertale::seventhStep), 1.9f / m_speed);
    }

    void seventhStep(float) {
        m_gameOver->runAction(CCSequence::create(
            CCFadeTo::create(1.2f / m_speed, 0),
            CCDelayTime::create(1.2f),
            CCCallFunc::create(this, callfunc_selector(Undertale::finalStep)),
            nullptr
        ));
        
        m_ogVolume = FMODAudioEngine::get()->m_sfxVolume;
        
        schedule(schedule_selector(Undertale::update));
    }
    
    void finalStep() {
        resetLevel();
    }
    
    void update(float dt) override {
        m_fadeTime += dt;
        
        FMODAudioEngine::get()->m_globalChannel->setVolume(
            m_ogVolume - std::min(m_fadeTime / 0.5f, 1.f) * m_ogVolume
        );
    }
    
    void keyDown(enumKeyCodes key) override {
        // CCLayer::keyDown(key);
        
        if (
            key != enumKeyCodes::KEY_W
            && key != enumKeyCodes::KEY_X
            && key != enumKeyCodes::KEY_Z
            && key != enumKeyCodes::KEY_Space
            && key != enumKeyCodes::KEY_ArrowUp
        ) {     
            return;
        }
        
        if (key == enumKeyCodes::KEY_X)
            skip();
        else if (key == enumKeyCodes::KEY_Z)
            next();
        else
            both();
    }
    
    bool ccTouchBegan(CCTouch*, CCEvent*) override {
        both();
        
        return false;
    }
    
    ~Undertale() {
        if (!m_isPreview)
            Utils::setHookEnabled("cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG", false);
        
        FMODAudioEngine::get()->m_globalChannel->setVolume(m_ogVolume);
    }
    
    ANIMATION_CTOR_CREATE(Undertale) {}

public:

    void start() override {
        disableRetryLayer();
        enableTouch();
        
        m_ogVolume = FMODAudioEngine::get()->m_sfxVolume;

        if (m_isPreview)
            setKeyboardEnabled(true);
        else
            Utils::setHookEnabled("cocos2d::CCKeyboardDispatcher::dispatchKeyboardMSG", true);
        
        scheduleOnce(schedule_selector(Undertale::playSound), 0.15f / m_speed);
        scheduleOnce(schedule_selector(Undertale::firstStep), 2.5f / m_speed);

        Utils::setHighestZ(this);
        
        addChild(CCLayerColor::create({0, 0, 0, 255}));

        if (m_isPreview)
            addHeart(m_delegate->getPlayer());
        else {
            addHeart(m_playLayer->m_player1);
            
            if (Utils::getSettingBool(Anim::Undertale, "second-player") && m_playLayer->m_gameState.m_isDualMode)
                addHeart(m_playLayer->m_player2, true);
        }
    }
    
    void onPause() override {
        FMODAudioEngine::get()->m_globalChannel->setVolume(m_ogVolume);
    }

    void onResume() override {
        m_ogVolume = FMODAudioEngine::get()->m_sfxVolume;
    }
    
};