#include "BaseAnimation.hpp"

class CCEaseCubeOut : public CCActionEase {
    
public:

    static CCEaseCubeOut* create(CCActionInterval* action) {
        auto ret = new CCEaseCubeOut();
        ret->initWithAction(action);
        ret->autorelease();
        return ret;
    }
    
    void update(float t) override {
        float tt = t - 1.f;
        m_pInner->update(tt * tt * tt + 1.f);
    }

};

class CelesteExplosion : public CCNode {
   
private:

    inline static const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif
        
        varying vec4 v_fragmentColor;
        varying vec2 v_texCoord;
        uniform sampler2D u_texture;
        
        uniform int u_white;
        uniform int u_fullWhite;
        uniform float u_time;
        
        void main() {
            vec2 u_pixelSize = vec2(1.0 / 48.0, 1.0 / 48.0);
                
            vec2 coord = vec2(
                floor(v_texCoord.x / u_pixelSize.x) * u_pixelSize.x,
                floor(v_texCoord.y / u_pixelSize.y) * u_pixelSize.y
            );
            
            vec2 pixelCoord = floor(v_texCoord / u_pixelSize) * u_pixelSize;
            if (distance(vec2(0.5, 0.5), pixelCoord) > mix(0.2, 0.08, clamp(u_time / 0.34, 0.0, 1.0))) {
                gl_FragColor = vec4(0, 0, 0, 0);
                return;
            }
        
            vec4 newColor = v_fragmentColor * texture2D(u_texture, coord);
            
            if ((u_white == 1 && newColor.rgb != vec3(0, 0, 0)) || u_fullWhite == 1)
                newColor = vec4(1, 1, 1, 1);
                
            vec3 grayscale = vec3(dot(newColor.rgb, vec3(0.299, 0.587, 0.114)));
            vec3 desaturated = mix(newColor.rgb, grayscale, 0.3);
            gl_FragColor = vec4(desaturated, newColor.a);
        }
    )";

    CCSprite* m_animationSprite = nullptr;
    CCSprite* m_playerSprite = nullptr;

    CCNodeRGBA* m_player = nullptr;
    
    CCGLProgram* m_program = nullptr;
    
    ccColor3B m_color;
    
    float m_speed;
    float m_time = 0.f;
    bool m_isWhite = false;
    
    CelesteExplosion(CCNodeRGBA* player, const ccColor3B& color, float speed)
        : m_player(player), m_color(color), m_speed(speed) {}

    ~CelesteExplosion() {
        m_player->setVisible(true);
    }

    bool init(const CCPoint& velocity) {
        setID("celeste-explosion"_spr);

        m_program = Utils::createShader(m_shader, true);

        m_playerSprite = Utils::renderPlayerSprite(m_player, false);
        m_playerSprite->setShaderProgram(m_program);
        
        setShaderState("u_white", 0);
        setShaderState("u_fullWhite", 0);
        glUniform1f(glGetUniformLocation(m_program->getProgram(), "u_time"), 0.f);
        
        m_player->setVisible(false);
        
        addChild(m_playerSprite);
        
        float angle = ccpToAngle(velocity) - M_PI;
        runAction(CCEaseCubeOut::create(CCMoveTo::create(0.41f / m_speed, getPosition() + ccp(cos(angle) * 47.f, sin(angle) * 47.f))));
        
        schedule(schedule_selector(CelesteExplosion::updateBall), 0, kCCRepeatForever, 0);
        scheduleOnce(schedule_selector(CelesteExplosion::firstStep), 1.f / 60.f / m_speed);
        
        return true;
    }
    
    void updateBall(float dt) {
        m_time += dt * m_speed;
        float t = std::min(m_time / 0.34f, 1.0f);
        float easedTime = 1.0f - cosf(t * M_PI_2);
        m_program->use();
        glUniform1f(glGetUniformLocation(m_program->getProgram(), "u_time"), easedTime);
    }
    
    void setShaderState(const char* name, int value) {
        m_program->use();
        glUniform1i(glGetUniformLocation(m_program->getProgram(), name), value);
    }
    
    void firstStep(float) {
        setShaderState("u_white", 1);
        scheduleOnce(schedule_selector(CelesteExplosion::secondStep), 3.f / 60.f / m_speed);
    }
    void secondStep(float) {
        setShaderState("u_white", 0);
        scheduleOnce(schedule_selector(CelesteExplosion::thirdStep), 2.f / 60.f / m_speed);
    }
    void thirdStep(float) {
        setShaderState("u_white", 1);
        scheduleOnce(schedule_selector(CelesteExplosion::fourthStep), 3.f / 60.f / m_speed);
    }
    void fourthStep(float) {
        setShaderState("u_white", 0);
        scheduleOnce(schedule_selector(CelesteExplosion::fifthStep), 3.f / 60.f / m_speed);
    }
    void fifthStep(float) {
        setShaderState("u_fullWhite", 1);
        scheduleOnce(schedule_selector(CelesteExplosion::sixthStep), 2.f / 60.f / m_speed);
    }
    void sixthStep(float) {
        setShaderState("u_fullWhite", 0);
        scheduleOnce(schedule_selector(CelesteExplosion::seventhStep), 3.f / 60.f / m_speed);
    }
    void seventhStep(float) {
        m_playerSprite->setColor({0, 0, 0});
        scheduleOnce(schedule_selector(CelesteExplosion::eighthStep), 2.f / 60.f / m_speed);
    }
    void eighthStep(float) {
        m_playerSprite->setColor({255, 255, 255});
        setShaderState("u_fullWhite", 1);
        scheduleOnce(schedule_selector(CelesteExplosion::ninthStep), 6.f / 60.f / m_speed);
    }
    void ninthStep(float) {
        CCArray* animFrames = CCArray::create();
        CCSpriteFrameCache* cache = CCSpriteFrameCache::get();
        cache->addSpriteFramesWithFile("celeste-explosion.plist"_spr);
        
        for (int i = 1; i <= 43; i++)
            if (CCSpriteFrame* frame = cache->spriteFrameByName((fmt::format("celeste-explosion-{}.png"_spr, i)).c_str()))
                if (frame->getTag() != 105871529)
                    animFrames->addObject(frame);
        
        m_animationSprite = CCSprite::createWithSpriteFrameName("celeste-explosion-1.png"_spr);
        m_animationSprite->setColor(m_color);
        m_animationSprite->setScale(8.75f);
        m_animationSprite->getTexture()->setAliasTexParameters();
        m_animationSprite->runAction(
            CCSequence::create(
                CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, 1.f / 60.f / m_speed)),
                CCDelayTime::create(1.f / 60.f / m_speed),
                CCCallFunc::create(this, callfunc_selector(CelesteExplosion::explosionEnded)),
                nullptr
            )
        );

        Utils::fixScaleTextureSizexd(m_animationSprite);
        
        m_animationSprite->setPosition(m_playerSprite->getPosition());
        m_playerSprite->setVisible(false);
        m_playerSprite->setShaderProgram(CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColor));
        
        addChild(m_animationSprite);
        
        unschedule(schedule_selector(CelesteExplosion::updateBall));
        schedule(schedule_selector(CelesteExplosion::updateColor), 5.f / 60.f / m_speed, kCCRepeatForever, 5.f / 60.f / m_speed);
    }
    
    void updateColor(float) {
        if (!m_animationSprite)
            return unschedule(schedule_selector(CelesteExplosion::updateColor));
        
        m_isWhite = !m_isWhite;
        
        m_animationSprite->setColor(m_isWhite ? ccc3(255, 255, 255) : m_color);
    }
    
    void explosionEnded(float) {
        m_animationSprite->removeFromParentAndCleanup(true);
        m_animationSprite = nullptr;
    }
    
