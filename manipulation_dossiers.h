
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
#include "divers.h"

#define SUCCESS 0
#define FAILURE 1
#define FICHIER 1
#define DOSSIER 2

typedef struct dirent dirent;

int verif_dossier (const char *chemin_programme);																																	// Verifie que le programme se lance bien dans le dossier dans lequel il � �t� cr��
char *chemin_to_dossier(const char *chemin);																																		// Transforme une adresse d'un fichier en adresse du dossier dans lequel il est contenu
int lire_dossier (char *adresse_in, char relatif_dossier_source [MAX_TAILLE_ADRESSE], char (**fichiers)[MAX_TAILLE_ADRESSE], short int inclure_sous_dossier, int nb_deja_lus);		// Stocke les noms des differents fichiers d'un dossier dans un tableau 
int is_dir (const char *adresse_dossier);																																			// Verifie si l'adresse corespond a un dosier ou a un fichier
char *cheminToNomFichier (const char *chemin);																																		// Extrait le nom du fichier d'une adresse absolue
