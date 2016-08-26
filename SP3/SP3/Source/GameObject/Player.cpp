/******************************************************************************/
/*!
\file	Player.cpp
\author Foo Jing Ting
\par	email: 152856H\@mymail.nyp.edu.sg
\brief
Player class that stores the game's player variables
*/
/******************************************************************************/
#include "Player.h"
#include "MyMath.h"
#include "../General/Application.h"
#include "../General/SharedData.h"

Player::Player()
{
    m_position.SetZero();
    m_velocity.SetZero();
    m_view.Set(1, 0, 0);
    m_up.Set(0, 1, 0);

    m_eyeLevel = 5.f;
    m_speed = 0.f;
    m_jumpSpeed = 0.f;
    m_gravity = -100.f;
    m_jumpHeight = 0.f;

    m_noiseFactor = 0.5f;
    m_bHiding = false;

    m_health = 100.f;

    PlayerHitBox.m_origin.SetZero();
    PlayerHitBox.m_scale = Vector3(10, 0.1, 10);

    m_movementState = MOVEMENT_STATE_IDLE;
    m_heightState = HEIGHT_STATE_STANDING;

    Item tempInventory[Item::NUM_TYPE] =
    {
        Item("Net", Item::TYPE_NET, 10, 100, 10),
        Item("Bait", Item::TYPE_BAIT, 10, 100, 10),
        Item("Meat", Item::TYPE_MEAT, 10, 100, 10),
        Item("Trap1", Item::TYPE_TRAP_ONE, 10, 100, 10),
        Item("Trap2", Item::TYPE_TRAP_TWO, 10, 100, 10),
        Item("Rock", Item::TYPE_ROCK, 10, 100, 10)
    };

    for (unsigned i = 0; i < Item::NUM_TYPE; ++i)
    {
        inventory[i] = tempInventory[i];
        inventory[i].Add(100);
    }

}

Player::~Player()
{
}

Vector3 Player::GetPositionVector()
{
    return m_position;
}

Vector3 Player::GetViewVector()
{
    return m_view;
}

Vector3 Player::GetUpVector()
{
    return m_up;
}

static const float CAMERA_SPEED = 5.0f;

