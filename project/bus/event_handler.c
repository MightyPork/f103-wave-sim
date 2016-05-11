#include "event_handler.h"
#include "com/debug.h"

typedef struct {
	uint32_t handler_id;
	uint32_t chained_handler; // if not 0, that handler is removed together with this handler.
	EventType type; // event type
	EventHandlerCallback handler; // returns True if event was handled.
	bool used; // this slot is currently used

	void *user_data;

} EventHandlerSlot;


#define EH_SLOT_COUNT 6
static EventHandlerSlot eh_slots[EH_SLOT_COUNT];

static uint32_t next_slot_pid = 1; // 0 is reserved

/** Get a valid free PID for a new handler slot. */
static uint32_t make_pid(void)
{
	uint32_t pid = next_slot_pid++;

	// make sure no task is given PID 0
	if (next_slot_pid == 0) {
		next_slot_pid++;
	}

	return pid;
}

/**
 * @brief Register an event handler for event type
 * @param type    : handled event type
 * @param handler : the handler func
 * @return handler ID. Can be used to remove the handler.
 */
uint32_t register_event_handler(EventType type, EventHandlerCallback handler, void *user_data)
{
	for (int i = 0; i < EH_SLOT_COUNT; i++) {
		if (eh_slots[i].used) continue;

		// Free slot found
		EventHandlerSlot *slot = &eh_slots[i];

		slot->handler = handler;
		slot->type = type;
		slot->handler_id = make_pid();
		slot->used = true;
		slot->chained_handler = 0;
		slot->user_data = user_data;

		return slot->handler_id;
	}

	error("Failed to register event handler for type %d", type);

	return 0; // fail
}

/** Chain for common destruction */
bool chain_event_handler(uint32_t from, uint32_t to, bool reci)
{
	uint8_t cnt = 0;

	for (int i = 0; i < EH_SLOT_COUNT; i++) {
		EventHandlerSlot *slot = &eh_slots[i];

		if (!slot->used) continue;

		if (slot->handler_id == from) {
			slot->chained_handler = to;
			cnt++;
		}

		// link back in two-handler reciprocal link
		if (reci && slot->handler_id == to) {
			slot->chained_handler = from;
			cnt++;
		}

		if (cnt == (reci ? 2 : 1)) {
			return true;
		}
	}

	return false;
}

/**
 * @brief check if exists
 */
bool event_handler_exists(uint32_t handler_id)
{
	for (int i = 0; i < EH_SLOT_COUNT; i++) {
		EventHandlerSlot *slot = &eh_slots[i];
		if (!slot->used) continue;
		if (slot->handler_id == handler_id) {
			return true;
		}
	}

	return false;
}

/**
 * @brief Remove event handler by handler ID
 * @param handler_id : handler ID, obtained when registering or in the callback.
 * @return number of removed handlers
 */
int remove_event_handler(uint32_t handler_id)
{
	int cnt = 0;
	while (handler_id != 0) { // outer loop because of chained handlers
		bool suc = false;
		for (int i = 0; i < EH_SLOT_COUNT; i++) {
			if (!eh_slots[i].used) {
				continue; // skip empty slot
			}

			// Free slot found
			EventHandlerSlot *slot = &eh_slots[i];
			if (slot->handler_id == handler_id) {
				slot->used = false;
				slot->user_data = NULL;
				suc = true;
				cnt++;

				handler_id = slot->chained_handler; // continue with the chained handler.
				break;
			}
		}
		if (!suc) break;
	}
	return cnt;
}

/** Handle an event */
void run_event_handler(Event *evt)
{
	bool handled = false;

	for (int i = 0; i < EH_SLOT_COUNT; i++) {
		EventHandlerSlot *slot = &eh_slots[i];

		if (!slot->used) continue; // unused

		if (slot->type != evt->type) continue; // wrong type

		handled = slot->handler(slot->handler_id, evt, &slot->user_data);
		if (handled) break;
	}

	if (!handled) {
		warn("Unhandled event, type %d", evt->type);
	}
}
