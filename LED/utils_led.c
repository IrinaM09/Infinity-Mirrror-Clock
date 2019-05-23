#include "utils_led.h"
#include "../RTC/i2c.h"
#include "../RTC/rtc.h"

uchar ZERO = 0;
uchar HALF = 127;
uchar FULL = 255;

/* RBG colors array */
RGB colors[LED_COUNT];

/* RGB constant colors */
RGB blue = { ZERO, ZERO, FULL };
RGB indigo = { 39, ZERO, 51 };
RGB violet = { 139, ZERO, FULL };
RGB red = { FULL, ZERO, ZERO };
RGB orange = { FULL, HALF, ZERO };
RGB yellow = { FULL, FULL, ZERO };
RGB green = { ZERO, FULL, ZERO };
RGB black = { ZERO, ZERO, ZERO };
RGB white = { FULL, FULL, FULL };

/** Send a series of colors to the LED strip.
 *  This function takes about 1.1 ms to update 30 LEDs.
 */
void __attribute__((noinline)) led_strip_write(RGB *colors, unsigned int count) 
{
  LED_STRIP_PORT &= ~(1<<LED_STRIP_PIN);
  LED_STRIP_DDR |= (1<<LED_STRIP_PIN);

  cli();
  while(count--)
  {
    asm volatile(
        "ld __tmp_reg__, %a0+\n"
        "ld __tmp_reg__, %a0\n"
        "rcall send_led_strip_byte%=\n"  // Send red.
        "ld __tmp_reg__, -%a0\n"
        "rcall send_led_strip_byte%=\n"  // Send green.
        "ld __tmp_reg__, %a0+\n"
        "ld __tmp_reg__, %a0+\n"
        "ld __tmp_reg__, %a0+\n"
        "rcall send_led_strip_byte%=\n"  // Send blue.
        "rjmp led_strip_asm_end%=\n"

        // Sends a byte to the LED strip.
        "send_led_strip_byte%=:\n"
        "rcall send_led_strip_bit%=\n" //MSB
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n"
        "rcall send_led_strip_bit%=\n" //LSB
        "ret\n"

        
        "send_led_strip_bit%=:\n"
#if F_CPU == 8000000
        "rol __tmp_reg__\n"                      // Rotate left through carry.
#endif
        "sbi %2, %3\n"                           // Drive the line high.

#if F_CPU != 8000000
        "rol __tmp_reg__\n"                      // Rotate left through carry.
#endif

#if F_CPU == 16000000
        "nop\n" "nop\n"
#elif F_CPU == 20000000
        "nop\n" "nop\n" "nop\n" "nop\n"
#elif F_CPU != 8000000
#error "Unsupported F_CPU"
#endif

        "brcs .+2\n" "cbi %2, %3\n"

#if F_CPU == 8000000
        "nop\n" "nop\n"
#elif F_CPU == 16000000
        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
#elif F_CPU == 20000000
        "nop\n" "nop\n" "nop\n" "nop\n" "nop\n"
        "nop\n" "nop\n"
#endif

        "brcc .+2\n" "cbi %2, %3\n"              

        "ret\n"
        "led_strip_asm_end%=: "
        : "=b" (colors)
        : "0" (colors),         
          "I" (_SFR_IO_ADDR(LED_STRIP_PORT)),
          "I" (LED_STRIP_PIN)
    );
  }
  sei();          
  _delay_us(80);
}

/* -----------------------------------------------------------------------------
 *                               LEDs FUNCTIONS
 *-----------------------------------------------------------------------------*/

RGB random_colors() {
    RGB color;
    color.r = rand() % 255;
    color.g = rand() % 255;
    color.b = rand() % 255;

    return color;
}

