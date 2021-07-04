/*
 * ws2812b.c
 *
 *  Created on: 26 maj 2021
 *      Author: polak
 */

#include "ws2812b.h"


ws2812b_color ws2812b_array[WS2812B_LEDS];

volatile uint8_t busy_flag = 0;

uint8_t reset_pulse = 0;

uint16_t actual_led = 0;

volatile uint8_t led_buffer[48];

void turn_off_all_leds(void)
{
	/* USER CODE BEGIN 2 */
	//zgas wszystkie
	for (int i = 0; i < WS2812B_LEDS; i++)
	{
		set_led(i, 0, 0, 0);
	}
	update_led();
}

void set_led_rgb (uint16_t number, COLOR_RGB color)
{
	uint8_t red, green, blue;

	red = (uint8_t)(color >> 16);
	green = (uint8_t)(color >> 8);
	blue = (uint8_t)color;

	set_led(number, red, green, blue);
}

void set_led(uint16_t number, uint8_t red, uint8_t green, uint8_t blue)
{
	if (number < sizeof(ws2812b_array))
	{
		ws2812b_array[number].red = red;
		ws2812b_array[number].green = green;
		ws2812b_array[number].blue = blue;
	}
}

void walk_through_circle(uint16_t * h_col)
{
	//zmieniaj kolor po okregu
	if (*h_col < 359)
	{
		(*h_col)++;
	}
	else
	{
		*h_col = 0;
	}
}

void set_leds_colors(uint8_t led_count, uint8_t red_tmp, uint8_t green_tmp,  uint8_t blue_tmp)
{
	for (int i = 0; i < led_count; i++)
	{
		set_led(i, red_tmp, green_tmp, blue_tmp);
	}
}

void effect_360(uint8_t * init, uint8_t led_count, uint32_t speed)
{
	  static uint8_t red_tmp=0, green_tmp=0, blue_tmp=0;
	  static uint16_t h_col = 0;
	  static uint32_t effect_delay = 0;

	  if (*init)
	  {
		  *init = 0;
		  h_col = 0;
	  }

	  if ( HAL_GetTick() - effect_delay > speed )
	  {
		  effect_delay = HAL_GetTick();
		  hsv_to_rgb( h_col, 255, 255, &red_tmp, &green_tmp, &blue_tmp);
		  set_leds_colors(led_count, red_tmp, green_tmp, blue_tmp);
		  update_led();
		  walk_through_circle(&h_col);
	}
}

void read_rgb_color( COLOR_RGB color, int * target_red, int * target_green,  int * target_blue)
{
	*target_red = (uint8_t) (color >> 16);
	*target_green = (uint8_t) (color >> 8);
	*target_blue = (uint8_t) color;
}

void bright( float * red_tmp,  int * target_red,  float * red_step, float * green_tmp,  int * target_green,
		 float * green_step,  float * blue_tmp,  int * target_blue, float * blue_step,  uint8_t * direction)
{
	//zwiekszaj skladowe o krok
	if (*red_tmp < *target_red)
	{
		*red_tmp += *red_step;

		if (*red_tmp > *target_red)
		{
			*red_tmp = *target_red;
		}
	}
	if (*green_tmp < *target_green)
	{
		*green_tmp += *green_step;

		if (*green_tmp > *target_green)
		{
			*green_tmp = *target_green;
		}
	}
	if (*blue_tmp < *target_blue)
	{
		*blue_tmp += *blue_step;

		if (*blue_tmp > *target_blue)
		{
			*blue_tmp = *target_blue;
		}
	}
	//jesli cos juz dojechalo
	if ( (*red_tmp == *target_red) && (*green_tmp == *target_green) && (*blue_tmp == *target_blue) )
	{
		*red_tmp = *target_red;
		*green_tmp = *target_green;
		*blue_tmp = *target_blue;
		*direction = !(*direction);
	}
}

