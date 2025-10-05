#include "BaseAnimation.hpp"

class NewBest : public BaseAnimation {

private:

    bool m_didRevive = false;

    void update(float) override {
        if (m_didRevive && m_playLayer->m_player1->m_isDead) {
            stopAllActions();
            unscheduleAllSelectors();
            setVisible(false);
            
            Loader::get()->queueInMainThread([self = Ref(this)] {
                self->BaseAnimation::end();
            });
        } else if (!m_didRevive && !m_playLayer->m_player1->m_isDead)
            m_didRevive = true;
    }

    ANIMATION_CTOR_CREATE(NewBest) {}

public:

    void start() override {
        if (!m_isPreview)
            schedule(schedule_selector(NewBest::update));
        
        bool platformer = false;
        
        if (!m_isPreview && m_playLayer->m_levelSettings->m_platformerMode)
            platformer = true;
        
        bool demonKey = false;
        int orbs = 0;
        int diamonds = 0;
        int percent = Utils::getSettingBool(Anim::NewBest, "use-custom-percent")
            ? static_cast<int>(Utils::getSettingFloat(Anim::NewBest, "custom-percent"))
            : (m_isPreview ? 99 : m_playLayer->getCurrentPercentInt());
            
        if (platformer)
            percent = static_cast<int>(m_playLayer->m_timePlayed);
        
        float fadeTime = orbs > 0 || diamonds > 0 ? 1.3f : 0.7f;
        
        runAction(CCSequence::create(
            CCDelayTime::create(fadeTime + 6.f),
            CallFuncExt::create([this] {
                removeFromParentAndCleanup(true);
            }),
            nullptr
        ));

        if (!m_isPreview && percent > 0) {
            if (m_playLayer->m_level->m_stars.value() > 0) {
                orbs = abs(percent - m_playLayer->m_level->m_normalPercent.value()) * (m_playLayer->m_level->m_stars.value() / 2.f) + Utils::getRandomInt(0, 1);
                orbs = std::min(orbs, 375);

                if (375 - orbs < 50) orbs /= 2.f;

                if (m_playLayer->m_level->m_dailyID.value() != 0)
                    diamonds = percent / 100.f * Utils::getRandomInt(5, 14);

                demonKey = Utils::getRandomInt(1, 100) < 9;
            }
        } else {
            orbs = Utils::getRandomInt(1, 100) > 10 ? Utils::getRandomInt(5, 15) : 0;
            diamonds = Utils::getRandomInt(1, 100) > 35 && orbs > 0 ? Utils::getRandomInt(1, 5) : 0;
            demonKey = orbs > 0 ? (Utils::getRandomInt(1, 100) < 20) : false;
        }

        CCNode* scaleContainer = CCNode::create();
        scaleContainer->setID("container");
        scaleContainer->setScale(0.01f);
        scaleContainer->setPosition({m_size.width * 0.5f, (orbs > 0 || diamonds ? 10 : 0) + m_size.height * 0.5f + 20.f});
        
        addChild(scaleContainer);

        CCSprite* spr = CCSprite::createWithSpriteFrameName("GJ_newBest_001.png");
        spr->setID("new-best-sprite");
        spr->setAnchorPoint({0.5f, 0.f});

        scaleContainer->addChild(spr);

        CCLabelBMFont* lbl = CCLabelBMFont::create(fmt::format("{}{}", percent, platformer ? "s" : "%").c_str(), "bigFont.fnt");
        lbl->setID("percent-label");
        lbl->setAnchorPoint({0.5f, 1.f});
        lbl->setPosition({0.f, -4.f});
        lbl->setScale(1.1f);

        scaleContainer->addChild(lbl);

        if (diamonds > 0 && orbs > 0) {
            lbl = CCLabelBMFont::create(fmt::format("+{}", diamonds).c_str(), "bigFont.fnt");
            lbl->setID("diamonds-label");
            lbl->setScale(0.6f);

            scaleContainer->addChild(lbl);

            spr = CCSprite::createWithSpriteFrameName("GJ_bigDiamond_001.png");
            spr->setID("diamonds-sprite");
            spr->setAnchorPoint({0.f, 0.5f});
            spr->setScale(0.55f);

            scaleContainer->addChild(spr);

            if (!demonKey)
                FMODAudioEngine::get()->playEffect("gold02.ogg");

            lbl->setPosition({20.f, -62.f});
            spr->setPosition(lbl->getPosition() + ccp(lbl->getScaledContentSize().width * 0.5f + 1.f, -1.f));
        }

        if (orbs > 0) {
            lbl = CCLabelBMFont::create(fmt::format("+{}", orbs).c_str(), "bigFont.fnt");
            lbl->setID("orbs-label");
            lbl->setScale(0.6f);
            scaleContainer->addChild(lbl);

            spr = CCSprite::createWithSpriteFrameName("currencyOrbIcon_001.png");
            spr->setID("orbs-sprite");
            spr->setAnchorPoint(ccp(0.f, 0.5f));
            scaleContainer->addChild(spr);

            FMODAudioEngine::get()->playEffect("magicExplosion.ogg");

            float offset = diamonds > 0
                ? -40.f
                : -(lbl->getContentWidth() * lbl->getScale() + 5.f + spr->getContentWidth() * spr->getScale()) / 2.f + lbl->getContentWidth() * lbl->getScale() / 2.f;

            lbl->setPosition({offset - (diamonds <= 0 ? 5.f : 20.f), -62.f});
            spr->setPosition(lbl->getPosition() + ccp(lbl->getScaledContentSize().width * 0.5f + 5.f, -1.f));
        }

        scaleContainer->runAction(CCSequence::create(
            CCEaseElasticOut::create(CCScaleTo::create(0.4f, 1.f), 0.6f),
            CCDelayTime::create(fadeTime),
            CCEaseIn::create(CCScaleTo::create(0.2f, 0.01f, 0.01f), 2.f),
            CCHide::create(),
            nullptr
        ));

        if (demonKey)
            FMODAudioEngine::get()->playEffect("secretKey.ogg");

        if (orbs <= 0 && diamonds <= 0) return;

        CurrencyRewardLayer* rewardLayer = CurrencyRewardLayer::create(
            orbs,
            0,
            0,
            diamonds,
            demonKey ? CurrencySpriteType::DemonKey : static_cast<CurrencySpriteType>(0),
            static_cast<int>(demonKey),
            CurrencySpriteType::Icon,
            0,
            getNodeScreenPos(m_isPreview ? m_delegate->getPlayer() : m_playLayer->m_player1),
            static_cast<CurrencyRewardType>(0),
            0.f,
            0.9f
        );
        rewardLayer->setID("reward-layer");
        
        addChild(rewardLayer);

        CCLayerColor* overlay = CCLayerColor::create({0, 0, 0, 0});
        overlay->setID("overlay-layer");

        overlay->runAction(CCSequence::create(
            CCFadeTo::create(0.3f, 100),
            CCDelayTime::create(fadeTime + 0.3f),
            CCFadeTo::create(0.4f, 0),
            nullptr
        ));

        addChild(overlay, -1);
    }

    void end() override {}

};