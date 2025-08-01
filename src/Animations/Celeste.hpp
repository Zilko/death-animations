#include "BaseAnimation.hpp"

class CCEaseCubeOut : public CCActionEase {
    
public:

    static CCEaseCubeOut* create(CCActionInterval* action) {
        auto ret = new CCEaseCubeOut();
        ret->initWithAction(action);
        ret->autorelease();
        return ret;
    }
    
    void update(float t) override {
        float tt = t - 1.f;
        m_pInner->update(tt * tt * tt + 1.f);
    }

};

class CelesteExplosion : public CCNode {
   
private:

    const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif
        
        varying vec4 v_fragmentColor;
        varying vec2 v_texCoord;
        uniform sampler2D u_texture;
        
        uniform int u_white;
        uniform int u_fullWhite;
        uniform float u_time;
        
        void main() {
            vec2 u_pixelSize = vec2(1.0 / 48.0, 1.0 / 48.0);
                
            vec2 coord = vec2(
                floor(v_texCoord.x / u_pixelSize.x) * u_pixelSize.x,
                floor(v_texCoord.y / u_pixelSize.y) * u_pixelSize.y
            );
            
            vec2 pixelCoord = floor(v_texCoord / u_pixelSize) * u_pixelSize;
            if (distance(vec2(0.5, 0.5), pixelCoord) > mix(0.2, 0.08, clamp(u_time / 0.34, 0.0, 1.0))) {
                gl_FragColor = vec4(0, 0, 0, 0);
                return;
            }
        
            vec4 newColor = v_fragmentColor * texture2D(u_texture, coord);
            
            if ((u_white == 1 && newColor.rgb != vec3(0, 0, 0)) || u_fullWhite == 1)
                newColor = vec4(1, 1, 1, 1);
                
            vec3 grayscale = vec3(dot(newColor.rgb, vec3(0.299, 0.587, 0.114)));
            vec3 desaturated = mix(newColor.rgb, grayscale, 0.7);
            gl_FragColor = vec4(desaturated, newColor.a);
        }
    )";

    CCSprite* m_animationSprite = nullptr;
    CCSprite* m_playerSprite = nullptr;
    
    CCGLProgram* m_program = nullptr;
    
    ccColor3B m_color;
    
    float m_speed;
    float m_time = 0.f;
    bool m_isWhite = false;
    
    CelesteExplosion(const ccColor3B& color, float speed) {
        m_color = color;
        m_speed = speed;
    }

    bool init(CCNodeRGBA* player, const CCPoint& velocity) {
        m_program = new CCGLProgram();
        m_program->autorelease();
        m_program->initWithVertexShaderByteArray(vertexShader.c_str(), m_shader.c_str());
        m_program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        m_program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        m_program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        m_program->link();
        m_program->updateUniforms();
        
        CCRenderTexture* texture = CCRenderTexture::create(100, 100);
        CCPoint ogPos = player->getPosition();
        bool ogVisibility = player->isVisible();
        GLubyte ogOpacity = player->getOpacity();
        
        texture->begin();
        
        player->setOpacity(255);
        player->setVisible(true);
        player->setPosition({50, 50});
        player->visit();
        player->setPosition(ogPos);
        player->setVisible(ogVisibility);
        player->setOpacity(ogOpacity);
        
        texture->end();

        m_playerSprite = CCSprite::createWithTexture(texture->getSprite()->getTexture());
        m_playerSprite->setFlipY(true);
        m_playerSprite->setShaderProgram(m_program);
        
        setShaderState("u_white", 0);
        setShaderState("u_fullWhite", 0);
        glUniform1f(glGetUniformLocation(m_program->getProgram(), "u_time"), 0.f);
        
        player->setVisible(false);
        
        addChild(m_playerSprite);
        
        float angle = ccpToAngle(velocity) - M_PI;
        runAction(CCEaseCubeOut::create(CCMoveTo::create(0.41f / m_speed, getPosition() + ccp(cos(angle) * 47.f, sin(angle) * 47.f))));
        
        schedule(schedule_selector(CelesteExplosion::updateBall), 0, kCCRepeatForever, 0);
        scheduleOnce(schedule_selector(CelesteExplosion::firstStep), 1.f / 60.f / m_speed);
        
        return true;
    }
    
    void updateBall(float dt) {
        m_time += dt;
        float t = std::min(m_time / 0.34f, 1.0f);
        float easedTime = 1.0f - cosf(t * M_PI_2);
        m_program->use();
        glUniform1f(glGetUniformLocation(m_program->getProgram(), "u_time"), easedTime);
    }
    
    void setShaderState(const char* name, int value) {
        m_program->use();
        glUniform1i(glGetUniformLocation(m_program->getProgram(), name), value);
    }
    
    void firstStep(float) {
        setShaderState("u_white", 1);
        scheduleOnce(schedule_selector(CelesteExplosion::secondStep), 3.f / 60.f / m_speed);
    }
    void secondStep(float) {
        setShaderState("u_white", 0);
        scheduleOnce(schedule_selector(CelesteExplosion::thirdStep), 2.f / 60.f / m_speed);
    }
    void thirdStep(float) {
        setShaderState("u_white", 1);
        scheduleOnce(schedule_selector(CelesteExplosion::fourthStep), 3.f / 60.f / m_speed);
    }
    void fourthStep(float) {
        setShaderState("u_white", 0);
        scheduleOnce(schedule_selector(CelesteExplosion::fifthStep), 3.f / 60.f / m_speed);
    }
    void fifthStep(float) {
        setShaderState("u_fullWhite", 1);
        scheduleOnce(schedule_selector(CelesteExplosion::sixthStep), 2.f / 60.f / m_speed);
    }
    void sixthStep(float) {
        setShaderState("u_fullWhite", 0);
        scheduleOnce(schedule_selector(CelesteExplosion::seventhStep), 3.f / 60.f / m_speed);
    }
    void seventhStep(float) {
        m_playerSprite->setColor({0, 0, 0});
        scheduleOnce(schedule_selector(CelesteExplosion::eighthStep), 2.f / 60.f / m_speed);
    }
    void eighthStep(float) {
        m_playerSprite->setColor({255, 255, 255});
        setShaderState("u_fullWhite", 1);
        scheduleOnce(schedule_selector(CelesteExplosion::ninthStep), 6.f / 60.f / m_speed);
    }
    void ninthStep(float) {
        CCArray* animFrames = CCArray::create();
        CCSpriteFrameCache* cache = CCSpriteFrameCache::get();
        cache->addSpriteFramesWithFile("celeste-explosion.plist"_spr);
        
        for (int i = 1; i <= 43; i++)
            if (CCSpriteFrame* frame = cache->spriteFrameByName((fmt::format("celeste-explosion-{}.png"_spr, i)).c_str()))
                animFrames->addObject(frame);
        
        m_animationSprite = CCSprite::createWithSpriteFrameName("celeste-explosion-1.png"_spr);
        m_animationSprite->setColor(m_color);
        m_animationSprite->setScale(8.75f);
        m_animationSprite->getTexture()->setAliasTexParameters();
        m_animationSprite->runAction(
            CCSequence::create(
                CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, 1.f / 60.f / m_speed)),
                CCDelayTime::create(1.f / 60.f / m_speed),
                CCCallFunc::create(this, callfunc_selector(CelesteExplosion::explosionEnded)),
                nullptr
            )
        );
        
        m_animationSprite->setPosition(m_playerSprite->getPosition());
        m_playerSprite->setVisible(false);
        m_playerSprite->setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));
        
        addChild(m_animationSprite);
        
        unschedule(schedule_selector(CelesteExplosion::updateBall));
        schedule(schedule_selector(CelesteExplosion::updateColor), 5.f / 60.f / m_speed, kCCRepeatForever, 5.f / 60.f / m_speed);
    }
    
    void updateColor(float) {
        if (!m_animationSprite)
            return unschedule(schedule_selector(CelesteExplosion::updateColor));
        
        m_isWhite = !m_isWhite;
        
        m_animationSprite->setColor(m_isWhite ? ccc3(255, 255, 255) : m_color);
    }
    
    void explosionEnded(float) {
        m_animationSprite->removeFromParentAndCleanup(true);
        m_animationSprite = nullptr;
    }
    
