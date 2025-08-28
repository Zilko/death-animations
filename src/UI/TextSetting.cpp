#include "TextSetting.hpp"

#include "../Other/Utils.hpp"

void TextSetting::textChanged(CCTextInputNode* node) {
    std::string str = node->getString();
    Utils::saveSetting(m_animation.id, m_setting.id, str);
}

void TextSetting::init(float y) {
    BaseSetting::init(y);

    TextInput* input = TextInput::create(210, "Text");
   	input->setPosition({m_nameLabel->getPositionX() + m_nameLabel->getContentWidth() * m_nameLabel->getScale() + 5, 10});
   	input->setAnchorPoint({0, 0.4f});
   	input->setScale(0.525f);
   	input->getInputNode()->setDelegate(this);
   	input->setString(Utils::getSettingString(m_animation.id, m_setting.id).c_str());
    
    addChild(input);
    
    setInfoButtonPos(input);
}