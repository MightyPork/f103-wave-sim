#include "display.h"
#include "com/debug.h"
#include "utils/timebase.h"
#include "utils/meanbuf.h"

#include <math.h>

#define PIXEL_COUNT 30

#define WAVEGRID_DEPTH 5
#define WAVEGRID_LEN PIXEL_COUNT*WAVEGRID_DEPTH
static float wavegrid[WAVEGRID_LEN];

#define WAVE_DISSIPATION 0.011f


static ws2812_rgb_t pixels[PIXEL_COUNT] = {};

static MeanBuf *mb;

void display_show(void)
{
	for (int i = 0; i < PIXEL_COUNT; i++) {

		// 0 1 2 3 #0+i
		// 7 6 5 4 #2-i-1
		// 8 9 A B #2+i
		// F E D C #
		// G I J K

		float x = wavegrid[i] +
				wavegrid[PIXEL_COUNT*2-i-1] +
				wavegrid[PIXEL_COUNT*2+i] +
				wavegrid[PIXEL_COUNT*4-i-1] +
				wavegrid[PIXEL_COUNT*4+i];

		if (x > 255) x = 255;

		pixels[i].r = (x);
		pixels[i].b = (255.0f-x);
	}

	colorled_set_many((uint32_t*) pixels, PIXEL_COUNT);
}


static void handle_sonar_value(float mm)
{
	for (int i = WAVEGRID_LEN-1; i > 0; i--) {
		wavegrid[i] = wavegrid[i-1] * (1.0f - WAVE_DISSIPATION);
	}

	float x = mm/5.0f;
	if (x>255) x = 255;

	wavegrid[0] = 255 - x;

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

	//info("Sonar");

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

	for (int i = 0; i < WAVEGRID_LEN; i++) {
		wavegrid[i] = 0;
	}

	display_show();

	add_periodic_task(sonar, NULL, 50, true);

	add_periodic_task(show, NULL, 75, true);
}