public:
    
    static CelesteExplosion* create(CCNodeRGBA* player, const CCPoint& velocity, const ccColor3B& color, float speed) {
        CelesteExplosion* ret = new CelesteExplosion(color, speed);
        ret->init(player, velocity);
        ret->autorelease();
        return ret;
    }

};

class CelesteTransition : public CCNode {

private:
    
    CCSize m_size;

    float m_speed = 1.f;
    bool m_reverse = false;
    
    CelesteTransition(CCSize size, float speed, bool reverse) {
        m_size = size;
        m_speed = speed;
        m_reverse = reverse;
    }
    
    void init(int transition, float delay) {        
        switch(transition) {
            case 1: scheduleOnce(schedule_selector(CelesteTransition::chapter1Transition), delay); break;
            case 2: scheduleOnce(schedule_selector(CelesteTransition::chapter2Transition), delay); break;
            case 3: scheduleOnce(schedule_selector(CelesteTransition::chapter3Transition), delay); break;
            case 4: scheduleOnce(schedule_selector(CelesteTransition::chapter4Transition), delay); break;
        }
    }
    
    void chapter1Transition(float) {
        CCNode* container = CCNode::create();
        container->setPosition({m_reverse ? 0 : -m_size.width, m_reverse ? m_size.height : 0    });
        container->setScale(m_reverse ? -1 : 1);
        
        addChild(container);
                
        for (int i = 0; i < 6; i++) {
            CCSprite* spr = CCSprite::create("pointy-thing-celeste.png"_spr);
            spr->setScale(m_size.height / 6.f / spr->getContentHeight());
            spr->setPosition({360 - spr->getContentWidth() * spr->getScale() * i + 19 * i, m_size.height -spr->getContentHeight() * spr->getScale() / 2.f - spr->getContentHeight() * spr->getScale() * i});
            container->addChild(spr);
            
            CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255}, spr->getPositionX() + m_size.width, spr->getContentHeight() * spr->getScale());
            layer->setPosition({-m_size.width, spr->getPositionY() - spr->getContentHeight() * spr->getScale() / 2.f});
            container->addChild(layer);
        }
        
        container->runAction(CCMoveTo::create(0.7f, {m_size.width * (m_reverse ? 2 : 1), container->getPositionY()}));
    }
    
    void chapter2Transition(float) {
        static const std::vector<std::vector<CCPoint>> circlePositions = {
            { { 24.f, 303.f }, { 26.f, 128.f }, { 8.f, 102.f } },
            { { 27.f, 11.f }, { 51.f, 68.f }, { 54.f, 149.f }, { 48.f, 310.f } },
            { { 47.17f, 270.52f }, { 99.31f, 257.48f }, { 67.61f, 229.63f }, { 20.20f, 214.52f }, { 23.76f, 185.19f }, { 65.54f, 176.89f }, { 68.20f, 92.15f }, { 19.31f, 47.70f } },
            { { 85.09f, 287.70f }, { 9.54f, 262.52f }, { 98.72f, 189.63f }, { 121.54f, 182.22f }, { 100.50f, 136.f }, { 131.02f, 136.30f }, { 88.06f, 48.f }, { 59.02f, 8.89f }, { 139.02f, 15.41f } },
            { { 161.24f, 301.93f }, { 143.76f, 215.11f }, { 142.87f, 92.44f }, { 133.39f, 64.59f }, { 88.06f, 100.44f } },
            { { 128.35f, 251.26f }, { 96.65f, 214.22f }, { 208.35f, 133.63f }, { 97.24f, 22.81f }, { 178.72f, 15.11f } },
            { { 143.17f, 308.15f }, { 169.83f, 221.04f }, { 177.24f, 187.56f }, { 200.35f, 179.85f }, { 163.91f, 147.26f }, { 168.94f, 107.85f }, { 211.61f, 99.26f }, { 211.91f, 68.74f }, { 179.61f, 48.89f } },
            { { 219.02f, 301.63f }, { 176.06f, 254.52f }, { 257.24f, 216.30f }, { 249.54f, 181.93f }, { 253.69f, 132.74f } },
            { { 248.06f, 293.93f }, { 246.57f, 251.85f }, { 209.83f, 248.59f }, { 197.69f, 214.52f }, { 327.17f, 94.22f }, { 240.06f, 65.19f }, { 257.24f, 29.33f }, { 219.61f, 15.11f } },
            { { 312.06f, 308.44f }, { 322.13f, 263.11f }, { 283.02f, 258.07f }, { 325.09f, 222.52f }, { 326.87f, 171.85f }, { 277.98f, 150.22f }, { 246.28f, 101.93f }, { 290.72f, 17.78f } },
            { { 279.46f, 306.07f }, { 288.35f, 208.89f }, { 279.76f, 175.70f }, { 312.35f, 129.19f }, { 292.80f, 108.15f }, { 285.09f, 53.63f }, { 331.02f, 23.70f }, { 361.83f, 28.74f } },
            { { 352.35f, 296.59f }, { 350.57f, 265.48f }, { 391.46f, 259.85f }, { 363.91f, 213.04f }, { 332.50f, 67.26f }, { 358.57f, 95.41f } },
            { { 442.13f, 310.22f }, { 408.35f, 208.30f }, { 361.83f, 186.07f }, { 364.20f, 130.96f }, { 389.98f, 106.96f }, { 426.72f, 128.30f }, { 350.87f, 69.33f }, { 409.54f, 28.44f }, { 433.24f, 10.07f } },
            { { 397.98f, 301.04f }, { 447.76f, 218.67f }, { 387.91f, 189.63f }, { 396.80f, 142.22f }, { 464.94f, 152.89f }, { 464.35f, 89.78f }, { 403.31f, 58.96f }, { 481.24f, 10.96f } },
            { { 442.13f, 271.70f }, { 480.65f, 216.f }, { 464.94f, 189.33f }, { 441.24f, 179.26f }, { 443.91f, 107.56f }, { 473.54f, 64.59f }, { 433.54f, 51.26f } },
            { { 478.87f, 295.70f }, { 513.83f, 293.93f }, { 549.69f, 142.22f }, { 548.80f, 52.15f }, { 523.02f, 71.41f } },
            { { 474.13f, 264.30f }, { 512.65f, 270.52f }, { 559.17f, 260.44f }, { 519.46f, 220.15f }, { 544.35f, 186.07f }, { 507.91f, 145.19f }, { 508.50f, 104.f }, { 560.06f, 100.15f }, { 503.46f, 12.44f } },
            { { 557.09f, 298.37f }, { 546.13f, 26.07f } },
            { { 540.80f, 218.67f } }
        };
        
        for (int i = 0; i < circlePositions.size(); i++)
            for (const CCPoint& position : circlePositions[i]) {
                CCSprite* spr = CCSprite::create("circle-celeste.png"_spr);
                spr->setScale(m_reverse ? 3.f : 0.f);
                spr->setPosition(position);
                 
                addChild(spr);
                spr->runAction(CCSequence::create(
                    CCDelayTime::create(i / 60.f / m_speed),
                    CCScaleTo::create(0.4f / m_speed, m_reverse ? 0.f : 3.f),
                    nullptr
                ));
            }
    }
    
    void chapter3Transition(float) {
        float halfWidth = m_size.width / 2.f;
        float time1 = 20.f / 60.f / m_speed;
        float time2 = 10.f / 60.f / m_speed;
        
        CCSprite* spr = CCSprite::create("keyhole-left-celeste.png"_spr);
        spr->setScaleX(0);
        spr->setScaleY(2.275f);
        spr->setAnchorPoint({1, 0.5f});
        spr->setPosition({0, m_size.height / 2.f});
        
        addChild(spr);
        
        spr->runAction(CCSequence::create(
            CCSpawn::create(CCMoveTo::create(time1, m_size / 2.f), CCScaleTo::create(time1, 1.5f, 1.5f), nullptr),
            CCScaleTo::create(time2, 0),
            nullptr
        ));
        
        spr = CCSprite::create("keyhole-right-celeste.png"_spr);
        spr->setScaleX(0);
        spr->setScaleY(2.275f);
        spr->setAnchorPoint({0, 0.5f});
        spr->setPosition({m_size.width, m_size.height / 2.f});
        
        addChild(spr);
        
        spr->runAction(CCSequence::create(
            CCSpawn::create(CCMoveTo::create(time1, m_size / 2.f), CCScaleTo::create(time1, 1.5f, 1.5f), nullptr),
            CCScaleTo::create(time2, 0),
            nullptr
        ));
        
        float layerHeight = (m_size.height - spr->getContentHeight() * 1.5f) / 2.f;
        float layerWidth = (m_size.height - spr->getContentHeight() * 2.275f) / 2.f;
        
        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255}, halfWidth, layerHeight);
        layer->setAnchorPoint({0, 0});
        layer->setScaleX(0);
        layer->setScaleY(layerWidth / layer->getContentHeight());
        layer->runAction(CCSequence::create(
            CCScaleTo::create(time1, 1, 1),
            CCScaleTo::create(time2, 1, m_size.height / 2.f / layerHeight),
            nullptr
        ));
        
        addChild(layer);
        
        layer = CCLayerColor::create({0, 0, 0, 255}, halfWidth, layerHeight);
        layer->setAnchorPoint({0, 1});
        layer->setScaleX(0);
        layer->setScaleY(layerWidth / layer->getContentHeight());
        layer->runAction(CCSequence::create(
            CCScaleTo::create(time1, 1, 1),
            CCScaleTo::create(time2, 1, m_size.height / 2.f / layerHeight),
            nullptr
        ));
        layer->setPosition({0, m_size.height - layerHeight});
        
        addChild(layer);
        
        layer = CCLayerColor::create({0, 0, 0, 255}, halfWidth, layerHeight);
        layer->setAnchorPoint({1, 1});
        layer->setScaleX(0);
        layer->setScaleY(layerWidth / layer->getContentHeight());
        layer->runAction(CCSequence::create(
            CCScaleTo::create(time1, 1, 1),
            CCScaleTo::create(time2, 1, m_size.height / 2.f / layerHeight),
            nullptr
        ));
        layer->setPosition({halfWidth, m_size.height - layerHeight});
        
        addChild(layer);
        
        layer = CCLayerColor::create({0, 0, 0, 255}, halfWidth, layerHeight);
        layer->setAnchorPoint({1, 0});
        layer->setScaleX(0);
        layer->setScaleY(layerWidth / layer->getContentHeight());
        layer->runAction(CCSequence::create(
            CCScaleTo::create(time1, 1, 1),
            CCScaleTo::create(time2, 1, m_size.height / 2.f / layerHeight),
            nullptr
        ));
        layer->setPosition({halfWidth, 0});
        
        addChild(layer);
        
        float layerWidth2 = halfWidth - spr->getContentWidth() * 1.5f;
        
        layer = CCLayerColor::create({0, 0, 0, 255}, layerWidth2, m_size.height);
        layer->setAnchorPoint({0, 0});
        layer->setScaleX(0);
        layer->runAction(CCSequence::create(
            CCScaleTo::create(time1, 1, 1),
            CCScaleTo::create(time2, halfWidth / layerWidth2, 1),
            nullptr
        ));
        
        addChild(layer);
        
        layer = CCLayerColor::create({0, 0, 0, 255}, layerWidth2, m_size.height);
        layer->setAnchorPoint({1, 0});
        layer->setScaleX(0);
        layer->runAction(CCSequence::create(
            CCScaleTo::create(time1, 1, 1),
            CCScaleTo::create(time2, halfWidth / layerWidth2, 1),
            nullptr
        ));
        layer->setPositionX(m_size.width - layerWidth2);
        
        addChild(layer);
    }
    
    void chapter4Transition(float) {
        
    }

