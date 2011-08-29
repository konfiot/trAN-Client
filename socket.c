
/*Copyright (C) 2011 TOUSSAINT Arthur

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.*/


#include <stdio.h>
#include <stdlib.h>
#include "socket.h"
#include "divers.h"

int initSocket(SOCKET *sock, SOCKADDR_IN *sin, socklen_t taille, short int port, short int type_machine, char adresse_ip[]){	// Fonction d'initialisation de la transmission
#ifdef WIN32
	WSADATA WSAData;
	int erreur = WSAStartup(MAKEWORD(2,2), &WSAData);											// Initialisation pour bibliotheque windows
#else
    int erreur = 0;
#endif
	
	int sock_err = 0;
    
	if (erreur != 0){
        stop("erreur � l'initialisation de la socket", EXIT_FAILURE);
    }

	*sock = socket(AF_INET, SOCK_STREAM, 0);													// Initialisation de la socket
	sin->sin_family = AF_INET;																	// Contexte d'adressage
	sin->sin_port = htons(port);
	if (type_machine == SERVEUR){
		sin->sin_addr.s_addr = htonl(INADDR_ANY);												// Contexte d'adressage specifique serveur
		sock_err = bind(*sock, (SOCKADDR*)sin, taille);											// liaison du contexte d'adressage et de la socket
		if (sock_err == SOCKET_ERROR){															// Gestion des erreurs
			stop("erreur a l'initialisation de la socket serveur", EXIT_FAILURE);
		}
		sock_err = listen(*sock, 5);
		if (sock_err == SOCKET_ERROR){															// Gestion des erreurs
			stop("erreur a la mise en etat d'ecoute de la socket serveur", EXIT_FAILURE);
		} else {
			printf("socket serveur initialisee\n");
		}
	} else if (type_machine == CLIENT){
		sin->sin_addr.s_addr = inet_addr(adresse_ip);											// Contexte d'adressage sp�cifique client
		printf("socket client initialisee\n");
	}
	return sock_err;
}
