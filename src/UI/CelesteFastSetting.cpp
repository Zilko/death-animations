#include "CelesteFastSetting.hpp"
#include "AnimationSettingsLayer.hpp"

#include "../Other/Utils.hpp"

void CelesteFastSetting::onToggle(CCObject* obj) {
    if (obj)
        ToggleSetting::onToggle(obj);

    CCLabelBMFont* durationLabel = m_parentLayer->getDurationLabel();
    CCSprite* durationSprite = m_parentLayer->getDurationSprite();

    float duration = m_animation.duration;
    if (Utils::getSettingBool(Anim::Celeste, "fast"))
        duration -= 25.f / 60.f;

    durationLabel->setString(fmt::format("{:.2f}s", duration).c_str());

    durationLabel->limitLabelWidth(45, durationLabel->getScale(), 0.0001f);
    durationSprite->setPositionX(
        durationLabel->getPositionX()
        - durationLabel->getContentWidth()
        * durationLabel->getScale()
        - durationSprite->getContentWidth()
        * durationSprite->getScale() / 2.f - 1.9f
    );
}

void CelesteFastSetting::init(float y) {
    ToggleSetting::init(y);

    onToggle(nullptr);
}