void dim( float * red_tmp,  float * red_step,  float * green_tmp, float * green_step,  float * blue_tmp,
		float * blue_step, uint8_t * direction )
{
	//zwiekszaj skladowe o krok
	if (*red_tmp > 0)
	{
		*red_tmp -= *red_step;

		if (*red_tmp < 0)
		{
			*red_tmp = 0;
		}
	}

	if (*green_tmp > 0)
	{
		*green_tmp -= *green_step;

		if (*green_tmp < 0)
		{
			*green_tmp = 0;
		}
	}

	if (*blue_tmp > 0)
	{
		*blue_tmp -= *blue_step;

		if (*blue_tmp < 0)
		{
			*blue_tmp = 0;
		}
	}
	//jesli cos juz dojechalo
	if ( (*red_tmp == 0) && (*green_tmp == 0) && (*blue_tmp == 0) )
	{
		*red_tmp = 0;
		*green_tmp = 0;
		*blue_tmp = 0;
		*direction = !(*direction);
	}
}

void effect_brightening(uint8_t * init, uint8_t led_count, COLOR_RGB color, uint32_t speed)
{
	static uint8_t direction = 0;
	static int target_red = 0, target_green = 0, target_blue = 0;
	static float red_tmp = 0, green_tmp = 0, blue_tmp = 0, red_step = 0, green_step = 0, blue_step = 0;
	static uint32_t effect_delay = 0;

	if (*init)
	{
		*init = 0;

		red_tmp = 0;
		green_tmp = 0;
		blue_tmp = 0;

		read_rgb_color(color, &target_red, &target_green, &target_blue);

		red_step = target_red / 100.0;
		green_step = target_green / 100.0;
		blue_step = target_blue / 100.0;
	}

	if ( HAL_GetTick() - effect_delay > speed )
	{
		effect_delay = HAL_GetTick();
		set_leds_colors(led_count, red_tmp, green_tmp, blue_tmp);
		update_led();

		if (!direction)
		{
			bright(&red_tmp, &target_red, &red_step, &green_tmp, &target_green,
					&green_step, &blue_tmp, &target_blue, &blue_step, &direction);
		}
		else //sciemniaj
		{
			dim(&red_tmp, &red_step, &green_tmp, &green_step, &blue_tmp, &blue_step, &direction);
		}
	}
}

void effect_four_pieces(uint8_t * init, uint8_t led_number, COLOR_RGB colors[4], uint32_t speed )
{
	static uint8_t red_tmp = 0, green_tmp = 0, blue_tmp = 0;
	static uint8_t step = 0;
	static uint32_t effect_delay = 0;

	if ( led_number > 3 )
	{
		int index = 0, limit = 0;

		//init
		if (*init)
		{
			*init = 0;

			uint8_t rest = led_number % 4;

			if (rest == 0)
			{
				step = led_number / 4;
			}
			else if ( ( rest == 1 ) || ( rest == 2 ) )
			{
				step = (uint8_t)floor(((double)led_number / 4.0));
			}
			else if ( rest == 3 )
			{
				step = (uint8_t)ceil(((double)led_number / 4.0));
			}

			//nadaj 4 kolory
			for (int i = 0; i < 4; i++)
			{
				if (i == 3)
				{
					limit = led_number;
				}
				else
				{
					limit = index + step;
				}

				for ( int j = index; j < limit; j++)
				{
					if (j < led_number)
					{
						red_tmp = (uint8_t)(colors[i] >> 16);
						green_tmp = (uint8_t)(colors[i] >> 8);
						blue_tmp = (uint8_t)colors[i];
						set_led( j, red_tmp, green_tmp, blue_tmp);
					}
				}
				index += step;
			}
		}

		if ( HAL_GetTick() - effect_delay > speed )
		{
			effect_delay = HAL_GetTick();

			//zaktualizuj
			update_led();

			//zachowaj ostatni led
			ws2812b_color tmp_led;
			tmp_led.red = ws2812b_array[led_number-1].red;
			tmp_led.green = ws2812b_array[led_number-1].green;
			tmp_led.blue = ws2812b_array[led_number-1].blue;

			int m = 0;

			for (m = led_number-1 ; m > 0; m--)
			{
				set_led( m, ws2812b_array[m-1].red, ws2812b_array[m-1].green, ws2812b_array[m-1].blue);
			}

			set_led( 0, tmp_led.red, tmp_led.green, tmp_led.blue);
		}
	}
}

