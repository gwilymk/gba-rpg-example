#include <lostgba/Input.h>

// Note that the GBA has a 1 at the bit position for not pressed and 0 for pressed
vu16 *Input_keyInputRegister = (vu16 *)0x04000130;

// These buffers store ~Input_keyInputRegister to make checking pressed state sane
static u16 Input_previousKeyBuffer = 0;
static u16 Input_keyBuffer = 0;

void Input_UpdateKeyState(void)
{
    Input_previousKeyBuffer = Input_keyBuffer;
    Input_keyBuffer = ~(*Input_keyInputRegister);
}

bool Input_IsKeyDown(enum InputKey key)
{
    return Input_keyBuffer & (1 << key);
}

bool Input_IsNewlyPressed(enum InputKey key)
{
    u16 keyMask = 1 << key;
    return !(Input_previousKeyBuffer & keyMask) && (Input_keyBuffer & keyMask);
}