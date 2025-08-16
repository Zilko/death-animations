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

        void main() {
            vec2 texelSize = vec2(1.0 / 1920.0, 1.0 / 1080.0);
            float amount = 0.5;

            float maxOffset = u_time < 0.1 ? 10.0 
                : 10.0 * (1.0 - min((u_time - 0.1) / 0.5, 1.0));

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

            float contrast = 5.0 - min(u_time / 0.6, 1.0) * 2.0;
            color.rgb = (color.rgb - 0.5) * contrast + 0.5;

            float shadowBoost = 0.5;
            color.rgb = mix(color.rgb * shadowBoost, color.rgb, smoothstep(0.4, 1.0, color.rgb));

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

        schedule(schedule_selector(Wasted::update), 1.f / 240.f, kCCRepeatForever, 1.f / 240.f); // papi

        CCPoint pos = m_frameSprite->getPosition();

        m_frameSprite->runAction(
            CCSpawn::create(
                CCSequence::create(
                    CCEaseSineOut::create(CCScaleTo::create(0.25f, 1.12f)),
                    CCEaseSineInOut::create(CCScaleTo::create(0.2f, 1.115f)),
                    nullptr
                ),
                CCSequence::create(
                    CCDelayTime::create(0.05f),
                    CCEaseSineOut::create(CCMoveTo::create(2.3f, pos - ccp(10, -5))),
                    nullptr
                ),
                nullptr
            )
        );
    }

    void update(float dt) override {
        if (m_frameSprite) m_frameSprite->setVisible(false);
        
        (void)Utils::takeScreenshot(m_renderTexture);

        if (m_frameSprite) m_frameSprite->setVisible(true);
    
        m_time += dt;

        m_program->use();
        m_program->setUniformLocationWith1f(glGetUniformLocation(m_program->getProgram(), "u_time"), m_time);
    }
};