public:
    
    static CelesteExplosion* create(CCNodeRGBA* player, const CCPoint& velocity, const ccColor3B& color, float speed) {
        CelesteExplosion* ret = new CelesteExplosion(player, color, speed);
        ret->init(velocity);
        ret->autorelease();
        return ret;
    }

};

class CelesteTransition : public BaseAnimation {

private:

    CCLayerColor* m_overlay = nullptr;
    
    bool m_reverse = false;
    bool m_didRevive = false;
    
    void chapter1Transition() {
        CCNode* container = CCNode::create();
        container->setPosition({m_reverse ? 0 : -m_size.width, m_reverse ? m_size.height : 0    });
        container->setScale(m_reverse ? -1 : 1);
        
        addChild(container);
                
        for (int i = 0; i < 6; i++) {
            CCSprite* spr = CCSprite::create("pointy-thing-celeste.png"_spr);
            spr->setScale(m_size.height / 6.f / spr->getContentHeight());
            spr->setPosition({360 - spr->getContentWidth() * spr->getScale() * i + 19 * i, m_size.height -spr->getContentHeight() * spr->getScale() / 2.f - spr->getContentHeight() * spr->getScale() * i});
            container->addChild(spr);
            
            CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255}, spr->getPositionX() + m_size.width, spr->getContentHeight() * spr->getScale());
            layer->setPosition({-m_size.width, spr->getPositionY() - spr->getContentHeight() * spr->getScale() / 2.f});
            container->addChild(layer);
        }
        
        container->runAction(CCMoveTo::create(0.5f / m_speed, {m_size.width * (m_reverse ? 2 : 1), container->getPositionY()}));
    }
    
    void chapter2Transition() {
        std::vector<std::vector<CCPoint>> circlePositions = {
            { { 24.f, 303.f }, { 26.f, 128.f }, { 8.f, 102.f } },
            { { 27.f, 11.f }, { 51.f, 68.f }, { 54.f, 149.f }, { 48.f, 310.f } },
            { { 47.17f, 270.52f }, { 99.31f, 257.48f }, { 67.61f, 229.63f }, { 20.20f, 214.52f }, { 23.76f, 185.19f }, { 65.54f, 176.89f }, { 68.20f, 92.15f }, { 19.31f, 47.70f } },
            { { 85.09f, 287.70f }, { 9.54f, 262.52f }, { 98.72f, 189.63f }, { 121.54f, 182.22f }, { 100.50f, 136.f }, { 131.02f, 136.30f }, { 88.06f, 48.f }, { 59.02f, 8.89f }, { 139.02f, 15.41f } },
            { { 161.24f, 301.93f }, { 143.76f, 215.11f }, { 142.87f, 92.44f }, { 133.39f, 64.59f }, { 88.06f, 100.44f } },
            { { 128.35f, 251.26f }, { 96.65f, 214.22f }, { 208.35f, 133.63f }, { 97.24f, 22.81f }, { 178.72f, 15.11f } },
            { { 143.17f, 308.15f }, { 169.83f, 221.04f }, { 177.24f, 187.56f }, { 200.35f, 179.85f }, { 163.91f, 147.26f }, { 168.94f, 107.85f }, { 211.61f, 99.26f }, { 211.91f, 68.74f }, { 179.61f, 48.89f } },
            { { 219.02f, 301.63f }, { 176.06f, 254.52f }, { 257.24f, 216.30f }, { 249.54f, 181.93f }, { 253.69f, 132.74f } },
            { { 248.06f, 293.93f }, { 246.57f, 251.85f }, { 209.83f, 248.59f }, { 197.69f, 214.52f }, { 327.17f, 94.22f }, { 240.06f, 65.19f }, { 257.24f, 29.33f }, { 219.61f, 15.11f } },
            { { 312.06f, 308.44f }, { 322.13f, 263.11f }, { 283.02f, 258.07f }, { 325.09f, 222.52f }, { 326.87f, 171.85f }, { 277.98f, 150.22f }, { 246.28f, 101.93f }, { 290.72f, 17.78f } },
            { { 279.46f, 306.07f }, { 288.35f, 208.89f }, { 279.76f, 175.70f }, { 312.35f, 129.19f }, { 292.80f, 108.15f }, { 285.09f, 53.63f }, { 331.02f, 23.70f }, { 361.83f, 28.74f } },
            { { 352.35f, 296.59f }, { 350.57f, 265.48f }, { 391.46f, 259.85f }, { 363.91f, 213.04f }, { 332.50f, 67.26f }, { 358.57f, 95.41f } },
            { { 442.13f, 310.22f }, { 408.35f, 208.30f }, { 361.83f, 186.07f }, { 364.20f, 130.96f }, { 389.98f, 106.96f }, { 426.72f, 128.30f }, { 350.87f, 69.33f }, { 409.54f, 28.44f }, { 433.24f, 10.07f } },
            { { 397.98f, 301.04f }, { 447.76f, 218.67f }, { 387.91f, 189.63f }, { 396.80f, 142.22f }, { 464.94f, 152.89f }, { 464.35f, 89.78f }, { 403.31f, 58.96f }, { 481.24f, 10.96f } },
            { { 442.13f, 271.70f }, { 480.65f, 216.f }, { 464.94f, 189.33f }, { 441.24f, 179.26f }, { 443.91f, 107.56f }, { 473.54f, 64.59f }, { 433.54f, 51.26f } },
            { { 478.87f, 295.70f }, { 513.83f, 293.93f }, { 549.69f, 142.22f }, { 548.80f, 52.15f }, { 523.02f, 71.41f } },
            { { 474.13f, 264.30f }, { 512.65f, 270.52f }, { 559.17f, 260.44f }, { 519.46f, 220.15f }, { 544.35f, 186.07f }, { 507.91f, 145.19f }, { 508.50f, 104.f }, { 560.06f, 100.15f }, { 503.46f, 12.44f } },
            { { 557.09f, 298.37f }, { 546.13f, 26.07f } },
            { { 540.80f, 218.67f } }
        };

        if (m_size.width > 569) {
            int count = 0;

            while (circlePositions.back().back().x < m_size.width + 50) {
                std::vector<CCPoint> extraPositions;
                
                for (const CCPoint& position : circlePositions[count])
                    extraPositions.push_back(position + ccp(565, 0));

                circlePositions.push_back(extraPositions);

                count++;

                if (count >= 19) count = 0;
            }
        }

        for (int i = 0; i < circlePositions.size(); i++)
            for (const CCPoint& position : circlePositions[i]) {
                CCSprite* spr = CCSprite::create("circle-celeste.png"_spr);

                Utils::fixScaleTextureSizexd(spr);

                float scale = 4.f * spr->getScale() * 0.5f;

                spr->setScale(m_reverse ? scale : 0.f);
                spr->setPosition(position);

                addChild(spr);

                spr->runAction(CCSequence::create(
                    CCDelayTime::create(i * (19.f / static_cast<int>(circlePositions.size()) / 60.f) / m_speed),
                    CCScaleTo::create(0.2f / m_speed, m_reverse ? 0.f : scale),
                    nullptr
                ));
            }
    }

    void chapter3Transition() {
        float time1 = 15.f / 60.f / m_speed;
        float time2 = 15.f / 60.f / m_speed;

        CCSprite* spr = CCSprite::create("keyhole-left-celeste.png"_spr);

        Utils::fixScaleTextureSizexd(spr);

        float scale1 = 2.275f * spr->getScale();
        float scale2 = 1.5f * spr->getScale();

        CCPoint originalLeftPos = ccp(0, m_size.height / 2.f);
        
        if (m_reverse) {
            spr->setPosition(m_size / 2.f);
            spr->setScale(0);
        } else {
            spr->setScaleX(0);
            spr->setScaleY(scale1);
            spr->setPosition(originalLeftPos);
        }
        spr->setAnchorPoint({1, 0.5f});

        addChild(spr);
        
        CCAction* leftAction = m_reverse ?
            CCSequence::create(
                CCScaleTo::create(time2, scale2, scale2),
                CCSpawn::create(
                    CCMoveTo::create(time1, originalLeftPos),
                    CCScaleTo::create(time1, 0, scale1),
                    nullptr
                ),
                nullptr
            ) :
            CCSequence::create(
                CCSpawn::create(
                    CCMoveTo::create(time1, m_size / 2.f),
                    CCScaleTo::create(time1, scale2, scale2),
                    nullptr
                ),
                CCScaleTo::create(time2, 0),
                nullptr
            );

        spr->runAction(leftAction);
        
        spr = CCSprite::create("keyhole-right-celeste.png"_spr);
        CCPoint originalRightPos = ccp(m_size.width, m_size.height / 2.f);

        if (m_reverse) {
            spr->setPosition(m_size / 2.f);
            spr->setScale(0);
        } else {
            spr->setScaleX(0);
            spr->setScaleY(scale1);
            spr->setPosition(originalRightPos);
        }
        spr->setAnchorPoint({0, 0.5f});

        addChild(spr);
        
        CCAction* rightAction = m_reverse ?
            CCSequence::create(
                CCScaleTo::create(time2, scale2, scale2),
                CCSpawn::create(
                    CCMoveTo::create(time1, originalRightPos),
                    CCScaleTo::create(time1, 0, scale1),
                    nullptr
                ),
                nullptr
            ) :
            CCSequence::create(
                CCSpawn::create(
                    CCMoveTo::create(time1, m_size / 2.f),
                    CCScaleTo::create(time1, scale2, scale2),
                    nullptr
                ),
                CCScaleTo::create(time2, 0),
                nullptr
            );
        spr->runAction(rightAction);
        
        float layerHeight = (m_size.height - spr->getContentHeight() * scale2) / 2.f;
        float layerWidth = (m_size.height - spr->getContentHeight() * scale1) / 2.f;
        float halfWidth = m_size.width / 2.f;
        float layerWidth2 = halfWidth - spr->getContentWidth() * scale2;
        
        auto createLayer = [this, time1, time2](const CCSize& size, const CCPoint& anchor, const CCPoint& pos, float scaleY, const CCPoint& targetScale) {
            CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255}, size.width, size.height);
            layer->setAnchorPoint(anchor);
            
            if (m_reverse) {
                layer->setScaleX(targetScale.x);
                layer->setScaleY(targetScale.y);
            } else {
                layer->setScaleX(0);
                layer->setScaleY(scaleY);
            }
            layer->setPosition(pos);
            
            CCAction* action = m_reverse ?
                CCSequence::create(
                    CCScaleTo::create(time2, 1, 1),
                    CCScaleTo::create(time1, 0, scaleY),
                    nullptr
                ) :
                CCSequence::create(
                    CCScaleTo::create(time1, 1, 1),
                    CCScaleTo::create(time2, targetScale.x, targetScale.y),
                    nullptr
                );
            
            layer->runAction(action);
            addChild(layer);
        };
        
        createLayer({halfWidth, layerHeight}, ccp(0,0), ccp(0,0), layerWidth / layerHeight, ccp(1, m_size.height / 2.f / layerHeight));
        createLayer({halfWidth, layerHeight}, ccp(0,1), ccp(0, m_size.height - layerHeight), layerWidth / layerHeight, ccp(1, m_size.height / 2.f / layerHeight));
        createLayer({halfWidth, layerHeight}, ccp(1,1), ccp(halfWidth, m_size.height - layerHeight), layerWidth / layerHeight, ccp(1, m_size.height / 2.f / layerHeight));
        createLayer({halfWidth, layerHeight}, ccp(1,0), ccp(halfWidth, 0), layerWidth / layerHeight, ccp(1, m_size.height / 2.f / layerHeight));
        createLayer({layerWidth2, m_size.height}, ccp(0,0), ccp(0,0), 1, ccp(halfWidth / layerWidth2, 1));
        createLayer({layerWidth2, m_size.height}, ccp(1,0), ccp(m_size.width - layerWidth2, 0), 1, ccp(halfWidth / layerWidth2, 1));
    }
    
    void chapter4Transition() {
        CCNode* container = CCNode::create();
        container->setScaleX(m_reverse ? -1 : 1);
        
        addChild(container);
        
        CCSprite* spr = nullptr;
                
        for (int i = 0; i < 27; i++) {
            spr = CCSprite::create("spiky-thing-celeste.png"_spr);
            spr->setScale(m_size.height / 27.f / spr->getContentHeight());
            spr->setAnchorPoint({0, 1});
            spr->setPosition({m_size.width, m_size.height - spr->getContentHeight() * spr->getScale() * i});
            container->addChild(spr);
            
            CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255});
            container->addChild(layer);
        }
        
        container->setPosition({m_reverse ? m_size.width : -m_size.width - spr->getContentWidth() * spr->getScale(), 0});
        container->runAction(CCMoveTo::create(0.3f / m_speed, {m_reverse ? m_size.width + m_size.width + spr->getContentWidth() * spr->getScale() : 0, 0}));
    }

    void chapter5Transition() {
        struct StripePair {
            CCLayerColor* bottom = nullptr;
            CCLayerColor* top = nullptr;
        };

        int count = static_cast<int>(m_size.width / 56.f + 0.5f);
        float width = m_size.width / count;
        float halfHeight = m_size.height / 2.f;
        float totalWidth = 0.f;

        std::vector<StripePair> stripes;

        for (int i = 0; i < count; i++) {
            CCLayerColor* stripe1 = CCLayerColor::create({0, 0, 0, 255}, width, halfHeight + Utils::getRandomInt(-halfHeight / 2.f, halfHeight));
            stripe1->setPositionX(totalWidth);
            stripe1->setAnchorPoint({0, 0});
            stripe1->setScaleY(m_reverse ? 1 : 0);

            addChild(stripe1);

            CCLayerColor* stripe2 = CCLayerColor::create({0, 0, 0, 255}, width, m_size.height - stripe1->getContentHeight());
            stripe2->setPosition({totalWidth, m_size.height - stripe2->getContentHeight()});
            stripe2->setAnchorPoint({0, 1});
            stripe2->setScaleY(m_reverse ? 1 : 0);

            addChild(stripe2);

            stripes.push_back(StripePair{stripe1, stripe2});

            totalWidth += width;
        }

        float i = 0;

        for (const auto& [stripe1, stripe2] : stripes) {
            float time = 0.35f / m_speed;

            i += (10.f / m_speed) / count;

            stripe1->runAction(CCSequence::create(
                CCDelayTime::create(0.06f * i),
                m_reverse 
                    ? static_cast<CCAction*>(CCEaseSineOut::create(
                        CCScaleTo::create(time - (i / 50.f), 1, m_reverse ? 0 : 1)))
                    : static_cast<CCAction*>(CCEaseSineIn::create(
                        CCScaleTo::create(time - (i / 50.f), 1, m_reverse ? 0 : 1))),
                nullptr
            ));

            stripe2->runAction(CCSequence::create(
                CCDelayTime::create(0.06f * i),
                m_reverse 
                    ? static_cast<CCAction*>(CCEaseSineOut::create(
                        CCScaleTo::create(time - (i / 50.f), 1, m_reverse ? 0 : 1)))
                    : static_cast<CCAction*>(CCEaseSineIn::create(
                        CCScaleTo::create(time - (i / 50.f), 1, m_reverse ? 0 : 1))),
                nullptr
            ));
        }
    }

    CCNode* createArrow() {
        auto createHalf = [this](bool right) -> CCNode* {
            CCNode* container = CCNode::create();

            float mult = right ? 1 : -1;
            float width = 0.f;
            int count = 0;

            CCSprite* spr = nullptr; 

            while (width < m_size.width / 2.f) {
                spr = CCSprite::create("celeste-boi.png"_spr);
                spr->setAnchorPoint(ccp(right ? 0 : 1, 1));
                spr->setFlipX(right);

                Utils::fixScaleTextureSizexd(spr);

                CCSize sprSize = spr->getContentSize() * spr->getScale();

                spr->setPosition({sprSize.width * mult * count, -sprSize.height * count});

                container->addChild(spr);

                width += sprSize.width;
                count++;

                CCLayerColor* layer = CCLayerColor::create(
                    {0, 0, 0, 255},
                    sprSize.width,
                    m_size.width + spr->getPositionY() - sprSize.height
                );
                layer->setPosition(spr->getPosition() - sprSize * spr->getAnchorPoint() - ccp(0, layer->getContentHeight()));

                container->addChild(layer);
            }

            if (!right) {
                CCPoint edge = spr->getPosition() - (spr->getContentSize() * spr->getScale() * spr->getAnchorPoint());

                CCLayerColor* layer = CCLayerColor::create(
                    {0, 0, 0, 255},
                    abs(edge.x) + m_size.width / 2.f,
                    m_size.height
                );
                layer->setPosition({edge.x - (m_reverse ? m_size.width / 2.f : 0), edge.y - m_size.height});

                container->addChild(layer);
            }

            return container;
        };

        CCNode* container = CCNode::create();

        CCNode* leftHalf = createHalf(false);
        CCNode* rightHalf = createHalf(true);

        if (m_reverse) {
            leftHalf->setPositionX(m_size.width / 2.f);
            rightHalf->setPositionX(-m_size.width / 2.f);
        }

        container->addChild(leftHalf);
        container->addChild(rightHalf);
        
        return container;
    }

    void chapter6Transition() {
        CCNode* container = createArrow();
        float finalY = static_cast<int>(m_size.width / 159.f + 0.5f) * 159.f * 0.5f + m_size.height;

        if (!m_reverse)
            container->setScaleY(-1);

        container->setPosition({m_size.width / 2.f, m_reverse ? finalY : m_size.height});
        container->runAction(CCMoveBy::create(0.35f / m_speed, {0, -finalY}));

        addChild(container);
    }
    
    void chapter7Transition() {
        CCNode* container = createArrow();
        float finalY = static_cast<int>(m_size.width / 159.f + 0.5f) * 159.f * 0.5f + m_size.height;

        if (m_reverse)
            container->setScaleY(-1);

        container->setPosition({m_size.width / 2.f, m_reverse ? -finalY + m_size.height : 0});
        container->runAction(CCMoveBy::create(0.35f / m_speed, {0, finalY}));

        addChild(container);
    }

    void chapter8Transition() {
        CCSprite* spr = CCSprite::create("celeste-heart.png"_spr);
        spr->setPosition(m_size / 2.f);

        float finalScale = 0.f;

        if (m_size.width > m_size.height)
            finalScale = m_size.width * 2.f / spr->getContentWidth();
        else
            finalScale = m_size.height * 2.f / spr->getContentHeight();

        spr->setScale(m_reverse ? 0 : finalScale);

        spr->runAction(CCScaleTo::create(0.4f / m_speed, m_reverse ? finalScale : 0));

        CCClippingNode* clapper = CCClippingNode::create();
        clapper->setStencil(spr);
        clapper->setInverted(true);
        clapper->setAlphaThreshold(0.05f);

        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255});
        clapper->addChild(layer);

        addChild(clapper);
    }

    void setAnimationID() override {
        setID("celeste-transition"_spr);
    }

    void startTransition(float) {
        if (m_overlay)
            m_overlay->setVisible(false);

        switch(m_extras.transition) {
            case 2: chapter1Transition(); break;
            case 3: chapter2Transition(); break;
            case 4: chapter3Transition(); break;
            case 5: chapter4Transition(); break;
            case 6: chapter5Transition(); break;
            case 7: chapter6Transition(); break;
            case 8: chapter7Transition(); break;
            case 9: chapter8Transition(); break;
        }
    }
    
    bool playerIsDead() {
        return (!m_isPreview && m_playLayer->m_player1->m_isDead)
            || (m_isPreview && m_delegate->isDead());
    }
    
    void update(float) override {
        if (m_didRevive && playerIsDead())
            setVisible(false);
        else if (!playerIsDead())
            m_didRevive = true;
    }
    
    void transitionEnded(float) {
        end();
    }

    ANIMATION_CTOR_CREATE(CelesteTransition) {}

