#include "BaseAnimation.hpp"

class Wasted : public BaseAnimation {

private:

    const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif

        varying vec2 v_texCoord;
        uniform sampler2D u_texture;
        uniform float u_time;
        uniform int u_secondPart;

        float flashyFlash(float t) {
            return t < 0.1 ? 10.0 : 10.0 * (1.0 - min((t - 0.1) / 0.5, 1.0));
        }

        void main() {
            vec2 texelSize = vec2(1.0 / 1920.0, 1.0 / 1080.0);
            float amount = 0.5;

            float maxOffset = 0.0;

            if (u_time >= 2.2)
                maxOffset = max(maxOffset, flashyFlash(u_time - 2.2));   
            else
                maxOffset = max(maxOffset, flashyFlash(u_time));

            float offset = maxOffset * amount;
            vec2 o = texelSize * offset;

            vec4 sum = vec4(0.0);
            sum += texture2D(u_texture, v_texCoord + vec2(-o.x, -o.y));
            sum += texture2D(u_texture, v_texCoord + vec2( 0.0, -o.y));
            sum += texture2D(u_texture, v_texCoord + vec2( o.x, -o.y));
            sum += texture2D(u_texture, v_texCoord + vec2(-o.x,  0.0));
            sum += texture2D(u_texture, v_texCoord + vec2( 0.0,  0.0));
            sum += texture2D(u_texture, v_texCoord + vec2( o.x,  0.0));
            sum += texture2D(u_texture, v_texCoord + vec2(-o.x,  o.y));
            sum += texture2D(u_texture, v_texCoord + vec2( 0.0,  o.y));
            sum += texture2D(u_texture, v_texCoord + vec2( o.x,  o.y));
            vec4 color = sum / 9.0;

            float holdTime = 0.6;
            float fadeOutDuration = 4.0;
            float fadeInDuration = 0.15;

            float t = u_time;
            float effectStrength = 1.0;

            if (t < holdTime) {
                effectStrength = 1.0;
            } else if (t < holdTime + fadeOutDuration && t < 2.22) {
                float progress = (t - holdTime) / fadeOutDuration;
                effectStrength = 1.0 - progress;
            } else if (t >= 2.22) {
                effectStrength = 1.0;
            }

            float contrast = mix(1.0, 5.0 - min(u_time / 0.6, 1.0) * 2.0, effectStrength);
            vec3 boosted = (color.rgb - 0.5) * contrast + 0.5;
            boosted = mix(boosted * 0.5, boosted, smoothstep(0.4, 1.0, boosted));

            color.rgb = mix(color.rgb, boosted, effectStrength);

            vec3 saturated = u_time < 0.1
                ? mix(vec3(dot(color.rgb, vec3(0.2126, 0.7152, 0.4))), color.rgb, 0.3 - min(u_time / 0.1, 1.0) * 0.1)
                : mix(color.rgb, vec3(dot(color.rgb, vec3(0.299, 0.587, 0.4))), 0.7 + min(u_time / 1.85, 1.0) * 0.3);

            vec2 uv = v_texCoord * 2.0 - 1.0;
            float vignette = 1.0 - dot(uv, uv) * 0.3;
            vignette = clamp(vignette, 0.6, 1.0);

            vec3 finalColor = saturated * vignette;

            gl_FragColor = vec4(finalColor, color.a);
        }
    )";

    CCGLProgram* m_program = nullptr;

    CCRenderTexture* m_renderTexture = nullptr;

    CCSprite* m_frameSprite = nullptr;
    CCSprite* m_wastedSprite = nullptr;

    CCNode* m_wastedContainer = nullptr;

    float m_time = 0.f;

    ~Wasted() {
        if (m_renderTexture)
            m_renderTexture->release();
        
        if (m_program)
            m_program->release();

        Utils::setHookEnabled("cocos2d::CCParticleSystem::update", false);
        Utils::setHookEnabled("CCCircleWave::updateTweenAction", false);
        Utils::setHookEnabled("GJBaseGameLayer::update", false);
        Utils::setHookEnabled("ExplodeItemNode::update", false);
    }

    void update(float dt) override {
        if (m_frameSprite) m_frameSprite->setVisible(false);
        if (m_wastedContainer) m_wastedContainer->setVisible(false);
        
        (void)Utils::takeScreenshot(m_renderTexture);

        if (m_frameSprite) m_frameSprite->setVisible(true);
        if (m_wastedContainer) m_wastedContainer->setVisible(true);
    
        m_time += dt;

        m_program->use();
        m_program->setUniformLocationWith1f(glGetUniformLocation(m_program->getProgram(), "u_time"), m_time / m_speed);
    }

    void addWasted(float) {
        m_wastedContainer = CCNode::create();

        addChild(m_wastedContainer);

        CCLayerColor* layer1 = CCLayerColor::create({0, 0, 0, 255}, m_size.width + 5, 15);
        layer1->setPositionY(181);
        layer1->setRotation(-1);

        m_wastedContainer->addChild(layer1);

        CCLayerColor* layer2 = CCLayerColor::create({0, 0, 0, 255}, m_size.width + 5, 15);
        layer2->setPositionY(132);
        layer2->setRotation(1);
     
        m_wastedContainer->addChild(layer2);

        CCLayerColor* layer3 = CCLayerColor::create({0, 0, 0, 255}, m_size.width, 46);
        layer3->setPositionY(140);
     
        m_wastedContainer->addChild(layer3);

        CCRenderTexture* texture = CCRenderTexture::create(m_size.width, m_size.height);
        texture->getSprite()->setAnchorPoint({0, 0});
        texture->getSprite()->setOpacity(100);
        texture->getSprite()->setScaleY(1);

        texture->begin();

        m_wastedContainer->visit();

        texture->end();

        m_wastedContainer->addChild(texture);

        layer1->removeFromParentAndCleanup(true);
        layer2->removeFromParentAndCleanup(true);
        layer3->removeFromParentAndCleanup(true);

        m_wastedSprite = CCSprite::create("wasted.png"_spr);
        m_wastedSprite->setColor({187, 57, 57});
        m_wastedSprite->setOpacity(230);
        m_wastedSprite->setScale(1.06f);
        m_wastedSprite->setPosition(m_size / 2.f - ccp(0, 2));

        m_wastedContainer->addChild(m_wastedSprite);
        m_wastedContainer->setPositionY(8);
    }

    void startFadeOut(float) {
        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255}, m_size.width, m_size.height);
        layer->setOpacity(0);
        layer->runAction(CCFadeIn::create(2.5f / m_speed));
     
        addChild(layer, 10);

        m_wastedSprite->runAction(CCFadeTo::create(2.5f / m_speed, 0));
    }