public:
    
    static CelesteTransition* create(int transition, float speed, float delay, bool reverse) {
        CelesteTransition* ret = new CelesteTransition(CCDirector::get()->getWinSize(), speed, reverse);
        ret->init(transition, delay);
        ret->autorelease();
        return ret;
    }

};

class CelesteRevive : public BaseAnimation {

private:

    std::vector<CCSprite*> m_animationSprites;
    std::unordered_map<CCSprite*, CCNodeRGBA*> m_players;
    std::unordered_map<CCNodeRGBA*, GLubyte> m_originalOpacities;
    
    ccColor3B m_color = {172, 62, 56};
    
    bool m_isWhite = false;

public:

    DEFINE_CREATE(CelesteRevive);

    void start() override {
        BaseAnimation::start();
                
        addChild(CelesteTransition::create(m_extras.transition, m_speed, 0.f, true));
        
        log::debug("{}", m_extras.transition);
        
        scheduleOnce(schedule_selector(CelesteRevive::playSound), 0.12f);
        scheduleOnce(schedule_selector(CelesteRevive::startAnimation), 0.25f);
        schedule(schedule_selector(CelesteRevive::updatePositions), 0, kCCRepeatForever, 0);
        schedule(schedule_selector(CelesteRevive::updateColors), 5.f / 60.f / m_speed, kCCRepeatForever, 5.f / 60.f / m_speed);
    }
    