/* Display a rainbow wheel on RGB LED strip */
void rainbow() {
  unsigned int i;
  uchar fade = 0;

  // assign color to LEDs
  for(i = 0; i <= LED_COUNT; i++)
  {
    // blue
    if (i < 7) {
        colors[i] = blue;
        blue.r += fade/3;
        blue.b -= fade/3;
    }
    // indigo
    if(i >= 7 && i <= 13) {
        colors[i] = indigo;
        indigo.r += fade/3;
        indigo.b += fade/3;
    }
    // violet
    if(i >= 14 && i <= 20) {
        colors[i] = violet;
        violet.r += fade/3;
        violet.b -= fade/3;
    }
    // red
    if(i >= 21 && i <= 27) {
        colors[i] = red;
        red.g += fade/3;
    }
    // orange
    if(i >= 28 && i <= 34) {
        colors[i] = orange;
        orange.g += fade;
    }
    // yellow
    if(i >= 35 && i <= 41) {
        colors[i] = yellow;
        yellow.r -= fade;
    }
    // green
    if(i >= 42 && i <= LED_COUNT) {
        colors[i] = green;
        green.g -= fade;
        green.b += fade;
   
    }
    fade = (fade + 1) % 4;
  }

  // send data
  led_strip_write(colors, LED_COUNT);
}

/* Display all LEDs with the same color */
void set_color(RGB color) {
    unsigned int i = 0;
    
    for (i = 0; i < LED_COUNT; i++) {
        colors[i] = color;
    }

    // send data
    led_strip_write(colors, LED_COUNT);
}
 
/* Display LEDs with a random background color
 * and some LEDs will blink with a random color.
 * Repeat for a random number of times < 5.
 */
void blink(int blink_count) {
    int i = 0;
    
    // Set a random background color
    RGB color = random_colors();
    set_color(color);

    while (blink_count-- > 0) {
        // Set a random color for blink LEDs
        RGB interior_color = random_colors();

        // Assign color
        for (i = 0; i < LED_COUNT; i++) {
            if (i % 5 == 0) {
                    colors[i] = interior_color;
            }

            // Send data
            led_strip_write(colors, LED_COUNT);
        }
        _delay_ms(500);
    }
}

/* Display a band of LEDs with the given radius that has
 * a background color and a color that moves forth.
 */
void forth(int led_id, int radius, RGB after, RGB before) {
    int i = 0;

    // Turn off LEDs
    set_color(black);

    for (i = led_id; i < led_id + radius; i++) {
        if (i < LED_COUNT) {
            colors[i] = after;
        }

        led_strip_write(colors, LED_COUNT);
        _delay_ms(100);

        colors[i] = before;
    }
}

/* Move a random length forwards and then backwards.
 * The colors assigned are random. 
 */
void back_and_forth(int stop) {
    int i = 0;
    int j = 0;
    // Turn off LEDs
    set_color(black);

    RGB forward_color = random_colors();
    RGB backwards_color = random_colors();

    while(1) {
        colors[i] = forward_color;
        i++;

        if(i == stop) {
            for (j = LED_COUNT; j >= 0; j--) { 
                colors[j] = backwards_color;
                led_strip_write(colors, LED_COUNT);
                _delay_ms(50);
            }
            break;
        }
        
        led_strip_write(colors, LED_COUNT);
        _delay_ms(100);
    }
}

void gradient_wheel() {
    int i = 0;
    int j = LED_COUNT;

    // Turn off LEDs
    set_color(black);

    RGB rand_color = random_colors();

    while (1) {
        if (i >= 24 && j <= 24) {
            break;
        }
        colors[i] = ((i % 2 == 0) || i >= 21) ? white : rand_color;
        colors[j] = ((j % 2 == 0) || j <= 27) ? white : rand_color;
        led_strip_write(colors, LED_COUNT);
        _delay_ms(100);
        i++;
        j--;
    }
}

/**
 * Choose random one of the LEDs display functions
 */
