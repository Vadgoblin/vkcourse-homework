#include "OrbitingHelicopter.h"
#include "../primitives/Cube.h"

OrbitingHelicopter::OrbitingHelicopter()
{
    m_helicopterBody = new ObjectGroup();
    m_helicopterRotor1 = new ObjectGroup();
    m_helicopterRotor2 = new ObjectGroup();
    m_helicopterRotor2Moved = new ObjectGroup();
    m_helicopterTilted = new ObjectGroup();
    m_helicopterMoved =  new ObjectGroup();
    m_helicopterTilted =  new ObjectGroup();
    m_helicopterOrbiting = new ObjectGroup();
}

OrbitingHelicopter::~OrbitingHelicopter()
{
    delete m_helicopterOrbiting;
}

void OrbitingHelicopter::draw(VkCommandBuffer cmdBuffer, const glm::mat4& parentModel)
{
    m_helicopterOrbiting->draw(cmdBuffer, parentModel * getModelMatrix());
}

void OrbitingHelicopter::destroy(VkDevice device)
{
    m_helicopterBody->destroyChildren(device);
    m_helicopterRotor1->destroyChildren(device);
    m_helicopterRotor2->destroyChildren(device);
    m_helicopterRotor2Moved->destroyChildren(device);
    m_helicopterTilted->destroyChildren(device);
    m_helicopterMoved->destroyChildren(device);
    m_helicopterOrbiting->destroyChildren(device);
}

void OrbitingHelicopter::create(const Context& context, VkFormat colorFormat, uint32_t pushConstantStart)
{
    Cube* helicopterCabin = new Cube(1,true);
    helicopterCabin->create(context, colorFormat, pushConstantStart);
    m_helicopterBody->addChild(helicopterCabin);

    Cube* helicopterTail = new Cube(1, true);
    helicopterTail->create(context, colorFormat, pushConstantStart);
    helicopterTail->setScale(3.0f, 0.5f, 0.5f);
    helicopterTail->setPosition(-2.0f,0.0f,0.0f);
    m_helicopterBody->addChild(helicopterTail);

    Cube* helicopterRotor1 = new Cube(1, true);
    helicopterRotor1->create(context, colorFormat, pushConstantStart);
    helicopterRotor1->setScale(4.0f, 0.25f, 0.5f);
    helicopterRotor1->setPosition(0.0f, 0.5f + (0.25f / 2.0f), 0.0f);
    m_helicopterRotor1->addChild(helicopterRotor1);

    Cube* helicopterRotor2 = new Cube(1, true);
    helicopterRotor2->create(context, colorFormat, pushConstantStart);
    helicopterRotor2->setScale(0.25f, 1.0f, 0.125f);
    m_helicopterRotor2->addChild(helicopterRotor2);
    m_helicopterRotor2Moved->addChild(m_helicopterRotor2);
    m_helicopterRotor2Moved->setPosition(-3.25f,0.0f,-0.25f);

    m_helicopterTilted->addChild(m_helicopterBody);
    m_helicopterTilted->addChild(m_helicopterRotor1);
    m_helicopterTilted->addChild(m_helicopterRotor2Moved);
    m_helicopterTilted->setRotation(-20.0f, 0.0f, -2.0f);

    m_helicopterMoved->addChild(m_helicopterTilted);
    m_helicopterMoved->setPosition(0.0f,0.0f,13.0f);

    m_helicopterOrbiting->addChild(m_helicopterMoved);
}

void OrbitingHelicopter::tick()
{
    m_time += 1.0f * m_speed;

    m_helicopterRotor1->setRotation(0.0f,m_time * m_rotor1Speed,0.0f);
    m_helicopterRotor2->setRotation(0.0f,0.0f,m_time * m_rotor2Speed);

    m_helicopterOrbiting->setRotation(0.0f,m_time * m_orbitSpeed, 0.0f);
}
