/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>


int main(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef gpio_cnf;
	gpio_cnf.GPIO_Pin = GPIO_Pin_13;
	gpio_cnf.GPIO_Mode = GPIO_Mode_Out_PP;
	gpio_cnf.GPIO_Speed = GPIO_Speed_2MHz;

	GPIO_Init(GPIOC, &gpio_cnf);


	while (1) {
		GPIOC->ODR ^= GPIO_Pin_13;

		// delay
		for (int i = 0; i < 1000000; i++);
	}
}

