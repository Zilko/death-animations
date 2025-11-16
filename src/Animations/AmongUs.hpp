#include "BaseAnimation.hpp"

class AmongUs : public BaseAnimation {

private:
    
    inline static const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif
        
        varying vec2 v_texCoord;
        uniform sampler2D u_texture;
        
        uniform vec3 u_color1;
        uniform vec3 u_color2;
        
        void main() {
            vec4 c = texture2D(u_texture, v_texCoord);
            
            if (c.r / c.b > 2.0) {
                c.rgb = u_color1 * vec3(c.r, c.r, c.r);
            } else if (c.b / c.g > 2.0) {
                c.rgb = u_color2 * vec3(c.b, c.b, c.b);
            }
            
            gl_FragColor = c;
        }
    )";

    constexpr static std::array m_audioDelays = { 0.22f, 0.15f, 0.15f, 0.15f };

    CCLayerColor* m_redLayer = nullptr;
    
    CCSprite* m_bg = nullptr;
    CCSprite* m_animationSprite = nullptr;
    
    int m_currentFrame = 0;
    int m_animation = 3;

    void firstStep(float) {
        m_redLayer->setVisible(false);
        
        scheduleOnce(schedule_selector(AmongUs::secondStep), 0.09f / m_speed);
    }
    
    void secondStep(float) {
        m_bg->setScaleY(1.47f);
        m_bg->setRotation(15);
        
        scheduleOnce(schedule_selector(AmongUs::thirdStep), 0.09f / m_speed);
    }
    
    void thirdStep(float) {
        m_bg->setScaleY(2.98f);
        m_bg->setRotation(0);
        
        m_animation = Utils::getSettingFloat(Anim::AmongUs, "animation");
        if (m_animation == 0)
            m_animation = Utils::getRandomInt(1, 4);
        
        CCArray* animFrames = CCArray::create();
        CCSpriteFrameCache* cache = CCSpriteFrameCache::get();
        CCSpriteFrame* lastValidFrame = nullptr;
        
        m_animationSprite = CCSprite::createWithSpriteFrameName(fmt::format("among-us-death-{}-1.png"_spr, m_animation).c_str());

        if (!m_animationSprite || m_animationSprite->getUserObject("geode.texture-loader/fallback")) {
            cache->addSpriteFramesWithFile(fmt::format("among-us-death-{}.plist"_spr, m_animation).c_str());
        }

        for (int i = 1; i <= 115; ++i) {
            CCSpriteFrame* frame = cache->spriteFrameByName((fmt::format("among-us-death-{}-{}.png"_spr, m_animation, i)).c_str());
    
            if (frame && frame->getTag() != 105871529)
                lastValidFrame = frame;
            
            if (lastValidFrame)
                animFrames->addObject(lastValidFrame);
        }
        
        if (!m_animationSprite)
            m_animationSprite = CCSprite::createWithSpriteFrameName(fmt::format("among-us-death-{}-1.png"_spr, m_animation).c_str());

        if (!m_animationSprite)
            return;

        m_animationSprite->setPosition(m_size / 2.f + ccp(31.5f, -6));
        m_animationSprite->setScale(1.19f);
        m_animationSprite->runAction(
            CCSequence::create(
                CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, 1.f / 60.f / m_speed)),
                CCDelayTime::create(0.09f / m_speed),
                CCCallFunc::create(this, callfunc_selector(AmongUs::fourthStep)),
                nullptr
            )
        );  
        
        addChild(m_animationSprite);
        
        Utils::fixScaleTextureSizexd(m_animationSprite);

        CCGLProgram* program = Utils::createShader(m_shader, true);
        
        m_animationSprite->setShaderProgram(program);
        
        GameManager* gm = GameManager::get();

        ccColor4F color1 = ccc4FFromccc3B(gm->colorForIdx(gm->getPlayerColor()));
        ccColor4F color2 = ccc4FFromccc3B(gm->colorForIdx(gm->getPlayerColor2()));
        
        if (static_cast<int>(Utils::getSettingFloat(Anim::AmongUs, "colors") == 1)) {        
            color1 = {
                Utils::getSettingFloat(Anim::AmongUs, "r1") / 255.f,
                Utils::getSettingFloat(Anim::AmongUs, "g1") / 255.f,
                Utils::getSettingFloat(Anim::AmongUs, "b1") / 255.f,
                1.f
            };
            
            color2 = {
                Utils::getSettingFloat(Anim::AmongUs, "r2") / 255.f,
                Utils::getSettingFloat(Anim::AmongUs, "g2") / 255.f,
                Utils::getSettingFloat(Anim::AmongUs, "b2") / 255.f,
                1.f
            };
        }
        
        program->use();
        program->setUniformsForBuiltins();
        program->setUniformLocationWith3f(glGetUniformLocation(program->getProgram(), "u_color1"), color1.r, color1.g, color1.b);
        program->setUniformLocationWith3f(glGetUniformLocation(program->getProgram(), "u_color2"), color2.r, color2.g, color2.b);
        
        scheduleOnce(schedule_selector(AmongUs::playSound), m_audioDelays[m_animation - 1] / m_speed);
    }
    
    void fourthStep() {
        m_animationSprite->setVisible(false);
        m_bg->runAction(CCScaleTo::create(0.23333f / m_speed, 2.9f, 0.f));
    }
    
    void playSound(float) {
        Utils::playSound(Anim::AmongUs, fmt::format("death-{}-among-us.ogg", m_animation), m_speed, 0.5f);
    }
    
    ANIMATION_CTOR_CREATE(AmongUs) {}
    
public:

    void start() override {
        Utils::setHighestZ(this);
        
        Utils::playSound(Anim::AmongUs, "scary-among-us.ogg", m_speed, 0.5f);
        
        CCNode* bgContainer = CCNode::create();
        bgContainer->setPosition(m_size / 2.f);

        m_bg = CCSprite::create("bg-among-us.png"_spr);
        m_bg->setScaleX(2.98f);
        m_bg->setScaleY(0.88f);
        m_bg->setRotation(-25);

        CCSize size = m_bg->getContentSize();
        bool flipped = true;
        int added = 0;

        while (size.width * 0.745f < m_size.width) {
            log::debug("ad");

            added++;

            CCSprite* extra = CCSprite::create("bg-among-us.png"_spr);
            extra->setAnchorPoint({0, 0});
            extra->setFlipX(flipped);
            extra->setPosition({extra->getContentSize().width * added, 0});

            m_bg->addChild(extra);

            extra = CCSprite::create("bg-among-us.png"_spr);
            extra->setAnchorPoint({1, 0});
            extra->setFlipX(flipped);
            extra->setPosition({-extra->getContentSize().width * (added - 1), 0});

            m_bg->addChild(extra);

            size.width += extra->getContentSize().width * 2;

            flipped = !flipped;
        }

        bgContainer->addChild(m_bg);
        addChild(bgContainer);
        
        Utils::fixScaleTextureSizexd(bgContainer);
        
        m_redLayer = CCLayerColor::create({255, 1, 0, 255});
        
        addChild(m_redLayer);
        
        scheduleOnce(schedule_selector(AmongUs::firstStep), 0.09f / m_speed);
    }

};