#include "sys/clock.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>  
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/syscfg.h>
#include <libopencm3/stm32/usart.h>
#include <libopencmsis/core_cm3.h>
#include <math.h>


#ifndef USE_RAW_REGISTER
#define USE_RAW_REGISTER 1
#endif

#define BUZZER_BASE 84000 

volatile uint32_t note_ticks = 0;
volatile uint32_t note_freq = 0;
volatile bool note_jouee = false;
volatile uint32_t index = 0;


#define NOTE_A0 28
#define NOTE_AS0 29
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_PAUSE 0


void buzzer_pwm_init(void) {
    /* PB9 = TIM2_CH2 en alternate function */
    GPIOB_MODER = (GPIOB_MODER & ~(3 << 18)) | (2 << 18); // AF mode
    GPIOB_AFRH = (GPIOB_AFRH & ~(0xf << 4)) | (1 << 4);  // AF1 = TIM2

    /* PWM mode configuration */
    TIM2_CR1 = 0x0080;   // ARPE
    TIM2_CCMR1 = 0x6800; // PWM mode 1 sur CH2
    TIM2_CCER = 0x0010;  // CH2 enable
    TIM2_PSC = get_APB1TIMCLK() / BUZZER_BASE - 1;
    TIM2_CNT = 0;
    TIM2_EGR |= 0x1; // update
}

void play_buzzer(uint32_t freq) {
    if(freq == 0) {
        TIM2_CR1 &= ~0x1;
        return;
    }
    uint32_t arr = BUZZER_BASE / freq - 1;
    TIM2_ARR = arr;
    TIM2_CCR2 = arr / 2; 
    TIM2_EGR |= 0x1;   
    TIM2_CR1 |= 0x1;     
}

void init_RGB(void) {
#if USE_RAW_REGISTER
    /* on positionne ce qu'il faut dans les différents
       registres concernés */
    RCC_AHB1ENR |= 0x01;
    GPIOA_MODER = (GPIOA_MODER & ~(0x3 << (16))) | (0x1 << (16));
    GPIOA_OTYPER &= ~(0x1 << 8);
    GPIOA_OSPEEDR = (GPIOA_OSPEEDR & ~(0x3 << 16)) | (0x3 << 16);
    GPIOA_PUPDR &= ~(0x3 << 16);

    
    GPIOA_MODER = (GPIOA_MODER & ~(0x3 << (18))) | (0x1 << (18));
    GPIOA_OTYPER &= ~(0x1 << 9);
    GPIOA_OSPEEDR = (GPIOA_OSPEEDR & ~(0x3 << 18)) | (0x3 << 18);
    GPIOA_PUPDR &= ~(0x3 << 18);

    
    GPIOA_MODER = (GPIOA_MODER & ~(0x3 << (20))) | (0x1 << (20));
    GPIOA_OTYPER &= ~(0x1 << 10);
    GPIOA_OSPEEDR = (GPIOA_OSPEEDR & ~(0x3 << 20)) | (0x3 << 20);
    GPIOA_PUPDR &= ~(0x3 << 20);
#else
    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO5);
#endif
}

void init_BP(void) { 
    RCC_AHB1ENR |= 0x04;
    GPIOC_MODER &= ~(3 << 26);
    GPIOC_PUPDR &= ~(3<<26);
}


/* fonction agissant comme une barrière pour les accès mémoires */
inline static void barriere(void) { __asm__ volatile("" ::: "memory"); }

__attribute__((optimize("O0"))) void tempo_500ms(void) {
    volatile uint32_t duree;
    /* estimation, suppose que le compilateur n'optimise pas trop... */
    for (duree = 0; duree < 5600000; duree++) {
        ;
    }
}

