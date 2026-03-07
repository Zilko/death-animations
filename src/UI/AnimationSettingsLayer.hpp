#include "../Includes.hpp"

class AnimationSettingsLayer : public Popup {

private:

    CCLabelBMFont* m_durationLabel = nullptr;
    CCLabelBMFont* m_speedLabel = nullptr;
    
    CCSprite* m_durationSprite = nullptr;

    DeathAnimation m_animation;

    AnimationSettingsLayer(const DeathAnimation&);

    bool init() override;
    
    void onRestore(CCObject*);

public:

    static AnimationSettingsLayer* create(const DeathAnimation&);

    CCLabelBMFont* getDurationLabel();
    CCSprite* getDurationSprite();
    
};