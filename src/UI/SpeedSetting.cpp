#include "SpeedSetting.hpp"
#include "AnimationSettingsLayer.hpp"

#include "../Other/Utils.hpp"

void SpeedSetting::onSlider(CCObject* obj) {
    SliderThumb* slider = static_cast<SliderThumb*>(obj);
    
    Utils::saveSetting(m_animation.id, m_setting.id, slider->getValue());
    
    float value = Utils::getSpeedValue(slider->getValue());

    m_speedLabel->setString(fmt::format("{:.2f}x", value).c_str());

    CCLabelBMFont* durationLabel = m_parentLayer->getDurationLabel();
    CCSprite* durationSprite = m_parentLayer->getDurationSprite();

    durationLabel->setString(fmt::format("{:.2f}s", m_animation.duration / value).c_str());
    durationLabel->limitLabelWidth(25, durationLabel->getScale(), 0.0001f);
    durationSprite->setPositionX(
        durationLabel->getPositionX()
        - durationLabel->getContentWidth()
        * durationLabel->getScale()
        - durationSprite->getContentWidth()
        * durationSprite->getScale() / 2.f - 1.f
    );

    setInfoButtonPos(m_speedLabel);
}

void SpeedSetting::init(float y) {
    BaseSetting::init(y);
    
    m_slider = Slider::create(this, menu_selector(SpeedSetting::onSlider));
   	m_slider->setPosition({87, 10});
   	m_slider->setAnchorPoint({ 0.f, 0.f });
   	m_slider->setScale(0.37f);
   	m_slider->setValue(1.f);
   	
    addChild(m_slider);
    
    m_speedLabel = CCLabelBMFont::create("1.2x", "bigFont.fnt");
    m_speedLabel->setOpacity(190);
    m_speedLabel->setScale(0.25f);
    m_speedLabel->setAnchorPoint({0, 0.5f});
    m_speedLabel->setPosition({134, 10});

    addChild(m_speedLabel);
    
    m_slider->setValue(Utils::getSettingFloat(m_animation.id, m_setting.id));
    onSlider(m_slider->getThumb());
    
    setInfoButtonPos(m_speedLabel);
}