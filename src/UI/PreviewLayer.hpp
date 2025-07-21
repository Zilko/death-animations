#include "../Includes.hpp"

#include "../Animations/BaseAnimation.hpp"

class PreviewLayer : public geode::Popup<>, public AnimationDelegate {
  
private:

    CCSprite* m_spike = nullptr;

    SimplePlayer* m_player = nullptr;
    
    CCCircleWave* m_circleWave = nullptr;

    BaseAnimation* m_animation = nullptr;
    
    ccColor3B m_mainColor;
    
    float m_time = 0.f;
    float m_speed = 1.f;
    float m_duration = 0.f;

    bool setup() override;
    
    void updateCircle(float);
    void spawnPlayer(float = 0.f);
    void playerDied(float);
    void playDeathEffect();
    
    void onClose(CCObject*) override;

public:
    
    static PreviewLayer* create();

};