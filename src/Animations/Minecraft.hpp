#include "BaseAnimation.hpp"

class MinecraftParticle : public CCNode {

private:

    CCSprite* m_sprite = nullptr;

    int m_currentStep = 1;

    bool init() override {
        CCNode::init();

        bool shouldFall = Utils::getRandomInt(1, 10) <= 3;
        bool shouldSlide = Utils::getRandomInt(1, 10) <= 5;

        m_sprite = CCSprite::create("minecraft-particle-1.png"_spr);
        m_sprite->getTexture()->setAliasTexParameters();
        m_sprite->setFlipX(Utils::getRandomInt(0, 1));
        m_sprite->setFlipY(Utils::getRandomInt(0, 1));
        m_sprite->runAction(CCSpawn::create(
            CCMoveBy::create(10.f, {0, 300.f}),
            CCEaseOut::create(CCMoveBy::create(1.2f, {0, shouldFall ? -15.f : 0.f}), 2.f),
            CCEaseOut::create(CCMoveBy::create(2.f, {shouldSlide ? (5.f * (Utils::getRandomInt(0, 1) ? 1 : -1)) : 0.f, 0}), 2.f),
            nullptr
        ));

        Utils::fixScaleTextureSizexd(m_sprite);

        m_sprite->setScale(4 * m_sprite->getScale() * (Utils::getRandomInt(0, 1) ? 1.f : 1.9f));

        if (Utils::getRandomInt(1, 10) <= 4) {
            m_sprite->setColor({ 190, 190, 190 });
            setZOrder(-1);
        }

        addChild(m_sprite);

        runAction(CCSequence::create(
            CCDelayTime::create(Utils::getRandomInt(1, 2) / 10.f),
            CCCallFunc::create(this, callfunc_selector(MinecraftParticle::nextStep)),
            nullptr
        ));

        return true;
    }

    void nextStep() {
        if (++m_currentStep > 9)
            return removeFromParentAndCleanup(true);

        runAction(CCSequence::create(
            CCDelayTime::create(1.f / 8.f + Utils::getRandomInt(1, 2) / 10.f),
            CCCallFunc::create(this, callfunc_selector(MinecraftParticle::nextStep)),
            nullptr
        ));

        if (m_currentStep == 9)
            return m_sprite->setVisible(false);

        m_sprite->setTexture(CCSprite::create(fmt::format("minecraft-particle-{}.png"_spr, m_currentStep).c_str())->getTexture());
        m_sprite->getTexture()->setAliasTexParameters();
    }
 
public:

    static MinecraftParticle* create() {
        MinecraftParticle* ret = new MinecraftParticle();

        ret->init();
        ret->autorelease();

        return ret;
    }

};

class MinecraftLabel : public CCNode {

private:

    CCLabelBMFont* m_mainLabel = nullptr;
    CCLabelBMFont* m_shadowLabel = nullptr;

    void init(const std::string& text, int shadow) {
        CCNode::init();

        m_shadowLabel = CCLabelBMFont::create(text.c_str(), "minecraft.fnt"_spr);
        m_shadowLabel->setColor(ccc3(shadow, shadow, shadow));
        m_shadowLabel->setPosition({2, -2});

        Utils::fixScaleTextureSizexd(m_shadowLabel);

        addChild(m_shadowLabel);

        m_mainLabel = CCLabelBMFont::create(text.c_str(), "minecraft.fnt"_spr);

        Utils::fixScaleTextureSizexd(m_mainLabel);

        addChild(m_mainLabel);
    }

public:

    static MinecraftLabel* create(const std::string& text, int shadow) {
        MinecraftLabel* ret = new MinecraftLabel();

        ret->init(text, shadow);
        ret->autorelease();

        return ret;
    }

    CCLabelBMFont* getMainLabel() {
        return m_mainLabel;
    }

    CCLabelBMFont* getShadowLabel() {
        return m_shadowLabel;
    }

    void setDisabled(bool disabled) {
        m_mainLabel->setColor(disabled ? ccc3(150, 150, 150) : ccc3(255, 255, 255));
        m_shadowLabel->setVisible(!disabled);
    }

};

class MinecraftButton : public CCNode {

private:

    CCSprite* m_disabledSprite = nullptr;
    CCSprite* m_normalSprite = nullptr;
    CCSprite* m_selectedSprite = nullptr;

    MinecraftLabel* m_label = nullptr;

    bool m_isDisabled = false;
    bool m_isSelected = false;

    void init(const std::string& text) {
        CCNode::init();

        setAnchorPoint({0.5f, 0.5f});
        setContentSize({177.5f, 17.75f});

        m_disabledSprite = CCSprite::create("button-disabled-minecraft.png"_spr);
        m_disabledSprite->setPosition(getContentSize() / 2.f);
        m_disabledSprite->getTexture()->setAliasTexParameters();
        m_disabledSprite->setScale(3.55f);
        m_disabledSprite->setVisible(false);

        Utils::fixScaleTextureSizexd(m_disabledSprite);

        addChild(m_disabledSprite);

        m_normalSprite = CCSprite::create("button-minecraft.png"_spr);
        m_normalSprite->setPosition(getContentSize() / 2.f);
        m_normalSprite->getTexture()->setAliasTexParameters();
        m_normalSprite->setScale(3.55f);

        Utils::fixScaleTextureSizexd(m_normalSprite);

        addChild(m_normalSprite);

        m_selectedSprite = CCSprite::create("button-selected-minecraft.png"_spr);
        m_selectedSprite->setPosition(getContentSize() / 2.f);
        m_selectedSprite->getTexture()->setAliasTexParameters();
        m_selectedSprite->setScale(3.55f);
        m_selectedSprite->setVisible(false);

        Utils::fixScaleTextureSizexd(m_selectedSprite);

        addChild(m_selectedSprite);

        m_label = MinecraftLabel::create(text, 50);
        m_label->setPosition(getContentSize() / 2.f + ccp(0, 2.5f));
        m_label->setScale(0.448f);

        addChild(m_label);
    }

public:

    static MinecraftButton* create(const std::string& text) {
        MinecraftButton* ret = new MinecraftButton();

        ret->init(text);
        ret->autorelease();

        return ret;
    }

    bool isSelected() {
        return m_isSelected;
    }

    void setSelected(bool selected) {
        if (m_isDisabled || selected == m_isSelected) return;

        m_isSelected = selected;

        m_selectedSprite->setVisible(selected);
        m_normalSprite->setVisible(!selected);
    }

    bool isDisabled() {
        return m_isDisabled;
    }

    void setDisabled(bool disabled) {
        m_isDisabled = disabled;

        m_label->setDisabled(disabled);
        m_disabledSprite->setVisible(disabled);
        m_normalSprite->setVisible(!m_isSelected && !disabled);
        m_selectedSprite->setVisible(m_isSelected && !disabled);
    }

};

class Minecraft : public BaseAnimation {

private:

    inline static const std::string m_shader = R"(
        #ifdef GL_ES
        precision mediump float;
        #endif

        varying vec4 v_fragmentColor;
        varying vec2 v_texCoord;
        uniform sampler2D u_texture;