    void startAnimation(float) {
        addAnimation(m_playLayer->m_player1);
    }
    
    void addAnimation(CCNodeRGBA* player) {
        CCArray* animFrames = CCArray::create();
        CCSpriteFrameCache* cache = CCSpriteFrameCache::get();
        cache->addSpriteFramesWithFile("celeste-revive.plist"_spr);
        
        for (int i = 1; i <= 22; i++)
            if (CCSpriteFrame* frame = cache->spriteFrameByName((fmt::format("celeste-revive-{}.png"_spr, i)).c_str()))
                animFrames->addObject(frame);
        
        CCSprite* sprite = CCSprite::createWithSpriteFrameName("celeste-revive-1.png"_spr);
        sprite->setColor(m_color);
        sprite->setScale(8.75f);
        sprite->getTexture()->setAliasTexParameters();
        sprite->runAction(
            CCSequence::create(
                CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, 1.f / 60.f / m_speed)),
                CCDelayTime::create(1.f / 60.f / m_speed),
                CCCallFunc::create(this, callfunc_selector(CelesteRevive::implosionEnded)),
                nullptr
            )
        );
        
        sprite->setPosition(player->getPosition());
        
        m_animationSprites.push_back(sprite);
        m_originalOpacities[player] = player->getOpacity();
        m_players[sprite] = player;
        
        player->setOpacity(0);
        player->getParent()->addChild(sprite);
    }
    
    void updatePositions(float) {
        for (CCSprite* sprite : m_animationSprites) {
            sprite->setPosition(m_players.at(sprite)->getPosition() + ccp(27, 4));
            m_players.at(sprite)->setOpacity(0);
        }
    }
    
    void updateColors(float) {
        m_isWhite = !m_isWhite;
        
        for (CCSprite* sprite : m_animationSprites)
            sprite->setColor(m_isWhite ? ccc3(255, 255, 255) : m_color);
    }
    
    void implosionEnded() {
        for (CCSprite* sprite : m_animationSprites) {
            sprite->removeFromParentAndCleanup(true);
            m_players.at(sprite)->setOpacity(m_originalOpacities.at(m_players.at(sprite)));
        }
        
        m_animationSprites.clear();
    }
    
    void playSound(float) {
        Utils::playSound(Anim::Celeste, "revive-celeste.wav", m_speed, 1.f);
    }
    
    void end() override {
        for (CCSprite* sprite : m_animationSprites)
            sprite->removeFromParentAndCleanup(true);
        
        m_animationSprites.clear();
            
        BaseAnimation::end();
    }
    
};

