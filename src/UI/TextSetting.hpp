#include "BaseSetting.hpp"

class TextSetting : public BaseSetting, public TextInputDelegate {

private:

    void textChanged(CCTextInputNode*) override;
    
    void init(float) override;

    SETTING_CTOR_CREATE(TextSetting, BaseSetting)

};