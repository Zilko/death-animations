#pragma once

#include "BaseSetting.hpp"

class ToggleSetting : public BaseSetting {

protected:

    CCMenuItemToggler* m_toggle = nullptr;

    virtual void onToggle(CCObject*);

    void init(float) override;

    SETTING_CTOR_CREATE(ToggleSetting, BaseSetting)

};