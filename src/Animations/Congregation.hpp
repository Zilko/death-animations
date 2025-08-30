#include "BaseAnimation.hpp"

struct xdObject {
    int id;
    CCPoint pos;
    std::vector<int> groups;
    int baseColor;
    int detailColor;
};

class Congregation : public BaseAnimation {

private:

    const std::array<xdObject, 2> m_objects = {
        xdObject{ 67, CCPoint{100, 100}, std::vector<int>{ 2 }, 0, 0 },
        xdObject{ 73, CCPoint{120, 100}, std::vector<int>{ 1 }, 0, 0 }
    };

    std::unordered_map<int, std::vector<GameObject*>> m_groupObjects;
    std::unordered_map<int, std::vector<GameObject*>> m_baseColorObjects;
    std::unordered_map<int, std::vector<GameObject*>> m_detailColorObjects;

    ANIMATION_CTOR_CREATE(Congregation)

    PlayerObject* player = nullptr;
    float time = 0.f;
    cocos2d::CCPoint _cameraPos;
bool _cameraPosInitialized = false;
float _cameraSmooth = 7.0f;     

    void update(float dt) {
        time += dt;

        player->updateInternalActions(dt);
        player->update(dt * 60.f);

        player->m_totalTime = time;

        // player->m_waveTrail->m_pulseSize = 1.4f;

        // float height = 0.f;

        // if (player->m_vehicleSize != 1.f)
        //     height = (1.f - player->m_vehicleSize) * player->m_unkAngle1 * 0.5;

        // height = (CCDirector::get()->getScreenBottom() + 90.f + player->m_unkAngle1 * 0.5) - height;

        if (player->getPosition().y < -2000) {

            startPip.y = 105.f;
            // player->setPosition({player->getPosition().x, -2000});
            // player->hitGround(nullptr, false);
        }

        player->updateRotation(dt * 60.f);

        // if (getPosition().x >= CCDirector::get()->getWinSize().width + 100.f)
        //     m_fields->m_shouldKill ? meetGrimReaper() : reset(videoOptionsOpen);

        // m_waveTrail->updateStroke(dt);

        // layerun->setContentSize((-m_playLayer->m_player1->getParent()->getPosition() + m_size / 2.f + player->getPosition() - startPip) * (1.f / layerun->getScale()));
        cocos2d::CCPoint target =
        (-m_playLayer->m_player1->getParent()->getPosition()
         + m_size / 2.f
         + player->getPosition()
         - startPip)
        * (1.f / layerun->getScale());

    // init first frame so camera doesn't jump
    if (!_cameraPosInitialized) {
        _cameraPos = target;
        _cameraPosInitialized = true;
    }

    // exponential smoothing: alpha = 1 - e^{-k * dt}
    float k = _cameraSmooth;                    // tuning parameter
    float alpha = 1.0f - expf(-k * dt);         // need <math.h> included
    _cameraPos = ccpLerp(_cameraPos, target, alpha);

    // apply smoothed value (keeps your original call)
    layerun->setContentSize(_cameraPos);
    log::debug("{}", startPip);
    }

    CCPoint startPip;
    CCNode* layerun = nullptr;
    
public:

