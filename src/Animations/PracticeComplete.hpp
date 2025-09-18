#include "LevelComplete.hpp"

class PracticeComplete : public LevelComplete {

private:

    PracticeComplete(const AnimationParams& params)
        : LevelComplete(params) {}

    ANIMATION_CREATE(PracticeComplete)
    
public:

    void start() override {
        m_isPractice = true;

        LevelComplete::start();
    }

};