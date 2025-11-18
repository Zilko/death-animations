#include "BaseSetting.hpp"

class CelesteColorSetting : public BaseSetting, public ColorPickPopupDelegate {

private:

    CCSprite* m_colorSprite = nullptr;

    CCMenuItemSpriteExtra* m_colorButton = nullptr;

    void onColorPicker(CCObject*);

    virtual void updateColor(const ccColor4B&) override;

    void init(float) override;

    SETTING_CTOR_CREATE(CelesteColorSetting, BaseSetting)

};
