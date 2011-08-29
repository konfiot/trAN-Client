
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

void pause(){												// Fonction de pause portable
	printf("Appuyez sur entree pour continuer");
        getchar();												// Commande linux
}

void stop(const char a_afficher[], short int code_exit){	// Fonction d'arr�t d'urgence du programme
	if (*a_afficher != 0){
		printf("%s\n", a_afficher);							// Affichage de l'erreur
		pause();
		exit(code_exit);									// Arr�t du programme
	}
}
