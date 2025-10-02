#include "BaseAnimation.hpp"

class Terraria : public BaseAnimation {
    
private:

    constexpr static std::array m_deathMessages = std::to_array<std::string_view>({
        "{} left a small crater.","{} discovered gravity.","{} didn't bounce.","{} faceplanted the ground.","{} fell victim to gravity.","{} fell to their death.","{} hit the ground face first.","{} is shark food.","{} forgot to breathe.","{} is sleeping with the fish.","{} drowned.","{} thought he was a fish.","{} likes to play in magma.","{} got melted.","{} tried to swim in lava.","{} was incinerated.","{} exploded.","{} had a mining accident.","{} forgot to run.","{} was destroyed.","{} was turned into pile of flesh.","{} got snapped in half.","{} was murdered.","{} was mangled.","{}'s vital organs were ruptured.","{}'s plead for death was answered.","{}'s innards were made outards.","{}'s skull was crushed.","{}'s body was mangled.","{} was licked.","{} tried to escape.","{} turned corrupt.","{} was torn in half.","{} was slain...","{} didn't materialize.","{}'s legs appeared where his/her head should be."
    });

    ANIMATION_CTOR_CREATE(Terraria) {}
    
public:

    void startEarly() override {
    
    }

    void start() override {
        log::debug("no");
        CCLabelBMFont* lbl = CCLabelBMFont::create("", "terraria.fnt"_spr);
        // lbl->setPosition(m_size / 2.f);
        
        // Utils::fixScaleTextureSizexd(lbl);
        
        // addChild(lbl);
    }

    };