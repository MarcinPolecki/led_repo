/*
 * ws2812b.h
 *
 *  Created on: 26 maj 2021
 *      Author: polak
 */

#ifndef INC_WS2812B_H_
#define INC_WS2812B_H_

#include "main.h"
#include "math.h"

#define zero 26
#define one 51
#define WS2812B_LEDS 5

typedef struct ws2812b_color
{
  uint8_t red, green, blue;
} ws2812b_color;

typedef enum
{
	RED = 0xFF0000,
	YELLOW = 0xFFFF00,
	BLUE = 0x0000FF,
	FUCHSIA = 0xFF00FF,
	PURPLE = 0x800080,
	GREEN = 0x008000,
	LIME = 0x00FF00,
	SILVER = 0xC0C0C0

}COLOR_RGB;

void RGBtoHSV(uint8_t r, uint8_t g, uint8_t b, uint16_t *h, uint8_t *s, uint8_t *v);
void hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b);
void turn_off_all_leds(void);
void set_led(uint16_t number, uint8_t red, uint8_t green, uint8_t blue);
void walk_through_circle(uint16_t * h_col);
void effect_360(uint8_t * init, uint8_t led_number, uint32_t speed);
void effect_brightening(uint8_t * init, uint8_t led_number, COLOR_RGB color, uint32_t speed);
void effect_four_pieces(uint8_t * init, uint8_t led_number, COLOR_RGB colors[4], uint32_t speed );
void knightrider(uint8_t * init, uint8_t led_number, COLOR_RGB colors[3], uint32_t speed);
void update_led(void);

#endif /* INC_WS2812B_H_ */