void Player::Update(double dt)
{
    //m_bHiding = false;
    m_movementState = MOVEMENT_STATE_IDLE;
    m_velocity.SetZero();

    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_W].isHeldDown)
    {
        m_movementState = MOVEMENT_STATE_WALK;

        m_velocity += m_view.Normalized();
    }
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_S].isHeldDown)
    {
        m_movementState = MOVEMENT_STATE_WALK;
        m_velocity += -m_view.Normalized();
    }
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_A].isHeldDown)
    {
        m_movementState = MOVEMENT_STATE_WALK;
        Vector3 right = m_view.Cross(m_up);
        right.y = 0;
        right.Normalize();
        m_velocity += -right;
    }
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_D].isHeldDown)
    {
        m_movementState = MOVEMENT_STATE_WALK;
        Vector3 right = m_view.Cross(m_up);
        right.y = 0;
        right.Normalize();
        m_velocity += right;
    }

    if (m_velocity.LengthSquared() > Math::EPSILON)
    {
        m_velocity.Normalized();
        m_prevVelocity = m_velocity;
    }
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_CTRL].isPressed)
    {
        if (m_heightState == HEIGHT_STATE_STANDING)
            crouch();
        else if (m_heightState == HEIGHT_STATE_CROUCH)
            prone();
        else if (m_heightState == HEIGHT_STATE_PRONE)
            standUp();
    }
    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_SPACE].isHeldDown)
    {
        jump();
    }

    if (SharedData::GetInstance()->inputManager->keyState[InputManager::KEY_SHIFT].isHeldDown && m_heightState == HEIGHT_STATE_STANDING)
    {
        if (m_movementState != MOVEMENT_STATE_IDLE) {
            m_movementState = MOVEMENT_STATE_RUN;
        }
    }

    if (m_heightState == HEIGHT_STATE_CROUCH)
    {
        m_movementState = MOVEMENT_STATE_CROUCH;
    }
    else if (m_heightState == HEIGHT_STATE_PRONE)
    {
        m_movementState = MOVEMENT_STATE_PRONE;
    }

    switch (m_movementState)
    {
    case MOVEMENT_STATE_IDLE:
        //speed = 0;
        if (m_speed != 0.f)
        {
            m_speed -= 100.f * (float)(dt);
            if (m_speed < 0.f)
                m_speed = 0.f;
        }
        m_velocity = m_prevVelocity;
        break;

    case MOVEMENT_STATE_CROUCH:
        //speed = 5.f;
        if (m_speed > 5.f)
        {
            m_speed -= 30.f * (float)(dt);
            if (m_speed < 5.f)
                m_speed = 5.f;
        }
        else if (m_speed < 5.f)
        {
            m_speed += 30.f * (float)(dt);
            if (m_speed > 5.f)
                m_speed = 5.f;
        }
        break;

    case MOVEMENT_STATE_PRONE:
        m_speed = 2.5f;
        if (m_speed > 1.5f)
        {
            m_speed -= 15.f * (float)(dt);
            if (m_speed < 1.5f)
                m_speed = 1.5f;
        }
        else if (m_speed < 1.5f)
        {
            m_speed += 15.f * (float)(dt);
            if (m_speed > 1.5f)
                m_speed = 1.5f;
        }
        break;

    case MOVEMENT_STATE_WALK:
        //speed = 20.f;
        if (m_speed > 20.f)
        {
            m_speed -= 40.f * (float)(dt);
            if (m_speed < 20.f)
                m_speed = 20.f;
        }
        else if (m_speed < 20.f)
        {
            m_speed += 40.f * (float)(dt);
            if (m_speed > 20.f)
                m_speed = 20.f;
        }
        break;

    case MOVEMENT_STATE_RUN:
        //speed = 60.f;
        if (m_speed > 60.f)
        {
            m_speed -= 60.f * (float)(dt);
            if (m_speed < 60.f)
                m_speed = 60.f;
        }
        else if (m_speed < 60.f)
        {
            m_speed += 60.f * (float)(dt);
            if (m_speed > 60.f)
                m_speed = 60.f;
        }
        break;
    }
    PlayerHitBox.m_origin = m_position;
    move(dt);

    //Move(dt); // move by Vector3 velocity

    // Turn player's view
    double x, y;
    Application::GetCursorPos(&x, &y);

    if (x != Application::cursorXPos)
    {
        double diff_xpos = Application::cursorXPos - x;

        //Vector3 view = (target - position).Normalized();
        float yaw = (float)(diff_xpos * CAMERA_SPEED * (float)dt);
        Mtx44 rotation;
        rotation.SetToRotation(yaw, 0, 1, 0);
        m_view = rotation * m_view;
        //target = position + view;
        Vector3 right = m_view.Cross(m_up);
        right.y = 0;
        right.Normalize();
        m_up = right.Cross(m_view).Normalized();
    }
    if (y != Application::cursorYPos - 0.5)
    {
        double diff_ypos = Application::cursorYPos - y;

        float pitch = (float)(diff_ypos * CAMERA_SPEED * (float)dt);
        //Vector3 view = (target - position).Normalized();
        Vector3 right = m_view.Cross(m_up);
        right.y = 0;
        right.Normalize();
        m_up = right.Cross(m_view).Normalized();
        Mtx44 rotation;
        rotation.SetToRotation(pitch, right.x, right.y, right.z);
        m_view = rotation * m_view;
        //target = position + view;
    }

    if (m_bHiding)
        m_noiseFactor *= 0.5f;
}

void Player::pitch(const double dt)
{

}

void Player::yaw(const double dt)
{

}