void init_USART(void) {
#if USE_RAW_REGISTER
    RCC_AHB1ENR |= 0x01;
    GPIOA_MODER = (GPIOA_MODER & 0xFFFFFF0F) | 0x000000A0;
    GPIOA_AFRL = (GPIOA_AFRL & 0xFFFF00FF) | 0x00007700;
    USART2_BRR = get_APB1CLK() / 9600;
    USART2_CR3 = 0;
    USART2_CR2 = 0;
    USART2_CR1 = (1 << 2) | (1 << 3) | (1 << 13);
#else
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_USART2);
    gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO2 | GPIO3);
    gpio_set_af(GPIOA, GPIO_AF7, GPIO2 | GPIO3);

    usart_set_baudrate(USART2, 9600);
    usart_set_databits(USART2, 8);
    usart_set_stopbits(USART2, USART_STOPBITS_1);
    usart_set_mode(USART2, USART_MODE_TX);
    usart_set_parity(USART2, USART_PARITY_NONE);
    usart_set_flow_control(USART2, USART_FLOWCONTROL_NONE);

    usart_enable(USART2);
#endif
}

/* Initialisation du timer système (systick) */
/* La fréquence en entrée est en Hz.
 * Elle doit être >= 11Hz du fait de la taille du registre LOAD */
void systick_init(uint32_t freq) {
#if USE_RAW_REGISTER
    /* ATTENTION: la valeur calculée doit tenir dans le registre sur 24 bits */
    uint32_t p = get_SYSCLK() / freq;
    STK_RVR = (p - 1) & 0x00FFFFFF;
    STK_CVR = 0;
    STK_CSR |= STK_CSR_CLKSOURCE | STK_CSR_TICKINT | STK_CSR_ENABLE;
#else
    /* De manière équivalente, mais en passant par l'API

       Note: l'API gère la configuration où la fréquence de l'horloge
       source divisé par 8, on peut alors descendre jusqu'à une
       fréquence de 2Hz (au lieu de 11Hz).
     */
    systick_set_frequency(freq, get_SYSCLK());
    systick_counter_enable();
    /* activation des interruptions en dernier */
    systick_interrupt_enable();
#endif
}

/* La liste des traitants pré-configurés avec libopencm3 est disponible ici :
   http://libopencm3.org/docs/latest/stm32f4/html/group__CM3__nvic__isrprototypes__STM32F4.html
   en plus des traitants de bases visibles ici :
   http://libopencm3.org/docs/latest/stm32f4/html/group__CM3__nvic__defines.html

   Vous pourrez être particulièrement intéressés par
   - void sys_tick_handler(void)
   - void exti15_10_isr(void)
   - void usart2_isr(void)
*/

/* L'attibut "interrupt" n'est plus nécessaire sur l'architecture
 * ARM-Cortex-M4 : Les fonctions d'interruption respectent les
 * conventions d'appel de la plateforme.  Voir par exemple le
 * paragraphe "Exception Entry & Exit" de
 * https://interrupt.memfault.com/blog/arm-cortex-m-exceptions-and-nvic
 */
// __attribute__((interrupt))
void sys_tick_handler() {
   if(note_ticks > 0) {
        note_ticks--;
        if(note_ticks == 0 && note_jouee) {
            play_buzzer(0); 
            index++;
            note_jouee = false;
        }
    }
}

void _putc(const char c) {
    while ((USART2_SR & 0x80) == 0)
        ;
    USART2_DR = c;
}

void _puts(const char *c) {
    int len = strlen(c);
    for (int i = 0; i < len; i++) {
        _putc(c[i]);
    }
}

char _getc(void) {
    char c = 'a';
    scanf(&c);
    return c;
}

/* Fonction non bloquante envoyant une chaîne par l'UART */
int _async_puts(const char *s) {
    /* Cette fonction doit utiliser un traitant d'interruption
     * pour gérer l'envoi de la chaîne s (qui doit rester
     * valide pendant tout l'envoi). Elle doit donc être
     * non bloquante (pas d'attente active non plus) et
     * renvoyer 0.
     *
     * Si une chaîne est déjà en cours d'envoi, cette
     * fonction doit renvoyer 1 (et ignorer la nouvelle
     * chaîne).
     *
     * Si s est NULL, le code de retour permet de savoir
     * si une chaîne est encore en cours d'envoi ou si
     * une nouvelle chaîne peut être envoyée.
     */
    /* À compléter */
    (void)s; // to avoid unused paramater warning
    return 0;
}

