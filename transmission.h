
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
#include"socket.h"					

void envoi (SOCKET csock, char *adresse_in);						// Fonction d'envoi de fichier par le reseau local
void reception (SOCKET sock, char *adresse_out);					// Fonction de reception de fichier par le reseau local
void denvoi (SOCKET csock, char (*fichiers)[MAX_TAILLE_ADRESSE], int nb_fichiers);	// Fonction d'envoi de dossiers par le reseau local
void dreception (SOCKET sock, char *adresse_out);                                       // Fonction de reception de dossiers par le reseau local
