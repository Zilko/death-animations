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

        setDuration(struct_.Duration);
        setLife(struct_.Life);
        setLifeVar(struct_.LifeVar);
        setEmissionRate(struct_.EmissionRate);
        setAngle(struct_.Angle);
        setAngleVar(struct_.AngleVar);
        setSpeed(struct_.Speed);
        setSpeedVar(struct_.SpeedVar);
        setPosVar(ccp(struct_.PosVarX, struct_.PosVarY));
        setGravity(ccp(struct_.GravityX, struct_.GravityY));
        setRadialAccel(struct_.RadialAccel);
        setRadialAccelVar(struct_.RadialAccelVar);
        setTangentialAccel(struct_.TangentialAccel);
        setTangentialAccelVar(struct_.TangentialAccelVar);
        setStartSize(struct_.StartSize);
        setStartSizeVar(struct_.StartSizeVar);
        setStartSpin(struct_.StartSpin);
        setStartSpinVar(struct_.StartSpinVar);
        setStartColor({struct_.StartColorR, struct_.StartColorG, struct_.StartColorB, struct_.StartColorA});
        setStartColorVar({struct_.StartColorVarR, struct_.StartColorVarG, struct_.StartColorVarB, struct_.StartColorVarA});
        setEndSize(struct_.EndSize);
        setEndSizeVar(struct_.EndSizeVar);
        setEndSpin(struct_.EndSpin);
        setEndSpinVar(struct_.EndSpinVar);
        setEndColor({struct_.EndColorR, struct_.EndColorG, struct_.EndColorB, struct_.EndColorA});
        setEndColorVar({struct_.EndColorVarR, struct_.EndColorVarG, struct_.EndColorVarB, struct_.EndColorVarA});
        setFadeInTime(struct_.FadeInTime);
        setFadeInTimeVar(struct_.FadeInTimeVar);
        setFadeOutTime(struct_.FadeOutTime);
        setFadeOutTimeVar(struct_.FadeOutTimeVar);
        setStartRadius(struct_.StartRadius);
        setStartRadiusVar(struct_.StartRadiusVar);
        setEndRadius(struct_.EndRadius);
        setEndRadiusVar(struct_.EndRadiusVar);
        setRotatePerSecond(struct_.RotatePerSecond);
        setRotatePerSecondVar(struct_.RotatePerSecondVar);
        setEmitterMode(struct_.EmitterMode);
        setPositionType((tCCPositionType)struct_.PositionType);
        setBlendAdditive(struct_.isBlendAdditive);
        setStartSpinEqualToEnd(struct_.startSpinEqualToEndSpin);
        setRotationIsDir(struct_.rotationIsDir);
        setDynamicRotationIsDir(struct_.dynamicRotationIsDir);
        // (struct_.customParticleIdx);
        toggleUniformColorMode(struct_.uniformColorMode);
        setFrictionPos(struct_.frictionPos);
        setFrictionPosVar(struct_.frictionPosVar);
        setRespawn(struct_.respawn);
        setRespawnVar(struct_.respawnVar);
        setOrderSensitive(struct_.orderSensitive);
        setStartSizeEqualToEnd(struct_.startSizeEqualToEndSize);
        setStartRadiusEqualToEnd(struct_.startRadiusEqualToEndRadius);
        setStartRGBVarSync(struct_.startRGBVarSync);
        setEndRGBVarSync(struct_.endRGBVarSync);
        setFrictionSize(struct_.frictionSize);
        setFrictionSizeVar(struct_.frictionSizeVar);
        setFrictionRot(struct_.frictionRot);
        setFrictionRotVar(struct_.frictionRotVar);

        setDisplayFrame(CCSpriteFrameCache::get()->spriteFrameByName(struct_.sFrame.c_str()));

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