void Player::move(const double dt)
{
    m_position += m_velocity * m_speed * (float)dt;
    //std::cout << m_movementState << " | " << m_heightState  << " | " << m_speed << std::endl;
    //std::cout << m_position << std::endl;
    switch (m_heightState)
    {
    case HEIGHT_STATE_STANDING:
        updateStandUp(dt);
        break;

    case HEIGHT_STATE_CROUCH:
        updateCrouch(dt);
        break;

    case HEIGHT_STATE_JUMP:
        updateJump(dt);
        break;

    case HEIGHT_STATE_PRONE:
        updateProne(dt);
        break;
    }
    m_position.y = m_eyeLevel + m_jumpHeight;

    if (m_speed <= 0.f)
    {
        m_noiseFactor = 0.f;
    }
    else if (m_speed > 20.f)
    {
        m_noiseFactor = 0.6f;
    }
}

void Player::crouch()
{
    if (m_heightState != HEIGHT_STATE_CROUCH && m_eyeLevel == 5.f)
    {
        m_heightState = HEIGHT_STATE_CROUCH;
    }

    if (m_heightState == HEIGHT_STATE_CROUCH && m_eyeLevel == 2.5f)
    {
        m_heightState = HEIGHT_STATE_STANDING;
    }
}

void Player::prone()
{
    if (m_heightState != HEIGHT_STATE_PRONE && m_eyeLevel == 5.f)
    {
        m_heightState = HEIGHT_STATE_PRONE;
    }

    if (m_heightState != HEIGHT_STATE_PRONE && m_eyeLevel == 2.5f)
    {
        m_heightState = HEIGHT_STATE_PRONE;
    }

    if (m_heightState == HEIGHT_STATE_PRONE && m_eyeLevel == 1.f)
    {
        m_heightState = HEIGHT_STATE_STANDING;
    }
}

void Player::standUp()
{
    if (m_heightState != HEIGHT_STATE_STANDING && m_eyeLevel == 1.f)
    {
        m_heightState = HEIGHT_STATE_STANDING;
    }
}

void Player::jump()
{
    if (m_heightState != HEIGHT_STATE_JUMP && m_heightState != HEIGHT_STATE_PRONE) {
        m_heightState = HEIGHT_STATE_JUMP;

        m_jumpSpeed = 30.f;    //dt not needed
    }
}

void Player::updateStandUp(const double dt)
{
    if (m_eyeLevel < 25.f)
    {
        m_eyeLevel += (float)(20.f * dt);
        m_eyeLevel = Math::Min(5.f, m_eyeLevel);
    }
    m_noiseFactor = 0.5f;
}

void Player::updateCrouch(const double dt)
{
    m_eyeLevel -= (float)(20.f * dt);
    m_eyeLevel = Math::Max(2.5f, m_eyeLevel);
    m_noiseFactor = 0.35f;
}

void Player::updateProne(const double dt)
{
    m_eyeLevel -= (float)(20.f * dt);
    m_eyeLevel = Math::Max(1.f, m_eyeLevel);
    m_noiseFactor = 0.2f;
}

void Player::updateJump(const double dt)
{
    // Factor in gravity
    m_jumpSpeed += (float)(m_gravity * dt);

    // Update camera and target position

    m_jumpHeight += m_jumpSpeed * (float)dt;

    // Check if camera reached the ground
    float newHeight = m_position.y + m_jumpHeight;  //player would be in the ground already
    if (newHeight + 1.f <= m_position.y)
    {
        // Camera landing on ground
        m_position.y = newHeight;

        //when landing, reduce MoveVel (impact)
        //m_speed = Math::Max(0.f, m_speed - 20.f);

        // Reset values
        m_jumpSpeed = 0.f;
        m_heightState = HEIGHT_STATE_STANDING;
        m_jumpHeight = 0.f;
    }
}

float Player::GetNoiseFactor()
{
    return this->m_noiseFactor;
}

float Player::GetHealth()
{
    return this->m_health;
}

void Player::TakeDamage(const int damage)
{
    this->m_health -= damage;
}