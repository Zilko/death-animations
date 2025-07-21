#include "../Includes.hpp"

class AnimationSettingsLayer : public geode::Popup<>, public TextInputDelegate {

private:

    CCLabelBMFont* m_durationLabel = nullptr;
    CCLabelBMFont* m_speedLabel = nullptr;
    
    CCSprite* m_durationSprite = nullptr;
    
    std::unordered_map<SliderThumb*, std::string> m_sliderIDs;
    std::unordered_map<CCMenuItemToggler*, std::string> m_toggleIDs;
    std::unordered_map<CCTextInputNode*, std::string> m_inputIDs;

    DeathAnimation m_animation;

    AnimationSettingsLayer(const DeathAnimation&);

    bool setup() override;
    
    void updateDuration(CCObject*);
    void onToggle(CCObject*);
    void onRestore(CCObject*);
    
    void textChanged(CCTextInputNode*) override;

public:

    static AnimationSettingsLayer* create(const DeathAnimation&);
    
};