#include "SelectSetting.hpp"

class AmongUsSetting : public SelectSetting, public ColorPickPopupDelegate {

private:

    CCSprite* m_colorSprite1 = nullptr;
    CCSprite* m_colorSprite2 = nullptr;

    CCMenuItemSpriteExtra* m_colorButton1 = nullptr;
    CCMenuItemSpriteExtra* m_colorButton2 = nullptr;

    int m_currentColor;

    void onColorPicker(CCObject*);

    virtual void updateColor(const ccColor4B&) override;

    void onSelectorArrow(CCObject*) override;

    void init(float) override;

    SETTING_CTOR_CREATE(AmongUsSetting, SelectSetting)

};