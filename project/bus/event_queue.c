#include "event_queue.h"
#include "com/debug.h"

/** Task queue */
static CircBuf *tq;

/** Event queue */
static CircBuf *eq;


void queues_init(size_t tq_size, size_t eq_size)
{
	tq = cbuf_create(tq_size, sizeof(QueuedTask));
	eq = cbuf_create(eq_size, sizeof(Event));
}


bool tq_post(void (*handler)(void*), void *arg)
{
	QueuedTask task;
	task.handler = handler;
	task.arg = arg;

	bool suc = cbuf_append(tq, &task);
	if (!suc) error("TQ overflow");
	return suc;
}


bool eq_post(const Event *event)
{
	bool suc = cbuf_append(eq, event);
	if (!suc) {
		error("EQ overflow, evt %d", event->type);
	}
	return suc;
}


bool tq_poll_one(void)
{
	QueuedTask task;

	// serve all tasks
	bool suc = cbuf_pop(tq, &task);

	if (suc) {
		task.handler(task.arg);
	}

	return suc;
}


void tq_poll(void)
{
	// serve all tasks
	while (tq_poll_one());
}


bool eq_take(Event *dest)
{
	bool suc = cbuf_pop(eq, dest);
	return suc;
}