static volatile uint32_t mode = 0;

void exti15_10_isr() {
#if USE_RAW_REGISTER
    EXTI_PR &= ~(1 << 13); /* Clear pending register EXTIPR[13] */
#else
    exti_reset_request(EXTI13);
#endif
    mode = mode ^ 1; /* switch the mode value 0 <-> 1 */
}



__attribute__((optimize("O0"))) void play_note(uint32_t freq, uint32_t duree_ms) {
    note_freq = freq;
    note_ticks = duree_ms;
    play_buzzer(freq);
    note_jouee = true;
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

    init_RGB();

    printf("\e[2J\e[1;1H\r\n");
    printf("STM32 PWM Buzzer Melody\n");

    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_TIM2);

    buzzer_pwm_init();
    systick_init(1000); // SysTick toutes les 1 ms

    init_BP();
        
    bool tmp;

    //printf("c?\n\r");
    //char c;
    //c = _getc();
    //_putc(c);
   //uint32_t notes[] = {NOTE_E4, 0, NOTE_E4, 0,NOTE_F4, 0,NOTE_G4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_PAUSE};
   //uint32_t durees[] = {500, 50, 500, 50, 500, 50 ,500, 500, 500, 500, 500, 500, 500, 1000, 1000, 1000, 1000, 250};
/*uint32_t notes[] = {
  NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4, NOTE_D5, NOTE_C5,
  NOTE_A4, NOTE_C4, NOTE_E4, NOTE_A4, NOTE_B4, NOTE_E4, NOTE_GS4, NOTE_B4,
  NOTE_C5, NOTE_E4, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_DS5, NOTE_E5, NOTE_B4,
  NOTE_D5, NOTE_C5, NOTE_A4, NOTE_PAUSE
};

uint32_t durees[] = {
  250, 250, 250, 250, 250, 250, 250, 250,
  500, 250, 250, 250, 250, 250, 250, 250,
  500, 250, 250, 250, 250, 250, 250, 250,
  500, 1000, 250
};*/

