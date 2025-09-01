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

    PlayerObject* m_player = nullptr;
    
    CCNode* m_objectLayer = nullptr;
    
    CCPoint m_playerStartPos;
    CCPoint m_firstCameraPos;
    CCPoint m_cameraStartPos;
    CCPoint m_layerStartPos;
    CCPoint m_cameraPos;

    std::unordered_map<int, std::vector<GameObject*>> m_groupObjects;
    std::unordered_map<int, std::vector<GameObject*>> m_baseColorObjects;
    std::unordered_map<int, std::vector<GameObject*>> m_detailColorObjects;

    float m_time = 0.f;
    float m_ogMusicVolume = 1.f;
    float m_ogSFXVolume = 1.f;

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

        m_player->updateInternalActions(dt);
        m_player->update(dt * 60.f);

        m_player->m_totalTime = m_time;

        if (m_player->getPosition().y < -2000) {

            m_playerStartPos.y = 105.f;
            // m_player->setPosition({m_player->getPosition().x, -2000});
            // m_player->hitGround(nullptr, false);
        }

        m_player->updateRotation(dt * 60.f);

        cocos2d::CCPoint targetPos =
            (-m_playLayer->m_objectLayer->getPosition()
            + m_size / 2.f
            + m_player->getPosition()
            - m_playerStartPos)
            / m_objectLayer->getScale();

        if (m_cameraPos == ccp(0, 0))
            m_cameraPos = targetPos;

        m_cameraPos = ccpLerp(m_cameraPos, targetPos, 1.f - expf(-7.f * dt));

        m_objectLayer->setContentSize(m_cameraPos);

        m_playLayer->m_objectLayer->getParent()->setPosition(
            m_cameraStartPos - (m_cameraPos - m_layerStartPos)
        );

        float progress = std::min(m_time / 0.5f, 1.f);

        setVolume(
            m_ogMusicVolume - progress * m_ogMusicVolume,
            m_ogSFXVolume - progress * m_ogSFXVolume
        );
    }

    ANIMATION_CTOR_CREATE(Congregation)
    
public:

    void start() override {
        BaseAnimation::start();

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
        
        m_player->update(0.f);

        m_objectLayer->addChild(m_player);

        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 0}, m_size.width, m_size.height);
        layer->runAction(CCSequence::create(
            CCDelayTime::create(0.f),
            CCFadeTo::create(0.5f, 255),
            nullptr
        ));

        addChild(layer, 3);

        schedule(schedule_selector(Congregation::update));

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

    void end() override {
        m_playLayer->m_objectLayer->getParent()->setPosition(m_cameraStartPos);

        BaseAnimation::end();
    }
};