# Chenillard client/serveur – Linux embarqué sur Raspberry Pi

Contrôle à distance d'un chenillard de LEDs sur **Raspberry Pi**, via une application **client/serveur TCP/IP** en **langage C**.
Projet de Linux embarqué réalisé en binôme en 4e année du cycle ingénieur (ING4) à l'**ECE Paris**.

## Le projet

Un serveur TCP tourne sur la Raspberry Pi et pilote une carte de 8 LEDs et un bouton poussoir.
Un client se connecte au serveur et demande le lancement du chenillard selon un protocole en 3 messages :

| Message | Sens | Signification |
|---|---|---|
| `c` | client → serveur | Le service chenillard est-il disponible ? |
| `d` | serveur → client | Oui (envoyé quand on appuie sur le bouton de la carte) |
| `y` | client → serveur | Lance le chenillard |

## Fonctionnalités

- Serveur TCP qui accepte les clients les uns après les autres
- Pilotage des GPIO avec la bibliothèque **bcm2835**
- Couche **BSP** (Board Support Package) qui isole le matériel : LEDs, bouton avec anti-rebond, chenillard aller-retour
- Arrêt propre avec Ctrl+C (LEDs éteintes, socket fermée)

## Organisation

```
bsp/        couche matérielle (LEDs, bouton, chenillard)
server/     serveur TCP (myserver)
client/     client TCP (myclient)
protocole.h messages et port communs au client et au serveur
```

## Compiler et lancer

Prérequis sur la Raspberry Pi : la [bibliothèque bcm2835](https://www.airspayce.com/mikem/bcm2835/).

```bash
make

# Sur la Raspberry Pi (sudo nécessaire pour accéder aux GPIO)
sudo ./myserver

# Sur le client (la Pi elle-même ou un autre PC Linux du réseau)
./myclient <adresse_ip_de_la_pi>
```

Le port par défaut est 5000. On peut en choisir un autre : `sudo ./myserver 6000` et `./myclient <ip> 6000`.

## Câblage

| Élément | GPIO (numérotation BCM) |
|---|---|
| LEDs 1 à 8 | 17, 18, 27, 22, 23, 24, 25, 5 |
| Bouton (vers la masse) | 26 |

Les numéros se modifient dans `bsp/bsp.c` (LEDs) et `bsp/bsp.h` (bouton).
