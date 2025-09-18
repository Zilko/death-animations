#include "ToggleSetting.hpp"

class PercentSetting;

class PercentToggleSetting : public ToggleSetting {

private:

    PercentSetting* m_percentSetting = nullptr;

    void onToggle(CCObject*) override;

    void init(float) override;

    SETTING_CTOR_CREATE(PercentToggleSetting, ToggleSetting)

};