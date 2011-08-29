
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

#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<string.h>
#include"copie.h"
#include"divers.h"
#include"transmission.h"
#include"manipulation_dossiers.h"
#include"socket.h"

int main (int argc, char *argv[]){
	FILE *fichier_in = NULL;																			   /* D�claration */
	int taille = 0, continuer = 0, nb_lus = 0, choix_menu = 0, dossier_fichier;
	short int sock_err = 0, err = 0;                													 /* des variables */
	short int type_machine = SERVEUR;
	double temp = 0;																					  /* g�n�rales */
	char adresse_in[500], adresse_out[500], ip_serveur[16], (*fichiers)[MAX_TAILLE_ADRESSE] = NULL;
	char client_connecte;
	
	SOCKET sock;
	SOCKADDR_IN sin;												/* D�claration des varibles */
	socklen_t taille_serveur = sizeof(sin);

	SOCKET csock;
	SOCKADDR_IN csin;												  /* sp�cifiques socket */
	socklen_t taille_client = sizeof(csin);
	
	do {

		memset(ip_serveur, 0, sizeof(ip_serveur));
		memset(adresse_in, 0, sizeof(adresse_in));						/* Initialisation des tableaux */
		memset(adresse_out, 0, sizeof(adresse_out));
                
		printf("Transfert_fichiers V1.2\n\n1. Copie de fichier en interne\n2. Copie de fichier via reseau local\n\nVotre choix : ");
		scanf("%d", &choix_menu);																												// Menu principal
		printf("\n");
		switch(choix_menu){
			case 1 :																						// Copie interne
				fflush(stdin);
				if (argc > 1){
					err = (short) verif_dossier(argv[0]);
					if (err == FAILURE){
						stop("Erreur a la verification du fichier entre en parametre", EXIT_FAILURE);
					}
					strcpy(adresse_in, argv[1]);
				} else {
					printf("Tapez l'adresse d'entree : ");
					fgets(adresse_in, 500, stdin);
					adresse_in[strlen(adresse_in)-1] = '\0';
				}	
				printf("Tapez l'adresse de sortie : ");														// Demande emplacement fichiers entr�e/sortie
				fgets(adresse_out, 500, stdin);
				adresse_out[strlen(adresse_out)-1] = '\0';
				if (is_dir(adresse_in)){
					temp = clock();
					nb_lus = lire_dossier(adresse_in, "", &fichiers, 1, 0);									// Condition si dossier
					dcopie(fichiers, adresse_in, adresse_out, nb_lus);
					printf("\ndossier copie en %f secondes\n", (double)(clock()-temp)/CLOCKS_PER_SEC);
				} else {
					fichier_in = fopen(adresse_in, "rb");
					if (fichier_in == NULL) {
						stop("erreur a l'ouverture du fichier\n", EXIT_FAILURE);							// Gestion erreur fichier
					}
					fseek(fichier_in, 0, SEEK_END);
					taille = ftell(fichier_in);
					rewind(fichier_in);
					printf("le fichier fait %d octets\n", taille);
					fclose(fichier_in);
					temp = clock();																			// Indication de l'�tat du processus de copie
					copie(adresse_in, adresse_out);
					printf("\nfichier copie en %f secondes\n", (double)(clock()-temp)/CLOCKS_PER_SEC);
					fclose(fichier_in);
				}
			break;
			case 2 :																						// Copie reseau local
				printf("Mode :\n1. Serveur\n2. Client\n\nVotre choix : ");
				scanf("%d", &type_machine);
				fflush(stdin);

				if (type_machine == CLIENT){
					printf("Entrez l'IP locale du serveur : ");
					fgets(ip_serveur, 16, stdin);
					ip_serveur[strlen(ip_serveur)-1] = '\0';	
					printf("entrez l'adresse du fichier de sortie : ");										// Configuration client (IP serveur et fichier de sortie)
					fgets(adresse_out, 500, stdin);	
					adresse_out[strlen(adresse_out)-1] = '\0';
				} else if (type_machine == SERVEUR){
					if (argc > 1){
						verif_dossier(argv[0]);
						strcpy(adresse_in, argv[1]);
					} else {
						printf("entrez l'adresse du fichier d'entree : ");
						fgets(adresse_in, 500, stdin);														// Configuration serveur (fichier d'entr�e)
						adresse_in[strlen(adresse_in)-1] = '\0';
					}
				}
				
				sock_err = (short)initSocket(&sock, &sin, taille_serveur, 2500, type_machine, ip_serveur);	// Initialisation socket

				if (type_machine == SERVEUR){																// Si l'�l�ment � copier est un dossier		// |
					csock = accept(sock, (SOCKADDR*)&csin, &taille_client);																				// |
					printf("un client s'est connecte\n");																								// |
					if (is_dir(adresse_in)){																											// |
						nb_lus = lire_dossier(adresse_in, "", &fichiers, 1, 0);																			// |
						dossier_fichier = 0;																											// |
						send(csock, (char*)&dossier_fichier, sizeof(int), 0);																			// |
						denvoi(csock, fichiers, nb_lus);																								// | Processus envoi serveur
					} else {																				// Sinon, si c'est un fichier				// |
						temp = clock();																													// |
						dossier_fichier = 1;																											// |
						send(csock, (char*)&dossier_fichier, sizeof(int), 0);								 											// |
						envoi(csock, adresse_in);																										// |
						printf("\nfichier envoye en %f secondes\n", (double)(clock()-temp)/CLOCKS_PER_SEC);												// |
					}																						// ----------------------------------------------------------------------------
				} else if (type_machine == CLIENT){																										// |
					sock_err = (short int) connect(sock, (SOCKADDR*)&sin, taille_serveur);																// |
					if (sock_err == SOCKET_ERROR){																										// |
						stop("La connexion avec le serveur n'a pas pu etre etablie", EXIT_FAILURE);														// |
					} else {																															// |
						printf("la connexion a ete etablie avec succes\n");																				// |
					}																																	// |
																																						// |
					sock_err = (short)recv(sock, (char*)&dossier_fichier, sizeof(int), 0);																// |
					if (sock_err == SOCKET_ERROR){																										// | Processus r�ception client
						stop("Erreur a la reception de la variable switch reception dossier/fichier", EXIT_FAILURE);									// |
					}																																	// |
					temp = clock();																			// Si l'�l�ment � copier est un dossier		// |
					if (dossier_fichier){																												// |
						reception(sock, adresse_out);																									// |
						printf("\nfichier recu en %f secondes\n", (double)(clock()-temp)/CLOCKS_PER_SEC);												// |
					} else {																				// Sinon, si c'est un fichier				// |
						dreception(sock, adresse_out);																									// |
						printf("\ndossier recu en %f secondes\n", (double)(clock()-temp)/CLOCKS_PER_SEC);												// |
					}																						// -----------------------------------------------------------------------------
				}
			break;
			case 3 :																						// Copie reseau local
				printf("Mode :\n1. Serveur\n2. Client\n\nVotre choix : ");
				scanf("%d", &type_machine);
				fflush(stdin);

				if (type_machine == CLIENT){
					printf("Entrez l'IP de la paserelle : ");
					fgets(ip_serveur, 16, stdin);
					ip_serveur[strlen(ip_serveur)-1] = '\0';	
					printf("entrez l'adresse du fichier de sortie : ");										// Configuration client (IP serveur et fichier de sortie)
					fgets(adresse_out, 500, stdin);	
					adresse_out[strlen(adresse_out)-1] = '\0';
				} else if (type_machine == SERVEUR){
					printf("Entrez l'IP de la paserelle : ");
					fgets(ip_serveur, 16, stdin);
					ip_serveur[strlen(ip_serveur)-1] = '\0';	
					if (argc > 1){
						verif_dossier(argv[0]);
						strcpy(adresse_in, argv[1]);
					} else {
						printf("entrez l'adresse du fichier d'entree : ");
						fgets(adresse_in, 500, stdin);														// Configuration serveur (fichier d'entr�e)
						adresse_in[strlen(adresse_in)-1] = '\0';
					}
				}
				
				sock_err = (short)initSocket(&sock, &sin, taille_serveur, 2500, CLIENT, ip_serveur);		// Initialisation socket

				if (type_machine == SERVEUR){																// Si l'�l�ment � copier est un dossier		// |
					sock_err = (short)connect(sock, (SOCKADDR*)&sin, taille_serveur);																	// |
																																						// |
					if (sock_err == SOCKET_ERROR){																										// |
						stop("La connexion avec le serveur n'a pas pu etre etablie", EXIT_FAILURE);														// |
					} else {																															// |
						printf("la connexion a ete etablie avec succes\n");																				// |
					}																																	// |
																																						// |
					if (is_dir(adresse_in)){																											// |
						nb_lus = lire_dossier(adresse_in, "", &fichiers, 1, 0);																			// |
						dossier_fichier = 0;																											// |
						while (recv(sock, &client_connecte, sizeof(char), 0) == SOCKET_ERROR);															// |
						if (!(client_connecte)) puts("yess");
						printf("un client s'est connecte\n");																							// |
						send(sock, (char*)&dossier_fichier, sizeof(int), 0);																			// |
						denvoi(sock, fichiers, nb_lus);																								// | Processus envoi serveur
					} else {																				// Sinon, si c'est un fichier				// |
						temp = clock();																													// |
						dossier_fichier = 1;																											// |
						while (recv(sock, &client_connecte, sizeof(char), 0) == SOCKET_ERROR);															// |
						if (!(client_connecte)) puts("yess");
						printf("un client s'est connecte\n");																							// |
						send(sock, (char*)&dossier_fichier, sizeof(int), 0);								 											// |
						envoi(sock, adresse_in);																										// |
						printf("\nfichier envoye en %f secondes\n", (double)(clock()-temp)/CLOCKS_PER_SEC);												// |
					}																						// ----------------------------------------------------------------------------
				} else if (type_machine == CLIENT){																										// |
					sock_err = (short int) connect(sock, (SOCKADDR*)&sin, taille_serveur);																// |
					if (sock_err == SOCKET_ERROR){																										// |
						stop("La connexion avec le serveur n'a pas pu etre etablie", EXIT_FAILURE);														// |
					} else {																															// |
						printf("la connexion a ete etablie avec succes\n");																				// |
					}																																	// |
																																						// |
					while (recv(sock, &client_connecte, sizeof(char), 0) == SOCKET_ERROR);																// |
																																						// |
					sock_err = (short)recv(sock, (char*)&dossier_fichier, sizeof(int), 0);																// |
					if (sock_err == SOCKET_ERROR){																										// | Processus r�ception client
						stop("Erreur a la reception de la variable switch reception dossier/fichier", EXIT_FAILURE);									// |
					}																																	// |
					temp = clock();																			// Si l'�l�ment � copier est un dossier		// |
					if (dossier_fichier){																												// |
						reception(sock, adresse_out);																									// |
						printf("\nfichier recu en %f secondes\n", (double)(clock()-temp)/CLOCKS_PER_SEC);												// |
					} else {																				// Sinon, si c'est un fichier				// |
						dreception(sock, adresse_out);																									// |
						printf("\ndossier recu en %f secondes\n", (double)(clock()-temp)/CLOCKS_PER_SEC);												// |
					}																				// -----------------------------------------------------------------------------
				}
			break;
			default :
				printf("entrez un nombre entre 1 et 2\n");													// Gestion des erreurs du choix du menu principal
			break;
		}																									
		printf("Voulez vous :\n1. Continuer\n2. Arreter\nVotre choix : ");
		scanf("%d", &continuer);
		shutdown(sock, 2);
		shutdown(csock, 2);																					// Liberation de l'espace memoire et fin du programme
#ifdef WIN32
		WSACleanup();
#endif
		free(fichiers);
	} while (continuer == 1);
	return EXIT_SUCCESS;
}
