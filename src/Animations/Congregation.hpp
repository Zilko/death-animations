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
        xdObject{ 67, CCPoint{277.82343, 379.19995}, std::vector<int>{ 2 }, 0, 0 },
        xdObject{ 73, CCPoint{257.82343, 379.19995}, std::vector<int>{ 1 }, 0, 0 }
    };    

    CCLayerColor* m_overlay = nullptr;
    
    CCNode* m_objectLayer = nullptr;

    PlayerObject* m_player = nullptr;
    
    CCPoint m_playerStartPos;
    CCPoint m_firstCameraPos;
    CCPoint m_cameraStartPos;
    CCPoint m_layerStartPos;
    CCPoint m_cameraPos;
    CCPoint m_startCameraOffset;

    std::unordered_map<int, std::vector<GameObject*>> m_groupObjects;
    std::unordered_map<int, std::vector<GameObject*>> m_baseColorObjects;
    std::unordered_map<int, std::vector<GameObject*>> m_detailColorObjects;

    float m_time = 0.f;
    float m_ogMusicVolume = 1.f;
    float m_ogSFXVolume = 1.f;
    float m_fallHeight = 0.f;

    bool m_isSecondStep = false;

    ~Congregation() {
        setVolume(m_ogMusicVolume, m_ogSFXVolume);
    }

    void setVolume(float music, float sfx) {
        FMODAudioEngine* fmod = FMODAudioEngine::get();

        fmod->m_backgroundMusicChannel->setVolume(music);
        fmod->m_globalChannel->setVolume(sfx);
    }

    void update(float dt) {
        m_time += dt;

        if (m_time > 0.9f && !m_isSecondStep) {
            if (m_time > 2.f)
                secondStep();

            return;
        }

        m_player->updateInternalActions(dt);
        m_player->update(dt * 60.f);

        m_player->m_totalTime = m_time;

        if (m_isSecondStep && m_player->getPositionY() < m_fallHeight) {
            m_player->setPositionY(m_fallHeight);
            m_player->hitGround(nullptr, false);
        }

        m_player->updateRotation(dt * 60.f);

        cocos2d::CCPoint targetPos;

        if (m_isSecondStep)
            targetPos = -m_player->getPosition() + ccp(211, 105);
        else
            targetPos = 
                (-m_playLayer->m_objectLayer->getPosition() + (m_startCameraOffset - m_playLayer->m_gameState.m_cameraOffset)
                + m_size / 2.f
                + m_player->getPosition()
                - m_playerStartPos)
                / m_objectLayer->getScale();

        if (m_cameraPos == ccp(0, 0))
            m_cameraPos = targetPos;

        m_cameraPos = ccpLerp(m_cameraPos, targetPos, 1.f - expf(-7.f * dt));

        m_isSecondStep ? m_objectLayer->setPosition(m_cameraPos) : m_objectLayer->setContentSize(m_cameraPos);

        if (m_isSecondStep) return;
        
        m_objectLayer->setRotation(m_playLayer->m_gameState.m_cameraAngle);

        float yPos = (m_cameraStartPos - (m_cameraPos - m_layerStartPos)).y;

        if (yPos >= 0)
            m_playLayer->m_objectLayer->getParent()->setPositionY(yPos);
        else
            m_playLayer->m_gameState.m_cameraOffset.y = (m_cameraPos - m_layerStartPos).y;

        m_playLayer->m_gameState.m_cameraOffset.x = (m_cameraPos - m_layerStartPos).x;

        float progress = std::min(m_time / 0.5f, 1.f);

        setVolume(
            m_ogMusicVolume - progress * m_ogMusicVolume,
            m_ogSFXVolume - progress * m_ogSFXVolume
        );
    }

    void secondStep() {
        m_isSecondStep = true;
        m_cameraPos = -ccp(50, 2000) + ccp(211, 0);

        m_player->setPosition({50, 2000});
        m_player->setScale(1.f);

        m_player->m_yVelocity = -15;
        m_player->m_fallSpeed = 0;
        m_player->m_platformerXVelocity = 0;
        m_player->m_gravity = 0.958199f;
        m_player->m_gravityMod = 1;
        m_player->m_stateFlipGravity = -607;
        m_player->m_isUpsideDown = false;
        m_player->m_rotationSpeed = 415.38f;
        m_player->m_rotateSpeed = 1;
        m_player->m_isRotating = true;

        m_objectLayer->setScale(1.f);
        m_objectLayer->setRotation(0.f);
        m_objectLayer->setAnchorPoint({0, 0});

        m_overlay->stopAllActions();
        m_overlay->runAction(CCFadeTo::create(0.97f, 0));

        addChild(CCLayerColor::create({0, 0, 0, 255}, m_size.width, m_size.height), 0);

        scheduleOnce(schedule_selector(Congregation::thirdStep), 1.5f);
    }

    void thirdStep(float) {
        FMODAudioEngine* fmod = FMODAudioEngine::get();

        fmod->m_backgroundMusicChannel->setPaused(true);
        fmod->m_globalChannel->stop();

        setVolume(m_ogMusicVolume, m_ogSFXVolume);

        Utils::playSound(Anim::Congregation, "congregation.mp3", m_speed, 1.f);

        scheduleOnce(schedule_selector(Congregation::fourthStep), 0.15f);
    }

    void fourthStep(float) {
        m_fallHeight = m_player->getPositionY();
    }

    ANIMATION_CTOR_CREATE(Congregation)
    
