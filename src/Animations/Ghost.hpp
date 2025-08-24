#include "BaseAnimation.hpp"

class GhostNode : public CCNode {

private:

    inline static const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif

        varying vec2 v_texCoord;
        uniform sampler2D u_texture;
        uniform float u_time;

        void main() {
            vec2 uv = v_texCoord;

            float freq = 10.5;
            float speed = 4.0;
            float amp = 0.02;

            vec2 offset;
            offset.x = sin(uv.y * freq + u_time * speed) * amp;
            offset.y = cos(uv.x * freq + u_time * speed * 0.9) * amp;

            vec4 color = texture2D(u_texture, uv + offset);

            float gray = dot(color.rgb, vec3(0.299, 0.587, 0.114));
            color.rgb = mix(color.rgb, vec3(gray), 0.75);

            if (color.a != 0.0) {
                color.a = 0.7 - 0.7 * min(u_time / 1.5, 1.0);
                color.r = min(color.r * 2, 1.0);
                color.g = min(color.g * 2, 1.0);
                color.b = min(color.b * 2, 1.0);
            }
            
            gl_FragColor = color;
        }
    )";

    CCGLProgram* m_program = nullptr;
    
    float m_speed = 0.f;
    float m_time = 0.f;

    GhostNode(float speed)
        : m_speed(speed) {}

    void init(CCNodeRGBA* player) {
        (void)CCNode::init();

        setID("player-ghost"_spr);

        m_program = Utils::createShader(m_shader, true);

        CCSprite* spr = Utils::renderPlayer(player, true);
        spr->setShaderProgram(m_program);

        addChild(spr);

        spr->runAction(CCMoveBy::create(2.3f / m_speed, {0, 500}));

        scheduleOnce(schedule_selector(GhostNode::setHighestZ), 0.05f);
        schedule(schedule_selector(GhostNode::update));
    }

    void update(float dt) override {
        m_time += dt;
        m_program->use();
        glUniform1f(glGetUniformLocation(m_program->getProgram(), "u_time"), m_time);
    }

    void setHighestZ(float) {
        Utils::setHighestZ(this);
    }

public:

    static GhostNode* create(CCNodeRGBA* player, float speed) {
        GhostNode* ret = new GhostNode(speed);

        ret->init(player);
        ret->autorelease();

        return ret;
    }

};

class Ghost : public BaseAnimation {

private:

    std::vector<std::pair<CCNodeRGBA*, GLubyte>> m_playerOpacities;
    std::vector<GhostNode*> m_ghosts;

    void addGhost(CCNodeRGBA* player) {
        GhostNode* ghost = GhostNode::create(player, m_speed);
        ghost->setPosition(player->getPosition());

        Utils::setHighestZ(ghost);

        m_playerOpacities.push_back(std::make_pair(player, player->getOpacity()));
        m_ghosts.push_back(ghost);

        player->getParent()->addChild(ghost);
        player->setOpacity(0);
    }

public:
    
    DEFINE_CREATE(Ghost)

    void start() override {
        BaseAnimation::start();

        Utils::playSound(Anim::Ghost, "ghost.ogg", m_speed * 0.8f, 0.3f);
    
        if (m_isPreview)
            addGhost(m_delegate->getPlayer());
        else {
            addGhost(m_playLayer->m_player1);
            
            if (Utils::getSettingBool(Anim::Ghost, "second-player") && m_playLayer->m_gameState.m_isDualMode)
                addGhost(m_playLayer->m_player2);
        }
    }

    void end() override {
        for (const auto& [player, opacity] : m_playerOpacities)
            player->setOpacity(opacity);

        for (GhostNode* ghost : m_ghosts)
            ghost->removeFromParentAndCleanup(true);

        BaseAnimation::end();
    }

    };