    void start() override {
        BaseAnimation::start();

        addChild(CCLayerColor::create({ 0, 0, 0, 255 }, m_size.width, m_size.height));

        GameManager* gm = GameManager::get();
    
        // auto m_mainColor = gm->colorForIdx(gm->getPlayerColor());
        // CCPoint playerPos = Utils::getPlayerScreenPos(m_playLayer, m_isPreview ? m_delegate->getPlayer() : m_playLayer->m_player1, m_isPreview);
        CCPoint playerPos = m_playLayer->m_player1->getPosition();
        startPip = playerPos;

        layerun = CCNode::create();

        layerun->setPosition(m_size / 2.f);
        layerun->setScale(m_playLayer->m_player1->getParent()->getScale());
        layerun->setContentSize((-m_playLayer->m_player1->getParent()->getPosition() + m_size / 2.f) * (1.f / layerun->getScale()));
        layerun->setRotation(m_playLayer->m_gameState.m_cameraAngle);
        layerun->setAnchorPoint({1, 1});

        player = PlayerObject::create(gm->getPlayerFrame(), 1, nullptr, this, false);
        // player->addAllParticles();

        // player->m_regularTrail->stopStroke();

        if (player->m_fadeOutStreak) {
            player->m_fadeOutStreak = false;
            player->fadeOutStreak2(player->m_playEffects ? 0.2f : 0.6f);
        }

        if (player->m_hasGroundParticles) {
            player->m_playerGroundParticles->stopSystem();
            player->m_hasGroundParticles = false;
        }


        player->resetAllParticles();
        player->toggleFlyMode(false, false);
        player->toggleBirdMode(false, false);
        player->toggleRollMode(false, false);
        player->toggleDartMode(false, false);
        player->toggleRobotMode(false, false);
        player->toggleSpiderMode(false, false);
        player->toggleSwingMode(false, false);

        // SimplePlayer* m_player = SimplePlayer::create(gm->getPlayerFrame());
        // m_player->m_hasGlowOutline = gm->getPlayerGlow();
        player->setPosition(playerPos);
        player->setRotation(m_playLayer->m_player1->getRotation());
        // m_player->setColors(m_mainColor, gm->colorForIdx(gm->getPlayerColor2()));
        
        // if (m_player->m_hasGlowOutline)
        //     m_player->enableCustomGlowColor(gm->colorForIdx(gm->getPlayerGlowColor()));
        // else
        //     m_player->disableCustomGlowColor();
            
        // m_player->updateColors();

        cocos2d::ccColor3B color1 = gm->colorForIdx(gm->getPlayerColor());
        cocos2d::ccColor3B color2 = gm->colorForIdx(gm->getPlayerColor2());
        
        player->setColor(color1);
        player->setSecondColor(color2);
        player->m_hasGlow = gm->getPlayerGlow();

        if (gm->getPlayerGlow())
            player->enableCustomGlowColor(gm->colorForIdx(gm->getPlayerGlowColor()));
        else
            player->disableCustomGlowColor();

        player->updateGlowColor();
        player->updatePlayerGlow();

        layerun->addChild(player);

        addChild(layerun, 1);

        player->updateTimeMod(0.3f, false);
        player->m_yVelocity = m_playLayer->m_player1->m_yVelocity;
        player->m_fallSpeed = m_playLayer->m_player1->m_fallSpeed;
        player->m_platformerXVelocity = m_playLayer->m_player1->m_platformerXVelocity;
        player->m_gravity = m_playLayer->m_player1->m_gravity;
        player->m_gravityMod = m_playLayer->m_player1->m_gravityMod;
        player->m_stateFlipGravity = m_playLayer->m_player1->m_stateFlipGravity;
        player->m_isUpsideDown = m_playLayer->m_player1->m_isUpsideDown;

        if (!m_isPreview) {
            player->setRotation(m_playLayer->m_player1->getRotation());
            player->setScale(m_playLayer->m_player1->getScale());
        }

        player->update(0.f);

        schedule(schedule_selector(Congregation::update));

        // m_player->runAction(CCEaseIn::create(CCRotateBy::create(5.f, 360 * 5), 1.2f));
        // m_player->runAction(CCEaseSineInOut::create(CCScaleTo::create(1.1f, 1.f)));
        // m_player->runAction(CCEaseSineInOut::create(CCMoveTo::create(1.1f, {211, -6})));
        // m_player->runAction(CCEaseSineInOut::create(CCMoveBy::create(0.7f, {-1, -10})));

        auto layer = CCLayerColor::create({0, 0, 0, 0}, m_size.width, m_size.height);
        layer->runAction(
            CCFadeTo::create(0.5f, 255)
        );

        // addChild(layer, 10);

        for (const xdObject& prop : m_objects) {
            break;
            GameObject* object = GameObject::createWithKey(prop.id);

            object->setPosition(prop.pos);

            addChild(object);

            for (int group : prop.groups)
                if (m_groupObjects.contains(group))
                    m_groupObjects.at(group).push_back(object);
                else
                    m_groupObjects[group] = { object };

            if (prop.baseColor != 0) {
                if (m_baseColorObjects.contains(prop.baseColor))
                    m_baseColorObjects.at(prop.baseColor).push_back(object);
                else
                    m_baseColorObjects[prop.baseColor] = { object };
            }

            if (prop.detailColor != 0) {
                if (m_detailColorObjects.contains(prop.detailColor))
                    m_detailColorObjects.at(prop.detailColor).push_back(object);
                else
                    m_detailColorObjects[prop.detailColor] = { object };
            }
        }

        if (m_groupObjects.contains(2))
            for (GameObject* object : m_groupObjects.at(2))
                object->runAction(CCEaseSineInOut::create(CCMoveBy::create(1.f, {0, 60})));
        
    }
};