#include "BaseAnimation.hpp"

class Pop : public BaseAnimation {

private:

    const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif

        varying vec2 v_texCoord;
        uniform sampler2D u_texture;

        uniform float u_time;

        void main() {
            vec4 c = texture2D(u_texture, v_texCoord);

            gl_FragColor = c;
        }
    )";

    CCGLProgram* m_program = nullptr;

    std::vector<std::pair<CCNodeRGBA*, GLubyte>> m_playerOpacities;
    std::vector<CCSprite*> m_sprites;

    float m_time = 0.f;

    void addPip(CCNodeRGBA* player) {
        CCSprite* spr = Utils::renderPlayer(player, true);
        spr->setPosition(player->getPosition());
        spr->setScale(m_isPreview ? 1.f : m_playLayer->m_gameState.m_cameraZoom);
        spr->setRotation(player->getRotation());
        spr->setZOrder(player->getZOrder());
        spr->setShaderProgram(m_program);

        m_playerOpacities.push_back(std::make_pair(player, player->getOpacity()));
        m_sprites.push_back(spr);

        player->getParent()->addChild(spr);
        player->setOpacity(0);
    }

    void update(float dt) {
        m_time += dt;
        m_program->use();
        glUniform1f(glGetUniformLocation(m_program->getProgram(), "u_time"), m_time);
    }

public:
    
    DEFINE_CREATE(Pop)

    void start() override {
        BaseAnimation::start();

        m_program = Utils::createShader(m_shader, true);

        schedule(schedule_selector(Pop::update));

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

        for (CCSprite* spr : m_sprites)
            spr->removeFromParentAndCleanup(true);

        BaseAnimation::end();
    }

};