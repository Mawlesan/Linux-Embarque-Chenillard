CC     = gcc
CFLAGS = -Wall -Wextra -O2

all: myserver myclient

# Serveur : à compiler sur la Raspberry Pi (bibliothèque bcm2835 requise)
myserver: server/myserver.c bsp/bsp.c bsp/bsp.h protocole.h
	$(CC) $(CFLAGS) -o $@ server/myserver.c bsp/bsp.c -lbcm2835

# Client : compile sur n'importe quel Linux
myclient: client/myclient.c protocole.h
	$(CC) $(CFLAGS) -o $@ client/myclient.c

clean:
	rm -f myserver myclient

.PHONY: all clean
