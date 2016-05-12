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
static float last_dist = 0;

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

		// clamp
		if (x > 255) x = 255;
		if (x < -255) x = -255;

		ws2812_rgb_t *p = &pixels[i];

		p->num = 0;

		p->b = 128 + x/2.0f;

		if (x > 128) {
			p->r = x-128;
			p->b -= p->r;
		}

		//p->g = 128;

		//if (x > 0) p->r = x;
		//else p->b = -x;


//		p->g = 0;
//		p->r = x > 0 ? x: 0;//x;
//		p->b = 128 - x/2.0f;//255-x;

//		if (x > 0) {
//		} else {
//			p->r = 0;
//			p->g = -x;
//			p->b = 255 + x;
//		}
	}

	colorled_set_many((uint32_t*) pixels, PIXEL_COUNT);
}


static void handle_sonar_value(float mm)
{
	for (int i = WAVEGRID_LEN-1; i > 0; i--) {
		wavegrid[i] = wavegrid[i-1] * (1.0f - WAVE_DISSIPATION);
	}

	const float max_level = 255.0f;
	const float scale = 1.0f;

	float x = mm * scale;
	if (x > max_level) x = max_level;
	if (x < -max_level) x = -max_level;

	wavegrid[0] = x;

	display_show();
}


static void show(void*arg)
{
	(void)arg;

	float now = meanbuf_current(mb);
	float diff = (last_dist - now);
	last_dist = now;

	handle_sonar_value(diff);
}


static void sonar_poll(void* arg)
{
	(void)arg;

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

	add_periodic_task(sonar_poll, NULL, 50, true);
	add_periodic_task(show, NULL, 50, true);
}
