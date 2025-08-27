#include "BaseAnimation.hpp"

class PlayerBalloon : public CCNode {

private:

    inline static const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif

        varying vec2 v_texCoord;
        uniform sampler2D u_texture;

        uniform float u_time;

        void main() {
            vec2 center = vec2(0.5, 0.5);
            vec2 uv = v_texCoord - center;

            float dist = length(uv);
            float factor = 1.0;

            if (dist < 0.5)
                factor = pow(dist, min(u_time / 0.4, 1.0) * 0.85);

            vec2 distortedUV = uv * factor + center;
            vec4 color = texture2D(u_texture, distortedUV);

            gl_FragColor = color;
        }
    )";

    CCGLProgram* m_program = nullptr;

    CCSprite* m_sprite = nullptr;

    float m_speed = 1.f;
    float m_time = 0.f;
    float m_delay = 0.f;

    PlayerBalloon(float speed)
    : m_speed(speed) {
        m_program = Utils::createShader(m_shader, true);
        m_delay = Utils::getRandomInt(0, 10) / 100.f;
    }

    void update(float dt) {
        m_time += dt;

        if (m_time < m_delay + 0.08f) return;

        m_program->use();
        glUniform1f(glGetUniformLocation(m_program->getProgram(), "u_time"), m_time - m_delay - 0.08f);
    }

    void pop(float) {
        CCSpriteFrameCache* cache = CCSpriteFrameCache::get();
        cache->addSpriteFramesWithFile("FireSheet_01.plist");

        CCArray* animFrames = CCArray::create();
        
        for (int i = 1; i <= 5; ++i) {
            if (CCSpriteFrame* frame = cache->spriteFrameByName(fmt::format("gj22_anim_05_color_00{}.png", i).c_str()))
                animFrames->addObject(frame);
        }
        
        CCSprite* explosion = CCSprite::createWithSpriteFrame(cache->spriteFrameByName("gj22_anim_05_color_001.png"));
        explosion->setPosition(m_sprite->getPosition());
        explosion->setScale(3.f);
        explosion->setOpacity(210);
        explosion->runAction(CCSequence::create(
            CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, 0.07f / 5.f)),
            CCDelayTime::create(0.07f / 5.f),
            CCHide::create(),
            nullptr
        ));

        m_sprite->getParent()->addChild(explosion);
        m_sprite->setVisible(false);
    }

    void playInflate(float) {
        Utils::playSound(Anim::Pop, "inflate.mp3", m_speed * 1.5f, 1.f);
    }

    void playPop(float) {
        Utils::playSound(Anim::Pop, "pop.mp3", m_speed, 1.f);
    }

    void init(CCNodeRGBA* player) {
        CCNode::init();

        Utils::playSound(Anim::Pop, "dart.mp3", m_speed * 1.5, 1.5f);

        m_sprite = Utils::renderPlayerSprite(player, true);
        m_sprite->setID("pop-sprite"_spr);
        m_sprite->setPosition(player->getPosition());
        m_sprite->setRotation(player->getRotation());
        m_sprite->setZOrder(player->getZOrder());
        m_sprite->setShaderProgram(m_program);

        addChild(m_sprite);

        player->getParent()->addChild(this);

        scheduleOnce(schedule_selector(PlayerBalloon::pop), 0.35f + m_delay);
        scheduleOnce(schedule_selector(PlayerBalloon::playPop), 0.25f + m_delay);
        scheduleOnce(schedule_selector(PlayerBalloon::playInflate), m_delay);
        schedule(schedule_selector(PlayerBalloon::update));
    }

public:

    static PlayerBalloon* create(CCNodeRGBA* player, float speed) {
        PlayerBalloon* ret = new PlayerBalloon(speed);

        ret->init(player);
        ret->autorelease();

        return ret;
    }

};

class Pop : public BaseAnimation {

private:

    std::vector<std::pair<CCNodeRGBA*, GLubyte>> m_playerOpacities;
    std::vector<PlayerBalloon*> m_balloons;

    void addPip(CCNodeRGBA* player) {
        PlayerBalloon* balloon = PlayerBalloon::create(player, m_speed);

        m_balloons.push_back(balloon);
        m_playerOpacities.push_back(std::make_pair(player, player->getOpacity()));

        player->setOpacity(0);
    }
    
    ANIMATION_CTOR_CREATE(Pop)
    
public:

    void start() override {
        BaseAnimation::start();

        if (m_isPreview)
            addPip(m_delegate->getPlayer());
        else {
            addPip(m_playLayer->m_player1);
            
            if (Utils::getSettingBool(Anim::Pop, "second-player") && m_playLayer->m_gameState.m_isDualMode)
                addPip(m_playLayer->m_player2);
        }
    }

    void end() override {
        for (const auto& [player, opacity] : m_playerOpacities)
            player->setOpacity(opacity);

        for (PlayerBalloon* balloon : m_balloons)
            balloon->removeFromParentAndCleanup(true);

        BaseAnimation::end();
    }

};