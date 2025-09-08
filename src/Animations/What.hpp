#include "BaseAnimation.hpp"

class What : public BaseAnimation {

    ANIMATION_CTOR_CREATE(What)
    
public:

    void start() override {
        Utils::setHighestZ(this);

        Utils::playSound(Anim::What, "what.mp3", m_speed, 1.f);

        CCSprite* spr = CCSprite::createWithTexture(Utils::takeScreenshot());
        spr->setFlipY(true);
        spr->setAnchorPoint({0, 0});
        spr->setBlendFunc(ccBlendFunc{GL_ONE, GL_ZERO});
        addChild(spr);

        float leftBorderWidth = m_size.width / 4.63f;
        addChild(CCLayerColor::create({0, 0, 0, 255}, leftBorderWidth, m_size.height));

        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 255}, leftBorderWidth, m_size.height);
        layer->setPositionX(m_size.width - layer->getContentWidth());
        addChild(layer);

        float bottomBorderHeight = m_size.height / 3.41f;
        layer = CCLayerColor::create({0, 0, 0, 255}, m_size.width - leftBorderWidth * 2, bottomBorderHeight);
        layer->setPositionX(leftBorderWidth);
        addChild(layer);

        float topBorderHeight = m_size.height - bottomBorderHeight - m_size.height / 1.48f;
        layer = CCLayerColor::create({0, 0, 0, 255}, m_size.width - leftBorderWidth * 2, topBorderHeight);
        layer->setPosition({leftBorderWidth, m_size.height - topBorderHeight});
        addChild(layer);

        float spacin = 4.7f;
        float lineWidth = 1.5f;

        layer = CCLayerColor::create({255, 255, 255, 255}, lineWidth, m_size.height - bottomBorderHeight - topBorderHeight + spacin * 2.f);
        layer->setPosition({leftBorderWidth - spacin, bottomBorderHeight - spacin});
        addChild(layer);

        layer = CCLayerColor::create({255, 255, 255, 255}, m_size.width - leftBorderWidth * 2.f + spacin * 2.f, lineWidth);
        layer->setPosition({leftBorderWidth - spacin, bottomBorderHeight - spacin});
        addChild(layer);

        layer = CCLayerColor::create({255, 255, 255, 255}, m_size.width - leftBorderWidth * 2.f + spacin * 2.f, lineWidth);
        layer->setPosition({leftBorderWidth - spacin, m_size.height - topBorderHeight + spacin - lineWidth});
        addChild(layer);

        layer = CCLayerColor::create({255, 255, 255, 255}, lineWidth, m_size.height - bottomBorderHeight - topBorderHeight + spacin * 2.f);
        layer->setPosition({m_size.width - leftBorderWidth + spacin - lineWidth, bottomBorderHeight - spacin});
        addChild(layer);

        CCLabelBMFont* lbl = CCLabelBMFont::create(Utils::getSettingString(Anim::What, "top-text").c_str(), "times-rouman.fnt"_spr);
        lbl->setPosition({m_size.width / 2.f, 64});

        addChild(lbl);

        lbl = CCLabelBMFont::create(Utils::getSettingString(Anim::What, "bottom-text").c_str(), "times-rouman.fnt"_spr);
        lbl->setPosition({m_size.width / 2.f, 31});
        lbl->setScale(0.55f);
        
        addChild(lbl);

        CCRect innerRect = CCRectMake(leftBorderWidth, bottomBorderHeight, m_size.width - leftBorderWidth * 2, m_size.height - bottomBorderHeight - topBorderHeight);
        CCPoint playerPos = Utils::getPlayerScreenPos(m_playLayer, m_isPreview ? m_delegate->getPlayer() : m_playLayer->m_player1, m_isPreview);
        CCPoint desiredPosition = ccp(innerRect.origin.x + innerRect.size.width / 2, innerRect.origin.y + innerRect.size.height / 2) - playerPos;

        spr->setScale(1.1f);

        if (!m_isPreview)
            spr->setScale(spr->getScale() * 1.f / m_playLayer->m_gameState.m_cameraZoom);

        desiredPosition -= playerPos * spr->getScale() - playerPos;

        float scaledWidth = spr->getContentWidth() * spr->getScaleX();
        float scaledHeight = spr->getContentHeight() * spr->getScaleY();
        
        desiredPosition.x = clampf(desiredPosition.x, innerRect.origin.x, innerRect.origin.x + innerRect.size.width - scaledWidth);
        desiredPosition.y = clampf(desiredPosition.y, innerRect.origin.y, innerRect.origin.y + innerRect.size.height - scaledHeight);

        spr->setPosition(desiredPosition);
    }

};