public:

    DEFINE_CREATE(Wasted)

    void start() override {
        BaseAnimation::start();

        Utils::playSound(Anim::Wasted, "wasted.wav", m_speed, 1.f);

        Utils::setHookEnabled("cocos2d::CCParticleSystem::update", true);
        Utils::setHookEnabled("CCCircleWave::updateTweenAction", true);
        Utils::setHookEnabled("GJBaseGameLayer::update", true);
        Utils::setHookEnabled("ExplodeItemNode::update", true);

        Utils::setHighestZ(this);

        m_program = Utils::createShader(m_shader, false);

        m_renderTexture = CCRenderTexture::create(m_size.width, m_size.height);
        m_renderTexture->retain();

        update(0.f);

        m_frameSprite = CCSprite::createWithTexture(m_renderTexture->getSprite()->getTexture());
        m_frameSprite->setFlipY(true);
        m_frameSprite->setPosition(m_size / 2.f);
        m_frameSprite->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        m_frameSprite->setShaderProgram(m_program);

        addChild(m_frameSprite);

        CCSprite* copy = CCSprite::createWithTexture(m_renderTexture->getSprite()->getTexture());
        copy->setAnchorPoint({0, 1});
        copy->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        copy->setShaderProgram(m_program);

        m_frameSprite->addChild(copy);

        copy = CCSprite::createWithTexture(m_renderTexture->getSprite()->getTexture());
        copy->setPosition({0, m_size.height});
        copy->setAnchorPoint({0, 0});
        copy->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        copy->setShaderProgram(m_program);

        m_frameSprite->addChild(copy);

        copy = CCSprite::createWithTexture(m_renderTexture->getSprite()->getTexture());
        copy->setFlipY(true);
        copy->setFlipX(true);
        copy->setAnchorPoint({1, 0});
        copy->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        copy->setShaderProgram(m_program);

        m_frameSprite->addChild(copy);

        copy = CCSprite::createWithTexture(m_renderTexture->getSprite()->getTexture());
        copy->setFlipY(true);
        copy->setFlipX(true);
        copy->setPosition({m_size.width, 0});
        copy->setAnchorPoint({0, 0});
        copy->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        copy->setShaderProgram(m_program);

        m_frameSprite->addChild(copy);

        schedule(schedule_selector(Wasted::update), 1.f / 240.f, kCCRepeatForever, 1.f / 240.f); // papi

        CCPoint pos = m_frameSprite->getPosition();

        scheduleOnce(schedule_selector(Wasted::addWasted), 2.17f / m_speed);
        scheduleOnce(schedule_selector(Wasted::startFadeOut), 5.2f / m_speed);

        m_frameSprite->runAction(
            CCSpawn::create(
                CCSequence::create(
                    CCEaseSineOut::create(CCScaleTo::create(0.25f / m_speed, 1.12f)),
                    CCEaseSineInOut::create(CCScaleTo::create(0.2f / m_speed, 1.115f)),
                    CCEaseSineInOut::create(CCScaleTo::create(1.72f / m_speed, 1.13f)),
                    CCEaseSineInOut::create(CCScaleTo::create(0.03f / m_speed, 1.12f)),
                    CCEaseSineInOut::create(CCScaleTo::create(7.f / m_speed, 1.f)),
                    nullptr
                ),
                CCSequence::create(
                    CCDelayTime::create(0.05f / m_speed),
                    CCEaseSineInOut::create(CCRotateBy::create(2.12f / m_speed, 5.02857f)),
                    CCRotateBy::create(0.f, 5),
                    CCDelayTime::create(0.5f / m_speed),
                    CCEaseSineInOut::create(CCRotateBy::create(4.f / m_speed, -5.02857f)),
                    nullptr
                ),
                CCSequence::create(
                    CCDelayTime::create(0.05f / m_speed),
                    CCEaseSineOut::create(CCMoveTo::create(1.8f / m_speed, pos - ccp(10, -8))),
                    nullptr
                ),
                CCSequence::create(
                    CCDelayTime::create(0.3f / m_speed),
                    CCEaseSineOut::create(CCMoveTo::create(2.4f / m_speed, pos - ccp(3, -8))),
                    nullptr
                ),
                CCSequence::create(
                    CCDelayTime::create(0.5f / m_speed),
                    CCEaseSineInOut::create(CCMoveTo::create(2.4f / m_speed, pos - ccp(7, 23))),
                    nullptr
                ),
                CCSequence::create(
                    CCDelayTime::create(1.5f / m_speed),
                    CCEaseSineInOut::create(CCMoveBy::create(2.4f / m_speed, ccp(13, -5))),
                    nullptr
                ),
                CCSequence::create(
                    CCDelayTime::create(3.9f / m_speed),
                    CCSpawn::create(
                        CCSequence::create(
                            CCDelayTime::create(0.05f / m_speed),
                            CCEaseSineOut::create(CCMoveBy::create(1.8f / m_speed, ccp(0, 3))),
                            nullptr
                        ),
                        CCSequence::create(
                            CCDelayTime::create(0.3f / m_speed),
                            CCEaseSineOut::create(CCMoveBy::create(2.4f / m_speed, ccp(-3, 3))),
                            nullptr
                        ),
                        CCSequence::create(
                            CCDelayTime::create(0.5f / m_speed),
                            CCEaseSineInOut::create(CCMoveBy::create(2.4f / m_speed, ccp(-7, 12))),
                            nullptr
                        ),
                        CCSequence::create(
                            CCDelayTime::create(1.5f / m_speed),
                            CCEaseSineInOut::create(CCMoveBy::create(2.4f / m_speed, ccp(-13, 5))),
                            nullptr
                        ),
                        nullptr
                    ),
                    nullptr
                ),
                nullptr
            )
        );
    }

};