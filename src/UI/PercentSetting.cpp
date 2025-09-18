#include "PercentSetting.hpp"

#include "../Other/Utils.hpp"

void PercentSetting::textChanged(CCTextInputNode* node) {
    int number = numFromString<int>(node->getString()).unwrapOr(0);
    
    number = std::clamp(number, 0, 100);
    node->setDelegate(nullptr);
    node->setString(std::to_string(number).c_str());
    node->setDelegate(this);
    
    Utils::saveSetting(m_animation.id, m_setting.id, number / 1.f);
}

void PercentSetting::init(float y) {
    BaseSetting::init(y);
    
    m_input = TextInput::create(70, "%");
   	m_input->setPosition({96, 10});
   	m_input->setScale(0.525f);
   	m_input->getInputNode()->setDelegate(this);
   	m_input->getInputNode()->setAllowedChars("0123456789");
   	m_input->setString(std::to_string(static_cast<int>(Utils::getSettingFloat(m_animation.id, m_setting.id))).c_str());
    
    addChild(m_input);
    
    m_percentLabel = CCLabelBMFont::create("%", "bigFont.fnt");
    m_percentLabel->setOpacity(220);
    m_percentLabel->setScale(0.325f);
    m_percentLabel->setPosition({125, 10});
    
    addChild(m_percentLabel);
    
    setInfoButtonPos(m_percentLabel);
}

void PercentSetting::setEnabled(bool enabled) {
    m_input->setEnabled(enabled);
    m_percentLabel->setOpacity(enabled ? 255 : 140);
    m_nameLabel->setOpacity(enabled ? 255 : 140);
}