class Celeste : public BaseAnimation {
  
private:

    CelesteExplosion* m_explosion1 = nullptr;
    CelesteExplosion* m_explosion2 = nullptr;
    
    int m_transition = 0;
    
public:
    
    DEFINE_CREATE(Celeste)

    void start() override {
        BaseAnimation::start();
        
        m_transition = Utils::getSettingFloat(Anim::Celeste, "transition");
        if (m_transition == 0)
            m_transition = Utils::getRandomInt(1, 9);

        addChild(CelesteTransition::create(m_transition, m_speed, 0.65f, false));
        
        if (m_isPreview) {
            m_explosion1 = CelesteExplosion::create(m_delegate->getPlayer(), {15, 0}, {172, 62, 56}, m_speed);
            m_explosion1->setPosition(m_delegate->getPlayer()->getPosition());
            addChild(m_explosion1, 100);
        } else {
            PlayerObject* player = m_playLayer->m_player1;
            
            m_explosion1 = CelesteExplosion::create(
                player,
                ccp(player->m_isPlatformer ? player->m_platformerXVelocity : 15, player->m_yVelocity),
                {172, 62, 56},
                m_speed
            );
            m_explosion1->setPosition(player->getPosition());
            player->getParent()->addChild(m_explosion1, 10293823);
            
            if (Utils::getSettingBool(Anim::Celeste, "second-player")) {
                player = m_playLayer->m_player2;
                
                if (player && m_playLayer->m_gameState.m_isDualMode) {
                    m_explosion2 = CelesteExplosion::create(
                        player,
                        ccp(player->m_isPlatformer ? player->m_platformerXVelocity : 15, player->m_yVelocity),
                        {172, 62, 56},
                        m_speed
                    );
                    m_explosion2->setPosition(player->getPosition());
                    player->getParent()->addChild(m_explosion2, 10293823);
                }
            }
        }
        
        Utils::playSound(Anim::Celeste, "predeath-celeste.wav", m_speed, 0.5f);
        scheduleOnce(schedule_selector(Celeste::playDeathSound), 0.41f / m_speed);
        
        if (Utils::getSettingBool(Anim::Celeste, "respawn-animation"))
            scheduleOnce(schedule_selector(Celeste::transitionOut), Utils::getSelectedAnimation(Anim::Celeste).duration / m_speed);
    }
    
    void transitionOut(float) {
        CelesteRevive::create({m_parentNode, m_playLayer, m_delegate, m_speed, ExtraParams{ .transition = m_transition }})->start();
    }
    
    void playDeathSound(float) {
        Utils::playSoundManual(Anim::Celeste, "death-celeste.wav", m_speed, 1.f);
    }
    
    void end() override {
        if (m_explosion1) {
            m_explosion1->removeFromParentAndCleanup(true);
            m_explosion1 = nullptr;
        }
        
        if (m_explosion2) {
            m_explosion2->removeFromParentAndCleanup(true);
            m_explosion2 = nullptr;
        }
        
        BaseAnimation::end();
    }
};