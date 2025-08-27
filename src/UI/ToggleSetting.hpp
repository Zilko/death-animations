#include "BaseSetting.hpp"

class ToggleSetting : public BaseSetting {

private:

    CCMenuItemToggler* m_toggle = nullptr;

    void onToggle(CCObject*);

    void init(float) override;

    SETTING_CTOR_CREATE(ToggleSetting, BaseSetting)

};