void knightrider(uint8_t * init, uint8_t led_number, COLOR_RGB colors[3], uint32_t speed)
{
	static int color_1 = 0, ball = 0, color_2 = 0;
	static uint8_t direction = 0;
	static uint32_t effect_delay = 0;

	if ( led_number > 2 )
	{
		if (*init)
		{
			*init = 0;

			direction = 0;

			//nadaj 1 kolor dla wszystkich
			color_1 = 1;

			//ustaw indeks kulki na -1
			ball = 0;

			//ustaw indeks 2 koloru na -2
			color_2 = -1;

			//kulka na 0
			set_led_rgb( 0, colors[0]);

			//reszta na kolor
			for ( int i = color_1; i < led_number; i++ )
			{
				set_led_rgb( i, colors[1]);
			}
		}

		if ( HAL_GetTick() - effect_delay > speed )
		{
			effect_delay = HAL_GetTick();

			update_led();

			//kierunek 0
			if ( direction == 0 )
			{
				ball++;
				color_2++;
				color_1++;

				if ( ball >= (led_number - 1) )
				{
					direction = 1;
				}
			}
			else //kierunek 1
			{
				ball--;
				color_2--;
				color_1--;

				if ( ball <= 0 )
				{
					direction = 0;
				}
			}

			//wpisz kolor 2
			for ( int i = 0; i < color_2+1; i++ )
			{
				set_led_rgb( i, colors[2]);
			}

			//wpisz kulke
			set_led_rgb( ball, colors[0]);

			//wpisz kolor 1
			for ( int i = ball+1; i < led_number; i++ )
			{
				set_led_rgb( i, colors[1]);
			}
		}


	}

}

void update_led(void)
{
	while(busy_flag);
	busy_flag = 1;
	memset( (void*)led_buffer, 0, 48);
	actual_led = 0;
	reset_pulse = 1;
	pwm_dma_start();
}

void RGBtoHSV(uint8_t r, uint8_t g, uint8_t b, uint16_t *h, uint8_t *s, uint8_t *v)
{
    uint16_t min, max, delta;
    int16_t h_tmp = *h;

    min = r < g ? r : g;
    min = min  < b ? min : b;

    max = r > g ? r : g;
    max = max  > b ? max : b;

    *v = max;	// v
    delta = max - min;
    if (delta < 1)
    {
        *s = 0;
        *h = 0; // undefined, maybe nan?
        return;
    }
    if( max > 0 )
    { // NOTE: if Max is == 0, this divide would cause a crash
        *s = (((delta * 100) / max) * 255) / 100; // s
    }
    else
    {
        // if max is 0, then r = g = b = 0
        // s = 0, h is undefined
        *s = 0;
        *h = 0;                            // its now undefined
        return;
    }

    if( r == max )
    {
    	h_tmp = (( g - b )*100) / delta;        // between yellow & magenta
    											// *100 to avoid fracts
    }
    else
    {
		if( g == max )
		{
			h_tmp = 720 + (( b - r )*100) / delta;  // between cyan & yellow
		}
		else
		{
			h_tmp = 1440 + (( r - g )*100) / delta;  // between magenta & cyan
		}
    }
    h_tmp *= 60; // Degrees
    h_tmp /= 100; // Back from fracts

    if( h_tmp < 0.0 )
    	h_tmp += 360;

    *h = h_tmp;
}

void hsv_to_rgb(uint16_t h, uint8_t s, uint8_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
	uint16_t Sector, Fracts, p, q, t;

	if(s == 0)
	{
		*r = v;
		*g = v;
		*b = v;
	}
	else
	{
		if(h >= 360) h = 359;

		Sector = h / 60; // Sector 0 to 5
		Fracts = h % 60;
		p = (v * (255 - s)) / 256;
		q = (v * (255 - (s * Fracts)/60)) / 256;
		t = (v * (255 - (s * (59 - Fracts))/60)) / 256;

		switch(Sector)
		{
		case 0:
			*r = v;
			*g = (uint8_t)t;
			*b = (uint8_t)p;
			break;
		case 1:
			*r = (uint8_t)q;
			*g = v;
			*b = (uint8_t)p;
			break;
		case 2:
			*r = (uint8_t)p;
			*g = v;
			*b = (uint8_t)t;
			break;
		case 3:
			*r = (uint8_t)p;
			*g = (uint8_t)q;
			*b = v;
			break;
		case 4:
			*r = (uint8_t)t;
			*g = (uint8_t)p;
			*b = v;
			break;
		default:		// case 5:
			*r = v;
			*g = (uint8_t)p;
			*b = (uint8_t)q;
			break;
		}
	}
}
