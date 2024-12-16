#include "sys/clock.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <stdint.h>
#include <stdio.h>

void buzzer_pwm_init(uint32_t TIM2_freq) {
    /* buzzer is on TIM2_CH2 */
    /* configure PB9 as alternate function TIM2 */
    GPIOB_MODER = (GPIOB_MODER & ~(3 << 18)) | (1 << 19);

    /* setup TIM2_CH2 as alternate function */
    GPIOB_AFRH = (GPIOB_AFRH & ~(0xf << 4)) | (1 << 4);

    /* configure TIM2_CH2 as PWM output */
    TIM2_CR1 = 0x0080; /* buffered preload up-counting mode */
    TIM2_CR2 = 0x0000;
    TIM2_SMCR = 0x0000;
    TIM2_DIER = 0x0000;
    TIM2_CCMR1 = 0x6800; /* PWM mode */
    TIM2_CCMR2 = 0x0000;
    TIM2_CCER = 0x0010; /* channel 2 output to pin PB9 */
    TIM2_CNT = 0x0000;
/* choix d'une base pour décomposer entre compteur et pré-scaling */
/* période = PSC * ARR / horloge */
#define TIM2_Base 84000
    TIM2_PSC = get_APB1TIMCLK() / TIM2_Base - 1;
    TIM2_ARR = TIM2_Base / TIM2_freq - 1;
    TIM2_CCR2 = (TIM2_Base / TIM2_freq) * 0.9;
    TIM2_EGR |= 0x1; /* update event */
}

void toggle_buzzer(void) { TIM2_CR1 ^= 0x1; /* counter on/off */ }

int main(void) {
    printf("\e[2J\e[1;1H\r\n");
    printf("\e[01;32m*** Welcome to Nucleo F446 ! ***\e[00m\r\n");

    printf("\e[01;31m\t%08lx-%08lx-%08lx\e[00m\r\n", DESIG_UNIQUE_ID0,
           DESIG_UNIQUE_ID1, DESIG_UNIQUE_ID2);
    printf("SYSCLK = %9lu Hz\r\n", get_SYSCLK());
    printf("AHBCLK = %9lu Hz\r\n", get_AHBCLK());
    printf("APB1CLK= %9lu Hz\r\n", get_APB1CLK());
    printf("APB2CLK= %9lu Hz\r\n", get_APB2CLK());
    printf("TIMCLK = %9lu Hz\r\n", get_APB1TIMCLK());
    printf("\r\n");

    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_TIM2);
    buzzer_pwm_init(440);
    toggle_buzzer();
    for (int i = 0; i < 1000000; i++) {
        for (int j = 0; j < 400; j++) {
            __asm__("" :::);
        }
    }
    toggle_buzzer();

    while (1)
        ;
    return 0;
}
