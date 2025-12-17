#include "sys/clock.h"
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/systick.h>  
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/syscfg.h>
#include <libopencm3/stm32/usart.h>
#include <libopencmsis/core_cm3.h>
#include <libopencm3/cm3/nvic.h>
#include <math.h>


#ifndef USE_RAW_REGISTER
#define USE_RAW_REGISTER 1
#endif

#define BUZZER_BASE 84000 

volatile uint32_t note_ticks = 0;
volatile uint32_t note_freq = 0;
volatile bool note_jouee = false;
volatile uint32_t index = 0;
volatile uint32_t ms = 0;
volatile uint32_t temps = 0;
volatile uint32_t start = 0;
volatile uint32_t end = 0;
volatile uint32_t secondes = 0;
volatile uint32_t tick = 0;

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

//Fonction donnée dans etu_projet
void buzzer_pwm_init(void) {
    /* buzzer is on TIM2_CH2 */
    /* configure PB9 as alternate function TIM2 */
    GPIOB_MODER = (GPIOB_MODER & ~(3 << 18)) | (2 << 18); 
    /* setup TIM2_CH2 as alternate function */
    GPIOB_AFRH = (GPIOB_AFRH & ~(0xf << 4)) | (1 << 4);  

     /* configure TIM2_CH2 as PWM output */
    TIM2_CR1 = 0x0080;   /* buffered preload up-counting mode */
    TIM2_CCMR1 = 0x6800; /* PWM mode */
    TIM2_CCER = 0x0010;  /* channel 2 output to pin PB9 */
    TIM2_PSC = get_APB1TIMCLK() / BUZZER_BASE - 1;
    TIM2_CNT = 0;
    TIM2_EGR |= 0x1; /* update event */
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

void button_irqPC13_init(void) {
#if USE_RAW_REGISTER
    RCC_APB2ENR |= 1 << 14;
    /* set PC13 as EXTI13 input */
    SYSCFG_EXTICR4 = (SYSCFG_EXTICR4 & ~(0xf << 4)) | (0x2 << 4);
    /* Setup interrupt for EXTI13, falling edge */
    EXTI_IMR |= (1 << 13);
    EXTI_RTSR &= ~(1 << 13);
    EXTI_FTSR |= (1 << 13);
    EXTI_PR |= (1 << 13);
    /* enable EXTI15-10 IRQ PC13 */
    NVIC_ISER(40 / 32) = (1 << (40 % 32));
#else
    rcc_periph_clock_enable(RCC_SYSCFG);

    exti_select_source(EXTI13, GPIOC);

    exti_enable_request(EXTI13);
    exti_set_trigger(EXTI13, EXTI_TRIGGER_FALLING);
    exti_reset_request(EXTI13);

    nvic_enable_irq(NVIC_EXTI15_10_IRQ);
    nvic_set_priority(NVIC_EXTI15_10_IRQ, 200);
#endif
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
    USART2_CR1 = (1 << 2) | (1 << 3) | (1 << 13) | (1 << 5)| (1<<7);
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
    tick++;
    if(tick >= 1000){
        secondes++;
        tick = 0;
    }
    ms++;
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

static const char *sentence;
volatile int position=0;
volatile bool busy = false;
static volatile uint32_t mode = 0;
volatile char buffer[256];
volatile int cursor=0;

void usart2_isr(void){
    if (USART2_SR & USART_SR_TXE) {
        if((USART2_SR & (1<<7))){
            if(sentence[position] =='\0'){
                usart_disable_tx_interrupt(USART2);
                busy = false;
            }else{
                USART2_DR= sentence[position];
                position++;
            }
        
        }
    }
    if (USART2_SR & USART_SR_RXNE) {
        char c = USART2_DR;
        if (cursor < 255) {
            buffer[cursor++] = c;
        }
    }
    
}

/* Fonction non bloquante envoyant une chaîne par l'UART */
int _async_puts(const char *s) {

    if(s == NULL){
        return busy ?  1 :0;
    }
    if(!busy){
        sentence = s;
        busy = true;
        position=0;
        
        usart_enable_tx_interrupt(USART2);
        
        return 0;
    }
    return 1;
}

void clavier(void){
    usart_disable_rx_interrupt(USART2); 
    
    int local_cursor = cursor; 
    cursor = 0;            
    
    usart_enable_rx_interrupt(USART2);

    char letter;
    int positionR=0;
    while(positionR<local_cursor){
        letter = buffer[positionR];
        switch(letter){
            case ' ':
                mode ^=1;
                break;
            case '1':
                mode =1;
                break;
            case '0':
                mode =0;
                break;
            case 'b':
                GPIOC_IDR &= ~(1 << 13);
                break;
            default:
                break;
        }
        positionR++;
    }
    cursor=0;
}




//si la led est rouge alors je la passe en vert et je stocke la valeur de seconde*1000+ms à ce moment la
void exti15_10_isr() { 
    
    static uint32_t last = 0;
    EXTI_PR = (1 << 13);

    if (ms - last > 200) { 
        switch(mode){
            case 0:
            mode = 1;
            break;
            case 1:
            end = ms;
            GPIOA_ODR  &= ~(1<<9);
            mode = 2;
            default:
            break;
        }
        last = ms;
    }
}




void play_note(uint32_t freq, uint32_t duree_ms) {
    note_freq = freq;
    note_ticks = duree_ms;
    play_buzzer(freq);
    note_jouee = true;
}

const char* c = "voiture\n\r";


int main(void) {
    printf("%ld\n\r", secondes);
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


    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_TIM2);

    buzzer_pwm_init();
    systick_init(1000); // SysTick toutes les 1 ms

    init_BP();
    init_USART();
    nvic_enable_irq(NVIC_USART2_IRQ);



    button_irqPC13_init();
    bool tmp;
    uint32_t alea_temp = 0;
    uint32_t temp_final = 0;
    uint32_t notes[] = {NOTE_E4, NOTE_E4,NOTE_F4,NOTE_G4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4};
    uint32_t durees[] = {500, 500, 500,500, 500, 500, 500, 500, 500, 500, 1000};
    bool change = false;
    
    while (1){
        clavier();
        switch(mode){
            case 0:
            if(!change){
                printf("Press button to start\n\r");
                change = true;
            }
            break;
            
            case 1:
            if(change){
                printf("Started\n\r");
                change = false;
            }
            alea_temp = ms % 15000 + 1;
            if(alea_temp > 5000 && (secondes*1000 % alea_temp == 0)){
                GPIOA_ODR  |= (1<<9);
                start = ms;
            }
            break;
            case 2:
            temp_final = end - start;
            printf("Vous avez pris : %ld ms\n\r", temp_final);
            mode = 3;
            break;
            case 3:
            end = 0;
            start = 0;
            mode = 0;
            break;
            case 4:
            break;
            default:
            break;
        }


        /*if(mode == 1){
            GPIOA_ODR  |= (1<<8) | (1<<9) | (1<<10);
        }
        else if(mode == 0){
            GPIOA_ODR &= ~((1<<8) | ~(1<<9) | ~(1<<10));
        }*/

        /*if(!note_jouee && index < 11){
            play_note(notes[index], durees[index]);
            note_jouee = true;
        }
        if(((GPIOC_IDR & (1 << 13)) == 0)){
            index = 0;
            note_jouee = false;
        }*/
    }
    return 0;
}