public:
    
    void start() override {
        m_reverse = m_extras.reverse;

        if (m_isPreview)
            setZOrder(10);
        else
            Utils::setHighestZ(this);

        if (m_reverse) {
            m_overlay = CCLayerColor::create({0, 0, 0, 255});

            addChild(m_overlay);

            scheduleOnce(schedule_selector(CelesteTransition::startTransition), 0.07f);
        }
        else
            startTransition(0.f);
        
        schedule(schedule_selector(CelesteTransition::update));
        scheduleOnce(schedule_selector(CelesteTransition::transitionEnded), 1.f / m_speed);
    }

};

class CelesteRevive : public BaseAnimation {

private:

    SoundUpdater* m_sound = nullptr;

    std::vector<CCSprite*> m_animationSprites;
    std::unordered_map<CCSprite*, CCNodeRGBA*> m_players;
    
    bool m_isWhite = false;

    ~CelesteRevive() {
        SoundManager::release(m_sound);
        SoundManager::stop();
    }

    void startAnimations(float) {
        for (CCSprite* sprite : m_animationSprites) {
            Utils::fixScaleTextureSizexd(sprite);

            CCArray* animFrames = CCArray::create();
            CCSpriteFrameCache* cache = CCSpriteFrameCache::get();
            cache->addSpriteFramesWithFile("celeste-revive.plist"_spr);
            
            for (int i = 1; i <= 22; i++)
                if (CCSpriteFrame* frame = cache->spriteFrameByName((fmt::format("celeste-revive-{}.png"_spr, i)).c_str()))
                    if (frame->getTag() != 105871529)
                        animFrames->addObject(frame);
            
            sprite->runAction(
                CCSequence::create(
                    CCAnimate::create(CCAnimation::createWithSpriteFrames(animFrames, 1.f / 60.f / m_speed)),
                    CCDelayTime::create(1.f / 60.f / m_speed),
                    CCCallFunc::create(this, callfunc_selector(CelesteRevive::implosionEnded)),
                    nullptr
                )
            );

            sprite->setVisible(true);
        }
    }
    