        void main() {
            vec4 c = texture2D(u_texture, v_texCoord);
            float brightness = dot(c.rgb, vec3(0.299, 0.587, 0.114));
            vec3 redTint = vec3(brightness, 0.0, 0.0);
            vec3 finalColor = mix(vec3(brightness), redTint, 0.55);
            gl_FragColor = vec4(finalColor, c.a);
        }
    )";


    constexpr static std::array m_deathMessages = std::to_array<std::string_view>({
        "{} was pricked to death","{} drowned","{} experienced kinetic energy","{} blew up","{} was killed by [Intentional Game Design]","{} hit the ground too hard","{} fell from a high place","{} fell off a ladder","{} fell off some vines","{} fell off some weeping vines","{} fell off some twisting vines","{} fell off scaffolding","{} fell while climbing","{} was impaled on a stalagmite","{} was skewered by a falling stalactite","{} was squashed by a falling anvil","{} went up in flames","{} burned to death","{} went off with a bang","{} tried to swim in lava","{} was struck by lightning","{} discovered the floor was lava","{} was killed by magic","{} froze to death","{} starved to death","{} suffocated in a wall","{} was squished too much","{} was poked to death by a sweet berry bush","{} fell out of the world","{} withered away","{} was stung to death","{} died from dehydration","{} was doomed to fall","{} was squashed by a falling block","{} died","{} was killed","{} was killed by even more magic","{} was roasted in dragon's breath","{} left the confines of this world","{} was too soft for this world","{} experienced the dark side of the moon","{} was turned into gold","{} held the hot potato for too long.","{} was killed by a bad-tempered potato","{} was devoured"
    });

    CCSprite* m_playerSprite = nullptr;

    SoundUpdater* m_quitSound = nullptr;

    MinecraftButton* m_respawnButton = nullptr;
    MinecraftButton* m_titleScreenButton = nullptr;

    bool m_isHolding = false;

    bool isHoveringButton(CCNode* button, const CCPoint& pos) {
        CCPoint localPos = button->convertToNodeSpace(pos);
        CCRect rect = CCRect(0, 0, button->getContentSize().width, button->getContentSize().height);

        return rect.containsPoint(localPos);
    }

    void enableButtons(float) {
        m_respawnButton->setDisabled(false);
        m_titleScreenButton->setDisabled(false);

        m_dontRestart = false;
    }

    void checkButtons(const CCPoint& pos) {
        m_respawnButton->setSelected(isHoveringButton(m_respawnButton, pos));
        m_titleScreenButton->setSelected(isHoveringButton(m_titleScreenButton, pos));
    }

    void update(float dt) override {
        checkButtons(getMousePos());

        if (!m_isPreview)
            PlatformToolbox::showCursor();
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        CCPoint pos = touch->getLocation();

        if (
            (m_isPreview && isHoveringButton(m_delegate->getBackButton(), pos))
            || (!m_isPreview && isHoveringButton(m_playLayer->m_uiLayer->m_pauseBtn, pos))
        ) {
            return false;
        }

        if (isHoveringButton(m_respawnButton, pos) && !m_respawnButton->isDisabled()) {
            Utils::playSoundManual(Anim::Minecraft, "minecraft-click.mp3", 1.f, FMODAudioEngine::get()->m_sfxVolume);
            resetLevel();

            return true;
        }

        if (isHoveringButton(m_titleScreenButton, pos) && !m_titleScreenButton->isDisabled()) {
            m_quitSound = Utils::playSoundManual(Anim::Minecraft, "minecraft-click.mp3", 1.f, FMODAudioEngine::get()->m_sfxVolume);

            SoundManager::retain(m_quitSound);

            if (m_isPreview)
                m_delegate->getBackButton()->activate();
            else
                m_playLayer->onQuit();

            return false;
        }

        #ifdef GEODE_IS_MOBILE

        m_isHolding = true;

        return true;

        #else

        return false;

        #endif
    }

    #ifdef GEODE_IS_MOBILE

    void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
        if (!m_isHolding) return;

        checkButtons(touch->getLocation());
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) override {
        m_isHolding = false;

        m_respawnButton->setSelected(false);
        m_titleScreenButton->setSelected(false);
    }

    #endif

    ~Minecraft() {
        if (m_quitSound)
            SoundManager::release(m_quitSound);
    }

    ANIMATION_CTOR_CREATE(Minecraft) {}
    