public:

    void start() override {
        BaseAnimation::start();

        Utils::setHighestZ(this);

        PlayerObject* player = m_playLayer->m_player1;
        FMODAudioEngine* fmod = FMODAudioEngine::get();

        player->setOpacity(0);
        fmod->m_backgroundMusicChannel->getVolume(&m_ogMusicVolume);
        fmod->m_globalChannel->getVolume(&m_ogSFXVolume);

        m_objectLayer = CCNode::create();
        m_objectLayer->setAnchorPoint({1, 1});
        m_objectLayer->setPosition(m_size / 2.f);
        m_objectLayer->setScale(m_playLayer->m_objectLayer->getScale());
        m_objectLayer->setRotation(m_playLayer->m_gameState.m_cameraAngle);
        m_objectLayer->setContentSize((-m_playLayer->m_objectLayer->getPosition() + m_size / 2.f) / m_objectLayer->getScale());
        
        m_playerStartPos = player->getPosition();
        m_cameraStartPos = m_playLayer->m_objectLayer->getParent()->getPosition();
        m_layerStartPos = m_objectLayer->getContentSize();
        m_startCameraOffset = m_playLayer->m_gameState.m_cameraOffset;

        addChild(m_objectLayer, 1);

        GameManager* gm = GameManager::get();

        m_player = PlayerObject::create(gm->getPlayerFrame(), 1, nullptr, this, false);
        m_player->addAllParticles();
        m_player->resetAllParticles();
        m_player->setPosition(player->getPosition());
        m_player->setRotation(player->getRotation());
        m_player->updateTimeMod(0.3f, false);
        m_player->setScale(player->getScale());

        cocos2d::ccColor3B color1 = gm->colorForIdx(gm->getPlayerColor());
        cocos2d::ccColor3B color2 = gm->colorForIdx(gm->getPlayerColor2());
        
        m_player->setColor(color1);
        m_player->setSecondColor(color2);
        m_player->m_hasGlow = gm->getPlayerGlow();

        if (gm->getPlayerGlow())
            m_player->enableCustomGlowColor(gm->colorForIdx(gm->getPlayerGlowColor()));
        else
            m_player->disableCustomGlowColor();

        m_player->updateGlowColor();
        m_player->updatePlayerGlow();

        m_player->m_fadeOutStreak = false;
        m_player->m_hasGroundParticles = false;
        m_player->m_yVelocity = player->m_yVelocity;
        m_player->m_fallSpeed = player->m_fallSpeed;
        m_player->m_platformerXVelocity = player->m_platformerXVelocity;
        m_player->m_gravity = player->m_gravity;
        m_player->m_gravityMod = player->m_gravityMod;
        m_player->m_stateFlipGravity = player->m_stateFlipGravity;
        m_player->m_isUpsideDown = player->m_isUpsideDown;
        m_player->m_rotationSpeed = player->m_rotationSpeed;
        m_player->m_rotateSpeed = player->m_rotateSpeed;
        m_player->m_isRotating = player->m_isRotating;
        
        m_player->update(0.f);

        m_objectLayer->addChild(m_player);

        m_overlay = CCLayerColor::create({0, 0, 0, 0}, m_size.width, m_size.height);
        m_overlay->runAction(CCSequence::create(
            CCDelayTime::create(0.2f),
            CCFadeTo::create(0.55f, 255),
            nullptr
        ));

        addChild(m_overlay, 3);

        schedule(schedule_selector(Congregation::update));

        for (const xdObject& prop : m_objects) {
            GameObject* object = GameObject::createWithKey(prop.id);

            object->setPosition(prop.pos);

            m_objectLayer->addChild(object);

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

    void end() override {
        m_playLayer->m_objectLayer->getParent()->setPosition(m_cameraStartPos);
        m_playLayer->m_gameState.m_cameraOffset = m_startCameraOffset;

        BaseAnimation::end();
    }
};