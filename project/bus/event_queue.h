#pragma once

#include "main.h"
#include "utils/circbuf.h"

#define TASK_QUEUE_SIZE 64
#define EVENT_QUEUE_SIZE 64


/** Application events */
typedef enum {
	EVENT_ONE // placeholder

} EventType;


/** Event Queue entry */
typedef struct {
	EventType type;
	void *data;
} Event;

typedef struct {
	void (*handler)(void*);
	void* arg;
} QueuedTask;


/**
 * @brief Set up the task and event queues
 * @param tq_size : number of slots in the task queue
 * @param eq_size : number of slots in the event queue
 */
void queues_init(size_t tq_size, size_t eq_size);


/**
 * @brief Post a task on the task queue, with arg.
 *
 * @see tq_post()
 *
 * @param handler : task function
 * @param arg     : argument for the handler
 * @return success
 */
bool tq_post(void (*handler)(void *), void *arg);


/**
 * @brief Post an event on the event queue
 * @param event : pointer to an event to post; will be copied.
 * @return success
 */
bool eq_post(const Event *event);


/**
 * @brief Run all pending tasks on the task queue
 */
void tq_poll(void);


/**
 * @brief Run one pending task on the task queue
 * @return true if a task was run.
 */
bool tq_poll_one(void);


/**
 * @brief Take one event off the event queue.
 * @param dest : pointer to a destination event variable.
 * @return success
 */
bool eq_take(Event *dest);
