/*
 * buttons.h
 *
 *  Created on: 12 lis 2020
 *      Author: polak
 */

#ifndef INC_BUTTONS_H_
#define INC_BUTTONS_H_

//#include "stm32l0xx_hal.h"
#include "main.h"

#define BUTTONS_COUNT 1
#define DEBOUNCE_TIME 60
#define LONG_PRESS_TIME 300

typedef struct button_struct button_struct;

struct button_struct
{
	GPIO_TypeDef * port;
	uint16_t pin;
	uint32_t time_pressed;

	struct
	{
		uint8_t pressed: 1;
		uint8_t pressed_long: 1;
		uint8_t released: 1;
		uint8_t press_handled: 1;
		uint8_t release_handled: 1;
		uint8_t long_push_handled: 1;
	};

	void (*press_callback) (void);
	void (*release_callback)(void);
	void (*long_press_callback)(void);
};

void buttons_detect(void);
void buttons_handle(void);
void handle_push_one_time(button_struct * this_button);
void handle_release_one_time(button_struct * this_button);
void press_action(button_struct * this_button);
void release_action(button_struct * this_button);
void long_push_action(button_struct * this_button);

#endif /* INC_BUTTONS_H_ */
