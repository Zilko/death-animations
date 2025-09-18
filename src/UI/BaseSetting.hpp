#pragma once

#include "../Includes.hpp"

class AnimationSettingsLayer;

#define SETTING_CTOR_CREATE(CLASS, PARENT) \
    protected: \
        CLASS(AnimationSettingsLayer* parentLayer, const AnimationSetting& setting, DeathAnimation animation) \
            : PARENT(parentLayer, setting, animation) {} \
    public: \
        static CLASS* create(AnimationSettingsLayer* parentLayer, const AnimationSetting& setting, DeathAnimation animation, float y) { \
            CLASS* ret = new CLASS(parentLayer, setting, animation); \
            ret->autorelease(); \
            ret->init(y); \
            return ret; \
        } \
    private:

class BaseSetting : public CCNode {

protected:

    CCLabelBMFont* m_nameLabel = nullptr;

    CCMenu* m_menu = nullptr;

    CCMenuItemSpriteExtra* m_infoBtn = nullptr;

    AnimationSettingsLayer* m_parentLayer = nullptr;

    AnimationSetting m_setting;
    DeathAnimation m_animation;

    int m_animationID = 0;

    BaseSetting(AnimationSettingsLayer* parentLayer, const AnimationSetting& setting, DeathAnimation animation)
        : m_parentLayer(parentLayer), m_setting(setting), m_animation(animation) {}

    void onInfo(CCObject*);

    void setInfoButtonPos(CCNode*);

    virtual void init(float y);

};