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
        m_sprite->setScale(Utils::getRandomInt(0, 1) ? 1.2f : 2.1f);
        m_sprite->runAction(CCSpawn::create(
            CCMoveBy::create(10.f, {0, 300.f}),
            CCEaseOut::create(CCMoveBy::create(1.2f, {0, shouldFall ? -15.f : 0.f}), 2.f),
            CCEaseOut::create(CCMoveBy::create(2.f, {shouldSlide ? (5.f * (Utils::getRandomInt(0, 1) ? 1 : -1)) : 0.f, 0}), 2.f),
            nullptr
        ));

        if (Utils::getRandomInt(1, 10) <= 2) {
            m_sprite->setColor({150, 150, 150});
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
        if (++m_currentStep > 9) log::debug("ded");
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

    inline static const std::unordered_map<int, std::string> m_solidObjectNames = {{1,"Black Gradient Square"},{2,"Grid Patterned Top Square"},{3,"Grid Patterned Outer Corner Square"},{4,"Grid Patterned Inner Corner Square"},{6,"Grid Patterned Top Pillar Square"},{7,"Grid Patterned Pillar Square"},{8,"Black Gradient Spike"},{9,"Non-Colorable Spike Black Pit Hazard"},{34,"1.0 Level End Trigger"},{39,"Half Black Gradient Spike"},{40,"Black Gradient Single Slab"},{61,"Non-Colorable Wavy Black Pit Hazard"},{62,"Wavy Black Slab"},{63,"Wavy Black Slab Outer Corner"},{64,"Wavy Black Slab Inner Corner"},{65,"Wavy Black Slab Right Edge"},{66,"Wavy Black Slab Left Edge"},{68,"Wavy Black Single Slab"},{69,"Cross Black Square"},{70,"Beveled Top Square"},{71,"Beveled Outer Corner Square"},{72,"Beveled Inner Corner Square"},{74,"Beveled Top Pillar Square"},{75,"Beveled Pillar Square"},{76,"Mechanical Square"},{77,"Mechanical Top Square"},{78,"Mechanical Outer Corner Square"},{81,"Mechanical Top Pillar Square"},{82,"Mechanical Pillar Square"},{83,"Grid Patterned Square"},{88,"Large Black Sawblade"},{89,"Medium Black Sawblade"},{90,"Black Square"},{91,"Black Top Square"},{92,"Black Outer Corner Square"},{93,"Black Inner Corner Square"},{94,"Black Inner Square"},{95,"Black Top Pillar Square"},{96,"Black Pillar Square"},{98,"Small Black Sawblade"},{103,"Small Black Gradient Spike"},{116,"Brick Square"},{117,"Brick Top Square"},{118,"Brick Outer Corner Square"},{119,"Brick Inner Corner Square"},{121,"Brick Top Pillar Square"},{122,"Brick Pillar Square"},{135,"Thorn Black Pit Hazard"},{144,"Invisible Spike"},{145,"Small Invisible Spike"},{146,"Invisible Square"},{147,"Invisible Slab"},{160,"Solid Stripe Top Square"},{161,"Solid Stripe Top Square"},{162,"Solid Stripe Outer Corner Square"},{163,"Solid Inner Corner Square"},{165,"Solid Stripe Top Pillar Square"},{166,"Solid Stripe Pillar Square"},{167,"Solid Stripe Pillar/Top Square"},{168,"Solid Stripe Single Square"},{169,"Solid Stripe Checkered 1 Square"},{170,"Metal Slab Outer Corner"},{171,"Metal Slab End Short Connection"},{172,"Metal Slab End Full Connection"},{173,"Metal Slab Outer Corner"},{174,"Three-Quarters Width Metal Slab Inner Corner"},{175,"Metal Block"},{176,"Small Metal Block"},{177,"Ice Spike"},{178,"Half Ice Spike"},{179,"Small Ice Spike"},{183,"Large Blade"},{184,"Medium Blade"},{185,"Small Blade"},{186,"Large Outline Blade"},{187,"Medium Outline Blade"},{188,"Small Outline Blade"},{192,"Single Metal Slab"},{194,"Metal Slab Inner Corner"},{195,"Black Gradient Small Square"},{196,"Black Gradient Small Slab"},{197,"Three-Quarters Width Metal Slab"},{204,"Invisible Small Slab"},{205,"Invisible Half Spike"},{206,"Invisible Small Square"},{207,"Colored Square"},{208,"Colored Top Square"},{209,"Colored Outer Corner Square"},{210,"Colored Inner Corner Square"},{212,"Colored Top Pillar Square"},{213,"Colored Pillar Square"},{215,"Colored Slab"},{216,"Colored Spike"},{217,"Colored Half Spike"},{218,"Colored Small Spike"},{219,"Colored Small Slab"},{220,"Colored Small Square"},{243,"Non-Colorable Wavy Black Pit Right Edge Hazard"},{244,"Non-Colorable Wavy Black Pit Left Edge Hazard"},{247,"Colored Grid Patterned Square"},{248,"Colored Grid Patterned Top Square"},{249,"Colored Grid Patterned Outer Corner Square"},{250,"Colored Grid Patterned Inner Corner Square"},{252,"Colored Grid Patterned Top Pillar Square"},{253,"Colored Grid Patterned Pillar Square"},{254,"Colored Grid Patterned Two Adjacent Corners Square"},{255,"Colored Beveled Square"},{256,"Colored Beveled Top Square"},{257,"Colored Beveled Outer Corner Square"},{258,"Colored Beveled Inner Corner Square"},{260,"Colored Beveled Top Pillar Square"},{261,"Colored Beveled Pillar Square"},{263,"Mechanical Colored Square"},{264,"Mechanical Colored Top Square"},{265,"Mechanical Colored Outer Corner Square"},{267,"Mechanical Colored Top Pillar Square"},{268,"Mechanical Colored Pillar Square"},{269,"Colored Bricks Square"},{270,"Colored Bricks Top"},{271,"Colored Bricks Outer Corner"},{272,"Colored Bricks Inner Corner"},{274,"Colored Bricks Pillar Top"},{275,"Colored Bricks Pillar"},{289,"Cornerless Grid Patterned Slope"},{291,"Cornerless Grid Patterned Wide Slope"},{294,"Wavy Black Slope"},{295,"Wavy Black Wide Slope"},{305,"Mechanical Slope"},{307,"Mechanical Wide Slope"},{349,"Mechanical Colored Slope"},{351,"Mechanical Colored Wide Slope"},{326,"Metal Slab Slope"},{327,"Metal Slab Slope"},{328,"Metal Slab Slope Connector"},{329,"Metal Slab Wide Slope Connector"},{363,"Non-Colorable Spike Black Slope Hazard"},{364,"Non-Colorable Spike Black Wide Slope Hazard"},{365,"Non-Colorable Spike Black Hazard"},{366,"Non-Colorable Wavy Black Slope Hazard"},{367,"Non-Colorable Wavy Black Wide Slope Hazard"},{368,"Non-Colorable Wavy Black Hazard"},{369,"Black Gradient Slab Middle"},{370,"Black Gradient Slab Side"},{371,"Black Gradient Slope"},{372,"Black Gradient Wide Slope"},{392,"Black Gradient Tiny Spike"},{397,"Large Black Spike Blade"},{398,"Medium Black Spike Blade"},{399,"Small Black Spike Blade"},{458,"Colored Tiny Spike"},{459,"Invisible Tiny Spike"},{421,"Non-Colorable Wide Spike Black Hazard"},{422,"Non-Colorable Wide Spike Black Edge Hazard"},{446,"Non-Colorable Round Black Hazard"},{447,"Non-Colorable Round Black Edge Hazard"},{467,"Outline Square"},{468,"Outline Top"},{469,"Outline Outer Corner"},{470,"Outline Top Pillar"},{471,"Outline Pillar"},{475,"Outline Slope Side"},{661,"Outline Small Square"},{662,"Outline Slab"},{663,"Outline Slab Middle"},{664,"Outline Slab Side"},{665,"Non-Colorable Outline Slope"},{666,"Non-Colorable Outline Wide Slope"},{667,"Non-Colorable Square Black Hazard"},{673,"Invisible Slope"},{674,"Invisible Wide Slope"},{678,"Large Colored Gear"},{679,"Medium Colored Gear"},{680,"Small Colored Gear"},{720,"Non-Colorable Square Black Edge Hazard"},{740,"Large Invisible Blade"},{741,"Medium Invisible Blade"},{742,"Small Invisible Blade"},{768,"Non-Colorable Wide Spike Black Half Hazard"},{918,"Large Beast Hazard"},{919,"Animated Black Pit Hazard"},{991,"Non-Colorable Square Black Slope Corner Hazard"},{1154,"Small Outline Top"},{1155,"Small Outline Outer Corner"},{1156,"Small Outline Top Pillar"},{1157,"Small Outline Pillar"},{1202,"Thick Outline Top"},{1203,"Thick Outline Outer Corner"},{1204,"Thick Outline Top Pillar"},{1208,"Thick Outline Small Square"},{1209,"Thick Outline Pillar"},{1210,"Thick Outline Square"},{1220,"Thicker Outline Top"},{1221,"Thicker Outline Outer Corner"},{1222,"Thicker Outline Top Pillar"},{1226,"Thicker Outline Pillar"},{1227,"Thicker Outline Square"},{1260,"Outer Outline Side"},{1262,"Thick Outer Outline Side"},{1264,"Thicker Outer Outline Side"},{1275,"Key Collectable"},{1327,"Small Monster Hazard"},{1328,"Large Monster Hazard"},{1329,"User Coin"},{1338,"Outline Slope"},{1339,"Outline Wide Slope"},{1340,"Thin Invisible Outline"},{1341,"Thin Invisible Outline Slope"},{1342,"Thin Invisible Outline Wide Slope"},{1343,"Thick Invisible Outline"},{1344,"Thick Invisible Outline Slope"},{1345,"Thick Invisible Outline Wide Slope"},{1582,"Rotating Fireball"},{1583,"Moving Fireball"},{1584,"Bat Hazard"},{1587,"Heart Collectable"},{1589,"Potion Collectable"},{1598,"Skull Collectable"},{1614,"Small Coin Collectable"},{1619,"Large Scythe Blade"},{1620,"Small Scythe Blade"},{1701,"Spiked Square Hazard"},{1702,"Spiked Circle Hazard"},{1703,"Triangle Hazard"},{1705,"Large Saw Blade"},{1706,"Medium Saw Blade"},{1707,"Small Saw Blade"},{1708,"Large Spike Blade"},{1709,"Medium Spike Blade"},{1710,"Small Spike Blade"},{1711,"Thorn Colored Pit Hazard 1"},{1712,"Thorn Colored Pit Hazard 2"},{1713,"Thorn Colored Pit Hazard 3"},{1714,"Thorn Colored Pit Hazard 4"},{1715,"Spike Black Pit Hazard"},{1716,"Spike Black Hazard"},{1717,"Spike Black Slope Hazard"},{1718,"Spike Black Wide Slope Hazard"},{1719,"Wavy Black Pit Hazard"},{1720,"Wavy Black Pit Right Edge Hazard"},{1721,"Wavy Black Pit Left Edge Hazard"},{1722,"Wavy Black Hazard"},{1723,"Wavy Black Slope Hazard"},{1724,"Wavy Black Wide Slope Hazard"},{1725,"Wide Spike Black Hazard"},{1726,"Wide Spike Black Edge Hazard"},{1727,"Wide Spike Black Half Hazard"},{1728,"Round Black Hazard"},{1729,"Round Black Edge Hazard"},{1730,"Square Black Hazard"},{1731,"Square Black Edge Hazard"},{1732,"Square Black Slope Hazard"},{1733,"Square Black Slope Corner Hazard"},{1734,"Large Gear Blade"},{1735,"Medium Gear Blade"},{1736,"Small Gear Blade"},{1743,"Grid Patterned Slope"},{1744,"Grid Patterned Wide Slope"},{1745,"Beveled Black Slope"},{1746,"Beveled Black Wide Slope"},{1747,"Black Slope"},{1748,"Black Wide Slope"},{1749,"Brick Slope"},{1750,"Brick Wide Slope"},{1903,"Colored Gradient Single Slab"},{1904,"Colored Gradient Slab Middle"},{1905,"Colored Gradient Slab Side"},{1906,"Colored Gradient Slope"},{1907,"Colored Gradient Wide Slope"},{1910,"Colored Gradient Small Block"},{1911,"Colored Gradient Small Slab"},{2012,"Spiked Round Monster Hazard"},{2064,"Unlinked Orange Teleport Portal"},{3034,"Full Spike Base"},{3035,"Four-Ninths Spike Base"},{3036,"One-Quarter Spike Base"},{3037,"One-Ninth Spike Base"},{3601,"Clock Collectable"},{3610,"Damage Square"},{3611,"Damage Circle"},{4401,"Small Potion Pixel Collectable"},{4402,"Medium Potion Pixel Collectable"},{4403,"Large Potion Pixel Collectable"},{4404,"Diagonal Key Pixel Collectable"},{4405,"Round Key Pixel Collectable"},{4406,"Flat Key Pixel Collectable"},{4407,"Coin Pixel Collectable"},{4408,"Small Coin Stack Pixel Collectable"},{4409,"Large Coin Stack Pixel Collectable"},{4410,"Candy Pixel Collectable"},{4411,"Mushroom Pixel Collectable"},{4412,"Bone Pixel Collectable"},{4413,"Sphere Pixel Collectable"},{4414,"Ingot Pixel Collectable"},{4415,"Square Gem Pixel Collectable"},{4416,"Hexagon Gem Pixel Collectable"},{4417,"Octagon Gem Pixel Collectable"},{4418,"Rectangle Gem Pixel Collectable"},{4419,"Pointy Gem Pixel Collectable"},{4420,"Diamond Pixel Collectable"},{4421,"Fish Pixel Collectable"},{4422,"Rocks Pixel Collectable"},{4423,"Wood Pixel Collectable"},{4424,"Egg Pixel Collectable"},{4425,"Large Heart Pixel Collectable"},{4426,"Small Heart Pixel Collectable"},{4427,"Map Pixel Collectable"},{4428,"Book Pixel Collectable"},{4429,"Device Pixel Collectable"},{4430,"Computer Pixel Collectable"},{4431,"Large Skull Pixel Collectable"},{4432,"Small Skull Pixel Collectable"},{4433,"Speech Bubble Pixel Collectable"},{4434,"Branch Pixel Collectable"},{4435,"Shard Gem Pixel Collectable"},{4436,"Eye Pixel Collectable"},{4437,"Eyeball Pixel Collectable"},{4438,"Up Arrow Pixel Collectable"},{4439,"Down Arrow Pixel Collectable"},{4440,"Lightning Pixel Collectable"},{4441,"No Symbol Pixel Collectable"},{4442,"Gear Pixel Collectable"},{4443,"Plus Pixel Collectable"},{4444,"Plus Symbols Pixel Collectable"},{4445,"Pebbles Pixel Collectable"},{4446,"Present Pixel Collectable"},{4447,"Chest Pixel Collectable"},{4448,"Bag Pixel Collectable"},{4449,"Backpack Pixel Collectable"},{4450,"Bundle Pixel Collectable"},{4451,"Ring Pixel Collectable"},{4452,"Patterned Ring Pixel Collectable"},{4453,"Gem Ring Pixel Collectable"},{4454,"Large Necklace Pixel Collectable"},{4455,"Small Necklace Pixel Collectable"},{4456,"Shoe Pixel Collectable"},{4457,"Boot Pixel Collectable"},{4458,"Pointy Boot Pixel Collectable"},{4459,"Hat Pixel Collectable"},{4460,"Pointy Hat Pixel Collectable"},{4461,"Curly Hat Pixel Collectable"},{4462,"Helmet 1 Pixel Collectable"},{4463,"Helmet 2 Pixel Collectable"},{4464,"Helmet 3 Pixel Collectable"},{4465,"Mask 1 Pixel Collectable"},{4466,"Mask 2 Pixel Collectable"},{4467,"Mask 3 Pixel Collectable"},{4468,"Round Mask Pixel Collectable"},{4469,"Horned Round Mask Pixel Collectable"},{4470,"Small Lab Coat Pixel Collectable"},{4471,"Large Lab Coat Pixel Collectable"},{4472,"Armor 1 Pixel Collectable"},{4473,"Armor 2 Pixel Collectable"},{4474,"Armor 3 Pixel Collectable"},{4475,"Armor 4 Pixel Collectable"},{4476,"Armor 5 Pixel Collectable"},{4477,"Armor 6 Pixel Collectable"},{4478,"Shield 1 Pixel Collectable"},{4479,"Shield 2 Pixel Collectable"},{4480,"Shield 3 Pixel Collectable"},{4481,"Shield 4 Pixel Collectable"},{4482,"Shield 5 Pixel Collectable"},{4483,"Shield 6 Pixel Collectable"},{4484,"Shield 7 Pixel Collectable"},{4485,"Shield 8 Pixel Collectable"},{4486,"Shield 9 Pixel Collectable"},{4487,"Sword Pixel Collectable"},{4488,"Bow Pixel Collectable"},{4489,"Axe Pixel Collectable"},{4490,"Spear Pixel Collectable"},{4491,"Large Staff Pixel Collectable"},{4492,"Shovel Pixel Collectable"},{4493,"Pickaxe Pixel Collectable"},{4494,"Hammer Pixel Collectable"},{4495,"Hoe Pixel Collectable"},{4496,"Small Staff Pixel Collectable"},{4497,"Hook Pixel Collectable"},{4498,"Exclamation Point Pixel Collectable"},{4499,"Question Mark Pixel Collectable"},{4500,"Plus Sign Pixel Collectable"},{4501,"Minus Sign Pixel Collectable"},{4502,"Equals Sign Pixel Collectable"},{4503,"Times Sign Pixel Collectable"},{4504,"Division Sign Pixel Collectable"},{4505,"Numeral 0 Pixel Collectable"},{4506,"Numeral 1 Pixel Collectable"},{4507,"Numeral 2 Pixel Collectable"},{4508,"Numeral 3 Pixel Collectable"},{4509,"Numeral 4 Pixel Collectable"},{4510,"Numeral 5 Pixel Collectable"},{4511,"Numeral 6 Pixel Collectable"},{4512,"Numeral 7 Pixel Collectable"},{4513,"Numeral 8 Pixel Collectable"},{4514,"Numeral 9 Pixel Collectable"},{4515,"Animal Skull Pixel Collectable"},{4516,"Cracked Skull Pixel Collectable"},{4517,"Small Bone Pixel Collectable"},{4518,"Large Key Pixel Collectable"},{4519,"Keyhole Chest Pixel Collectable"},{4520,"Bread Pixel Collectable"},{4521,"Small Diamond Pixel Collectable"},{4522,"Cloak Pixel Collectable"},{4523,"Scroll Pixel Collectable"},{4524,"Banner Pixel Collectable"},{4525,"Bomb Pixel Collectable"},{4526,"Metal Nugget Pixel Collectable"},{4527,"Small Arrow Pixel Collectable"},{4528,"Large Arrow Pixel Collectable"},{4529,"Cheese Pixel Collectable"},{4530,"Apple Pixel Collectable"},{4531,"Carrot Pixel Collectable"},{4532,"Steak Pixel Collectable"},{4533,"Fire Pixel Collectable"},{4534,"Wave Pixel Collectable"},{4535,"Spike Pixel Collectable"},{4536,"Cauldron Pixel Collectable"},{4537,"Fishing Rod Pixel Collectable"},{4538,"Pointy Bow Pixel Collectable"},{4539,"Floppy Disk Pixel Collectable"}};

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
    }

    void checkButtons(const CCPoint& pos) {
        m_respawnButton->setSelected(isHoveringButton(m_respawnButton, pos));
        m_titleScreenButton->setSelected(isHoveringButton(m_titleScreenButton, pos));
    }

    void update(float dt) {
        checkButtons(getMousePos());

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

        m_playLayer->m_player1->setVisible(false);

        m_playerSprite = Utils::renderPlayerSprite(m_playLayer->m_player1, false);
        m_playerSprite->setPosition(m_playLayer->m_player1->getPosition());
        m_playerSprite->setShaderProgram(Utils::createShader(m_shader, true));

        m_playLayer->m_player1->getParent()->addChild(m_playerSprite);


        m_playerSprite->runAction(CCSpawn::create(
            CCSequence::create(
                CCDelayTime::create(0.55f),
                CCHide::create(),
                CallFuncExt::create([this] {
                    MinecraftParticle* particle = MinecraftParticle::create();
                    particle->setPosition(m_playLayer->m_player1->getPosition());

                    m_playLayer->m_player1->getParent()->addChild(particle);

                    for (int i = 0; i < Utils::getRandomInt(6, 8); i++) {
                        MinecraftParticle* particle = MinecraftParticle::create();
                        particle->setPosition(
                            m_playLayer->m_player1->getPosition()
                            + ccp(Utils::getRandomInt(-30, 30), Utils::getRandomInt(-5, 25))
                        );

                        m_playLayer->m_player1->getParent()->addChild(particle);
                    }
                }),
                nullptr
            ),
            CCEaseExponentialOut::create(CCRotateBy::create(2.3f, 60)),
            nullptr 
        ));

        if (Utils::getSettingBool(Anim::Minecraft, "no-overlay"))
            return;

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
        m_respawnButton->setDisabled(true);

        addChild(m_respawnButton);

        m_titleScreenButton = MinecraftButton::create("Title Screen");
        m_titleScreenButton->setPosition(m_size / 2.f - ccp(0, 14));
        m_titleScreenButton->setDisabled(true);
    
        addChild(m_titleScreenButton);

        enableTouch();
        scheduleOnce(schedule_selector(Minecraft::enableButtons), 1.f);

        #ifndef GEODE_IS_MOBILE

        schedule(schedule_selector(Minecraft::update));

        #endif
    }

    void end() override {
        if (m_playerSprite)
            m_playerSprite->removeFromParentAndCleanup(true);

        #ifndef GEODE_IS_MOBILE

        if (!GameManager::get()->getGameVariable("0024"))
            PlatformToolbox::hideCursor();
        
        #endif

        BaseAnimation::end();
    }

};