#pragma once

#include "BaseSetting.hpp"

class SelectSetting : public BaseSetting {

protected:

    CCMenuItemSpriteExtra* m_rightArrow = nullptr;

    virtual void onSelectorArrow(CCObject*);

    void init(float) override;

private:

    CCLabelBMFont* m_label = nullptr;

    SETTING_CTOR_CREATE(SelectSetting, BaseSetting)

};