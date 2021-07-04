/*
 * buttons.c
 *
 *  Created on: 12 lis 2020
 *      Author: polak
 */

#include "buttons.h"

extern button_struct buttons[BUTTONS_COUNT];


//sprawdzanie co zostalo wcisniete - wykonywane jest w przerwanie systick 1ms
void buttons_detect(void)
{
	  for(int cnt = 0; cnt < BUTTONS_COUNT; cnt++)
	  {
		  if ( !HAL_GPIO_ReadPin(buttons[cnt].port,buttons[cnt].pin) )
		  {
			  buttons[cnt].time_pressed++;

			  if (!buttons[cnt].pressed)
			  {
				  if ( buttons[cnt].time_pressed > DEBOUNCE_TIME )
				  {
					  buttons[cnt].pressed = 1;
					  buttons[cnt].released = 0;
					  buttons[cnt].pressed_long = 0;

					  buttons[cnt].press_handled = 0;
				  }
			  }
			  else
			  {
				  if( buttons[cnt].time_pressed > LONG_PRESS_TIME )
				  {
					  buttons[cnt].time_pressed = 0;

					  //buttons[cnt].pressed = 0;
					  buttons[cnt].pressed_long = 1;
					  buttons[cnt].released = 0;

					  buttons[cnt].long_push_handled = 0;
				  }
			  }
		  }
		  else
		  {
			  buttons[cnt].time_pressed = 0;

			  if (buttons[cnt].pressed)
			  {
				  buttons[cnt].pressed = 0;
				  buttons[cnt].pressed_long = 0;
				  buttons[cnt].released = 1;

				  buttons[cnt].release_handled = 0;
			  }
		  }
	  }

}

void buttons_handle(void)
{
	for (int cnt = 0; cnt < BUTTONS_COUNT; cnt++)
	{
		if(buttons[cnt].pressed_long)
		{
			long_push_action(&buttons[cnt]);
		}
		else if (buttons[cnt].pressed)
		{
			press_action(&buttons[cnt]);
		}
		else if(buttons[cnt].released)
		{
			release_action(&buttons[cnt]);
		}
	}
}

void handle_push_one_time(button_struct * this_button)
{
	if (!(this_button->press_handled) )
	{
		this_button->press_handled = 1;
	}
}

void handle_release_one_time(button_struct * this_button)
{
	if (!(this_button->release_handled) )
	{
		this_button->release_handled = 1;
	}
}

void handle_long_push(button_struct * this_button)
{
	if (!(this_button->long_push_handled))
	{
		this_button->long_push_handled = 1;
	}
}

// ---------------------- PRESS ----------------------------
void press_action(button_struct * this_button)
{
	if (!(this_button->press_handled) )
	{
		this_button->press_callback();
	}

	handle_push_one_time(this_button);
}


// ----------------- RELEASE ---------------------
void release_action(button_struct * this_button)
{
	if (!(this_button->release_handled) )
	{
		this_button->release_callback();
	}

	handle_release_one_time(this_button);
}


// ----------------- LONG PRESS ---------------------
void long_push_action(button_struct * this_button)
{
	if (!(this_button->long_push_handled))
	{
		if (this_button->long_press_callback != NULL)
		{
			this_button->long_press_callback();
		}
	}

	handle_long_push(this_button);
}


