#pragma once

/* Includes ------------------------------------------------------------------*/

#include "main.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

// PB8 - WS2812B data line
#define COLORLED_GPIO GPIOB
#define COLORLED_PIN GPIO_Pin_12

#define RGB_RED     rgb(255,   0,   0)
#define RGB_ORANGE  rgb(255, 110,   0)
#define RGB_YELLOW  rgb(255, 255,   0)
#define RGB_LIME    rgb(160, 255,   0)
#define RGB_GREEN   rgb(  0, 255,   0)
#define RGB_CYAN    rgb(  0, 255, 120)
#define RGB_BLUE    rgb(  0,   0, 255)
#define RGB_MAGENTA rgb(255,   0, 255)
#define RGB_WHITE   rgb(255, 255, 255)
#define RGB_BLACK   rgb(  0,   0,   0)

/**
 * @brief Struct for easy manipulation of RGB colors.
 *
 * Set components in the xrgb.r (etc.) and you will get
 * the hex in xrgb.num.
 */
typedef union {

	/** Struct for access to individual color components */
	struct __attribute__((packed)) {
		uint8_t b;
		uint8_t g;
		uint8_t r;
	};

	/** RGB color as a single uint32_t */
	uint32_t num;

} ws2812_rgb_t;

/* Exported macros -----------------------------------------------------------*/

/**
 * @brief Compose an RGB color.
 * @param r, g, b - components 0xFF
 * @returns integer 0xRRGGBB
 */
#define rgb(r, g, b) (((0xFF & (r)) << 16) | ((0xFF & (g)) << 8) | (0xFF & (b)))

/* Get components */
#define rgb_r(rgb) (((rgb) >> 16) & 0xFF)
#define rgb_g(rgb) (((rgb) >> 8) & 0xFF)
#define rgb_b(rgb) ((rgb) & 0xFF)

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Turn OFF the rgb LED
 */
void colorled_off(void);


/**
 * @brief Set color of a WS2812B
 * @param rgb - color 0xRRGGBB
 */
void colorled_set(uint32_t rgb);


/**
 * @brief Set color of multiple chained RGB leds
 * @param rgbs - array of colors (0xRRGGBB)
 * @param count - number of LEDs
 */
void colorled_set_many(uint32_t *rgbs, int count);
