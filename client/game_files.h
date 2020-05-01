#ifndef _GAME_FILES_H
#define _GAME_FILES_H

#include "game_engine.h"

extern char *tab_pseudo[MAX_CLIENT]; // tableau qui va contenire 10 pseudo
extern char *tab_motDepasse[MAX_CLIENT];
extern int nbJoueurActuelle;// c'est le nombre de joueur actuelle dans la base de donnee pas dans le jeu
void allocation_tab_pseudo( char *tab_pseudo[], char *tab_motDepasse[]);
FILE * ouvrireFichier(const char *nom_fichier);
void ajouteJoueur( char pseudo[], char motpasse[], char* tab_pseudo[], char* tab_motDepasse[], int *nbJoueur);
void recupereJoueurs( FILE *flux, char *tab_pseudo[], char *tab_motDepasse[], int *nbJoueur);

#endif

