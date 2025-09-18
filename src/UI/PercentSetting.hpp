#include "BaseSetting.hpp"

class PercentSetting : public BaseSetting, public TextInputDelegate {

private:

    CCLabelBMFont* m_percentLabel = nullptr;

    TextInput* m_input = nullptr;

    void textChanged(CCTextInputNode*) override;

    void init(float) override;

    SETTING_CTOR_CREATE(PercentSetting, BaseSetting)

public:

    void setEnabled(bool);

};