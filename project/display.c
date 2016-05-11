#include "display.h"
#include "com/debug.h"
#include "utils/timebase.h"
#include "utils/meanbuf.h"

#include <math.h>

#define PIXEL_COUNT 30

static ws2812_rgb_t pixels[PIXEL_COUNT] = {};

static MeanBuf *mb;

void display_show(void)
{
	colorled_set_many((uint32_t*) pixels, PIXEL_COUNT);
}


static void handle_sonar_value(float mm)
{
	for (int i = PIXEL_COUNT-1; i > 0; i--) {
		pixels[i].num = pixels[i-1].num;
	}

	float x = mm/5.0f;
	if (x>255) x = 255;

	pixels[0].r = 255-x;
	pixels[0].b = x;

	display_show();
}


static void show(void*arg)
{
	(void)arg;

	handle_sonar_value(meanbuf_current(mb));
}


static void sonar(void* arg)
{
	(void)arg;

	info("Sonar");

	GPIOB->BSRR = GPIO_Pin_13;
	delay_us(10);
	GPIOB->BRR = GPIO_Pin_13;

	// wait for response

	bool suc = false;
	until_timeout(50) {
		if((GPIOB->IDR & (1 << 14)) != 0) {
			suc = true;
			break;
		}
	}

	if (!suc) {
		dbg("Not suc");
		return;
	}

	uint32_t cnt = 0;
	until_timeout(50) {
		if((GPIOB->IDR & (1 << 14)) == 0) break;
		cnt++;
	}

	float t = cnt / 11.2f;

	meanbuf_add(mb, t);
}


void display_init(void)
{
	mb = meanbuf_create(10);

	for (int i = 0; i < PIXEL_COUNT; i++) {
		pixels[i].num = rgb(0, 0, 255);
	}

	display_show();

	add_periodic_task(sonar, NULL, 50, true);

	add_periodic_task(show, NULL, 50, true);
}
