#include "BaseSetting.hpp"

class CelesteColorSetting : public BaseSetting {

private:

    CCSprite* m_colorSprite = nullptr;

    CCMenuItemSpriteExtra* m_colorButton = nullptr;

    void onColorPicker(CCObject*);

    void init(float) override;

    SETTING_CTOR_CREATE(CelesteColorSetting, BaseSetting)

};
