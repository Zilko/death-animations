#include "ScaleSetting.hpp"

#include "../Other/Utils.hpp"

void ScaleSetting::onSlider(CCObject* obj) {
    SliderThumb* slider = static_cast<SliderThumb*>(obj);

    Utils::saveSetting(m_animation.id, m_setting.id, slider->getValue());

    m_speedLabel->setString(fmt::format("{:.2f}x", slider->getValue()).c_str());
    
    setInfoButtonPos(m_speedLabel);
}

void ScaleSetting::init(float y) {
    SpeedSetting::init(y);
    
    m_slider->setPositionX(103);
    m_speedLabel->setPositionX(m_speedLabel->getPositionX() + 16);

    setInfoButtonPos(m_speedLabel);
}