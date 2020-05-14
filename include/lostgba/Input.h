/**
 * @file Input.h
 * @brief Handle keyboard input
 * 
 * @defgroup INPUT Input management
 * @{
 */

#pragma once

#include "GbaTypes.h"

/** Each of the 10 keys on the GBA */
enum InputKey
{
    InputKey_A,      /**< A button */
    InputKey_B,      /**< B button */
    InputKey_Select, /**< Select button */
    InputKey_Start,  /**< Start button */
    InputKey_Right,  /**< Right D-Pad */
    InputKey_Left,   /**< Left D-Pad */
    InputKey_Up,     /**< Up D-Pad */
    InputKey_Down,   /**< Down D-Pad */
    InputKey_R,      /**< R button */
    InputKey_L       /**< L button */
};

/** Updates the internal key state to ensure that all key press answers are consistent */
void Input_UpdateKeyState(void);

/** Whether a given key is pressed (at the time the last call to Input_UpdateKeyState was called) */
bool Input_IsKeyDown(enum InputKey key);

/** Whether a given key is newly pressed since the last time Input_UpdateKeyState was called */
bool Input_IsNewlyPressed(enum InputKey key);

/** @} */