/*uint32_t notes[] = {
  NOTE_E5, NOTE_E5, NOTE_PAUSE, NOTE_E5,
  NOTE_PAUSE, NOTE_C5, NOTE_E5, NOTE_PAUSE,
  NOTE_G5, NOTE_PAUSE, NOTE_G4, NOTE_PAUSE,
  NOTE_C5, NOTE_PAUSE, NOTE_G4, NOTE_PAUSE,
  NOTE_E4, NOTE_PAUSE, NOTE_A4, NOTE_PAUSE,
  NOTE_B4, NOTE_PAUSE, NOTE_AS4, NOTE_A4,
  NOTE_G4, NOTE_E5, NOTE_G5, NOTE_A5,
  NOTE_F5, NOTE_G5, NOTE_PAUSE, NOTE_E5,
  NOTE_C5, NOTE_D5, NOTE_B4
};

uint32_t durees[] = {
  250, 250, 250, 250,
  250, 250, 250, 250,
  250, 250, 250, 250,
  250, 250, 250, 250,
  250, 250, 250, 250,
  250, 250, 250, 250,
  250, 250, 250, 250,
  250, 250, 250, 250,
  250, 250, 250
};*/
/* GAME OF THRONE
uint32_t notes[] = {
  NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
  NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
  NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4,
  NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_E4, NOTE_F4,
  NOTE_G4, NOTE_C4, // dotted notes approximated as double durations

  NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
  NOTE_D4,
  NOTE_F4, NOTE_AS3,
  NOTE_DS4, NOTE_D4, NOTE_F4, NOTE_AS3,
  NOTE_DS4, NOTE_D4, NOTE_C4,

  NOTE_G4, NOTE_C4,
  NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,
  NOTE_D4,
  NOTE_F4, NOTE_AS3,
  NOTE_DS4, NOTE_D4, NOTE_F4, NOTE_AS3,
  NOTE_DS4, NOTE_D4, NOTE_C4,
  NOTE_G4, NOTE_C4,
  NOTE_DS4, NOTE_F4, NOTE_G4, NOTE_C4, NOTE_DS4, NOTE_F4,

  NOTE_D4,
  NOTE_F4, NOTE_AS3,
  NOTE_D4, NOTE_DS4, NOTE_D4, NOTE_AS3,
  NOTE_C4,
  NOTE_C5,
  NOTE_AS4,
  NOTE_C4,
  NOTE_G4,
  NOTE_DS4,
  NOTE_DS4, NOTE_F4,
  NOTE_G4,

  NOTE_C5,
  NOTE_AS4,
  NOTE_C4,
  NOTE_G4,
  NOTE_DS4,
  NOTE_DS4, NOTE_D4,
  NOTE_C5, NOTE_G4, NOTE_GS4, NOTE_AS4, NOTE_C5, NOTE_G4, NOTE_GS4, NOTE_AS4,
  NOTE_C5, NOTE_G4, NOTE_GS4, NOTE_AS4, NOTE_C5, NOTE_G4, NOTE_GS4, NOTE_AS4,

  NOTE_PAUSE, NOTE_GS5, NOTE_AS5, NOTE_C6, NOTE_G5, NOTE_GS5, NOTE_AS5,
  NOTE_C6, NOTE_G5, NOTE_GS5, NOTE_AS5, NOTE_C6, NOTE_G5, NOTE_GS5, NOTE_AS5
};

uint32_t durees[] = {
  250, 250, 125, 125, 250, 250, 125, 125,
  250, 250, 125, 125, 250, 250, 125, 125,
  250, 250, 125, 125, 250, 250, 125, 125,
  250, 250, 125, 125, 250, 250, 125, 125,
  500, 500, // dotted notes

  125, 125, 250, 250, 125, 125,
  1000,
  500, 500,
  125, 125, 250, 500,
  125, 125, 1000,

  500, 500,
  125, 125, 250, 250, 125, 125,
  1000,
  500, 500,
  125, 125, 250, 500,
  125, 125, 1000,
  500, 500,
  125, 125, 250, 250, 125, 125,

  1000,
  500, 500,
  125, 125, 125, 125,
  500,
  500,
  500,
  500,
  500,
  500,
  125, 125,
  1000,

  500,
  500,
  500,
  500,
  500,
  125, 125,
  250, 250, 125, 125, 250, 250, 125, 125,
  250, 250, 125, 125, 250, 250, 125, 125,

  500, 125, 125, 250, 500, 125, 125,
  250, 125, 125, 125, 250, 500, 125, 125
};
*/

