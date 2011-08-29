
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


#include <stdlib.h>
#include <stdio.h>
#include "socket.h"
#include "divers.h"
#include "manipulation_dossiers.h"

void envoi (SOCKET csock, char *adresse_in){											// Fonction d'envoi par le reseau local
	void *buffer = malloc(TAILLE_BUFFER);
	int temp_taille = 0, test_fin = 0, taille = 0, temp_test_fin = 0, sock_err = 0, temp_taille_nom = 0, taille_nom = 0;     // Declaration des variables
	char fin = 0, *nom_fichier = NULL;
	FILE *fichier = NULL;

	fichier = fopen(adresse_in, "rb");
	if (fichier == NULL){
		stop("Erreur a l'ouverture du fichier d'entree", EXIT_FAILURE);
	}

    nom_fichier = cheminToNomFichier(adresse_in);										// Extraction du nom du fichier de l'adresse d'origine
	
	if (nom_fichier == NULL){
		stop("NULL", EXIT_FAILURE);
	
	}
    taille_nom = strlen(nom_fichier) + 1;												// Envoi du nom du fichier
	temp_taille_nom = htonl(taille_nom);
    send(csock, (char*)&temp_taille_nom, sizeof(int), 0);
    send(csock, nom_fichier, taille_nom * sizeof(char), 0);


    sock_err = recv(csock, &fin, sizeof(char), 0);

    if (sock_err == SOCKET_ERROR){
			stop("erreur a la reception du dernier octet\n", EXIT_FAILURE);
    }

	printf("envoi de %s en cours\n", adresse_in);

	do {																																	// |
		test_fin = fread(buffer, 1, TAILLE_BUFFER, fichier);							// Lecture du fichier par bloc de 1024 octets		// |
		temp_test_fin = htonl (test_fin);																									// |
		send(csock, (char*)&temp_test_fin, sizeof(int), 0);																					// |
		send(csock, (char*)buffer, TAILLE_BUFFER, 0);									// Envoi du fichier par bloc de 1024 octets			// |	Boucle d'envoi
		sock_err = recv(csock, &fin, sizeof(char), 0);																						// |
		if (sock_err == SOCKET_ERROR){																										// |
			stop("erreur a la reception du dernier octet\n", EXIT_FAILURE);																	// |
		}																																	// |
	} while (test_fin == TAILLE_BUFFER);																									// |

	printf("envoi termine\n");
	printf("--------------------------------------------------------------------------------\n");

	free(buffer);																		// Liberation de l'espace m�moire
	free(nom_fichier);
	fclose(fichier);
}

void reception (SOCKET sock, char *adresse_out){										// Fonction de reception par le reseau local
	void *buffer = malloc(TAILLE_BUFFER);
	int temp_taille = 0, test_fin = 0, taille = 0, sock_err = 0, temp_test_fin = 0, temp_taille_nom = 0, taille_nom = 0;		// D�claration des variables
	char fin = 0, *nom_fichier = NULL, *fichier_sortie = NULL;
	FILE *fichier = NULL;

	sock_err = recv(sock, (char*)&temp_taille_nom, sizeof(int), 0);						// R�ception du nom du fichier
	if (sock_err == SOCKET_ERROR){
		stop("erreur a la reception de la taille de la chaine contenant le nom du fichier", EXIT_FAILURE);
	}
	taille_nom = ntohl(temp_taille_nom);
	
	nom_fichier = malloc(taille_nom * sizeof(char));
	sock_err = recv(sock, nom_fichier, taille_nom * sizeof(char), 0);
	if (sock_err == SOCKET_ERROR){
		stop("erreur a la reception de la chaine contenant le nom du fichier", EXIT_FAILURE);
	}

	send(sock, &fin, sizeof(char), 0);													// Synchronisation avec le serveur

	if (is_dir(adresse_out)){															// Si l'adresse de sortie est un dossier
		fichier_sortie = malloc(strlen(adresse_out) + strlen(nom_fichier) + 2);
		strcpy(fichier_sortie, adresse_out);											// On concat�ne le nom du fichier et l'adresse de sortie et on cr�e le fichier correspondant � l'adresse r�sultante
		if (fichier_sortie[strlen(fichier_sortie)] != '\\'){
			strcat(fichier_sortie, "\\");
		}
		strcat(fichier_sortie, nom_fichier);
		fichier = fopen(fichier_sortie, "wb+");
		if (fichier == NULL){
			stop("Erreur a l'ouverture du fichier de sortie (dossier)", EXIT_FAILURE);
		}
	} else {																			// Sinon, on ouvre juste l'adresse de sortie
		fichier = fopen(adresse_out, "wb+");
		if (fichier == NULL){
			stop("Erreur a l'ouverture du fichier de sortie (fichier)", EXIT_FAILURE);
		}
	}

	printf("reception de %s en cours\n", (is_dir(adresse_out)) ? fichier_sortie : adresse_out);

	do {																																	// |
		test_fin = 0;																														// |
		sock_err = recv(sock, (char*)&temp_test_fin, sizeof(test_fin), 0);																	// |
		if (sock_err == SOCKET_ERROR){																										// |
			stop("erreur � la reception du nombre d'octets lus\n", EXIT_FAILURE);															// |
		}																				// Lecture du fichier par bloc dse 1024 octets		// |
		test_fin = ntohl(temp_test_fin);																									// |
		sock_err = recv(sock, (char*)buffer, TAILLE_BUFFER, 0);																				// |	Boucle de r�ception
		if (sock_err == SOCKET_ERROR){																										// |
			stop("erreur a la reception du fichier\n", EXIT_FAILURE);																		// |
		}																																	// |
		fwrite(buffer, 1, test_fin, fichier);																								// |
		send(sock, &fin, sizeof(fin), 0);												// Ecriture du fichier par bloc de 1024 octets		// |
	} while (test_fin == TAILLE_BUFFER);																									// |
	printf("reception terminee\n");
	printf("--------------------------------------------------------------------------------\n");

	free(buffer);
	free(fichier_sortie);
	free(nom_fichier);
	fclose(fichier);
}

