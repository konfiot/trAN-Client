
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
#include<string.h>
#include"divers.h"
#include"manipulation_dossiers.h"

void copie(char *adresse_in, char *adresse_out){											// Fonction de copie interne
	void *buffer = NULL;
	int test_fin = 0;
	char *nom_fichier = NULL, *adresse_absolu_out = NULL;
	FILE *fichier_in = NULL, *fichier_out = NULL;

	if (is_dir(adresse_out)){																// Si l'adresse de sortie est celle d'un dossier
		nom_fichier = cheminToNomFichier(adresse_in);
		adresse_absolu_out = malloc(strlen(adresse_out) + strlen(nom_fichier) + 2);
		strcpy(adresse_absolu_out, adresse_out);											// On extrait le nom du fichier de l'adresse d'entr�e et on le concat�ne avec l'adresse de sortie
		if (adresse_absolu_out[strlen(adresse_absolu_out)] != '\\'){
			strcat(adresse_absolu_out, "\\");
		}
		strcat(adresse_absolu_out, nom_fichier);
		fichier_out = fopen(adresse_absolu_out, "wb+");
	} else {																				// Sinon, si c'est un fichier
		fichier_out = fopen(adresse_out, "wb+");											// On ouvre juste l'adresse de sortie
	}

	fichier_in = fopen(adresse_in, "rb");

	if (fichier_in == NULL){																// ouverture du fichier d'entr�e et gestion des erreurs
		stop("Erreur a l'ouverture du fichier d'entree", EXIT_FAILURE);
	}
	if (fichier_out == NULL){
		stop("Erreur a l'ouverture du fichier de sortie", EXIT_FAILURE);
	}	
	buffer = malloc(TAILLE_BUFFER);
	printf("copie de %s en cours\n", adresse_in);
	rewind(fichier_in);																		// Allocation du buffer et retour au debut des fichiers d'entr�e et de sortie
	rewind(fichier_out);
	do {																					// |
		test_fin = fread(buffer, 1, TAILLE_BUFFER, fichier_in);								// |	Copie par bloc de 1024
		fwrite(buffer, 1, test_fin, fichier_out);											// |
	} while (test_fin == TAILLE_BUFFER);													// |
	printf("copie terminee\n");
	printf("--------------------------------------------------------------------------------\n");
	fclose(fichier_in);
	fclose(fichier_out);																	// Liberation du buffer et retour au debut des fichiers d'entr�e et de sortie
	free(nom_fichier);
	free(adresse_absolu_out);
	free(buffer);
}

void dcopie(char (*fichiers)[MAX_TAILLE_ADRESSE], char *adresse_in, char *adresse_out, int nb_a_lire){
	char adresse_absolu_out [MAX_TAILLE_ADRESSE], adresse_absolu_in [MAX_TAILLE_ADRESSE];
	int i = 1;
	
	for (i = 1 ; i < nb_a_lire ; i++){
		strcpy(adresse_absolu_out, adresse_out);											// Copie des adresses pass�es en param�tres dans des variables modifiables
		strcpy(adresse_absolu_in, adresse_in);
		if (adresse_absolu_in[strlen(adresse_absolu_in)-1] != '\\'){
			adresse_absolu_in[strlen(adresse_absolu_in)+1] = '\0';							// Si l'adresse de sortie ne fini pas par un antislash, ou lui ajout un antislash (j'aurais aussi pu tout simplement faire un strcat
			adresse_absolu_in[strlen(adresse_absolu_in)] = '\\';
		}

		if (adresse_absolu_out[strlen(adresse_absolu_out)-1] != '\\'){
			adresse_absolu_out[strlen(adresse_absolu_out)+1] = '\0';						// M�me chose mais pour l'adresse de sortie
			adresse_absolu_out[strlen(adresse_absolu_out)] = '\\';
		}
		strcat(adresse_absolu_out, fichiers[i]);											// Ajout du nom du fichier aux adresses d'entr�e et de sortie
		strcat(adresse_absolu_in, fichiers[i]);			
		if (is_dir(adresse_absolu_in)){														// Si l'adresse de sortie est un dossier, on le cr�e
                        mkdir(adresse_absolu_out);
		} else {
			copie(adresse_absolu_in, adresse_absolu_out);									// Sinon, on copie le fichier
		}
	}
}
