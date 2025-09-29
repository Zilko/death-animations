#include "BaseAnimation.hpp"

#include "../Other/Variables.hpp"

class FadeOut : public BaseAnimation {

private:

    inline static const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif

        varying vec2 v_texCoord;
        uniform sampler2D u_texture;
        uniform float u_progress;

        void main() {
            vec4 c = texture2D(u_texture, v_texCoord);
            float g = dot(c.rgb, vec3(0.299, 0.587, 0.114));
            vec3 wa = mix(c.rgb, vec3(g, g, g), u_progress);
            gl_FragColor = vec4(wa.r, wa.g, wa.b, c.a);
        }
    )";

    CCGLProgram* m_program = nullptr;

    CCRenderTexture* m_renderTexture = nullptr;

    CCSprite* m_frameSprite = nullptr;

    float m_time = 0.f;
    float m_ogMusicPitch = 1.f;
    float m_ogSfxPitch = 1.f;
    float m_ogMusicVolume = 1.f;
    float m_ogSfxVolume = 1.f;

    bool m_audioOnly = false;

    void update(float dt) {
        m_time += dt * m_speed;

        FMODAudioEngine* fmod = FMODAudioEngine::get();

        float mainProgress = 1.f - std::min(m_time / 1.f, 1.f);
        float volumeProgress = 1.f - std::min(m_time / 0.95f, 1.f);

        fmod->m_backgroundMusicChannel->setPitch(m_ogMusicPitch * mainProgress);
        fmod->m_globalChannel->setPitch(m_ogSfxPitch * mainProgress);

        fmod->m_backgroundMusicChannel->setVolume(m_ogMusicVolume * volumeProgress);
        fmod->m_globalChannel->setVolume(m_ogSfxVolume * volumeProgress);

        Variables::setSpeed(std::max(mainProgress, 0.0001f));
    }

    void updateShader(float dt) {
        m_frameSprite->setVisible(false);

        if (m_renderTexture)
            Utils::takeScreenshot(m_renderTexture);

        m_frameSprite->setVisible(true);

        if (m_program) {
            m_program->use();
            m_program->setUniformLocationWith1f(
                glGetUniformLocation(m_program->getProgram(), "u_progress"),
                std::min(m_time / 1.f, 1.f) * 0.5f
            );
        }
    }

    void toggleHooks(bool toggled) {
        if (m_audioOnly) return;
        
        Utils::setHookEnabled("cocos2d::CCParticleSystem::update", toggled);
        Utils::setHookEnabled("CCCircleWave::updateTweenAction", toggled);
        Utils::setHookEnabled("GJBaseGameLayer::update", toggled);
        Utils::setHookEnabled("ExplodeItemNode::update", toggled);
    }

    void onAnimationEnd() override {
        m_didFinish = true;
    }

    ~FadeOut() {
        if (m_renderTexture) {
            m_renderTexture->release();
            m_renderTexture = nullptr;
        }

        FMODAudioEngine* fmod = FMODAudioEngine::get();

        fmod->m_backgroundMusicChannel->setPitch(m_ogMusicPitch);
        fmod->m_globalChannel->setPitch(m_ogSfxPitch);

        fmod->m_backgroundMusicChannel->setVolume(m_ogMusicVolume);
        fmod->m_globalChannel->setVolume(m_ogSfxVolume);

        Variables::setSpeed(1.f);

        toggleHooks(false);
    }

    ANIMATION_CTOR_CREATE(FadeOut) {}
    
public:

    void start() override {
        if (Utils::getSettingBool(Anim::FadeOut, "desaturate")) {
            m_program = Utils::createShader(m_shader, false);

            m_renderTexture = CCRenderTexture::create(m_size.width, m_size.height);
            m_renderTexture->retain();

            updateShader(0.f);

            m_frameSprite = CCSprite::createWithTexture(m_renderTexture->getSprite()->getTexture());
            m_frameSprite->setFlipY(true);
            m_frameSprite->setPosition(m_size / 2.f);
            m_frameSprite->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
            // m_frameSprite->setShaderProgram(m_program);

            addChild(m_frameSprite);

            float fps = std::min(static_cast<int>(GameManager::get()->m_customFPSTarget), 240);
            schedule(schedule_selector(FadeOut::updateShader), 1.f / fps, kCCRepeatForever, 1.f / fps);
        }

        m_audioOnly = !Utils::getSettingBool(Anim::FadeOut, "slow-down");

        Variables::setSpeed(1.f);

        toggleHooks(true);

        FMODAudioEngine* fmod = FMODAudioEngine::get();

        fmod->m_backgroundMusicChannel->getPitch(&m_ogMusicPitch);
        fmod->m_globalChannel->getPitch(&m_ogSfxPitch);
        
        m_ogMusicVolume = fmod->m_musicVolume;
        m_ogSfxVolume = fmod->m_sfxVolume;

        schedule(schedule_selector(FadeOut::update));
    }

    void onPause() override {
        toggleHooks(false);

        FMODAudioEngine* fmod = FMODAudioEngine::get();

        fmod->m_backgroundMusicChannel->setVolume(m_ogMusicVolume);
        fmod->m_globalChannel->setVolume(m_ogSfxVolume);
    }

    void onResume() override {
        toggleHooks(true);

        FMODAudioEngine* fmod = FMODAudioEngine::get();

        m_ogMusicVolume = fmod->m_musicVolume;
        m_ogSfxVolume = fmod->m_sfxVolume;
    }
    
};