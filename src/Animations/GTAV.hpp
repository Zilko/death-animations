#include "BaseAnimation.hpp"

class GTAV : public BaseAnimation {

private:

    const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif
        
        varying vec2 v_texCoord;
        uniform sampler2D u_texture;
        
        uniform float u_time;
        
        void main() {
            vec4 color = texture2D(u_texture, v_texCoord);
            
            if (color.rgb == vec3(0, 0, 0))
                color = vec4(1, 1, 1, 1);
            
            gl_FragColor = color;
        }
    )";

    CCGLProgram* m_program = nullptr;

    CCRenderTexture* m_renderTexture = nullptr;

    CCSprite* m_frameSprite = nullptr;

    ~GTAV() {
        if (m_renderTexture)
            m_renderTexture->release();
        
        if (m_program)
            m_program->release();
    }

public:

    DEFINE_CREATE(GTAV)

    void start() override {
        BaseAnimation::start();

        setZOrder(12933);

        m_program = new CCGLProgram();
        m_program->retain();
        m_program->initWithVertexShaderByteArray(vertexShader.c_str(), m_shader.c_str());
        m_program->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        m_program->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        m_program->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        m_program->link();
        m_program->updateUniforms();

        m_renderTexture = CCRenderTexture::create(m_size.width, m_size.height);
        m_renderTexture->retain();

        readFrame(0.f);

        m_frameSprite = CCSprite::createWithTexture(m_renderTexture->getSprite()->getTexture());
        m_frameSprite->setFlipY(true);
        m_frameSprite->setPosition(m_size / 2.f);
        m_frameSprite->setShaderProgram(m_program);

        addChild(m_frameSprite);

        schedule(schedule_selector(GTAV::readFrame), 1.f / 240.f, kCCRepeatForever, 1.f / 240.f); // papi
    }

    void readFrame(float) {
        if (m_frameSprite) m_frameSprite->setVisible(false);
        
        (void)Utils::takeScreenshot(m_renderTexture);

        if (m_frameSprite) m_frameSprite->setVisible(true);
    }

    void end() override {
        BaseAnimation::end();

        
    }
};