#include "SpeedSetting.hpp"

class ScaleSetting : public SpeedSetting {

private:

    void onSlider(CCObject*) override;

    void init(float) override;

    SETTING_CTOR_CREATE(ScaleSetting, SpeedSetting)

};