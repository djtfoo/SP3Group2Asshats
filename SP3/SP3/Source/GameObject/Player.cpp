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

Player::Player() : d_invulnerabilityTime(0.2)
{
    m_position.Set(Scene::tileSize, 0.f, Scene::tileSize);
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
    b_dead = false;

    PlayerHitBox.m_origin = m_position;
    PlayerHitBox.m_scale = Vector3(5, 5, 5);

    m_movementState = MOVEMENT_STATE_IDLE;
    m_heightState = HEIGHT_STATE_STANDING;

    Item tempInventory[Item::NUM_TYPE] =
    {
        Item("Net", Item::TYPE_NET, 10, 100, 0),
        Item("Bait", Item::TYPE_BAIT, 1, 100, 0),
        Item("Meat", Item::TYPE_MEAT, 10, 100, 10),
        Item("Trap", Item::TYPE_TRAP, 10, 100, 10),
        Item("Rock", Item::TYPE_ROCK, 10, 100, 10)
    };

    for (unsigned i = 0; i < Item::NUM_TYPE; ++i)
    {
        inventory[i] = tempInventory[i];
        switch (inventory[i].GetItemType())
        {
        case Item::TYPE_NET:
        case Item::TYPE_BAIT:
        case Item::TYPE_TRAP:
        case Item::TYPE_ROCK:
            inventory[i].Add(50);
            break;

        default:
            inventory[i].Add(0);
            break;
        }
    }

    d_damageTimer = d_invulnerabilityTime;

    m_currency = 1000000;
}

Player::~Player()
{
}

void Player::ResetPlayer()
{
    m_position.Set(Scene::tileSize, 0.f, Scene::tileSize);
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
    b_dead = false;

    PlayerHitBox.m_origin = m_position;
    PlayerHitBox.m_scale = Vector3(10, 5, 10);

    m_movementState = MOVEMENT_STATE_IDLE;
    m_heightState = HEIGHT_STATE_STANDING;

    d_damageTimer = d_invulnerabilityTime;
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

Vector3 Player::GetVelocityVector()
{
    return m_velocity;
}

static const float CAMERA_SPEED = 5.0f;

void Player::Update(double dt)
{
    // timer
    d_damageTimer += dt;

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

    //Move(dt);

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

void Player::Move(const double dt)
{
    m_position += m_velocity * m_speed * (float)dt;
    //std::cout << m_movementState << " | " << m_heightState  << " | " << m_speed << std::endl;
    //std::cout << m_position << std::endl;
}

void Player::UpdatePlayerHeight(const double dt)
{
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

    PlayerHitBox.m_origin = m_position;
}

void Player::UpdateNoiseFactor()
{
    // UPDATE NOISE FACTOR
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

bool Player::IsJumping()
{
    if (m_heightState == HEIGHT_STATE_JUMP)
        return true;

    return false;
}

void Player::SetMudSlow()
{
    m_speed /= 2.f;
}

void Player::SetLavaDamage()
{
	TakeDamage(1);
}

float Player::GetHealth()
{
    return this->m_health;
}

void Player::TakeDamage(const int damage)
{
    if (d_damageTimer >= d_invulnerabilityTime)
    {
        this->m_health -= damage;
		this->m_health = Math::Max(0.f, m_health);

        if (m_health == 0.f)
        {
            b_dead = true;
            SharedData::GetInstance()->sound->PlaySoundEffect("Sound//PlayerFainted.wav");
        }
        else
        {
            SharedData::GetInstance()->sound->PlaySoundEffect("Sound//PlayerDamaged.wav");
        }

        d_damageTimer = 0.0;
    }
}

void Player::ClearCapturedMonsters()
{
    if (!capturedMonstersList.empty())
    {
        if (SharedData::GetInstance()->sceneManager->GetGameState() == SceneManager::GAMESTATE_GAMEPLAY && m_health > 0.f)
        {   //player is alive & exiting to town
            for (unsigned i = 0; i < capturedMonstersList.size(); ++i)
            {
                monsterList.push_back(capturedMonstersList[i]);
            }
        }

        capturedMonstersList.clear();
    }
}

bool Player::IsInvulnerable()
{
    if (d_damageTimer < d_invulnerabilityTime)
        return true;
    
    return false;
}

bool Player::IsDead()
{
    if (b_dead)
        return true;

    return false;
}

int Player::GetCapturedQuantity(std::string name)
{
    int count = 0;
    for (unsigned i = 0; i < capturedMonstersList.size(); ++i)
    {
        if (capturedMonstersList[i] == name)
            ++count;
    }

    return count;
}