uint32_t notes[] = {
  NOTE_C5,
  NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_E5,
  NOTE_D5, NOTE_D5, NOTE_D5,
  NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_E5, NOTE_C5, NOTE_C5,
  NOTE_A5, NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_G5,
  NOTE_F5, NOTE_D5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_G5, NOTE_E5,

  NOTE_F5, NOTE_C5,
  NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_E5,
  NOTE_D5, NOTE_D5, NOTE_D5,
  NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_E5, NOTE_C5, NOTE_C5,
  NOTE_A5, NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_G5,
  NOTE_F5, NOTE_D5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_G5, NOTE_E5,
  NOTE_F5, NOTE_C5,

  NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_E5, NOTE_E5,
  NOTE_F5, NOTE_E5, NOTE_D5,
  NOTE_C5, NOTE_A5,
  NOTE_AS5, NOTE_A5, NOTE_G5,
  NOTE_C6, NOTE_C5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_G5, NOTE_E5,
  NOTE_F5, NOTE_C5,
  NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_E5,
  NOTE_D5, NOTE_D5, NOTE_D5,

  NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_E5, NOTE_C5, NOTE_C5,
  NOTE_A5, NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_G5,
  NOTE_F5, NOTE_D5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_G5, NOTE_E5,
  NOTE_F5, NOTE_C5,
  NOTE_F5, NOTE_F5, NOTE_F5,
  NOTE_E5, NOTE_E5,
  NOTE_F5, NOTE_E5, NOTE_D5,

  NOTE_C5, NOTE_A5,
  NOTE_AS5, NOTE_A5, NOTE_G5,
  NOTE_C6, NOTE_C5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_G5, NOTE_E5,
  NOTE_F5, NOTE_C5,
  NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_E5,
  NOTE_D5, NOTE_D5, NOTE_D5,
  NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_E5, NOTE_C5, NOTE_C5,

  NOTE_A5, NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_G5,
  NOTE_F5, NOTE_D5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_G5, NOTE_E5,
  NOTE_F5, NOTE_C5,
  NOTE_F5, NOTE_F5, NOTE_G5, NOTE_F5, NOTE_E5,
  NOTE_D5, NOTE_D5, NOTE_D5,
  NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, NOTE_F5,
  NOTE_E5, NOTE_C5, NOTE_C5,

  NOTE_A5, NOTE_A5, NOTE_AS5, NOTE_A5, NOTE_G5,
  NOTE_F5, NOTE_D5, NOTE_C5, NOTE_C5,
  NOTE_D5, NOTE_G5, NOTE_E5,
  NOTE_F5, NOTE_C5,
  NOTE_PAUSE
};

uint32_t durees[] = {
  429, // C5, 4 (quarter note)
  429, 214, 214, 214, 214,
  429, 429, 429,
  429, 214, 214, 214, 214,
  429, 429, 429,
  429, 214, 214, 214, 214,
  429, 429, 214, 214,
  429, 429, 429,

  857, 429,
  429, 214, 214, 214, 214,
  429, 429, 429,
  429, 214, 214, 214, 214,
  429, 429, 429,
  429, 214, 214, 214, 214,
  429, 429, 214, 214,
  429, 429, 429,
  857, 429,

  429, 429, 429,
  857, 429,
  429, 429, 429,
  857, 429,
  429, 429, 429,
  429, 429, 214, 214,
  429, 429, 429,
  857, 429,
  429, 214, 214, 214, 214,
  429, 429, 429,

  429, 214, 214, 214, 214,
  429, 429, 429,
  429, 214, 214, 214, 214,
  429, 429, 214, 214,
  429, 429, 429,
  857, 429,
  429, 429, 429,
  857, 429,
  429, 429, 429,

  857, 429,
  429, 429, 429,
  429, 429, 214, 214,
  429, 429, 429,
  857, 429,
  429, 214, 214, 214, 214,
  429, 429, 429,
  429, 214, 214, 214, 214,
  429, 429, 429,

  429, 214, 214, 214, 214,
  429, 429, 214, 214,
  429, 429, 429,
  857, 429
};



 
   while (1){
        
        //tmp = ((GPIOC_IDR & (1 << 13)) == 0);
        // if(((GPIOC_IDR & (1 << 13)) == 0) ){
        //     GPIOA_ODR  ^= (1<<8);
        //     GPIOA_ODR  ^= (1<<9);
        //     GPIOA_ODR  ^= (1<<10);
        //     GPIOB_ODR  |= (1<<9);
        //     tempo_500ms();
            
        // }

        // GPIOB_ODR  ^= (1<<9);
        // for (int duree = 0; duree < 1000000; duree++){
        //     ;
        // }

        // GPIOB_ODR  &=  ~(1<<9);

        
        if(!note_jouee && index < 15){
            play_note(notes[index], durees[index]);
            note_jouee = true;
        }
    }
    return 0;
}
