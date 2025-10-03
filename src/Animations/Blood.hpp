#include "BaseAnimation.hpp"

#include "../Other/SpeedCCParticleSystemQuad.hpp"

class BloodEffect : public CCNode {

private:

    inline static const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif

        varying vec4 v_fragmentColor;
        varying vec2 v_texCoord;
        uniform sampler2D u_texture;
        uniform float u_time;

        float rand(vec2 co) {
            return fract(sin(dot(co, vec2(12.9898,78.233))) * 43758.5453);
        }

        void main() {
            vec2 uv = v_texCoord * 2.0 - 1.0;
            float angle = atan(uv.y, uv.x);
            float radius = length(uv);

            float star = cos(float(min(rand(v_texCoord * u_time) + 0.5, 1.0) * 10.0) * angle) * 0.5 + 0.5;
            float progress = clamp(u_time / 0.25, 0.0, 1.0);
            float threshold = mix(0.0, 1.5, progress);
            float mask = step(threshold, radius * (0.6 + 0.4 * star));

            vec4 c = texture2D(u_texture, v_texCoord) * mask;

            if (max(max(c.r, c.g), c.b) > 0.11)
                gl_FragColor = vec4(0.4, 0.0, 0.0, c.a);
            else
                gl_FragColor = vec4(0.2, 0.0, 0.0, c.a);
        }
    )";

    inline static const ParticleStruct m_particleStruct = {137,1,1,0,3000,-90,180,230,132,11,0,0,-1000,0,0,0,0,3,4,0,0,0.4,0,0,0,0,0,1,0,0,3,0,0,0.4,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,2,false,false,false,false,2,false,0,0,0,0,false,false,false,false,false,0,0,0,0,"particle_02_001.png"};

    CCNode* m_player = nullptr;

    CCSprite* m_playerSprite = nullptr;

    CCGLProgram* m_program = nullptr;

    float m_speed = 1.f;
    float m_time = 0.f;

    BloodEffect(CCNode* player, float speed)
        : m_player(player), m_speed(speed) {}

    void update(float dt) override {
        m_time += dt * m_speed;

        m_program->use();
        glUniform1f(glGetUniformLocation(m_program->getProgram(), "u_time"), m_time);
    }

    void blood(float) {        
        SpeedCCParticleSystemQuad* particle = SpeedCCParticleSystemQuad::create(m_particleStruct);
        particle->setUpdateSpeed(m_speed);
        particle->resetSystem();
        particle->setAutoRemoveOnFinish(true);

        addChild(particle);
    }

    bool init() override {
        CCNode::init();

        setID("blood-effect"_spr);
        
        m_program = Utils::createShader(m_shader, true);

        m_playerSprite = Utils::renderPlayerSprite(static_cast<CCNodeRGBA*>(m_player), false);
        m_playerSprite->setShaderProgram(m_program);

        addChild(m_playerSprite);

        schedule(schedule_selector(BloodEffect::update));
        scheduleOnce(schedule_selector(BloodEffect::blood), 0.f);
        scheduleOnce(schedule_selector(BloodEffect::killMyself), 2.f / m_speed);

        return true;
    }

    void killMyself(float) {
        removeFromParentAndCleanup(true);
    }

public:

    static BloodEffect* create(CCNode* player, float speed) {
        BloodEffect* ret = new BloodEffect(player, speed);

        ret->init();
        ret->autorelease();

        return ret;
    }

};

class Blood : public BaseAnimation {

private:

    struct BloodDrop {
        int id;
        CCPoint offset;
        float rotation;
        float scale;
    };

    std::vector<BloodDrop> m_bloodDrops;
    std::vector<CCNode*> m_visiblePlayers;

