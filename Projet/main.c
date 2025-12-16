#include "sys/clock.h"
#include <libopencm3/cm3/scb.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/syscfg.h>
#include <libopencm3/stm32/usart.h>
#include <libopencmsis/core_cm3.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef USE_RAW_REGISTER
#define USE_RAW_REGISTER 1
#endif

volatile uint32_t tick=0;
volatile uint32_t frequence=1000;


#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523

// Notes supplémentaires pour Mario
#define NOTE_C5 523
#define NOTE_E5 659
#define NOTE_G5 784
#define NOTE_PAUSE 0 // Pour les silences

// --- Fréquences de la gamme (Octave 4 et 5) ---
#define NOTE_C4 262
// ... (Autres notes de l'Octave 4 inchangées)
#define NOTE_A4 440
#define NOTE_B4 494

// Notes nécessaires pour "Pour Élise"
#define NOTE_C5 523
#define NOTE_D5 587 // Nouveau
#define NOTE_DS5 622 // D#5, Nouveau
#define NOTE_E5 659
#define NOTE_G5 784 
#define NOTE_PAUSE 0


// --- Fréquences de la gamme (Octave 3, 4 et 5) ---
#define NOTE_F3 175 // Nouveau - Basse
#define NOTE_G3 196 // Nouveau - Basse

#define NOTE_C4 262
// ... (Autres notes de l'Octave 4 inchangées)
#define NOTE_A4 440
#define NOTE_B4 494

// ... (Notes C5, D5, DS5, E5, G5)
#define NOTE_PAUSE 0

#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494

#define NOTE_C5 523 // Pour les notes aiguës
#define NOTE_PAUSE 0


// --- Mélodie de Star Wars (Début du Main Theme) ---
uint32_t starwars_melody[] = {
    NOTE_C4, NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C5, 
    NOTE_G4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C5, NOTE_G4
};

uint32_t starwars_duration[] = {
    500, 500, 125, 125, 125, 500, 
    250, 125, 125, 125, 500, 250
};

uint32_t melody_length = sizeof(starwars_melody) / sizeof(uint32_t);





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
    /* Le fait de définir cette fonction suffit pour
     * qu'elle soit utilisée comme traitant,
     * cf les fichiers de compilation et d'édition de lien
     * pour plus de détails.
     */
     /*
    tick++;
    if (tick >= frequence) {   
        GPIOA_ODR  ^= (1<<8);
        //GPIOA_ODR  ^= (1<<9);
        GPIOA_ODR  ^= (1<<10);
        tick=0;
    }
    *//*
    GPIOA_ODR  ^= (1<<8);
    GPIOA_ODR  ^= (1<<9);
    GPIOA_ODR  ^= (1<<10);
    */
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
    EXTI_PR |= (1 << 13); /* Clear pending register EXTIPR[13] */
#else
    exti_reset_request(EXTI13);
#endif
    mode = mode ^ 1; /* switch the mode value 0 <-> 1 */
}

__attribute__((optimize("O0"))) void delay_ms(volatile uint32_t ms) {
    // Cette valeur doit être ajustée en fonction de la vitesse d'horloge
    // et de l'optimisation du compilateur.
    // Pour une fréquence d'horloge de 100MHz (typique F446), une valeur
    // de ~10000 donne environ 1ms.
    volatile uint32_t cycles_per_ms = get_SYSCLK() / 100000; // Ajustement grossier pour l'exemple
    volatile uint32_t i, j;
    for (i = 0; i < ms; i++) {
        for (j = 0; j < cycles_per_ms; j++) {
            __asm__("nop"); // ou simplement ;
        }
    }
}


// Fonction pour jouer une note avec une fréquence et une durée
__attribute__((optimize("O0"))) void play_note(uint32_t frequency, uint32_t duration_ms) {
    if (frequency == 0) { // Pour une pause
        GPIOB_ODR  &=  ~(1<<9);
        delay_ms(duration_ms);
        return;
    }

    // Le demi-période en microsecondes (pour basculer le pin)
    // Period (s) = 1 / frequency
    // Half Period (µs) = (1 / (2 * frequency)) * 1,000,000
    uint32_t half_period_us = 500000 / frequency;
    
    // Le nombre total de cycles pour la durée
    // Total Cycles = duration_ms / (Period_ms)
    uint32_t num_cycles = (duration_ms * frequency) / 1000;

    // Ajustement de la durée de la micro-boucle pour une horloge rapide
    // Ce calage est très dépendant de l'horloge et de l'optimisation, 
    // l'idéal serait d'utiliser un timer périphérique.
    uint32_t delay_count = half_period_us * (get_SYSCLK() / 10000000); 

    for (uint32_t i = 0; i < num_cycles; i++) {
        // État Haut (High)
        GPIOB_ODR  |= (1<<9);
        for(volatile uint32_t k = 0; k < delay_count; k++);

        // État Bas (Low)
        GPIOB_ODR  &= ~(1<<9);
        for(volatile uint32_t k = 0; k < delay_count; k++);
    }
    
    // S'assurer que le buzzer s'éteint après la note
    GPIOB_ODR  &= ~(1<<9);
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

    //systick_init((uint32_t)1000);

    init_BP();
    init_BZ();
        
    bool tmp;
    uint32_t tempo = 250;

    //printf("c?\n\r");
    //char c;
    //c = _getc();
    //_putc(c);
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

        // Jouer la gamme montante
        // play_note(NOTE_C4, tempo);
        // play_note(NOTE_D4, tempo);
        // play_note(NOTE_E4, tempo);
        // play_note(NOTE_F4, tempo);
        // play_note(NOTE_G4, tempo);
        // play_note(NOTE_A4, tempo);
        // play_note(NOTE_B4, tempo);
        // play_note(NOTE_C5, tempo*2); // Le do aigu est plus long
        
        // // Petite pause
        // delay_ms(500);

        // // Optionnel: Jouer la gamme descendante
        // play_note(NOTE_C5, tempo);
        // play_note(NOTE_B4, tempo);
        // play_note(NOTE_A4, tempo);
        // play_note(NOTE_G4, tempo);
        // play_note(NOTE_F4, tempo);
        // play_note(NOTE_E4, tempo);
        // play_note(NOTE_D4, tempo);
        // play_note(NOTE_C4, tempo*2); // Le do grave est plus long

        // // Longue pause entre les répétitions
        // delay_ms(2000);
        
        for (uint32_t i = 0; i < melody_length; i++) {
            play_note(starwars_melody[i], starwars_duration[i]);
            // Petite pause de séparation
            delay_ms(10); 
        }

        // Longue pause avant de recommencer la boucle
        delay_ms(3000);

    }
    return 0;
}