public:

    void start() override {
        Utils::setHighestZ(this);
        Utils::playSound(Anim::Minecraft, "minecraft-hurt.mp3", 1.f, 0, 0, 9999, 1.f, false, 90);

        CCNodeRGBA* player = m_isPreview ? m_delegate->getPlayer() : m_playLayer->m_player1;

        player->setVisible(false);

        m_playerSprite = Utils::renderPlayerSprite(player, false);
        m_playerSprite->setID("minecraft-death"_spr);
        m_playerSprite->setPosition(player->getPosition());
        m_playerSprite->setShaderProgram(Utils::createShader(m_shader, true));

        player->getParent()->addChild(m_playerSprite);

        m_playerSprite->runAction(CCSpawn::create(
            CCSequence::create(
                CCDelayTime::create(0.55f),
                CCHide::create(),
                CallFuncExt::create([this, player] {
                    MinecraftParticle* particle = MinecraftParticle::create();
                    particle->setPosition(player->getPosition());

                    player->getParent()->addChild(particle);

                    for (int i = 0; i < Utils::getRandomInt(6, 8); i++) {
                        MinecraftParticle* particle = MinecraftParticle::create();
                        particle->setPosition(
                            player->getPosition()
                            + ccp(Utils::getRandomInt(-30, 30), Utils::getRandomInt(-5, 25))
                        );

                        player->getParent()->addChild(particle);
                    }
                }),
                nullptr
            ),
            CCEaseExponentialOut::create(CCRotateBy::create(2.3f, 60)),
            nullptr 
        ));

        if (Utils::getSettingBool(Anim::Minecraft, "no-overlay"))
            return;

        disableRetryLayer();

        std::string username = GJAccountManager::get()->m_username;
        if (username.empty())
            username = "Player";

        int index = Utils::getRandomInt(0, static_cast<int>(m_deathMessages.size()) - 1);
        std::string deathMessage = fmt::format(fmt::runtime(m_deathMessages[index]), username);

        addChild(CCLayerColor::create({100, 0, 0, 81}));

        MinecraftLabel* youDied = MinecraftLabel::create("You Died!", 60);
        youDied->setScale(0.9f);
        youDied->setPosition({m_size.width / 2.f, m_size.height / 1.2121212121f});

        addChild(youDied);

        for (CCLabelBMFont* label : { youDied->getMainLabel(), youDied->getShadowLabel() }) {
            int count = 0;

            for (CCNodeRGBA* c : CCArrayExt<CCNodeRGBA*>(label->getChildren())) {
                if (++count >= 4) break;

                c->setPositionX(c->getPositionX() - 1.5f);
            }
        }

        MinecraftLabel* lbl = MinecraftLabel::create(deathMessage.c_str(), 60);
        lbl->setScale(0.448f);
        lbl->setPosition(youDied->getPosition() - ccp(0, 21));

        addChild(lbl);

        int score;

        if (!m_isPreview) {
            if (m_playLayer->m_levelSettings->m_platformerMode) {
                score = m_playLayer->m_gameState.m_points > 0
                    ? m_playLayer->m_gameState.m_points
                    : m_playLayer->m_gameState.m_levelTime;
            } else
                score = m_playLayer->getCurrentPercentInt();
        } else
            score = Utils::getRandomInt(0, 100);

        lbl = MinecraftLabel::create(fmt::format("Score: {}", score), 60);
        lbl->setScale(0.448f);
        lbl->setPosition(youDied->getPosition() - ccp(0, 34.5f));

        addChild(lbl);

        int count = 0;

        for (CCNodeRGBA* c : CCArrayExt<CCNodeRGBA*>(lbl->getMainLabel()->getChildren())) {
            count++;

            if (count >= 8)
                c->setColor({255, 246, 122});
        }
        
        m_respawnButton = MinecraftButton::create("Respawn");
        m_respawnButton->setPosition(m_size / 2.f + ccp(0, 7));

        addChild(m_respawnButton);

        m_titleScreenButton = MinecraftButton::create("Title Screen");
        m_titleScreenButton->setPosition(m_size / 2.f - ccp(0, 14));
    
        addChild(m_titleScreenButton);

        enableTouch();

        if (!Utils::getSettingBool(Anim::Minecraft, "no-cooldown")) {
            m_titleScreenButton->setDisabled(true);
            m_respawnButton->setDisabled(true);

            m_dontRestart = true;

            scheduleOnce(schedule_selector(Minecraft::enableButtons), 1.f);
        }

        #ifndef GEODE_IS_MOBILE

        schedule(schedule_selector(Minecraft::update));

        #endif
    }

    void end() override {
        if (m_playerSprite)
            m_playerSprite->removeFromParentAndCleanup(true);
        
        if (!m_isPreview)
            m_playLayer->m_player1->setVisible(true);

        #ifndef GEODE_IS_MOBILE

        if (!GameManager::get()->getGameVariable("0024") && !m_isPreview)
            PlatformToolbox::hideCursor();
        
        #endif

        BaseAnimation::end();
    }

};