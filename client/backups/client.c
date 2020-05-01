#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_engine.h"
#include "game_files.h"
#include "game_sockets.h"
#include "game_login.h"
#include "common.h"

int main(int argc, char *argv[])
{
	char pseud[TAILLE_PSEUDO] = {0};
    char buffer[1024] = {0};
	
	int i, j;
	int sock;
	if( argc != 3)
	{
		printf("Erreur il n y a pas assez d'argument\nvous devez entrez : pseudo adresseIP port mot_de_passe\n");	
		exit(-1);
	}


	char* adresse = argv[1];
	char *port = argv[2];
	Move their_move;
	Board board;

	ps.csock = socket_connect( adresse, port);

	definirModeConnexionClient( ps.csock, buffer);
	ps.lock = definirCouleurClient( ps.csock, buffer);
	
	CreateBoard(&board);
	InitializeBoard(&board, ps.lock);
	InitializePlayer(&board, &ps, ps.lock);
	
	do
	{
		ShowBoard(&board, ps.lock);
		printf("ps.lock %c\n", ps.lock);
		if( ps.lock == 'W' )
		{
			// c'est a blanc de jouer
			MovePawn(&board, &ps);
			ShowBoard(&board, ps.lock);

			socket_recv(ps.csock, &their_move, sizeof(their_move));
									printf("icii\n");
			AdversaryMove(&board, &ps, &their_move);

		}
		else if( ps.lock == 'B' )
		{
			// c'est a noir de jouer
			socket_recv(ps.csock, &their_move, sizeof(their_move));
			AdversaryMove(&board, &ps, &their_move);
			ShowBoard(&board, ps.lock);
			MovePawn(&board, &ps);
			
		}
	}while( 1 );
		
	recv_msg(sock, buffer);
	
	return 0;
}

