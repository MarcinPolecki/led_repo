/*
 * keyboard.c
 *
 *  Created on: Feb 2, 2021
 *      Author: polak
 */

#include "keyboard.h"

button_struct buttons[BUTTONS_COUNT] =
{
		//START
		{
				BUTTON_CHANGE_GPIO_Port,
				BUTTON_CHANGE_Pin,
				0,
				{
					0,
					0,
					0,
					0,
					0,
				},
				change_button_press,
				change_button_release,
				0
		}
};

extern uint8_t new_mode;
extern uint8_t ws2812b_mode;

void change_button_press(void)
{
	ws2812b_mode++;
	new_mode = 1;

	if (ws2812b_mode >= EFFECTS_COUNT)
	{
		ws2812b_mode = 0;
	}
}

void change_button_release(void)
{

}


