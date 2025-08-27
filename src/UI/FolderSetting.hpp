#include "BaseSetting.hpp"

class FolderSetting : public BaseSetting {

private:

    void onFolder(CCObject*);

    void init(float) override;

    SETTING_CTOR_CREATE(FolderSetting, BaseSetting)

};