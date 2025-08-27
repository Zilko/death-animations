#include "BaseAnimation.hpp"

class Maro : public BaseAnimation {

private:

    inline static const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif
        
        varying vec2 v_texCoord;
        uniform sampler2D u_texture;
        
        void main() {
            vec2 u_pixelSize = vec2(1.0 / 48.0, 1.0 / 48.0);
                
            vec2 coord = vec2(
                floor(v_texCoord.x / u_pixelSize.x) * u_pixelSize.x,
                floor(v_texCoord.y / u_pixelSize.y) * u_pixelSize.y
            );
            
            vec2 pixelCoord = floor(v_texCoord / u_pixelSize) * u_pixelSize;

            gl_FragColor = texture2D(u_texture, pixelCoord);
        }
    )";

    inline static const std::string m_shaderNES = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif

        varying vec2 v_texCoord;
        uniform sampler2D u_texture;

        void main() {
            vec2 u_pixelSize = vec2(1.0 / 48.0, 1.0 / 48.0);
            vec2 pixelCoord = floor(v_texCoord / u_pixelSize) * u_pixelSize;
            vec3 col = texture2D(u_texture, pixelCoord).rgb;

            if (texture2D(u_texture, pixelCoord).a == 0) {
                gl_FragColor = vec4(0, 0, 0, 0);
                return;
            }
            
            vec3 pal[64];
            pal[0] = vec3(0.329,0.329,0.329);
            pal[1] = vec3(0.000,0.118,0.455);
            pal[2] = vec3(0.031,0.063,0.565);
            pal[3] = vec3(0.188,0.000,0.533);
            pal[4] = vec3(0.294,0.000,0.294);
            pal[5] = vec3(0.294,0.000,0.000);
            pal[6] = vec3(0.235,0.000,0.000);
            pal[7] = vec3(0.141,0.047,0.000);
            pal[8] = vec3(0.047,0.071,0.000);
            pal[9] = vec3(0.000,0.071,0.000);
            pal[10] = vec3(0.000,0.071,0.035);
            pal[11] = vec3(0.000,0.063,0.141);
            pal[12] = vec3(0.000,0.000,0.000);
            pal[13] = vec3(0.000,0.000,0.000);
            pal[14] = vec3(0.000,0.000,0.000);
            pal[15] = vec3(0.000,0.000,0.000);
            pal[16] = vec3(0.565,0.565,0.565);
            pal[17] = vec3(0.047,0.298,0.882);
            pal[18] = vec3(0.141,0.141,0.878);
            pal[19] = vec3(0.329,0.047,0.827);
            pal[20] = vec3(0.494,0.000,0.494);
            pal[21] = vec3(0.494,0.000,0.000);
            pal[22] = vec3(0.376,0.000,0.000);
            pal[23] = vec3(0.376,0.188,0.000);
            pal[24] = vec3(0.188,0.235,0.000);
            pal[25] = vec3(0.000,0.235,0.000);
            pal[26] = vec3(0.000,0.235,0.188);
            pal[27] = vec3(0.000,0.188,0.376);
            pal[28] = vec3(0.000,0.000,0.000);
            pal[29] = vec3(0.000,0.000,0.000);
            pal[30] = vec3(0.000,0.000,0.000);
            pal[31] = vec3(0.000,0.000,0.000);
            pal[32] = vec3(0.878,0.878,0.878);
            pal[33] = vec3(0.329,0.471,1.000);
            pal[34] = vec3(0.494,0.494,1.000);
            pal[35] = vec3(0.667,0.329,1.000);
            pal[36] = vec3(0.878,0.329,0.878);
            pal[37] = vec3(0.878,0.329,0.329);
            pal[38] = vec3(0.878,0.220,0.220);
            pal[39] = vec3(0.878,0.533,0.000);
            pal[40] = vec3(0.600,0.600,0.000);
            pal[41] = vec3(0.329,0.878,0.329);
            pal[42] = vec3(0.329,0.878,0.667);
            pal[43] = vec3(0.329,0.710,0.878);
            pal[44] = vec3(0.400,0.400,0.400);
            pal[45] = vec3(0.000,0.000,0.000);
            pal[46] = vec3(0.000,0.000,0.000);
            pal[47] = vec3(0.000,0.000,0.000);
            pal[48] = vec3(1.000,1.000,1.000);
            pal[49] = vec3(0.706,0.706,1.000);
            pal[50] = vec3(0.800,0.800,1.000);
            pal[51] = vec3(0.878,0.706,1.000);
            pal[52] = vec3(1.000,0.706,1.000);
            pal[53] = vec3(1.000,0.706,0.706);
            pal[54] = vec3(1.000,0.600,0.600);
            pal[55] = vec3(1.000,0.878,0.400);
            pal[56] = vec3(1.000,1.000,0.400);
            pal[57] = vec3(0.706,1.000,0.706);
            pal[58] = vec3(0.706,1.000,0.878);
            pal[59] = vec3(0.706,0.878,1.000);
            pal[60] = vec3(0.706,0.706,0.706);
            pal[61] = vec3(0.000,0.000,0.000);
            pal[62] = vec3(0.000,0.000,0.000);
            pal[63] = vec3(0.000,0.000,0.000);
            
            vec3 nes = vec3(0.0);
            float best = 10.0;
            for(int i=0; i<64; i++) {
                float d = distance(col, pal[i]);
                if(d < best) {
                    best = d;
                    nes = pal[i];
                }
            }

            gl_FragColor = vec4(nes, texture2D(u_texture, pixelCoord).a);
        }
    )";

    CCGLProgram* m_program = nullptr;

    std::vector<std::pair<CCNodeRGBA*, GLubyte>> m_playerOpacities;
    std::vector<CCSprite*> m_sprites;

    bool m_isMarioSprite = false;

    ~Maro() {
        Utils::setHookEnabled("GJBaseGameLayer::update", false);
    }

    void playForPlayer(CCNodeRGBA* player) {
        CCNode* parent = m_isPreview ? this : player->getParent()->getParent();
        CCPoint position = m_isPreview ? player->getPosition() : parent->convertToNodeSpaceAR(player->convertToWorldSpaceAR({0, 0}));
        float scale = m_isPreview ? 1.f : m_playLayer->m_objectLayer->getScale();

        if (m_isMarioSprite) {
            CCSprite* spr = CCSprite::create("maro.png"_spr);
            spr->setID("maro-sprite"_spr);
            spr->setPosition(position);
            spr->setScale(7.f * scale);
            spr->getTexture()->setAliasTexParameters();
            spr->runAction(
                CCSequence::create(
                    CCDelayTime::create(0.35f),
                    CCJumpBy::create(2.f / m_speed, {0, -320}, 220, 1),
                    nullptr
                )
            );

            Utils::fixScaleTextureSizexd(spr);

            m_playerOpacities.push_back(std::make_pair(player, player->getOpacity()));

            parent->addChild(spr);
            player->setOpacity(0);
            Utils::setHighestZ(spr);

            m_sprites.push_back(spr);

            return;
        }
            

        CCSprite* spr = Utils::renderPlayerSprite(player, true);
        spr->setID("player-pixel-sprite"_spr);
        spr->setPosition(position);
        spr->setScale(scale);
        spr->setShaderProgram(m_program);
        spr->runAction(
            CCSequence::create(
                CCDelayTime::create(0.35f),
                CCJumpBy::create(2.f / m_speed, {0, -320}, 220, 1),
                nullptr
            )
        );

        m_playerOpacities.push_back(std::make_pair(player, player->getOpacity()));
        m_sprites.push_back(spr);

        parent->addChild(spr);
        player->setOpacity(0);
        Utils::setHighestZ(spr);
    }
    
    ANIMATION_CTOR_CREATE(Maro)
    
public:

    void start() override {
        BaseAnimation::start();

        m_isMarioSprite = Utils::getSettingBool(Anim::Maro, "use-maro-sprite");

        Utils::playSound(Anim::Maro, "maro-death.mp3", m_speed, 1.f);
        Utils::setHookEnabled("GJBaseGameLayer::update", true);

        if (!m_isMarioSprite)
            m_program = Utils::createShader(Utils::getSettingBool(Anim::Maro, "use-nes-colors") ? m_shaderNES : m_shader, true);

        if (m_isPreview)
            playForPlayer(m_delegate->getPlayer());
        else {
            playForPlayer(m_playLayer->m_player1);
            
            if (Utils::getSettingBool(Anim::Maro, "second-player") && m_playLayer->m_gameState.m_isDualMode)
                playForPlayer(m_playLayer->m_player2);
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