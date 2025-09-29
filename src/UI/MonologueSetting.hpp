#include "SelectSetting.hpp"

class MonologueSetting : public SelectSetting {

private:

    void onSelectorArrow(CCObject*) override;

    void init(float) override;

    SETTING_CTOR_CREATE(MonologueSetting, SelectSetting)

};