/*
 * bsp.c - Implémentation du BSP avec la bibliothèque bcm2835
 */
#include <bcm2835.h>
#include <stdio.h>
#include "bsp.h"

/* Numéros GPIO (numérotation BCM) des LEDs, de gauche à droite.
 * À adapter selon le câblage de la carte. */
static const int BSP_LEDS[BSP_NB_LEDS] = {17, 18, 27, 22, 23, 24, 25, 5};

int bsp_init(void)
{
    if (!bcm2835_init()) {
        fprintf(stderr, "Erreur : bcm2835_init a échoué (lancer avec sudo ?)\n");
        return -1;
    }

    /* LEDs en sortie, éteintes au départ */
    for (int i = 0; i < BSP_NB_LEDS; i++) {
        bcm2835_gpio_fsel(BSP_LEDS[i], BCM2835_GPIO_FSEL_OUTP);
        bcm2835_gpio_write(BSP_LEDS[i], LOW);
    }

    /* Bouton en entrée avec résistance de pull-up interne :
     * repos = HIGH, appuyé = LOW */
    bcm2835_gpio_fsel(BSP_BOUTON, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_set_pud(BSP_BOUTON, BCM2835_GPIO_PUD_UP);

    return 0;
}

void bsp_close(void)
{
    bsp_leds_off();
    bcm2835_close();
}

void bsp_led(int index, int etat)
{
    if (index < 0 || index >= BSP_NB_LEDS)
        return;
    bcm2835_gpio_write(BSP_LEDS[index], etat ? HIGH : LOW);
}

void bsp_leds_off(void)
{
    for (int i = 0; i < BSP_NB_LEDS; i++)
        bcm2835_gpio_write(BSP_LEDS[i], LOW);
}

int bsp_bouton_appuye(void)
{
    return bcm2835_gpio_lev(BSP_BOUTON) == LOW;
}

void bsp_attendre_bouton(void)
{
    /* Attente de l'appui, confirmé après 20 ms (anti-rebond) */
    while (1) {
        if (bsp_bouton_appuye()) {
            bcm2835_delay(20);
            if (bsp_bouton_appuye())
                break;
        }
        bcm2835_delay(10);
    }

    /* Attente du relâchement pour ne pas compter un appui deux fois */
    while (bsp_bouton_appuye())
        bcm2835_delay(10);
}

void bsp_chenillard(int nb_tours, int delai_ms)
{
    for (int t = 0; t < nb_tours; t++) {
        /* Aller : de la LED 0 à la dernière */
        for (int i = 0; i < BSP_NB_LEDS; i++) {
            bsp_led(i, 1);
            bcm2835_delay(delai_ms);
            bsp_led(i, 0);
        }
        /* Retour : de l'avant-dernière à la LED 1 */
        for (int i = BSP_NB_LEDS - 2; i > 0; i--) {
            bsp_led(i, 1);
            bcm2835_delay(delai_ms);
            bsp_led(i, 0);
        }
    }
    bsp_leds_off();
}
