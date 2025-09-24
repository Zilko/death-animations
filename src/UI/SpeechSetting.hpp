#include "SelectSetting.hpp"

class SpeechSetting : public SelectSetting {

private:

    void onSelectorArrow(CCObject*) override;

    void init(float) override;

    SETTING_CTOR_CREATE(SpeechSetting, SelectSetting)

};