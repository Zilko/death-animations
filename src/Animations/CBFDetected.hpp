#include "BaseAnimation.hpp"

class CBFDetected : public BaseAnimation {

public:
    
    DEFINE_CREATE(CBFDetected)

    void start() override {
        BaseAnimation::start();

        setPositionY((m_size.height - m_size.width / 1.777777f) / 5.f);

        CCLayerColor* layer = CCLayerColor::create({0, 0, 0, 144}, m_size.width, 60);

        addChild(layer);

        CCLabelBMFont* realLbl = CCLabelBMFont::create(
            fmt::format(
                "Click Between Frames is illegitimate and will not be allowed for use in {}.",
                Utils::getSettingBool(Anim::CBFDetected, "use-level-name") && !m_isPreview ? m_playLayer->m_level->m_levelName : "Nullscapes"
            ).c_str(),
            "bigFont.fnt"
        );
        realLbl->limitLabelWidth(m_size.width - 20.f, 0.32f, 0.0001f);
        realLbl->setPosition({5.76f, 24.5f});
        realLbl->setAnchorPoint({0, 0.5f});

        addChild(realLbl);

        CCLabelBMFont* lbl = CCLabelBMFont::create("CBF Detected, loser!", "bigFont.fnt");
        lbl->setColor({255, 56, 35});
        lbl->setScale(0.6f);
        lbl->setPosition({115, 41});

        addChild(lbl);

        lbl = CCLabelBMFont::create("Please disable the mod in order to continue playing.", "bigFont.fnt");
        lbl->limitLabelWidth(m_size.width, 0.32f, 0.0001f);
        lbl->setScale(0.32f);
        lbl->setPosition({5.56f, 14.6f});
        lbl->setAnchorPoint({0, 0.5f});

        addChild(lbl);

        layer->setContentWidth(m_size.width / realLbl->getScale() / 0.32f);
        setScale(realLbl->getScale() / 0.32f);

        realLbl->setScale(0.32f);
    }

    };