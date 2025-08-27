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
    
    TextInput* input = TextInput::create(70, "%");
   	input->setPosition({96, 10});
   	input->setScale(0.525f);
   	input->getInputNode()->setDelegate(this);
   	input->getInputNode()->setAllowedChars("0123456789");
   	input->setString(std::to_string(static_cast<int>(Utils::getSettingFloat(m_animation.id, m_setting.id))).c_str());
    
    addChild(input);
    
    CCLabelBMFont* lbl = CCLabelBMFont::create("%", "bigFont.fnt");
    lbl->setOpacity(220);
    lbl->setScale(0.325f);
    lbl->setPosition({125, 10});
    
    addChild(lbl);
    
    setInfoButtonPos(lbl);
}