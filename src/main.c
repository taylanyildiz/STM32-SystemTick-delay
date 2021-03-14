#include "stm32f10x.h"

volatile uint32_t TimeDelay;

void Time_Delay(uint32_t time);
void delay(uint32_t time);
void SystemConfig(void);
void SysTick_Initialize(uint32_t tick);


void delay(uint32_t time)
{
	while(time--);
}

void Time_Delay(uint32_t time)
{
	TimeDelay = time;
	while(TimeDelay != 0);
}

void SystemConfig(void)
{
	/*
	 * 72 MHz SYSCLK
	 * 72 MHz HCLK
	 * SystemTick
	 * Reload = (10ms/(1/72 MHz)) - 1
	 * 	*
	 *  *		--
	 *  *		--
	 *  *		--
	 * --
	 */
	RCC->CR |= 0x00010000;
	while(!(RCC->CR & 0x00020000));
	FLASH->ACR |= 0x00000012; // Prefetch buffer enable
	RCC->CFGR |= 0x00010000; // PLL entry clock source
	RCC->CFGR |= 0x00000000; // HSE divider for PLL entry not divided
	RCC->CFGR |= 0x001C0000; // PLLMUL multiplication factor x9
	RCC->CFGR |= 0x0000000; // AHB prescaler SYSCLK not divided
	RCC->CFGR |= 0x00000400; // APB1 prescaler divided by 2
	RCC->CFGR |= 0x00002800; // APB2 prescaler divided by 2
	RCC->CFGR |= 0x00000002; // System clock switch PLL selected
	while(!(RCC->CFGR & 0x00000008)); // System clock switch status PLL used
	RCC->CR |= 0x01000000;
	while(!(RCC->CR & 0x02000000));
	RCC->CIR = 0x00980000;
}

void SysTick_Initialize(uint32_t tick)
{
	SysTick->CTRL = 0;
	SysTick->LOAD = tick;
	SysTick->VAL = 100;
	SysTick->CTRL |= 0x07;
}

void SysTick_Handler(void)
{
	GPIOA->ODR ^= 1<<4;
}

int main(void)
{
	SystemConfig();
	RCC->APB2ENR |= 1<<2;
	GPIOA->CRL = 0x00033000;
	SysTick_Initialize(719999);

	while(1)
	{
		GPIOA->ODR ^= 1 << 3;
		delay(7200000);
	}
}