void denvoi (SOCKET csock, char (*fichiers)[MAX_TAILLE_ADRESSE], int nb_fichiers){
	int temp = htonl(nb_fichiers), i = 1, temp_taille_chaine, taille_chaine_adresse = (strlen(*fichiers) + 2) * sizeof(char), dossier_fichier = 0;
	char *adresse_absolu_in = NULL, fin;

	adresse_absolu_in = malloc(taille_chaine_adresse);
	send(csock, (char*) &temp, sizeof(int), 0);											// Envoi du nombre de fichiers qui seront envoy�s

	for (i = 1 ; i < nb_fichiers ; i++){												// i est initialis� a un car fichiers[0] contient l'adresse d'entr�e
		adresse_absolu_in = realloc(adresse_absolu_in, taille_chaine_adresse + (strlen(fichiers[i]) * sizeof(char)));
		strcpy(adresse_absolu_in, *fichiers);											// Cr�ation de l'adresse absolue du fichier d'entr�e 
		if (adresse_absolu_in[strlen(adresse_absolu_in)] != '\\'){
			strcat(adresse_absolu_in, "\\");
		}
		strcat(adresse_absolu_in, fichiers[i]);
		temp_taille_chaine = htonl(strlen(fichiers[i]));								// Envoi de l'adresse relative au dossier racine
		send(csock, (char*) &temp_taille_chaine, sizeof(int), 0);
		send(csock, fichiers[i], (strlen(fichiers[i]) + 1) * sizeof(char), 0);

		recv(csock, &fin, sizeof(char), 0);												// Synchronisation avec le client
		if ((is_dir(adresse_absolu_in))){												// Envoi de l'information l'�l�ment est un fichier/un dossier
			dossier_fichier = 0;
			send(csock, (char*)&dossier_fichier, sizeof(int), 0);
		} else {
			dossier_fichier = 1;
			send(csock, (char*)&dossier_fichier, sizeof(int), 0);
			envoi(csock, adresse_absolu_in);	
		}
	}
}

void dreception (SOCKET sock, char *adresse_out){
	int temp = 0, i = 0, nb_fichiers = 0, taille_chaine, temp_taille_chaine, sock_err = 0, dossier_fichier = 0;
	char absolu_temp_adresse_out [MAX_TAILLE_ADRESSE], *nom_fichier = NULL, fin;

	recv(sock, (char*) &temp, sizeof(int), 0);											// Reception du nombre de fichiers a lire
	nb_fichiers = ntohl(temp);

	for (i = 0 ; i < nb_fichiers - 1; i++){
		recv(sock, (char*) &temp_taille_chaine, sizeof(int), 0);						// Reception du nom du fichier actuel aisi que de sa place dans l'arborescence
		taille_chaine = ntohl(temp_taille_chaine);
		nom_fichier = malloc((taille_chaine + 1) * sizeof(char));
		recv(sock, nom_fichier, (taille_chaine + 1) * sizeof(char), 0);

		send(sock, &fin, sizeof(char), 0);												// Synchronisation avec le serveur

		strcpy(absolu_temp_adresse_out, adresse_out);									// Cr�ation de l'adresse de sortie
		if (absolu_temp_adresse_out[strlen(absolu_temp_adresse_out)] != '\\'){
			strcat(absolu_temp_adresse_out, "\\");
		}
		strcat(absolu_temp_adresse_out, nom_fichier);

		sock_err = recv(sock, (char*)&dossier_fichier, sizeof(int), 0);					// R�ception de l'information l'�l�ment est un fichier/un dossier
		if (sock_err == SOCKET_ERROR){
			stop("Erreur a la reception de la variable switch reception dossier/fichier", EXIT_FAILURE);
		}
		if (dossier_fichier){															// Si l'�l�ment est un fichier, on le re�oit
			reception(sock, absolu_temp_adresse_out);
		} else {																		// Sinon, si c'est un dossier, on cr�e le dossier de sortie
                        mkdir(absolu_temp_adresse_out);
		}
	}
}
