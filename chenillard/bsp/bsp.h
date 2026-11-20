/*
 * bsp.h - Board Support Package pour la carte à LEDs (Raspberry Pi)
 *
 * Couche d'abstraction matérielle : le reste du programme ne manipule
 * jamais directement les GPIO, il passe par ces fonctions.
 */
#ifndef BSP_H
#define BSP_H

/* Nombre de LEDs (leurs numéros GPIO sont définis dans bsp.c) */
#define BSP_NB_LEDS 8

/* GPIO du bouton poussoir (câblé entre le GPIO et la masse) */
#define BSP_BOUTON 26

int  bsp_init(void);                 /* initialise bcm2835 et les GPIO, 0 si OK */
void bsp_close(void);                /* éteint les LEDs et libère bcm2835 */
void bsp_led(int index, int etat);   /* allume (1) ou éteint (0) une LED */
void bsp_leds_off(void);             /* éteint toutes les LEDs */
int  bsp_bouton_appuye(void);        /* 1 si le bouton est appuyé */
void bsp_attendre_bouton(void);      /* bloque jusqu'à un appui (anti-rebond) */
void bsp_chenillard(int nb_tours, int delai_ms); /* lance le chenillard */

#endif
