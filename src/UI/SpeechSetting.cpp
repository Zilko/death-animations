#include "SpeechSetting.hpp"
#include "AnimationSettingsLayer.hpp"

#include "../Other/Utils.hpp"

void SpeechSetting::onSelectorArrow(CCObject* obj) {
    if (obj)
        SelectSetting::onSelectorArrow(obj);

    CCLabelBMFont* durationLabel = m_parentLayer->getDurationLabel();
    CCSprite* durationSprite = m_parentLayer->getDurationSprite();
    int speech = Utils::getSettingFloat(Anim::MrHippo, "monologue");

    if (speech == 0) {
        durationLabel->setString("Random");
    } else {
        float duration = 0.f;

        switch (speech) {
            case 1: duration = 215.f; break;
            case 2: duration = 180.f; break;
            case 3: duration = 147.f; break;
            case 4: duration = 148.f; break;
        }

        durationLabel->setString(fmt::format("{:.2f}s", duration).c_str());
    }

    durationLabel->limitLabelWidth(45, durationLabel->getScale(), 0.0001f);
    durationSprite->setPositionX(
        durationLabel->getPositionX()
        - durationLabel->getContentWidth()
        * durationLabel->getScale()
        - durationSprite->getContentWidth()
        * durationSprite->getScale() / 2.f - 1.9f
    );
}

void SpeechSetting::init(float y) {
    SelectSetting::init(y);

    onSelectorArrow(nullptr);
}