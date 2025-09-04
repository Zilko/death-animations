#include "BaseAnimation.hpp"

struct ObjectProperties {
    int id;
    CCPoint position;
    std::vector<int> groups;
    int baseColor;
    int detailColor;
    float scale;
    float rotation;
    int zLayer;
    int zOrder;
    bool isFlipX; 
    bool isFlipY;
    float hue;
    bool saturationEnabled;
    bool brightnessEnabled;
    float saturation;
    float brightness;
    float hue2;
    bool saturationEnabled2;
    bool brightnessEnabled2;
    float saturation2;
    float brightness2;
};

class NoobGameObject : public CCNode {

private:

    GameObject* m_baseObject = nullptr;
    Ref<GameObject> m_detailObject = nullptr;

    ccColor3B m_baseColor = {255, 255, 255};
    ccColor3B m_detailColor = {255, 255, 255};

    std::vector<CCSprite*> m_extraBaseSprites;
    std::vector<CCSprite*> m_extraDetailSprites;

    bool m_baseBlending;
    bool m_detailBlending;

    void init(int id) {
        CCNode::init();

        setTag(id);

        m_baseObject = GameObject::createWithKey(id);

        addChild(m_baseObject, 2);

        m_defaultZLayer = static_cast<int>(m_baseObject->m_defaultZLayer);
        m_defaultZOrder = static_cast<int>(m_baseObject->m_defaultZOrder);

        if (id == 507) {
            m_defaultZLayer = 3;
            m_defaultZOrder = -6;
        }

        if (id == 469) {
            m_defaultZLayer = 5;
            m_defaultZOrder = 3;
        }

        if (id == 255) {
            m_detailObject = GameObject::createWithKey(259);

            Loader::get()->queueInMainThread([self = Ref(this)] {
                self->getParent()->addChild(self->m_detailObject, self->getZOrder() - 1);

                self->m_detailObject->setPosition(self->getPosition());
                self->m_detailObject->setScale(self->getScale());
                self->m_detailObject->setFlipX(self->m_baseObject->isFlipX());
                self->m_detailObject->setFlipY(self->m_baseObject->isFlipY());
            });

            schedule(schedule_selector(NoobGameObject::updateDetailObjectPosition));
        }

        if (id == 220) {
            CCSprite* spr = CCSprite::createWithSpriteFrameName("colorSquare_01_small_color_001.png");

            m_extraDetailSprites.push_back(spr);

            addChild(spr);
        }

        if (id == 1888) {
            CCSprite* spr = CCSprite::createWithSpriteFrameName("d_gradient_c_02_001.png");
            spr->setAnchorPoint({1, 0});

            m_extraBaseSprites.push_back(spr);

            addChild(spr);

            spr = CCSprite::createWithSpriteFrameName("d_gradient_c_02_001.png");
            spr->setAnchorPoint({1, 0});
            spr->setScaleX(-1);

            m_extraBaseSprites.push_back(spr);

            addChild(spr);

            spr = CCSprite::createWithSpriteFrameName("d_gradient_c_02_001.png");
            spr->setAnchorPoint({1, 0});
            spr->setScaleY(-1);

            m_extraBaseSprites.push_back(spr);

            addChild(spr);

            spr = CCSprite::createWithSpriteFrameName("d_gradient_c_02_001.png");
            spr->setAnchorPoint({1, 0});
            spr->setScale(-1);

            m_extraBaseSprites.push_back(spr);

            addChild(spr);
        }
    }

    void updateDetailObjectPosition(float) {
        m_detailObject->setPosition(getPosition());
    }

public:

    NoobHSV m_baseHSV;
    NoobHSV m_detailHSV;
    NoobHSV m_baseColorHSV;
    NoobHSV m_detailColorHSV;

    int m_defaultZLayer;
    int m_defaultZOrder;

    static NoobGameObject* create(int id) {
        NoobGameObject* ret = new NoobGameObject();

        ret->autorelease();
        ret->init(id);

        return ret;
    }

