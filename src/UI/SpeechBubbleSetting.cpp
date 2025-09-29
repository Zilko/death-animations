#include "SpeechBubbleSetting.hpp"
#include "AnimationSettingsLayer.hpp"

#include "../Other/Utils.hpp"

void SpeechBubbleSetting::onSelectorArrow(CCObject* obj) {
    SelectSetting::onSelectorArrow(obj);

    m_label->limitLabelWidth(116.f, 0.35f, 0.0001f);
}

void SpeechBubbleSetting::init(float y) {
    for (const std::filesystem::path& path : Utils::getAllFilesFromFolder(Mod::get()->getSaveDir() / "speech-bubbles", { ".png", ".jpg" }))
        m_setting.elements.push_back(utils::string::pathToString(path.stem()));

    SelectSetting::init(y);

    m_rightArrow->setPositionX(m_rightArrow->getPositionX() + 39.325f);
    m_bg->setContentWidth(380);
    m_bg->setPositionX(m_bg->getPositionX() + 19.6625f);
    m_label->setPositionX(m_bg->getPositionX());
    m_label->limitLabelWidth(97.325f, 0.35f, 0.0001f);

    setInfoButtonPos(m_rightArrow);
}