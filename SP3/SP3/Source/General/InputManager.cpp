#include "InputManager.h"

InputManager::InputManager()
{
    for (unsigned i = 0; i < NUM_KEYS; ++i)
    {
        keyState[i].state = false;
        keyState[i].isReleased = false;
        keyState[i].isHeldDown = false;
        keyState[i].isPressed = false;
    }

    keyState[KEY_SPACE].Assign(VK_SPACE);
    keyState[KEY_ENTER].Assign(VK_RETURN);

    keyState[KEY_W].Assign('W');
    keyState[KEY_A].Assign('A');
    keyState[KEY_S].Assign('S');
    keyState[KEY_D].Assign('D');

    keyState[KEY_Q].Assign('Q');
    keyState[KEY_E].Assign('E');

    keyState[KEY_G].Assign('G');
    keyState[KEY_X].Assign('X');

    keyState[KEY_C].Assign('C');    // temp debug keys

    keyState[KEY_CTRL].Assign(VK_CONTROL);
    keyState[KEY_SHIFT].Assign(VK_SHIFT);

	keyState[MOUSE_L].Assign(VK_LBUTTON);

    keyState[KEY_1].Assign('1');
    keyState[KEY_2].Assign('2');
    keyState[KEY_3].Assign('3');
    keyState[KEY_4].Assign('4');
    keyState[KEY_5].Assign('5');
    keyState[KEY_6].Assign('6');

    keyState[KEY_UP].Assign(VK_UP);
    keyState[KEY_DOWN].Assign(VK_DOWN);
    keyState[KEY_LEFT].Assign(VK_LEFT);
    keyState[KEY_RIGHT].Assign(VK_RIGHT);

    keyState[KEY_TAB].Assign(VK_TAB);
}


InputManager::~InputManager()
{
}


void InputManager::Update()
{
    for (unsigned i = 0; i < NUM_KEYS; ++i)
    {
        keyState[i].Update();
    }
}


void Key::Assign(unsigned short vk_key)
{
    this->value = vk_key;
}


void Key::Update()
{
    isPressed = false;
    isReleased = false;

    if (isHeldDown && (GetAsyncKeyState(value) & 0x8000) != 0)
    {
        isHeldDown = true;
    }
    else if (isHeldDown && !(GetAsyncKeyState(value) & 0x8000) != 0)
    {   
        isHeldDown = false;
        isReleased = true;
        isPressed = false;
    }

    if (!state && (GetAsyncKeyState(value) & 0x8000) != 0)
    {
        isPressed = true;
        isHeldDown = true;
        state = true;
    }
    else if (state && !(GetAsyncKeyState(value) & 0x8000) != 0)
    {
        state = false;
    }
}