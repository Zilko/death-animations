#pragma once

#include "BaseSetting.hpp"

class SpeedSetting : public BaseSetting {

protected:

    CCLabelBMFont* m_speedLabel = nullptr;

    Slider* m_slider = nullptr;

    virtual void onSlider(CCObject*);

    virtual void init(float) override;

private:

    SETTING_CTOR_CREATE(SpeedSetting, BaseSetting)

};