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
        (void)CCNode::init();

        m_color = !m_isSecondPlayer ? ccc3(255, 0, 0) : ccc3(0, 0, 255);

        CCSprite* spr = CCSprite::create("heart-undertale.png"_spr);
        spr->setColor(m_color);
        spr->setScale(3.f * m_scale);
        spr->getTexture()->setAliasTexParameters();

        addChild(spr);

        CCSprite* spr2 = CCSprite::create("broken-heart-undertale.png"_spr);
        spr2->setColor(m_color);
        spr2->setScale(3.f * m_scale);
        spr2->getTexture()->setAliasTexParameters();
        spr2->setVisible(false);

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

    void createShard(const CCPoint& velocity) {
        CCArray* animFrames = CCArray::create();
        CCSpriteFrameCache* cache = CCSpriteFrameCache::get();
        cache->addSpriteFramesWithFile("heart-shard-undertale.plist"_spr);
        
        for (int i = 1; i <= 3; i++)
            if (CCSpriteFrame* frame = cache->spriteFrameByName((fmt::format("heart-shard-{}-undertale.png"_spr, i)).c_str()))
                animFrames->addObject(frame);
        
        CCSprite* shard = CCSprite::createWithSpriteFrameName("heart-shard-1-undertale.png"_spr);
        shard->setColor(m_color);
        shard->setScale(3.225f);
        shard->getTexture()->setAliasTexParameters();

        shard->runAction(CCRepeatForever::create(CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, 1.f / 15.f / m_speed))));
        shard->runAction(CCJumpBy::create(2.3f, {0, -320}, velocity.y + Utils::getRandomInt(-30, 30), 1));
        shard->runAction(CCMoveBy::create(2.f, ccp(velocity.x + Utils::getRandomInt(-100, 100), 0)));

        addChild(shard);
    }

public:

    static UndertaleHeart* create(float speed, float scale, bool isSecondPlayer) {
        UndertaleHeart* ret = new UndertaleHeart(speed, scale, isSecondPlayer);

        (void)ret->init();
        ret->autorelease();
        
        return ret;
    }    

};

class Undertale : public BaseAnimation {

private:

    CCSprite* m_gameOver = nullptr;

    void addHeart(CCNodeRGBA* player, bool isSecondPlayer = false) {
        CCPoint playerPos = !m_isPreview
            ? player->convertToWorldSpaceAR({0, 0})
            : player->getPosition();

        CCPoint cameraCenter = m_playLayer->m_cameraObb2->m_center;
        float cameraAngleDegrees = -m_playLayer->m_gameState.m_cameraAngle;
        float cameraAngleRadians = CC_DEGREES_TO_RADIANS(cameraAngleDegrees);

        float cosAngle = cosf(cameraAngleRadians);
        float sinAngle = sinf(cameraAngleRadians);

        float offsetX = playerPos.x - cameraCenter.x;
        float offsetY = playerPos.y - cameraCenter.y;

        float rotatedX = offsetX * cosAngle - offsetY * sinAngle;
        float rotatedY = offsetX * sinAngle + offsetY * cosAngle;

        CCPoint finalPos = ccp(cameraCenter.x + rotatedX, cameraCenter.y + rotatedY);

        UndertaleHeart* heart = UndertaleHeart::create(m_speed, m_isPreview ? 1.f : m_playLayer->m_gameState.m_cameraZoom, isSecondPlayer);
        heart->setPosition(finalPos);

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

        addChild(m_gameOver);

        Utils::playSound(Anim::Undertale, "determination.mp3", m_speed, 100, 1000, 13000, 0.5f);

        scheduleOnce(schedule_selector(Undertale::secondStep), 12.f / m_speed);
    }

    void secondStep(float) {
        m_gameOver->runAction(CCFadeTo::create(1.2f, 0));
    }

public:
    
    DEFINE_CREATE(Undertale)

    void start() override {
        BaseAnimation::start();

        scheduleOnce(schedule_selector(Undertale::playSound), 0.15f / m_speed);
        scheduleOnce(schedule_selector(Undertale::firstStep), 2.5f / m_speed);

        setZOrder(120983917);

        addChild(CCLayerColor::create({0, 0, 0, 255}, m_size.width, m_size.height));

        if (m_isPreview)
            addHeart(m_delegate->getPlayer());
        else {
            addHeart(m_playLayer->m_player1);
            
            if (Utils::getSettingBool(Anim::Undertale, "second-player") && m_playLayer->m_gameState.m_isDualMode)
                addHeart(m_playLayer->m_player2, true);
        }

        // addChild(CCLabelBMFont::create("xdd", "8bitoperator.fnt"_spr));
    }

    

    };