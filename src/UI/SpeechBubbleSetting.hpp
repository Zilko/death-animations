#include "SelectSetting.hpp"

class SpeechBubbleSetting : public SelectSetting {

private:

    void onSelectorArrow(CCObject*) override;

    void init(float) override;

    SETTING_CTOR_CREATE(SpeechBubbleSetting, SelectSetting)

};