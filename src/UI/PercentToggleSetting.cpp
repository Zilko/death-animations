#include "PercentToggleSetting.hpp"
#include "PercentSetting.hpp"

#include "../Other/Utils.hpp"

void PercentToggleSetting::onToggle(CCObject*) {
    ToggleSetting::onToggle(nullptr);

    m_percentSetting->setEnabled(!m_toggle->isToggled());
}

void PercentToggleSetting::init(float y) {
    ToggleSetting::init(y);

    AnimationSetting percentSetting = {
        .id = "custom-percent",
        .name = "Percentage",
        .description = "",
        .type = SettingType::Percent 
    };

    m_percentSetting = PercentSetting::create(m_parentLayer, percentSetting, m_animation, -12.5f);
    m_percentSetting->setEnabled(m_toggle->isToggled());

    addChild(m_percentSetting);
}