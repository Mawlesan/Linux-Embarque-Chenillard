/*
 * protocole.h - Messages échangés entre le client et le serveur
 *
 *   client  --- 'c' --->  serveur   : le service chenillard est-il disponible ?
 *   client  <--- 'd' ---  serveur   : oui (envoyé quand le bouton est appuyé)
 *   client  --- 'y' --->  serveur   : lance le chenillard
 */
#ifndef PROTOCOLE_H
#define PROTOCOLE_H

#define PORT_DEFAUT   5000

#define MSG_DEMANDE   'c'
#define MSG_DISPO     'd'
#define MSG_LANCER    'y'

#endif
