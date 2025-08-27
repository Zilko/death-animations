#include "BaseSetting.hpp"

class PercentSetting : public BaseSetting, public TextInputDelegate {

private:

    void textChanged(CCTextInputNode*) override;

    void init(float) override;

    SETTING_CTOR_CREATE(PercentSetting, BaseSetting)

};