    void setFlipX(bool flipX) {
        if (m_baseObject)
            m_baseObject->setFlipX(flipX);
        
        if (m_detailObject)
            m_detailObject->setFlipX(flipX);
    }

    void setFlipY(bool flipY) {
        if (m_baseObject)
            m_baseObject->setFlipY(flipY);
        
        if (m_detailObject)
            m_detailObject->setFlipY(flipY);
    }

    void setOpacity(int opacity) {
        if (m_baseObject)
            m_baseObject->setOpacity(opacity);

        if (m_detailObject)
            m_detailObject->setOpacity(opacity);
    }

    void updateColor() {
        setBaseColor(Utils::applyHSV(Utils::applyHSV(m_baseColor, m_baseColorHSV), m_baseHSV), m_baseBlending, true);
        setDetailColor(Utils::applyHSV(Utils::applyHSV(m_detailColor, m_detailColorHSV), m_detailHSV), m_detailBlending, true);
    }

    void setBaseColor(const ccColor3B& color, bool blending, bool modified = false) {
        if (m_baseObject)
            m_baseObject->setObjectColor(color);

        for (CCSprite* spr : m_extraBaseSprites)
            spr->setColor(color);

        if (!modified) {
            m_baseColor = color;
            m_baseBlending = blending;
            updateColor();
        }

        if (blending) {
            ccBlendFunc blendFunc = { GL_SRC_ALPHA, GL_ONE };
            
            if (m_baseObject)
                m_baseObject->setBlendFunc(blendFunc);

            for (CCSprite* spr : m_extraBaseSprites)
                spr->setBlendFunc(blendFunc);
        }
    }

    void setDetailColor(const ccColor3B& color, bool blending, bool modified = false) {
        if (m_detailObject)
            m_detailObject->setObjectColor(color);

        for (CCSprite* spr : m_extraDetailSprites)
            spr->setColor(color);

        if (!modified) {
            m_detailBlending = blending;
            m_detailColor = color;
            updateColor();
        }

        if (blending) {
            ccBlendFunc blendFunc = { GL_SRC_ALPHA, GL_ONE };

            if (m_detailObject)
                m_detailObject->setBlendFunc(blendFunc);

            for (CCSprite* spr : m_extraDetailSprites)
                spr->setBlendFunc(blendFunc);
        }
    }

};

class Congregation : public BaseAnimation {

private:

