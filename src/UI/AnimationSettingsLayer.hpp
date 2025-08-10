#include "../Includes.hpp"

class AnimationSettingsLayer : public geode::Popup<>, public TextInputDelegate, public ColorPickPopupDelegate {

private:

    struct SelectorStuff {
        std::string id;
        CCLabelBMFont* label;
        std::vector<std::string> elements;
    };

    CCLabelBMFont* m_durationLabel = nullptr;
    CCLabelBMFont* m_speedLabel = nullptr;
    
    CCSprite* m_durationSprite = nullptr;
    CCSprite* m_colorSprite1 = nullptr;
    CCSprite* m_colorSprite2 = nullptr;
    
    CCMenuItemSpriteExtra* m_colorButton1 = nullptr;
    CCMenuItemSpriteExtra* m_colorButton2 = nullptr;
    CCMenuItemSpriteExtra* m_amogusInfo = nullptr;
    
    CCNode* m_amogusArrow = nullptr;
    
    std::unordered_map<SliderThumb*, std::string> m_sliderIDs;
    std::unordered_map<CCMenuItemToggler*, std::string> m_toggleIDs;
    std::unordered_map<CCTextInputNode*, std::string> m_inputIDs;
    std::unordered_map<CCObject*, SelectorStuff> m_selectors;
    std::unordered_map<CCObject*, std::pair<std::string, std::string>> m_descriptions;
    
    std::unordered_set<CCTextInputNode*> m_percentInputs;

    DeathAnimation m_animation;
    
    int m_currentColor = 1;

    AnimationSettingsLayer(const DeathAnimation&);

    bool setup() override;
    
    CCNode* addToggleSetting(float, const AnimationSetting&, CCMenu*);
    CCNode* addSpeedSetting(float, const AnimationSetting&, ScrollLayer*);
    CCNode* addPercentSetting(float, const AnimationSetting&, ScrollLayer*);
    CCNode* addSelectSetting(float, const AnimationSetting&, CCMenu*, ScrollLayer*, CCLabelBMFont*);
    CCNode* addAmongUsSetting(float, const AnimationSetting&, CCMenu*, ScrollLayer*, CCLabelBMFont*);
    CCNode* addJumpscareSetting(float, const AnimationSetting&, CCMenu*);
    
    void onSlider(CCObject*);
    void onToggle(CCObject*);
    void onRestore(CCObject*);
    void onSelectorArrow(CCObject*);
    void onColorPicker(CCObject*);
    void onInfo(CCObject*);
    void onFolder(CCObject*);
    
    void textChanged(CCTextInputNode*) override;
    virtual void updateColor(const ccColor4B&) override;

public:

    static AnimationSettingsLayer* create(const DeathAnimation&);
    
};