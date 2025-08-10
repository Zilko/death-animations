#include "BaseAnimation.hpp"

class ToBeContinued : public BaseAnimation {

private:

    const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif
        
        varying vec2 v_texCoord;
        uniform sampler2D u_texture;
        
        vec3 applyYellowTint(vec3 color) {
            float gray = dot(color, vec3(0.299, 0.587, 0.114));
            vec3 yellow = vec3(1.0, 0.9, 0.4);
            return gray * yellow;
        }
        
        void main() {
            vec4 texColor = texture2D(u_texture, v_texCoord);
            vec3 tinted = applyYellowTint(texColor.rgb);
            vec3 finalColor = tinted * 1.1;
            gl_FragColor = vec4(finalColor, texColor.a);
        }
    )";
    
public:
    
    DEFINE_CREATE(ToBeContinued)

    void start() override {
        BaseAnimation::start();
        
        CCSprite* freezeSprite = CCSprite::createWithTexture(Utils::takeScreenshot());
        freezeSprite->setFlipY(true);
        freezeSprite->setScale(m_size.width / freezeSprite->getContentWidth());
        freezeSprite->setAnchorPoint({0, 0});
        freezeSprite->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        
        CCGLProgram* program = new CCGLProgram();
        program->autorelease();
        program->initWithVertexShaderByteArray(vertexShader.c_str(), m_shader.c_str());
        program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        program->link();
        program->updateUniforms();
        
        freezeSprite->setShaderProgram(program);
        
        addChild(freezeSprite);
        
        Utils::playSound(Anim::ToBeContinued, "roundabout.mp3", m_speed, 1.f);
        
        CCSprite* spr = CCSprite::create("to-be-continued.png"_spr);
        spr->setPosition({m_size.width, 16});
        spr->setAnchorPoint({0, 0});
        spr->runAction(CCMoveTo::create(0.07f, {16, 16}));
        
        addChild(spr);
    }
    
};