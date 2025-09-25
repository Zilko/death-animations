#pragma once

#include "../Includes.hpp"

class SpeedCCParticleSystemQuad : public CCParticleSystemQuad {

private:

    float m_speed = 1.f;

    virtual void update(float dt) override {
        CCParticleSystem::update(dt * m_speed);
    }

    bool initWithStruct(ParticleStruct struct_) {
        if (!CCParticleSystemQuad::initWithTotalParticles(struct_.TotalParticles, false))
            return false;

        GameToolbox::particleFromStruct(struct_, this, false);

        return true;
    }

public:

    static SpeedCCParticleSystemQuad* create(const char* file, bool p1) {
        SpeedCCParticleSystemQuad* ret = new SpeedCCParticleSystemQuad();

        if (ret->initWithFile(file, p1)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    static SpeedCCParticleSystemQuad* create(ParticleStruct struct_) {
        SpeedCCParticleSystemQuad* ret = new SpeedCCParticleSystemQuad();

        if (ret->initWithStruct(struct_)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    void setUpdateSpeed(float speed) {
        m_speed = speed;
    }

};