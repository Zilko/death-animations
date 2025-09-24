#include "../Includes.hpp"

#include "../Animations/BaseAnimation.hpp"

class PreviewLayer : public geode::Popup<>, public PreviewDelegate {
  
private:

    CCMenuItemSpriteExtra* m_backButton = nullptr;

    CCSprite* m_spike = nullptr;

    SimplePlayer* m_player = nullptr;
    
    CCCircleWave* m_circleWave = nullptr;

    BaseAnimation* m_animation = nullptr;
    
    ccColor3B m_mainColor;

    DeathAnimation m_animationStruct;
    
    float m_time = 0.f;
    float m_speed = 1.f;
    float m_duration = 0.f;

    bool m_isDead = false;

    ~PreviewLayer();

    bool setup() override;
    
    void updateCircle(float);
    void spawnPlayer(float = 0.f);
    void playerDied(float);
    void playDeathEffect();

    virtual void keyDown(enumKeyCodes) override;
    
public:
    
    static PreviewLayer* create();
    
    CCNodeRGBA* getPlayer() override;
    CCNode* getBackButton() override;
    void setBackgroundOpacity(int) override;
    bool isDead() override;

};