    void addAnimation(CCNodeRGBA* player) {
        CCSpriteFrameCache* cache = CCSpriteFrameCache::get();
        cache->addSpriteFramesWithFile("celeste-revive.plist"_spr);

        CCSprite* sprite = CCSprite::createWithSpriteFrame(cache->spriteFrameByName("celeste-revive-1.png"_spr));
        sprite->setColor({172, 62, 56});
        sprite->setScale(8.75f);
        sprite->getTexture()->setAliasTexParameters();
        sprite->setPosition(player->getPosition());
        sprite->setVisible(false);
        
        m_animationSprites.push_back(sprite);
        m_players[sprite] = player;
        
        player->setVisible(false);
        player->getParent()->addChild(sprite);
    }
    
    void update(float) override {
        for (CCSprite* sprite : m_animationSprites) {
            sprite->setPosition(m_players.at(sprite)->getPosition() + ccp(27, 4));
            m_players.at(sprite)->setVisible(false);
        }
    }
    
    void updateColors(float) {
        m_isWhite = !m_isWhite;
        
        for (CCSprite* sprite : m_animationSprites)
            sprite->setColor(m_isWhite ? ccc3(255, 255, 255) : ccc3(172, 62, 56));
    }
    
    void implosionEnded() {
        for (CCSprite* sprite : m_animationSprites) {
            sprite->removeFromParentAndCleanup(true);
            m_players.at(sprite)->setVisible(true);
        }
        
        m_animationSprites.clear();
    }
    
