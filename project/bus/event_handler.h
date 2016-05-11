#pragma once
#include "main.h"
#include "event_queue.h"

typedef bool (*EventHandlerCallback) (uint32_t hdlr_id, Event *evt, void **user_data);

/**
 * @brief Register an event handler for event type
 * @param type    : handled event type
 * @param handler : the handler func
 * @return handler ID. Can be used to remove the handler.
 */
uint32_t register_event_handler(EventType type, EventHandlerCallback handler, void *user_data);

/**
 * @brief Remove event handler by handler ID
 * @param handler_id : handler ID, obtained when registering or in the callback.
 * @return number of removed handlers
 */
int remove_event_handler(uint32_t handler_id);

/**
 * @brief Handle an event
 * @param event : pointer to the event to handle
 */
void run_event_handler(Event *event);

/**
 * @brief Check if hansler exists
 * @param handler_id : handler
 * @return exists
 */
bool event_handler_exists(uint32_t handler_id);

/**
 * @brief Create a link between two handlers (one direction).
 *
 * If handler A is linked to handler B, and handler A is removed,
 * both handlers will perish.
 *
 * Make a circle if you need to chain more than two handlers.
 *
 * @param from : handler A
 * @param to   : handler B
 * @param reciprocal : link also from B to A
 * @return
 */
bool chain_event_handler(uint32_t from, uint32_t to, bool reciprocal);
