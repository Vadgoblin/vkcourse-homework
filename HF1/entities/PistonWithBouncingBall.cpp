#include "PistonWithBouncingBall.h"
#include "../primitives/Cube.h"
#include "../primitives/Cylinder.h"
#include "../primitives/Sphere.h"

PistonWithBouncingBall::PistonWithBouncingBall()
{
    m_pistonBase = new ObjectGroup();
    m_pistonMovingPart = new ObjectGroup();
    m_ball = new ObjectGroup();
}

PistonWithBouncingBall::~PistonWithBouncingBall()
{
    delete m_pistonBase;
    delete m_pistonMovingPart;
    delete m_ball;
}

void PistonWithBouncingBall::draw(const VkCommandBuffer cmdBuffer, const glm::mat4& parentModel)
{
    m_pistonBase->draw(cmdBuffer, parentModel * getModelMatrix());
    m_pistonMovingPart->draw(cmdBuffer, parentModel * getModelMatrix());
    m_ball->draw(cmdBuffer, parentModel * getModelMatrix());
}

void PistonWithBouncingBall::create(Context& context)
{
    Cube* pistonBase = new Cube(true);
    pistonBase->setPosition(0.0f, 0.45f,0.0f);
    pistonBase->setScale(1.0f,0.9f,1.0f);
    pistonBase->create(context, "piston_body");
    m_pistonBase->addChild(pistonBase);

    Cube* pistonRod = new Cube(true);
    pistonRod->setPosition(0.0f, 0.5f,0.0f);
    pistonRod->setScale(0.2f,0.95f,0.2f);
    pistonRod->create(context, "piston_head");
    m_pistonMovingPart->addChild(pistonRod);

    Cube* pistonHead = new Cube(true);
    pistonHead->setPosition(0.0f, 0.95f,0.0f);
    pistonHead->setScale(1.0f,0.1f,1.0f);
    pistonHead->create(context, "piston_head");
    m_pistonMovingPart->addChild(pistonHead);

    Sphere* ball = new Sphere(0.4f,25,25);
    ball->setPosition(0.0f, 1.4f,0.0f);
    ball->create(context, "earth");
    m_ball->addChild(ball);
}

void PistonWithBouncingBall::destroy(const VkDevice device)
{
    m_pistonBase->destroyChildren(device);
    m_pistonMovingPart->destroyChildren(device);
    m_ball->destroyChildren(device);
}

void PistonWithBouncingBall::tick()
{
    m_animationProgress += 1.0f * m_speed;
    if (m_animationProgress >= 5 * PI)
        m_animationProgress -= 5 * PI;

    float t = std::fmod(m_animationProgress, PI);

    float bounceHeight = std::abs(std::sin(t)) * std::exp(-0.14f * m_animationProgress);
    m_ball->setPosition(0.0f, bounceHeight* 4, 0.0f);

    if (m_animationProgress <= PI / 2.0f) {
        m_pistonMovingPart->setPosition(0.0f, 0.9 * std::sin(t * 2),0.0f);
    } else {
        m_pistonMovingPart->setPosition(0.0f, 0.0f, 0.0f);
    }
}
