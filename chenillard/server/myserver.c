/*
 * myserver.c - Serveur TCP de contrôle du chenillard (sur la Raspberry Pi)
 *
 * Usage : sudo ./myserver [port]
 *
 * Déroulement pour chaque client :
 *   1. reçoit 'c'  (demande de disponibilité)
 *   2. attend l'appui sur le bouton, puis envoie 'd'
 *   3. reçoit 'y'  et lance le chenillard sur les LEDs
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../bsp/bsp.h"
#include "../protocole.h"

static int sock_ecoute = -1;

/* Ctrl+C : on éteint les LEDs et on ferme proprement */
static void arret(int sig)
{
    (void)sig;
    printf("\nArrêt du serveur.\n");
    if (sock_ecoute >= 0)
        close(sock_ecoute);
    bsp_close();
    exit(0);
}

/* Reçoit exactement un caractère. Retourne 1 si OK, 0 si le client s'est déconnecté. */
static int recevoir(int sock, char *c)
{
    ssize_t n = recv(sock, c, 1, 0);
    return n == 1;
}

static void traiter_client(int sock)
{
    char msg;

    /* Message 1 : 'c' */
    if (!recevoir(sock, &msg)) {
        printf("Client déconnecté.\n");
        return;
    }
    if (msg != MSG_DEMANDE) {
        printf("Message inattendu '%c' (attendu '%c').\n", msg, MSG_DEMANDE);
        return;
    }
    printf("Reçu '%c' : le client demande si le chenillard est disponible.\n", msg);

    /* Message 2 : 'd' après appui sur le bouton */
    printf("Appuyez sur le bouton pour confirmer la disponibilité...\n");
    bsp_attendre_bouton();
    msg = MSG_DISPO;
    if (send(sock, &msg, 1, 0) != 1) {
        perror("send");
        return;
    }
    printf("Envoyé '%c' : service disponible.\n", msg);

    /* Message 3 : 'y' */
    if (!recevoir(sock, &msg)) {
        printf("Client déconnecté.\n");
        return;
    }
    if (msg != MSG_LANCER) {
        printf("Le client n'a pas lancé le chenillard (reçu '%c').\n", msg);
        return;
    }
    printf("Reçu '%c' : lancement du chenillard.\n", msg);
    bsp_chenillard(3, 100);
    printf("Chenillard terminé.\n");
}

int main(int argc, char *argv[])
{
    int port = (argc > 1) ? atoi(argv[1]) : PORT_DEFAUT;

    if (bsp_init() != 0)
        return EXIT_FAILURE;
    signal(SIGINT, arret);

    /* Création de la socket TCP */
    sock_ecoute = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_ecoute < 0) {
        perror("socket");
        bsp_close();
        return EXIT_FAILURE;
    }

    /* Permet de relancer le serveur tout de suite sur le même port */
    int opt = 1;
    setsockopt(sock_ecoute, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in adr = {0};
    adr.sin_family      = AF_INET;
    adr.sin_addr.s_addr = htonl(INADDR_ANY);   /* écoute sur toutes les interfaces */
    adr.sin_port        = htons(port);

    if (bind(sock_ecoute, (struct sockaddr *)&adr, sizeof(adr)) < 0) {
        perror("bind");
        arret(0);
    }
    if (listen(sock_ecoute, 1) < 0) {
        perror("listen");
        arret(0);
    }
    printf("Serveur en écoute sur le port %d (Ctrl+C pour quitter).\n", port);

    /* Un client à la fois, en boucle */
    while (1) {
        struct sockaddr_in adr_client;
        socklen_t taille = sizeof(adr_client);

        int sock_client = accept(sock_ecoute, (struct sockaddr *)&adr_client, &taille);
        if (sock_client < 0) {
            perror("accept");
            continue;
        }
        printf("\nClient connecté : %s\n", inet_ntoa(adr_client.sin_addr));

        traiter_client(sock_client);

        close(sock_client);
        printf("Connexion fermée. En attente d'un nouveau client...\n");
    }
}
