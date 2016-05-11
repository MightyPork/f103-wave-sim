#include "colorled.h"
#include "utils/timebase.h"

#define LONG_DELAY() for (volatile uint32_t __j = 4; __j > 0; __j--)
#define SHORT_DELAY() for (volatile uint32_t __j = 1; __j > 0; __j--)

static inline
__attribute__((always_inline))
void colorled_byte(uint8_t b)
{
	for (register volatile uint8_t i = 0; i < 8; i++) {
		COLORLED_GPIO->BSRR = COLORLED_PIN; // set pin high

		// duty cycle determines bit value
		if (b & 0x80) {
			LONG_DELAY();
			COLORLED_GPIO->BRR = COLORLED_PIN; // set pin low
			SHORT_DELAY();
		} else {
			SHORT_DELAY();
			COLORLED_GPIO->BRR = COLORLED_PIN; // set pin low
			LONG_DELAY();
		}

		b <<= 1; // shift to next bit
	}
}


/** Set one RGB LED color */
void colorled_set(uint32_t rgb)
{
	__disable_irq(); // SysTick interrupt when sending data would break the timing

	colorled_byte((rgb & 0x00FF00) >> 8);
	colorled_byte((rgb & 0xFF0000) >> 16);
	colorled_byte(rgb & 0x0000FF);

	__enable_irq();

	delay_us(50); // show
}


/** Set many RGBs */
void colorled_set_many(uint32_t *rgbs, int count)
{
	__disable_irq();

	for (int i = 0; i < count; i++) {
		uint32_t rgb = *rgbs++;
		colorled_byte((rgb & 0x00FF00) >> 8);
		colorled_byte((rgb & 0xFF0000) >> 16);
		colorled_byte(rgb & 0x0000FF);
	}

	__enable_irq();

	delay_us(50); // show
}


void colorled_off(void)
{
	colorled_set(RGB_BLACK);
}
