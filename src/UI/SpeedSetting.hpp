#include "BaseSetting.hpp"

class SpeedSetting : public BaseSetting {

private:

    CCLabelBMFont* m_speedLabel = nullptr;

    void onSlider(CCObject*);

    void init(float) override;

    SETTING_CTOR_CREATE(SpeedSetting, BaseSetting)

};