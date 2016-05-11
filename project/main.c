#include "main.h"
#include "hw_init.h"

#include "com/debug.h"
#include "com/com_fileio.h"
#include "com/com_iface.h"
#include "bus/event_queue.h"
#include "bus/event_handler.h"
#include "utils/timebase.h"

#include "colorled.h"
#include "display.h"
#include <math.h>
#include <sbmp.h>

void poll_subsystems(void)
{
	// poll serial buffers (runs callback)
	com_poll(debug_iface);
	com_poll(data_iface);

	// run queued tasks
	tq_poll();

	// handle queued events
	Event evt;

	until_timeout(2) { // take 2 ms max
		if (eq_take(&evt)) {
			run_event_handler(&evt);
		} else {
			break;
		}
	}
}



void blinky(void* arg)
{
	(void)arg;
	GPIOC->ODR ^= 1<<13;
}




int main(void)
{
	hw_init();
	display_init();

	banner("*** STM32F103K8T6 RGB LED demo ***");
	banner_info("(c) Ondrej Hruska, 2016");
	banner_info("Katedra mereni K338, CVUT FEL");


	add_periodic_task(blinky, NULL, 500, false);

	while (1) {
		poll_subsystems();
	}
}


void dlnk_rx(SBMP_Datagram *dg)
{
	dbg("Rx dg type %d", dg->type);
}