    void addSpatter(CCNode* player) {
        CCPoint velocity = {0, 0};

        if (!m_isPreview) {
            PlayerObject* playerObject = static_cast<PlayerObject*>(player);

            velocity = ccp(
                playerObject->m_isPlatformer ? playerObject->m_platformerXVelocity : 15,
                playerObject->m_yVelocity
            );
        }
        
        float zoom = getCurrentZoom();
        float cameraAngle = m_isPreview ? 0.f : m_playLayer->m_gameState.m_cameraAngle;
        float angleReal = ccpToAngle(velocity) - M_PI;
        int bigOnes = 0;

        for (int i = 0; i < 70; i++) {
            float angle = (Utils::getRandomInt(1, 100) > 45 ? angleReal : CC_DEGREES_TO_RADIANS(Utils::getRandomInt(0, 360))) + cameraAngle;
            float spread = Utils::getRandomInt(-30, 30) * zoom;
            float dist = Utils::getRandomInt(50, 210) * zoom;
            float dx = cosf(angle);
            float dy = sinf(angle);
            float px = -dy;
            float py = dx;

            int id = Utils::getRandomInt(0, 100) > 14 || bigOnes > 1 ? Utils::getRandomInt(1, 6) : Utils::getRandomInt(7, 8);

            if (id >= 7) bigOnes++;

            m_bloodDrops.push_back({
                id,
                {dx * dist + px * spread, dy * dist + py * spread},
                static_cast<float>(Utils::getRandomInt(0, 360)),
                Utils::getRandomInt(-5, 5) / 10.f + (id <= 6 ? (Utils::getRandomInt(10, 25) / 10.f) : 2),
            });
        }

        for (size_t i = 0; i < m_bloodDrops.size(); i += 9) {
            runAction(CCSequence::create(
                CCDelayTime::create((i / 9.f + 1) * (0.01f / m_speed)),
                CallFuncExt::create([this, player, i, zoom] {
                    Utils::playSound(Anim::Blood, "blood-spat.mp3", 1.f + Utils::getRandomInt(-1, 1) / 10.f, 0, 0, 9999, 1.8f, false, 100);
                    
                    for (size_t j = i; j < i + 9 && j < m_bloodDrops.size(); ++j) {
                        const BloodDrop& bloodDrop = m_bloodDrops[j];

                        CCSprite* spr = CCSprite::create(fmt::format("blood-drop-{}.png"_spr, bloodDrop.id).c_str());
                        spr->setPosition(getNodeScreenPos(player) + bloodDrop.offset);
                        spr->setRotation(bloodDrop.rotation);
                        spr->setScale(bloodDrop.scale * zoom);

                        Utils::fixScaleTextureSizexd(spr);

                        addChild(spr);
                    }
                }),
                nullptr
            ));
        }
    }

    void playBlood(CCNode* player) {
        BloodEffect* blood = BloodEffect::create(player, m_speed);
        blood->setPosition(player->getPosition());

        player->getParent()->addChild(blood);
        
        if (player->isVisible()) {
            m_visiblePlayers.push_back(player);
            player->setVisible(false); 
        }

        Utils::setHighestZ(blood);

        if (Utils::getSettingBool(Anim::Blood, "blood-spatter"))
            addSpatter(player);
    }

    void onAnimationEnd() override {
        m_didFinish = true;
    }
    
    ANIMATION_CTOR_CREATE(Blood) {}

public:

    void start() override {
        Utils::playSound(Anim::Blood, "blood.mp3", m_speed, 0, 0, 9999, 1.f, false, 125);

        CCLayerColor* layer = CCLayerColor::create({100, 0, 0, 15});
        layer->runAction(CCEaseSineOut::create(CCFadeTo::create(1.6f, 0)));

        addChild(layer);

        Utils::setHighestZ(this);
        
        if (m_isPreview)
            playBlood(m_delegate->getPlayer());
        else {
            playBlood(m_playLayer->m_player1);
            
            if (Utils::getSettingBool(Anim::Blood, "second-player") && m_playLayer->m_gameState.m_isDualMode)
                playBlood(m_playLayer->m_player2);
        }
    }
    
    void end() override {
        for (CCNode* player : m_visiblePlayers)
            player->setVisible(true);
            
        BaseAnimation::end();
    }

};