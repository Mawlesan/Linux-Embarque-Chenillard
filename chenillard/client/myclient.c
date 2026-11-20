/*
 * myclient.c - Client TCP qui demande le chenillard au serveur
 *
 * Usage : ./myclient <adresse_ip_du_serveur> [port]
 *
 * Déroulement :
 *   1. envoie 'c'  (le chenillard est-il disponible ?)
 *   2. attend 'd'  (le serveur confirme quand on appuie sur son bouton)
 *   3. envoie 'y'  pour lancer le chenillard
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../protocole.h"

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage : %s <adresse_ip_du_serveur> [port]\n", argv[0]);
        return EXIT_FAILURE;
    }
    const char *ip = argv[1];
    int port = (argc > 2) ? atoi(argv[2]) : PORT_DEFAUT;

    /* Création de la socket TCP */
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in adr = {0};
    adr.sin_family = AF_INET;
    adr.sin_port   = htons(port);
    if (inet_pton(AF_INET, ip, &adr.sin_addr) != 1) {
        fprintf(stderr, "Adresse IP invalide : %s\n", ip);
        close(sock);
        return EXIT_FAILURE;
    }

    /* Connexion au serveur */
    if (connect(sock, (struct sockaddr *)&adr, sizeof(adr)) < 0) {
        perror("connect");
        close(sock);
        return EXIT_FAILURE;
    }
    printf("Connecté au serveur %s:%d\n", ip, port);

    /* Message 1 : 'c' */
    char msg = MSG_DEMANDE;
    if (send(sock, &msg, 1, 0) != 1) {
        perror("send");
        close(sock);
        return EXIT_FAILURE;
    }
    printf("Envoyé '%c' : le chenillard est-il disponible ?\n", msg);

    /* Message 2 : attente de 'd' */
    printf("En attente de la confirmation du serveur (appui sur le bouton)...\n");
    if (recv(sock, &msg, 1, 0) != 1) {
        printf("Le serveur a fermé la connexion.\n");
        close(sock);
        return EXIT_FAILURE;
    }
    if (msg != MSG_DISPO) {
        printf("Réponse inattendue '%c'.\n", msg);
        close(sock);
        return EXIT_FAILURE;
    }
    printf("Reçu '%c' : le chenillard est disponible.\n", msg);

    /* Message 3 : 'y' */
    msg = MSG_LANCER;
    if (send(sock, &msg, 1, 0) != 1) {
        perror("send");
        close(sock);
        return EXIT_FAILURE;
    }
    printf("Envoyé '%c' : lancement du chenillard demandé.\n", msg);

    close(sock);
    return EXIT_SUCCESS;
}
