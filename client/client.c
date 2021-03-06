#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_engine.h"
#include "game_graphics.h"
#include "game_files.h"
#include "game_sockets.h"
#include "game_login.h"
#include "common.h"

// TO DO
// make sockets non blocking
// eat pawn

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
	
	char *adresse = argv[1];
	char *port = argv[2];
	
	// their_move will hold information on the opponent's move
	Move their_move;
	Board board;

	ps.csock = socket_connect( adresse, port);

	definirModeConnexionClient( ps.csock, buffer);
	ps.lock = definirCouleurClient( ps.csock, buffer);
	
	CreateBoard(&board);
	InitializeBoard(&board, ps.lock);
	InitializePlayer(&board, &ps, ps.lock);
	
	fprintf(stderr, "ps.lock : %c\n", ps.lock);
	
	// main window for game
	GameSurface window;
	
	SetDimensions(&window.dimensions, WINDOW_WIDTH, WINDOW_HEIGHT);
	
	InitializeGameWindow(&window);
	
	Uint32 light_blue = CreateColor(&window, 163, 160, 254);
	ColorSurface(&window, light_blue);
	// refresh the window (update changes)
	Update(&window);
	
	Uint32 light_red = CreateColor(&window, 226, 80, 80);
	Uint32 light_black = CreateColor(&window, 79, 85, 94);
	
	// squares - red and black
	GameSurface red_black_rect;
	
	SetDimensions(&red_black_rect.dimensions, SQUARE_WIDTH, SQUARE_HEIGHT);
	red_black_rect.color = light_red;
	
	CreateRect(&red_black_rect);
	
	// draw the squares here
	CreateGraphicsBoard(&window, &red_black_rect, light_red, light_black);
	
	// pawns - white and black
	// pawns are in the Board structure
	// drawing of all pawns happens here
	GameSurface black_pawn;
	GameSurface white_pawn;
	
	LoadFromImage(&black_pawn, "sprites/black_piece.png", 5, 5);
	LoadFromImage(&white_pawn, "sprites/red_piece.png", 3, 3);
	// draw all black pawns on the board
	ShowGraphicsBoard(&window, &white_pawn, &black_pawn, &board, 'W');
	
	Update(&window);
	
	Move playerMove;
	int played = 0, received = 0;
	
	while( 1 )
	{
		SDL_Event event;
		if( ps.lock == 'W' )
		{
			if( SDL_PollEvent(&event) )
			{
				if( event.type == SDL_QUIT )
				{
					break;
				}
				else if( event.type == SDL_MOUSEBUTTONUP )
				{
					if( event.button.button == SDL_BUTTON_LEFT )
					{
						Point mouse_coords;
						
						// get board equivalent coordinates here
						// to send to MovePawn
						Point board_coords;
						
						SDL_GetMouseState(&mouse_coords.x, &mouse_coords.y);
						BoxClicked(&mouse_coords, &board_coords);
						
						// check index of pawn
						// if index > -1 then it's a selection
						// fill the `Point from` in the `Move` struct
						int index = FindPawnIndex(&board, &board_coords);
						if( index > - 1 )
						{
							// fill pawn structure here and access pawn coordinates
							UpdatePoint(&playerMove.from, board.pawns[index].position.x, board.pawns[index].position.y, board.pawns[index].position.color);
							ShowPoint(&playerMove.from);
						}
						else
						{
							if( playerMove.from.x > -1 && playerMove.from.y > -1 )
							{
								// if here then a pawn is selected
								// and the square clicked currently is empty
								// fill the square information in the `Point to` struct in `Move`
								// check if the move is valid
								// board coords has the `Point to` coordinates
								UpdatePoint(&playerMove.to, board_coords.x, board_coords.y, board_coords.color);
								// clear the current square
								// move the pawn here
								int success = MovePawnWithPoint(&board, &ps, &playerMove);
								
								if( success )
								{
									// update graphical board here
									ClearSquare(&window, &red_black_rect, &playerMove.from, light_red);
									// update pawn image coordinates
									SetCoordinates(&white_pawn.coordinates, playerMove.to.x * SQUARE_WIDTH, playerMove.to.y * SQUARE_HEIGHT);
									// draw the pawn
									DrawObject(&white_pawn, &window);
									Update(&window);
									
									memset(&playerMove, -1, sizeof(Move));
									played = 1;
									received = 0;
								}
							}
						}
						//ClearSquare(&window, &red_black_rect, &board_coords, light_red);
					}
				}
			}
			// if we've played and not received, then we receive
			else if( played && !received )
			{
				ShowBoard(&board, ps.lock);
				socket_recv(ps.csock, &their_move, sizeof(their_move));
				printf("icii\n");
				AdversaryMove(&board, &ps, &their_move);
				
				// update graphical board here
				ClearSquare(&window, &red_black_rect, &their_move.from, light_red);
				// update pawn image coordinates
				SetCoordinates(&black_pawn.coordinates, their_move.to.x * SQUARE_WIDTH, their_move.to.y * SQUARE_HEIGHT);
				// draw the pawn
				DrawObject(&black_pawn, &window);
				Update(&window);
				played = 0;
				received = 1;
			}
		}
		else if( ps.lock == 'B' )
		{
			// c'est a noir de jouer
			// we haven't played and haven't received
			if( !received )
			{
				ShowBoard(&board, ps.lock);
				socket_recv(ps.csock, &their_move, sizeof(their_move));
				fprintf(stderr, "rcvd something\n");
				AdversaryMove(&board, &ps, &their_move);
				ShowBoard(&board, ps.lock);
				// update graphical board here
				ClearSquare(&window, &red_black_rect, &their_move.from, light_black);
				// update pawn image coordinates
				SetCoordinates(&white_pawn.coordinates, their_move.to.x * SQUARE_WIDTH, their_move.to.y * SQUARE_HEIGHT);
				// draw the pawn
				DrawObject(&white_pawn, &window);
				Update(&window);
				
				received = 1;
			}
			if( SDL_PollEvent(&event) )
			{
				if( event.type == SDL_QUIT )
				{
					fprintf(stderr, "rcvd quit\n");
					break;
				}
				else if( event.type == SDL_MOUSEBUTTONUP )
				{
					fprintf(stderr, "button down\n");
					if( event.button.button == SDL_BUTTON_LEFT )
					{
						Point mouse_coords;
						
						// get board equivalent coordinates here
						// to send to MovePawn
						Point board_coords;
						
						SDL_GetMouseState(&mouse_coords.x, &mouse_coords.y);
						BoxClicked(&mouse_coords, &board_coords);
						
						// check index of pawn
						// if index > -1 then it's a selection
						// fill the `Point from` in the `Move` struct
						int index = FindPawnIndex(&board, &board_coords);
						if( index > - 1 )
						{
							// fill pawn structure here and access pawn coordinates
							UpdatePoint(&playerMove.from, board.pawns[index].position.x, board.pawns[index].position.y, board.pawns[index].position.color);
							ShowPoint(&playerMove.from);
						}
						else
						{
							if( playerMove.from.x > -1 && playerMove.from.y > -1 )
							{
								// if here then a pawn is selected
								// and the square clicked currently is empty
								// fill the square information in the `Point to` struct in `Move`
								// check if the move is valid
								// board coords has the `Point to` coordinates
								UpdatePoint(&playerMove.to, board_coords.x, board_coords.y, board_coords.color);
								// clear the current square
								// move the pawn here
								int success = MovePawnWithPoint(&board, &ps, &playerMove);
								
								if( success )
								{
									// update graphical board here
									ClearSquare(&window, &red_black_rect, &playerMove.from, light_black);
									// update pawn image coordinates
									SetCoordinates(&black_pawn.coordinates, playerMove.to.x * SQUARE_WIDTH, playerMove.to.y * SQUARE_HEIGHT);
									// draw the pawn
									DrawObject(&black_pawn, &window);
									Update(&window);
									
									memset(&playerMove, -1, sizeof(Move));
									played = 1;
									received = 0;
								}
							}
						}
						//ClearSquare(&window, &red_black_rect, &board_coords, light_red);
					}
				}
			}
			else if( played )
			{
				played = 0;
			}
		}
		//SDL_Delay(1500);
	}
	
	return 0;
}

