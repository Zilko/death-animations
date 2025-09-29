#pragma once

#include "BaseSetting.hpp"

class SelectSetting : public BaseSetting {

protected:

    CCScale9Sprite* m_bg = nullptr;

    CCMenuItemSpriteExtra* m_rightArrow = nullptr;

    CCLabelBMFont* m_label = nullptr;

    virtual void onSelectorArrow(CCObject*);

    virtual void init(float) override;

private:

    SETTING_CTOR_CREATE(SelectSetting, BaseSetting)

};