    void playSound(float) {
        m_sound = Utils::playSoundManual(Anim::Celeste, "revive-celeste.wav", m_speed, FMODAudioEngine::get()->m_sfxVolume);
        SoundManager::retain(m_sound);
    }

    void setAnimationID() override {
        setID("celeste-revive"_spr);
    }

    ANIMATION_CTOR_CREATE(CelesteRevive) {}

public:

    void start() override {
        if (m_isPreview)
            setZOrder(10);
        else
            Utils::setHighestZ(this);
        
        if (m_isPreview)
            addAnimation(m_delegate->getPlayer());
        else {
            addAnimation(m_playLayer->m_player1);
            
            if (Utils::getSettingBool(Anim::Celeste, "second-player") && m_playLayer->m_gameState.m_isDualMode)
                addAnimation(m_playLayer->m_player2);
        }

        bool hasTransition = m_extras.transition != 0;
        
        scheduleOnce(schedule_selector(CelesteRevive::playSound), (hasTransition ? 0.12f : 0.05f) / m_speed);
        scheduleOnce(schedule_selector(CelesteRevive::startAnimations), (hasTransition ? 0.25f : 0.18f) / m_speed);
        schedule(schedule_selector(CelesteRevive::updateColors), 5.f / 60.f / m_speed, kCCRepeatForever, 5.f / 60.f / m_speed);
        schedule(schedule_selector(CelesteRevive::update));
    }
    
