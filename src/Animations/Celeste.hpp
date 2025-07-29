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
        
        for (int i = 1; i <= 43; ++i)
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

class Celeste : public BaseAnimation {
  
private:

    CelesteExplosion* m_explosion1 = nullptr;
    CelesteExplosion* m_explosion2 = nullptr;
    
    ~Celeste() {
        if (m_explosion1) {
            m_explosion1->removeFromParentAndCleanup(true);
            m_explosion1 = nullptr;
        }
        
        if (m_explosion2) {
            m_explosion2->removeFromParentAndCleanup(true);
            m_explosion2 = nullptr;
        }
    }
    
public:
    
    DEFINE_CREATE(Celeste)

    void start() override {
        BaseAnimation::start();
        
        
        
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
    }
    
    
    void playDeathSound(float) {
        Utils::playSound(Anim::AmongUs, "death-celeste.wav", m_speed, 1.f);
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