    const std::array<ObjectProperties, 80> m_objects = {
        ObjectProperties{220,{24883.5,436.5},{82,79},4,4,1.f,90,7,0,0,0,-150,0,0,1,1,-150,0,0,1,1},
ObjectProperties{495,{24900,420},{34,76,82,79},4,0,2,0,3,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{255,{24900,420},{82,79},4,5,2,0,3,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{220,{24973.5,524.5},{82,79},4,4,1.f,90,7,0,0,0,-150,0,0,1,1,-150,0,0,1,1},
ObjectProperties{508,{24917.5,433},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{24917.5,429},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{202,{24915,495},{},0,0,1.f,0,0,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{495,{24990,540},{34,76,82,79},4,0,2,0,3,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{24917.5,437},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{24917.5,441},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{24905,375},{34,82,79},6,0,1.f,-90,0,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{24901,375},{34,82,79},6,0,1.f,-90,0,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{24919,375},{34,82,79},6,0,1.f,-90,3,-5,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{24999.5,346.5},{34,82,79},6,0,1.f,-90,1,-6,0,1,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{24999,495},{34,82,79},6,0,1.f,-90,0,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{24911,367},{34,82,79},6,0,1.f,-90,1,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{24999,465},{34,82,79},6,0,1.f,-90,0,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{255,{24945,435},{82,79},4,5,1.f,0,3,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{255,{24960.6,420.64},{82,79},4,5,1.f,0,3,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{468,{24978.5,324.126},{34,82,79},6,0,1.f,0,1,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{24991,465},{34,82,79},6,0,1.f,-90,0,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{24994.5,584.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{24915,371},{34,82,79},6,0,1.f,-90,1,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{24947.5,346.5},{34,82,79},6,0,1.f,-90,1,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{24939.5,338.5},{34,82,79},6,0,1.f,-90,1,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{24943.5,342.5},{34,82,79},6,0,1.f,-90,1,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{24995,465},{34,82,79},6,0,1.f,-90,0,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{255,{24996.4,399.904},{82,79},4,5,2,0,3,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{255,{24975,435},{82,79},4,5,1.f,0,3,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{24990.5,584.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{468,{24973.5,328.126},{34,82,79},6,0,1.f,0,1,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{468,{24971,332.126},{34,82,79},6,0,1.f,0,1,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{468,{24969.5,324.126},{34,82,79},6,0,1.f,0,1,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{255,{24985.2,379.112},{82,79},4,5,1.f,0,3,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{255,{24990,540},{82,79},4,5,2,0,3,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{24991,495},{34,82,79},6,0,1.f,-90,0,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{24995,495},{34,82,79},6,0,1.f,-90,0,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{211,{24987.2,439.49},{80,81,970},2,0,1.8,0,-3,4,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{211,{24987.2,439.49},{80,81,970},6,0,2,0,-3,3,0,0,0,0,1,1,-0.6,0,0,0,1,1},
ObjectProperties{1765,{25001,439},{82,79},2,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25019,672.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{1888,{25020.4,718.724},{82,79},3,0,4,0,3,9,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25023,702.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{25037.5,433},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25027,642.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{25037.5,437},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{25037.5,429},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25023,672.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{25007.5,515},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25023,642.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25027,672.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{25037.5,441},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25019,702.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25027,702.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{25007.5,523},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{25007.5,519},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{508,{25007.5,511},{82,79},4,0,1.f,0,5,2,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25013.5,355},{34,82,79},6,0,1.f,-270,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25017.5,355},{34,82,79},6,0,1.f,-270,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25023,617},{34,82,79},6,0,1.f,-270,0,0,0,1,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{255,{25020,420},{82,79},4,5,2,0,3,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{25003.5,342.5},{34,82,79},6,0,1.f,-90,1,-6,0,1,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25009.5,355},{34,82,79},6,0,1.f,-270,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{255,{25030.1,392.116},{82,79},4,5,1.f,0,3,0,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{495,{25020,420},{34,76,82,79},4,0,2,0,3,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25019,621},{34,82,79},6,0,1.f,-270,0,0,0,1,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{25007.5,338.5},{34,82,79},6,0,1.f,-90,3,-6,0,1,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{25008.5,584.5},{82,34,79},6,0,1.f,0,3,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{25000.5,592.5},{82,34,79},6,0,1.f,0,1,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25027,613},{34,82,79},6,0,1.f,-270,0,0,0,1,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{469,{25004.5,588.5},{82,34,79},6,0,1.f,0,1,-6,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{507,{25019,642.5},{34,82,79},6,0,1.f,-90,0,0,1,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{211,{25025.3,478.771},{80,81,970},6,0,2,0,-3,3,0,0,0,0,1,1,-0.6,0,0,0,1,1},
ObjectProperties{211,{25025.3,478.771},{80,81,970},2,0,1.8,0,-3,4,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{211,{25034.8,400.524},{80,81,970},2,0,1.8,0,-3,4,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{211,{25034.8,400.524},{80,81,970},6,0,2,0,-3,3,0,0,0,0,1,1,-0.6,0,0,0,1,1},
ObjectProperties{211,{25035.5,704.643},{80,81,970},6,0,2,0,-3,3,0,0,0,0,1,1,-0.6,0,0,0,1,1},
ObjectProperties{211,{25035.5,704.643},{80,81,970},2,0,1.8,0,-3,4,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{211,{25036.1,633.286},{80,81,970},2,0,1.8,0,-3,4,0,0,0,0,0,1,1,0,0,0,1,1},
ObjectProperties{211,{25036.1,633.286},{80,81,970},6,0,2,0,-3,3,0,0,0,0,1,1,-0.6,0,0,0,1,1}
    };    

    CCLayerColor* m_overlay = nullptr;
    
    CCNode* m_objectLayer = nullptr;
    CCNode* m_congregationContainer = nullptr;

    PlayerObject* m_player = nullptr;
    
    CCPoint m_playerStartPos;
    CCPoint m_firstCameraPos;
    CCPoint m_cameraStartPos;
    CCPoint m_layerStartPos;
    CCPoint m_cameraPos;
    CCPoint m_startCameraOffset;

    std::unordered_map<int, std::vector<NoobGameObject*>> m_groupObjects;
    std::unordered_map<int, std::vector<NoobGameObject*>> m_baseColorObjects;
    std::unordered_map<int, std::vector<NoobGameObject*>> m_detailColorObjects;

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
        m_objectLayer->setContentSize({0, 0});

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
        
        addChild(m_objectLayer, 1);

        m_playerStartPos = player->getPosition();
        m_cameraStartPos = m_playLayer->m_objectLayer->getParent()->getPosition();
        m_layerStartPos = m_objectLayer->getContentSize();
        m_startCameraOffset = m_playLayer->m_gameState.m_cameraOffset;

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

        m_congregationContainer = CCNode::create();
        m_congregationContainer->setPosition({309, 147});

        m_objectLayer->addChild(m_congregationContainer);

        for (const ObjectProperties& prop : m_objects) {
            NoobGameObject* object = NoobGameObject::create(prop.id);

            object->setPosition(prop.position - ccp(24843, 450));
            object->setScale(prop.scale);
            object->setRotation(prop.rotation);
            object->setZOrder(
                (prop.zLayer == 0 ? object->m_defaultZLayer : prop.zLayer) * 1000
                + (prop.zOrder == 0 ? object->m_defaultZOrder : prop.zOrder)
            );
            object->setFlipX(prop.isFlipX);
            object->setFlipY(prop.isFlipY);

            object->m_baseHSV = {
                prop.hue,
                prop.saturation,
                prop.brightnessEnabled ? prop.brightness + 1.f : prop.brightness
            };

            object->m_detailHSV = {
                prop.hue2,
                prop.saturation2, 
                prop.brightnessEnabled2 ? prop.brightness2 + 1.f : prop.brightness2
            };

            object->updateColor();

            m_congregationContainer->addChild(object);

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

        if (m_groupObjects.contains(76))
            for (NoobGameObject* object : m_groupObjects.at(76)) {
                object->setOpacity(255 / 2);
            }

        if (m_baseColorObjects.contains(4))
            for (NoobGameObject* object : m_baseColorObjects.at(4)) {
                object->setBaseColor({44, 71, 255}, true);
                object->m_baseColorHSV.hue = 140;
                object->updateColor();
            }

        if (m_baseColorObjects.contains(6))
            for (NoobGameObject* object : m_baseColorObjects.at(6)) {
                object->setBaseColor({44, 71, 255}, false);
                object->m_baseColorHSV.hue = 140;
                object->updateColor();
            }

        if (m_baseColorObjects.contains(5))
            for (NoobGameObject* object : m_baseColorObjects.at(5)) {
                    object->setBaseColor({195, 40, 1}, false);
            }

        if (m_baseColorObjects.contains(2))
            for (NoobGameObject* object : m_baseColorObjects.at(2)) {
                object->setBaseColor({0, 0, 0}, false);
            }

        if (m_baseColorObjects.contains(3))
            for (NoobGameObject* object : m_baseColorObjects.at(3)) {
                object->setBaseColor({0, 0, 0}, false);
            }

        if (m_detailColorObjects.contains(5))
            for (NoobGameObject* object : m_detailColorObjects.at(5))
                object->setDetailColor({195, 40, 1}, false);

        if (m_detailColorObjects.contains(4))
            for (NoobGameObject* object : m_detailColorObjects.at(4)) {
                object->setDetailColor({44, 71, 255}, true);
                object->m_detailColorHSV.hue = 140;
                object->updateColor();
            }
             
    }

    void end() override {
        m_playLayer->m_objectLayer->getParent()->setPosition(m_cameraStartPos);
        m_playLayer->m_gameState.m_cameraOffset = m_startCameraOffset;

        BaseAnimation::end();
    }
};