    void end() override {
        for (CCSprite* sprite : m_animationSprites)
            sprite->removeFromParentAndCleanup(true);
        
        m_animationSprites.clear();
                
        BaseAnimation::end();
    }
    
};

class Celeste : public BaseAnimation {
  
private:

    inline static const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif

        varying vec2 v_texCoord;
        uniform sampler2D u_texture;

        uniform float u_time;
        uniform float u_aspectRatio;
        uniform vec2 u_origin;

        void main() {
            vec4 baseColor = texture2D(u_texture, v_texCoord);

            vec2 diff = (v_texCoord - u_origin) * vec2(u_aspectRatio, 1.0);

            float d2 = dot(diff, diff);

            float radius = u_time * 3.5;
            float thickness = u_time < 0.3 ? 0.17 : 0.04;

            if (radius < 0.1)
                radius = 0.1;

            float r1 = radius - thickness;
            float r2 = radius;
            float r3 = radius + thickness;

            float r1sq = r1 * r1;
            float r2sq = r2 * r2;
            float r3sq = r3 * r3;

            float edge = smoothstep(r1sq, r2sq, d2) - smoothstep(r2sq, r3sq, d2);

            vec2 dir = diff;
            if (d2 > 0.0) dir /= sqrt(d2);

            vec2 distortedCoord = v_texCoord + dir * edge * 0.02;

            gl_FragColor = mix(baseColor, texture2D(u_texture, distortedCoord), edge);
        }
    )";

    CCGLProgram* m_program = nullptr;

    CCRenderTexture* m_renderTexture = nullptr;

    CCSprite* m_frameSprite = nullptr;

    CelesteTransition* m_transitionNode = nullptr;

    CelesteExplosion* m_explosion1 = nullptr;
    CelesteExplosion* m_explosion2 = nullptr;

    const std::unordered_map<int, float> m_transitionDelays = {
        { 2, 0.91f },
        { 3, 0.93f },
        { 4, 0.84f },
        { 5, 1.f },
        { 6, 0.6f },
        { 7, 0.95f },
        { 8, 0.95f },
        { 9, 0.9f }
    };

    float m_time = 0.f;

    int m_transition = 0;

    void transitionOut(float) {
        if (Utils::getSettingBool(Anim::Celeste, "respawn-animation"))
            CelesteRevive::create({
                .parentNode = m_parentNode,
                .playLayer = m_playLayer,
                .delegate = m_delegate,
                .extras = { .transition = m_transition },
                .speed = Utils::getSettingBool(Anim::Celeste, "speed-affects-respawn") ? m_speed : 1.f,
            })->start();
        
        if (m_transition != 0)
            CelesteTransition::create({
                .parentNode = m_parentNode,
                .playLayer = m_playLayer,
                .delegate = m_delegate, 
                .extras = { .transition = m_transition, .reverse = true},
                .speed = m_speed,
            })->start();

        if (m_isDelayRestart) {
            m_dontRestart = false;
            m_isDelayRestart = false;

            resetLevel();
        }
    }
    
    void playDeathSound(float) {
        Utils::playSoundManual(Anim::Celeste, "death-celeste.wav", m_speed, FMODAudioEngine::get()->m_sfxVolume);
    }

    void enableDelayedRestart(float) {
        m_isDelayRestart = true;
    }
    
    void playTransition(float) {
        scheduleOnce(schedule_selector(Celeste::transitionOut), m_duration / m_speed - m_transitionDelays.at(m_transition));

        if (m_transition != 0) {
            m_transitionNode = CelesteTransition::create({
                .parentNode = this,
                .playLayer = m_playLayer,
                .delegate = m_delegate,
                .extras = { .transition = m_transition },
                .speed = m_speed, 
            });
        
            m_transitionNode->start();
        }
    }

    void updateShockwave(float dt) {
        if (m_frameSprite) m_frameSprite->setVisible(false);
        if (m_transitionNode) m_transitionNode->setVisible(false);
        if (m_isPreview) m_delegate->getBackButton()->setVisible(false);
        
        Utils::takeScreenshot(m_renderTexture);

        if (m_frameSprite) m_frameSprite->setVisible(true);
        if (m_transitionNode) m_transitionNode->setVisible(true);
        if (m_isPreview) m_delegate->getBackButton()->setVisible(true);

        m_time += dt * m_speed;

        if (m_time > 1.f) {
            m_frameSprite->setVisible(false);
            return CCScheduler::get()->unscheduleSelector(schedule_selector(Celeste::updateShockwave), this);
        }

        m_program->use();
        m_program->setUniformLocationWith1f(glGetUniformLocation(m_program->getProgram(), "u_time"), m_time);
    }

    void startShockwave(float) {
        m_program = Utils::createShader(m_shader, false);

        m_renderTexture = CCRenderTexture::create(m_size.width, m_size.height);
        m_renderTexture->retain();

        update(0.f);

        m_frameSprite = CCSprite::createWithTexture(m_renderTexture->getSprite()->getTexture());
        m_frameSprite->setVisible(false);
        m_frameSprite->setFlipY(true);
        m_frameSprite->setPosition(m_size / 2.f);
        m_frameSprite->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        m_frameSprite->setShaderProgram(m_program);

        addChild(m_frameSprite);

        m_program->use();

        CCPoint pos = getNodeScreenPos(m_explosion1);

        m_program->setUniformLocationWith1f(
            glGetUniformLocation(m_program->getProgram(), "u_aspectRatio"),
            m_size.width / m_size.height
        );

        m_program->setUniformLocationWith1f(
            glGetUniformLocation(m_program->getProgram(), "u_time"),
            0.f
        );

        m_program->setUniformLocationWith2f(
            glGetUniformLocation(m_program->getProgram(), "u_origin"),
            pos.x / m_size.width,
            pos.y / m_size.height
        );

        runAction(CCSequence::create(
            CCDelayTime::create(1.f / 240.f),
            CallFuncExt::create([this] {
                schedule(schedule_selector(Celeste::updateShockwave));
            }),
            nullptr
        ));
    }

    void onAnimationEnd() override {
        m_didFinish = true;
    }

    ~Celeste() {
        if (m_renderTexture)
            m_renderTexture->release();
    }
        
    ANIMATION_CTOR_CREATE(Celeste) {}
    
