#include "sys/clock.h"
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/syscfg.h>
#include <libopencm3/stm32/usart.h>
#include <libopencmsis/core_cm3.h>
#include <libopencm3/cm3/nvic.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef USE_RAW_REGISTER
#define USE_RAW_REGISTER 1
#endif

volatile uint32_t tick=0;
volatile uint32_t frequence=1000;
volatile uint32_t note_ticks=0;
volatile uint32_t demi_ticks=0;
volatile uint32_t note_freq=0;
volatile uint32_t index=0;
volatile bool note_jouee = false;


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


void init_BZ(void){

    RCC_AHB1ENR |= (1<<1);
    GPIOB_MODER = (GPIOB_MODER & ~(0x3 << (18))) | (0x1 << (18));
    GPIOB_OTYPER &= ~(0x1 << 9);
    GPIOB_OSPEEDR = (GPIOB_OSPEEDR & ~(0x3 << 18)) | (0x3 << 18);
    GPIOB_PUPDR &= ~(0x3 << 18);

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
    // RCC_AHB1ENR |= 0x01;
    // RCC_APB1ENR |= 0x01;
    GPIOA_MODER = (GPIOA_MODER & 0xFFFFFF0F) | 0x000000A0;
    GPIOA_AFRL = (GPIOA_AFRL & 0xFFFF00FF) | 0x00007700;
    USART2_BRR = get_APB1CLK() / 9600;
    USART2_CR3 = 0;
    USART2_CR2 = 0;
    USART2_CR1 = (1 << 2) | (1 << 3) | (1 << 13) | (1<<7) ;
    

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
   if(note_ticks > 0){
    note_ticks--;
   }
   if(note_freq != 0){
    demi_ticks--;
    if(demi_ticks <= 0 && note_freq != 0){
        GPIOB_ODR  ^= (1<<9);
        demi_ticks = frequence / (2*note_freq);
    }
   }
   
   if(note_ticks <= 0){
    GPIOB_ODR  |= (1<<9);
    index++;
    note_jouee = false;
    note_freq = 0;
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
volatile int position=-1;
volatile bool busy = false;

void usart2_isr(void){

    if((USART2_SR & (1<<7))){
        position++;
        if(sentence[position] =='\0'){
            usart_disable_tx_interrupt(USART2);
            busy = false;
        }else{
            USART2_DR= sentence[position];
        }
        
    }
    
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
    if(s == NULL){
        return busy ?  1 :0;
    }
    if(!busy){
        sentence = s;
        busy = true;
        position=-1;
        
        usart_enable_tx_interrupt(USART2);
        
        return 0;
    }
    return 1;
}

static volatile uint32_t mode = 0;

void exti15_10_isr() {
#if USE_RAW_REGISTER
    EXTI_PR |= (1 << 13); /* Clear pending register EXTIPR[13] */
#else
    exti_reset_request(EXTI13);
#endif
    mode = mode ^ 1; /* switch the mode value 0 <-> 1 */
}



__attribute__((optimize("O0"))) void play_note(uint32_t freq, uint32_t duree_ms) {
    note_ticks = duree_ms;
    note_freq = freq;
    if(freq == 0) {
        demi_ticks = 0;
        GPIOB_ODR |= (1<<9);
    } else {
        demi_ticks = frequence / (2*freq);
    }
}

const char* c = "voiture\n\r";

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

    systick_init((uint32_t)1000);

    init_BP();
    init_BZ();
    init_USART();
    nvic_enable_irq(NVIC_USART2_IRQ);
    bool tmp;

    //printf("c?\n\r");
    //char c;
    //c = _getc();
    //_putc(c);
    uint32_t notes[] = {NOTE_E4, 0, NOTE_E4, 0,NOTE_F4, 0,NOTE_G4, NOTE_G4, NOTE_F4, NOTE_D4, NOTE_C4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_E4, NOTE_E4, NOTE_E4};
    uint32_t durees[] = {500, 50, 500, 50, 500, 50 ,500, 500, 500, 500, 500, 500, 500, 1000, 1000, 1000, 1000};
    _async_puts(c);
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

        
        // if(!note_jouee && index < 17){
        //     play_note(notes[index], durees[index]);
        //     note_jouee = true;
        // }

    
    }
    return 0;
}
