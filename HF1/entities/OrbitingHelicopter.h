#pragma once
#include "BaseEntity.h"
#include "../containers/ObjectGroup.h"

class OrbitingHelicopter : public BaseEntity {
public:
    OrbitingHelicopter();
    ~OrbitingHelicopter();
    void draw(VkCommandBuffer cmdBuffer, const glm::mat4& parentModel = glm::mat4(1.0f)) override;
    void create(Context& context, LightningPass& lightningPass) override;
    void destroy(VkDevice device) override;
    void tick() override;

private:
    float        m_time = 0.0f;
    float        m_speed = 1.0f;
    float        m_orbitSpeed = 1.0f;
    float        m_rotor1Speed = 20.0f;
    float        m_rotor2Speed = 10.0f;

    ObjectGroup* m_helicopterBody;
    ObjectGroup* m_helicopterRotor1;
    ObjectGroup* m_helicopterRotor2;
    ObjectGroup* m_helicopterRotor2Moved;
    ObjectGroup* m_helicopterTilted;
    ObjectGroup* m_helicopterMoved;
    ObjectGroup* m_helicopterOrbiting;
};