public:

    void onRestart() override {
        if (!m_isDelayRestart || m_dontRestart) return;

        m_dontRestart = true;

        playTransition(0.f);
    }

    void startEarly() override {
        if (!m_isPreview)
            Utils::setHighestZ(this);

        if (Utils::getSettingBool(Anim::Celeste, "shockwave"))
            scheduleOnce(schedule_selector(Celeste::startShockwave), 0.5f / m_speed);

        m_transition = Utils::getSettingFloat(Anim::Celeste, "transition");
        if (m_transition == 1)
            m_transition = Utils::getRandomInt(2, 9);   
                
        scheduleOnce(schedule_selector(Celeste::playDeathSound), 0.45f / m_speed);

        if (GameManager::get()->getGameVariable("0026") || m_isPreview)
            scheduleOnce(schedule_selector(Celeste::playTransition), m_transitionDelays.at(m_transition) / m_speed);
        else
            scheduleOnce(schedule_selector(Celeste::enableDelayedRestart), 0.7f / m_speed);

        if (m_isPreview) {
            m_explosion1 = CelesteExplosion::create(m_delegate->getPlayer(), {15, 0}, {172, 62, 56}, m_speed);
            m_explosion1->setPosition(m_delegate->getPlayer()->getPosition());
            addChild(m_explosion1, 9);
            
            return;
        }

        DashRingObject* dashOrb1 = nullptr;
        DashRingObject* dashOrb2 = nullptr;

        if (!m_isPreview) {
            if (m_playLayer->m_player1->m_isDashing)
                dashOrb1 = m_playLayer->m_player1->m_dashRing;

            if (m_playLayer->m_player2->m_isDashing && m_playLayer->m_gameState.m_isDualMode)
                dashOrb2 = m_playLayer->m_player2->m_dashRing;
        }
        
        PlayerObject* player = m_playLayer->m_player1;
        ccColor3B color = dashOrb1 ? (dashOrb1->m_objectID == 1704 ? ccc3(0, 255, 0) : ccc3(254, 1, 212)) : ccc3(172, 62, 56);

        m_explosion1 = CelesteExplosion::create(
            player,
            ccp(player->m_isPlatformer ? player->m_platformerXVelocity : 15, player->m_yVelocity),
            color,
            m_speed
        );
        m_explosion1->setPosition(player->getPosition());
        
        player->getParent()->addChild(m_explosion1);

        Utils::setHighestZ(m_explosion1);
        
        if (!Utils::getSettingBool(Anim::Celeste, "second-player") || !m_playLayer->m_gameState.m_isDualMode) return;
        
        player = m_playLayer->m_player2;
        color = dashOrb2 ? (dashOrb2->m_objectID == 1704 ? ccc3(0, 255, 0) : ccc3(254, 1, 212)) : ccc3(172, 62, 56);
        
        m_explosion2 = CelesteExplosion::create(
            player,
            ccp(player->m_isPlatformer ? player->m_platformerXVelocity : 15, player->m_yVelocity),
            color,
            m_speed
        );
        m_explosion2->setPosition(player->getPosition());

        player->getParent()->addChild(m_explosion2);

        Utils::setHighestZ(m_explosion2);
    }
    
    void start() override {
        Utils::playSound(Anim::Celeste, "predeath-celeste.wav", m_speed, 0.5f);
    }
    
    void end() override {
        if (m_explosion1) {
            m_explosion1->removeFromParentAndCleanup(true);
            m_explosion1 = nullptr;
        }
        
        if (m_explosion2) {
            m_explosion2->removeFromParentAndCleanup(true);
            m_explosion2 = nullptr;
        }

        BaseAnimation::end();
    }
};