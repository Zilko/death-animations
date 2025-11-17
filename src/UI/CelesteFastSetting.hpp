#include "ToggleSetting.hpp"

class CelesteFastSetting : public ToggleSetting {

private:

    void onToggle(CCObject*) override;

    void init(float) override;

    SETTING_CTOR_CREATE(CelesteFastSetting, ToggleSetting)

};
