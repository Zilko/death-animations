#include "BaseSetting.hpp"

class JumpscareSetting : public BaseSetting {

private:

    void onFolder(CCObject*);

    void init(float) override;

    SETTING_CTOR_CREATE(JumpscareSetting, BaseSetting)

};