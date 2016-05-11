#pragma once
#include "main.h"
#include "utils/timebase.h"

// Debouncer requires that you setup SysTick first.

/** Debounced pin ID - used for state readout */
typedef uint32_t debo_id_t;

/** debo_id_t indicating unused slot */
#define DEBO_PIN_NONE 0


/**
 * @brief Initialize the debouncer.
 *
 * You have to also register the periodic task to timebase.
 *
 * @param pin_count : number of pin slots to allocate
 */
void debounce_init(size_t pin_count);


/**
 * @brief 1 ms periodic callback for debouncer. Must be registered to timebase.
 */
void debo_periodic_task(void);


typedef struct {
	GPIO_TypeDef *GPIOx;		///< GPIO base
	uint16_t pin;				///< pin mask
	ms_time_t debo_time;		///< debounce time in ms, 0 = default (20 ms)
	bool invert;				///< invert value read from GPIO (button to ground)
	void (*rising_cb)(void);	///< callback when the pin goes HIGH
	void (*falling_cb)(void);	///< callback when the pin goes LOW
} debo_init_t;


/**
 * @brief Add a pin for debouncing.
 *
 * The pin state will be checked with the configured hysteresis
 * and callbacks will be called when a state change is detected.
 */
debo_id_t debo_register_pin(debo_init_t *init_struct);


/**
 * @brief Check if a pin is high
 * @param pin_id : Slot ID
 * @return true if the pin is registered and is HIGH
 */
bool debo_pin_state(debo_id_t pin_id);


/**
 * @brief Remove a pin entry from the debouncer.
 * @param pin_id : Slot ID
 * @return true if task found & removed.
 */
bool debo_remove_pin(debo_id_t pin_id);
