#ifndef _UTILS_LED_H_
#define _UTILS_LED_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LED_COUNT 49
#define LED_STRIP_PORT PORTB
#define LED_STRIP_DDR  DDRB
#define LED_STRIP_PIN  0

typedef unsigned char uchar;

typedef struct RGB
{
  uchar r, g, b;
} RGB;

/* constant values */
extern uchar ZERO;
extern uchar HALF;
extern uchar FULL;

/* RBG colors array */
extern RGB colors[LED_COUNT];

/* RGB constant colors */
extern RGB blue;
extern RGB indigo;
extern RGB violet;
extern RGB red;
extern RGB orange;
extern RGB yellow;
extern RGB green;
extern RGB black;
extern RGB white;

void __attribute__((noinline)) led_strip_write(RGB *colors, unsigned int count);
void rainbow();
void blink(int blink_count);
void forth(int led_id, int radius, RGB after, RGB before);
void color_leds(int *type);
void gradient_wheel();
void back_and_forth(int stop);
RGB random_colors();

void set_color(RGB color);
void clock_time_color(RGB screen, RGB sec_color, RGB minute_color, RGB hour_color);
int hex2dec(uchar value);
int hour2led(uchar value);
int min2led(uchar value);
void set_clock_screen_color(RGB color, uchar sec, uchar min, uchar hour);
#endif