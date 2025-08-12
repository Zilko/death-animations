#include "BaseAnimation.hpp"

class UndertaleHeart : public BaseAnimation {

private:

public:

    DEFINE_CREATE(UndertaleHeart)

    void start() override {
        BaseAnimation::start();

        CCPoint position = m_isPreview ? m_delegate->getPlayer() : ;

        CCSprite* spr = CCSprite::create("heart-undertale.png"_spr);

        spr->
    }

};

class Undertale : public BaseAnimation {

public:
    
    DEFINE_CREATE(Undertale)

    void start() override {
        BaseAnimation::start();

        setZOrder(120983917);

        addChild(CCLayerColor::create({0, 0, 0, 255}, m_size.width, m_size.height));

        
    }

    };