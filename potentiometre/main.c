#include "sys/clock.h"
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <stdint.h>
#include <stdio.h>

void configuration_potentiometre(void) {
    RCC_AHB1ENR |= (1 << 1);
    RCC_APB2ENR |= (1 << 8);
    GPIOB_MODER |= (3 << 0);

    ADC1_CR2 |= (1 << 0);
    ADC1_CR1 &= ~(3 << 24);
    ADC1_SMPR2 |= (7 << 24);
    ADC1_SQR1 = 0;
    ADC2_SQR2 = 0;
    ADC1_SQR3 = (8 << 0);
}

uint32_t mesure_potentiometre(void) {
    ADC1_CR2 |= (1 << 30);
    while (!(ADC1_SR & (1 << 1)))
        ;
    return ADC1_DR;
}

int main(void) {
    printf("\e[2J\e[1;1H\r\n");
    printf("\e[01;32m*** Welcome to Nucleo F446 ! ***\e[00m\r\n");

    printf("\e[01;31m\t%08lx-%08lx-%08lx\e[00m\r\n", DESIG_UNIQUE_ID0,
           DESIG_UNIQUE_ID1, DESIG_UNIQUE_ID2);
    printf("SYSCLK = %9lu Hz\r\n", get_SYSCLK());
    printf("AHBCLK = %9lu Hz\r\n", get_AHBCLK());
    printf("APB1CLK= %9lu Hz\r\n", get_APB1CLK());
    printf("APB2CLK= %9lu Hz\r\n", get_APB2CLK());
    printf("\r\n");

    configuration_potentiometre();

    while (1) {
        printf("Value: %4lu\r", mesure_potentiometre());
    }
    return 0;
}