void color_leds(int *type) {
    static int function = 0;

    switch(function) {
        case 0: {
            rainbow();
            function = 1;
            break;
        }
        case 1: {
            blink(rand() % 5);
            function = 2;
            break;
        }
        case 2: {
            RGB after, before;
            after = random_colors();
            before = random_colors();
            int start = 0;
            int radius = rand() % 15;

            forth(start, radius, after, before);

            after = random_colors();
            start = (radius + rand() % 10) % 20;
            radius = (start + rand() % 10) % 30;
            forth(start, radius, after, before);

            after = random_colors();
            start = (radius + rand() % 10) % 35;
            radius = (start + rand() % 10) % LED_COUNT;
            forth(start, radius, after, before);
            function = 3;
            break;
        }
        case 3: {
            int random_stop = rand() % LED_COUNT;
            back_and_forth(random_stop);
            function = 4;
            break;
        }
        case 4: {
            gradient_wheel();
            function = 0;
            break;
        }
        default: {
            gradient_wheel();
        };
    }
    _delay_ms(3000);

    *type = 0;
     _delay_ms(300);
    return;
}

/* -----------------------------------------------------------------------------
 *                               CLOCK FUNCTIONS
 *-----------------------------------------------------------------------------*/

/**
 * Display a clock using LEDs.
 * Current time is given by RTC.
 */
void clock_time_color(RGB screen, RGB sec_color, RGB minute_color, RGB hour_color) {
    uint8_t sec, min, hour;
    
    /* Initialize RTC */
    RTC_Init();

   // RTC_SetTime(0x19, 0x00, 0x00);

    /* Read the Time from RTC */ 
    RTC_GetTime(&hour, &min, &sec);      
    
    // Set screen color
    set_clock_screen_color(screen, sec, min, hour);

    // Set seconds
    if (hex2dec(sec) > LED_COUNT && hex2dec(sec) % 2 == 0)
        colors[0] = black;
    else
        colors[0] = sec_color;

    colors[hex2dec(sec) % LED_COUNT] = sec_color;

    // Set hour 
    if (hour != 0x06 && hour != 0x12)
        colors[hour2led(hour) % LED_COUNT] = hour_color;
    
    // Set minute 
    colors[min2led(min)] = minute_color;

    // Send data
    led_strip_write(colors, LED_COUNT);

    colors[hex2dec(sec) % LED_COUNT] = screen;

}

/*
 * Set the clock background color except the LEDs for hour, min, sec.
 */
void set_clock_screen_color(RGB color, uchar sec, uchar min, uchar hour) {
    int i = 0;
    
    for (i = 0; i < LED_COUNT; i++) {
        if (i != hex2dec(sec) && i != min2led(min) && i != hour2led(hour))
            colors[i] = color;
    }

    // send data
    led_strip_write(colors, LED_COUNT);   
}

/**
 * Map current minute to a LED.
 */
int min2led(uchar val) {
    int dec = 0;

    dec = hex2dec(val);

    // 0-5 minutes => between [12 - 1]
    if (dec >= 0 && dec < 5) return 24 - dec;
    if (dec == 5) return 20;
    
    // 5-10 minutes => between [1 - 2]
    if (dec >= 6 && dec < 9) return 20 + (6 - dec - 1) ;
    if (dec == 9 || dec == 10) return 16;

    // 10-15 minutes => between [2 - 3]
    if (dec >= 11 && dec < 15) return 16 + (11 - dec - 1);
    if (dec == 14 || dec == 15) return 12;

    // 15-20 minutes => between [3 - 4]
    if (dec >= 16 && dec < 20) return 12 + (16 - dec - 1);
    if (dec == 20) return 7;

    // 20-25 minutes => between [4 - 5]
    if (dec >= 21 && dec < 25) return 7 + (21 - dec - 1);
    if (dec == 24 || dec == 25) return 3;

    // 25-30 minutes => between [5 - 6]
    if (dec >= 26 && dec < 30) return 3 + (26 - dec - 1);
    if (dec == 29 || dec == 30) return 0;

    // 30-35 minutes => between [6 - 7]
    if (dec >= 31 && dec < 35) return 48 + (31 - dec);
    if (dec == 34 || dec == 35) return 45;

    // 35-40 minutes => between [7 - 8]
    if (dec >= 36 && dec < 40) return 45 + (36 - dec - 1);
    if (dec == 39 || dec == 40) return 41;

    // 40-45 minutes => between [8 - 9]
    if (dec >= 41 && dec < 45) return 41 + (41 - dec - 1);
    if (dec == 45) return 36; 

    // 45-50 minutes => between [9 - 10]
    if (dec >= 46 && dec < 50) return 36 + (46 - dec - 1);
    if (dec == 49 || dec == 50) return 32;

    // 50-55 minutes => between [10 - 11]
    if (dec >= 51 && dec < 55) return 32 + (51 - dec - 1);
    if (dec == 54 || dec == 55) return 28;

    // 55-60 minutes => between [11 - 12]
    if (dec >= 56 && dec < 60) return 28 + (56 - dec - 1);
    if (dec == 59 || dec == 60) return 24;

    return 0;
}

