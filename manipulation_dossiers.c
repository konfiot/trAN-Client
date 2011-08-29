
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
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/types.h>
#include "manipulation_dossiers.h"
#include "divers.h"

int verif_dossier (const char *chemin_programme){
	char dossier_lancement[500] = {' '}, *dossier_programme = (char*)malloc(strlen(chemin_programme) * sizeof(char));
	int err = 0;
	getcwd(dossier_lancement, 500);														// R�cup�re le dossier de lancement du programme
	strcpy(dossier_programme, chemin_programme);
	dossier_programme = chemin_to_dossier(chemin_programme);
	if (strcmp(dossier_lancement, dossier_programme) != 0){									// Si le dossier de lancement du programme est different du dossier dans lequel est stock� le programme
                err = chdir(dossier_programme);													// On change le dossier de lancement afin de pouvoir utiliser les fichiers utilis�s par le programme
		free(dossier_programme);
		if (err != 0){																		// Gestion des erreurs
			return FAILURE;
		} else {
			return SUCCESS;
		}
	} else {
            return SUCCESS;
        }
}

char *chemin_to_dossier(const char *chemin){
	int i = 0;
	char *temp_chemin = NULL;
	temp_chemin = malloc(strlen(chemin) + 1);
	strcpy(temp_chemin, chemin);
	for (i = strlen(chemin) - 1 ; i > 0 ; i--){												// on percourt la chaine en partant de la fin - 1, car si l'adresse termine par un antislash, aucun traitement ne sera fait.
		if (chemin[i] == '\\'){																// Si on trouve un antislash, on met un \0 pour indiquer la fin de la chaine
			temp_chemin[i+1] = '\0';
			temp_chemin = realloc(temp_chemin, (strlen(temp_chemin) + 1) * sizeof(char));	// Puis on ralloue la dose correcte de m�moire
			return temp_chemin;
		}
	}
	return NULL;
}

int lire_dossier (char *adresse_in, char relatif_dossier_source [MAX_TAILLE_ADRESSE], char (**fichiers)[MAX_TAILLE_ADRESSE], short int inclure_sous_dossier, int nb_deja_lus){
	DIR *dossier = NULL;
	dirent *item = NULL;
	char temp_absolu[MAX_TAILLE_ADRESSE] = {'\0'}, temp_relatif[MAX_TAILLE_ADRESSE] = {'\0'};
	int i = 0;

	dossier = opendir(adresse_in);															// Ouverture du dossier et gestion des erreurs
	if (dossier == NULL){
		stop("impossible d'ouvrir le dossier", EXIT_FAILURE);
	}
	for (i = 0 ; i < 2 ; i++){																// Lecture des deux premiers �l�m�nts du dossier soit "." et ".." repr�sentant le dossier actuel et le dossier parent
		readdir(dossier);
	}
	item = readdir(dossier);

	if (relatif_dossier_source[0] == '\0'){													// Si on est au d�but de l'arborescence, on copie l'adresse d'entr�e dans la premi�re case du tableau afin de la r�utiliser plus tard
		*fichiers = malloc(sizeof(**fichiers));
		strcpy(**fichiers, adresse_in);
		nb_deja_lus++;
	}

	if (item != NULL){																		// Si le dossier n'est pas vide
		while (item != NULL){																// On lit les items du dossier jusqu'a la fin
			strcpy(temp_absolu, adresse_in);												// On rempli la variable temporaire qui va contenir l'adresse absolue de l'�l�ment
			strcat(temp_absolu, "\\");
			strcat(temp_absolu, item->d_name);
			if (relatif_dossier_source[1] != '\0'){
				strcpy(temp_relatif, relatif_dossier_source);								// On remplis la variable temporaire qui va contenir l'adresse relative � l'adresse d'entr�e (afin de cr�er un arborescence)
				strcat(temp_relatif, "\\");
			}
			strcat(temp_relatif, item->d_name);
			if (is_dir(temp_absolu) == 0){													// Si l'adresse absolue de l'�l�ment n'est pas un dossier, on copie juste l'adresse dans le tableau
				*fichiers = realloc(*fichiers, (nb_deja_lus + 1) * sizeof(**fichiers));
				strcpy(*(*fichiers + nb_deja_lus), temp_relatif);
				nb_deja_lus++;
			} else if (inclure_sous_dossier == 1){											// Sinon, si c'est un dossier et si on d�cide d'inclure les sous dossiers, on copie l'adresse dans le tableau et on apelle r�cursivement la fonction lire_dosier
				*fichiers = realloc(*fichiers, (nb_deja_lus + 1) * sizeof(**fichiers));
				strcpy(*(*fichiers + nb_deja_lus), temp_relatif);
				nb_deja_lus++;
				nb_deja_lus = lire_dossier(temp_absolu, temp_relatif, fichiers, 1, nb_deja_lus);
			}
			item = readdir(dossier);
			temp_absolu[0] = '\0';
			temp_relatif[0] = '\0';
		}
	}
	closedir(dossier);
	free(item);																				// On lib�re l'espace m�moire allou� temporairement et on retourne le nombre d'items lus
	return nb_deja_lus;
}

int is_dir (const char *adresse_dossier){
	DIR *dossier = NULL;
	dossier = opendir(adresse_dossier);														// Si l'�lement peut �tre ouvert comme un dossier, c'est que c'est un dossier
	if (dossier != NULL){
		closedir(dossier);
		return 1;
	} else {																				// Sinon, c'est un fichier
		closedir(dossier);
		return 0;
	}
}

char *cheminToNomFichier (const char *chemin){
    char *temp_adresse_fichier = NULL, *adresse_fichier = NULL;
    int i = 0;

    for (i = strlen(chemin) ; i > 0 ; i--){													// On parcourt la chaine en partant de la fin
        if (chemin[i] == '\\'){																// Une fois arriv� au premier antislash
            temp_adresse_fichier = (char*) &chemin[i+1];									// On alloue de la m�moire pour la taille du nom du fichier et on copie le reste de la chaine initiale dans cette nouvelle chaine
            adresse_fichier = malloc((strlen(temp_adresse_fichier) * sizeof(char)) + 1);
			if (adresse_fichier == NULL){
				printf("Erreur d'allocation", EXIT_FAILURE);
			}
            strcpy(adresse_fichier, temp_adresse_fichier);
            return adresse_fichier;
        }
    }
    return NULL;																			// Si on a rien trouv�, on renvoie NULL
}