/**
 * Map current hour to a LED.
 */
int hour2led(uchar val) {
    int dec = 0;

    dec = hex2dec(val);

    if (dec == 5 || dec == 17) return 3;   //  5 AM | 17 PM
    if (dec == 4 || dec == 16) return 7;   //  4 AM | 16 PM
    if (dec == 3 || dec == 15) return 12;  //  3 AM | 15 PM
    if (dec == 2 || dec == 14) return 16;  //  2 AM | 14 PM
    if (dec == 1 || dec == 13) return 20;  //  1 AM | 13 PM
    if (dec == 12 || dec == 0) return 24;  // 12 AM | 00 PM
    if (dec == 11 || dec == 23) return 28; // 11 AM | 23 PM
    if (dec == 10 || dec == 22) return 32; // 10 AM | 22 PM
    if (dec == 9 || dec == 21) return 36;  //  9 AM | 21 PM
    if (dec == 8 || dec == 20) return 41;  //  8 AM | 20 PM
    if (dec == 7 || dec == 19) return 45;  //  7 AM | 19 PM
    
    return 0;
}

/**
 * Get integer value from hex.
 */
int hex2dec(uchar val) {
    if (val == 0x00) return 0; if (val == 0x01) return 1;
    if (val == 0x02) return 2; if (val == 0x03) return 3;
    if (val == 0x04) return 4; if (val == 0x05) return 5;
    if (val == 0x06) return 6; if (val == 0x07) return 7;
    if (val == 0x08) return 8; if (val == 0x09) return 9;
    if (val == 0x10) return 10; if (val == 0x11) return 11;
    if (val == 0x12) return 12; if (val == 0x13) return 13;
    if (val == 0x14) return 14; if (val == 0x15) return 15;
    if (val == 0x16) return 16; if (val == 0x17) return 17;
    if (val == 0x18) return 18; if (val == 0x19) return 19;
    if (val == 0x20) return 20; if (val == 0x21) return 21;
    if (val == 0x22) return 22; if (val == 0x23) return 23;
    if (val == 0x24) return 24; if (val == 0x25) return 25;
    if (val == 0x26) return 26; if (val == 0x27) return 27;
    if (val == 0x28) return 28; if (val == 0x29) return 29;
    if (val == 0x30) return 30; if (val == 0x31) return 31;
    if (val == 0x32) return 32; if (val == 0x33) return 33;
    if (val == 0x34) return 34; if (val == 0x35) return 35;
    if (val == 0x36) return 36; if (val == 0x37) return 37;
    if (val == 0x38) return 38; if (val == 0x39) return 39;
    if (val == 0x40) return 40; if (val == 0x41) return 41;
    if (val == 0x42) return 42; if (val == 0x43) return 43;
    if (val == 0x44) return 44; if (val == 0x45) return 45;
    if (val == 0x46) return 46; if (val == 0x47) return 47;
    if (val == 0x48) return 48; if (val >= 0x49) return LED_COUNT;

    return 0;
}