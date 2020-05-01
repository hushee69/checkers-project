#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game_engine.h"
#include "game_sockets.h"

int IsEven(int num)
{
	return num % 2 == 0;
}

void UpdatePoint(Point *point, int x, int y, char c)
{
	point->x = x;
	point->y = y;
	point->color = c;
	
	return;
}

void ShowPoint(Point *point)
{
	fprintf(stderr, "(%d, %d, %c)\n", point->x, point->y, point->color);
}

char GetColor(Point *point)
{
	return point->color;
}

void SetColor(Point *point, char color)
{
	point->color = color;
	
	return;
}

void InitializePlayer(Board *board, Player *player, char color)
{
	player->num_pawns = 12;
	player->pawns = (Pawn*)malloc(sizeof(Pawn) * player->num_pawns);
	
	if( color == 'B' )
	{
		memcpy(&player->pawns[0], &board->pawns[0], sizeof(Pawn) * 12);
		player->lock = 'B';
	}
	else if( color == 'W' )
	{
		memcpy(&player->pawns[0], &board->pawns[12], sizeof(Pawn) * 12);
		player->lock = 'W';
	}
	else
	{
		fprintf(stderr, "Invalid color %c\n", color);
		
		return;
	}
	
	return;
}

void InitializeBoard(Board *b, char color)
{
	int i = 0, pc = 0;
	b->num_pawns = NUM_PAWNS;
	b->pawns = (Pawn*)malloc(sizeof(Pawn) * b->num_pawns);
	
	if( color == 'W' )
	{
		for( i = 0; i < 24; ++i )
		{
			// fill with black pawns
			if( IsEven(b->squares[i].x) && !IsEven(b->squares[i].y) )
			{
				UpdatePoint(&b->pawns[pc].position, b->squares[i].x, b->squares[i].y, 'B');
				b->squares[i].color = 'B';
				pc++;
			}
			if( IsEven(b->squares[i].y) && !IsEven(b->squares[i].x) )
			{
				UpdatePoint(&b->pawns[pc].position, b->squares[i].x, b->squares[i].y, 'B');
				b->squares[i].color = 'B';
				pc++;
			}
		}
		
		for( i = 40; i < BOARD_SIZE; ++i )
		{
			// fill with white pawns
			if( IsEven(b->squares[i].x) && !IsEven(b->squares[i].y) )
			{
				UpdatePoint(&b->pawns[pc].position, b->squares[i].x, b->squares[i].y, 'W');
				b->squares[i].color = 'W';
				pc++;
			}
			if( IsEven(b->squares[i].y) && !IsEven(b->squares[i].x) )
			{
				UpdatePoint(&b->pawns[pc].position, b->squares[i].x, b->squares[i].y, 'W');
				b->squares[i].color = 'W';
				pc++;
			}
		}
	}
	else if( color == 'B' )
	{
		for( i = 0; i < 24; ++i )
		{
			// fill with white pawns
			if( IsEven(b->squares[i].x) && IsEven(b->squares[i].y) )
			{
				UpdatePoint(&b->pawns[pc].position, b->squares[i].x, b->squares[i].y, 'W');
				b->squares[i].color = 'W';
				pc++;
			}
			if( !IsEven(b->squares[i].y) && !IsEven(b->squares[i].x) )
			{
				UpdatePoint(&b->pawns[pc].position, b->squares[i].x, b->squares[i].y, 'W');
				b->squares[i].color = 'W';
				pc++;
			}
		}
		
		for( i = 41; i < BOARD_SIZE; ++i )
		{
			// fill with black pawns
			if( IsEven(b->squares[i].x) && IsEven(b->squares[i].y) )
			{
				UpdatePoint(&b->pawns[pc].position, b->squares[i].x, b->squares[i].y, 'B');
				b->squares[i].color = 'B';
				pc++;
			}
			if( !IsEven(b->squares[i].y) && !IsEven(b->squares[i].x) )
			{
				UpdatePoint(&b->pawns[pc].position, b->squares[i].x, b->squares[i].y, 'B');
				b->squares[i].color = 'B';
				pc++;
			}
		}
	}
	
	return;
}

void CreateBoard(Board *b)
{
	int i = 0, j = 0, sq = 0;
	for( i = 0; i < NUM_SQUARES; ++i )
	{
		for( j = 0; j < NUM_SQUARES; ++j )
		{
			// assign x and y position to square
			// x is j and y is i
			UpdatePoint(&b->squares[sq], j, i, ' ');
			sq++;
		}
	}
	
	return;
}

int AdversaryMove(Board *b, Player *p, Move *m)
{
	int pawn_index = -1;
	Point from, to;
	
	from = m->from;
	to = m->to;
	
	pawn_index = FindPawnIndex(b, &from);
	
	fprintf(stderr, "moving pawn %d from (%d, %d) to (%d, %d)\n", pawn_index, from.x, from.y, to.x, to.y);
	
	int square_src = FindSquareIndex(&from);
	int square_dest = FindSquareIndex(&to);
	Pawn *to_move = &b->pawns[pawn_index];
	
	// update pawn
	printf("color of pawn is %c\n", GetColor(&to_move->position));
	printf("index of pawn is %d\n", pawn_index);
	UpdatePoint(&to_move->position, to.x, to.y, GetColor(&to_move->position));
	
	// update squares source and destination
	SetColor(&b->squares[square_src], ' ');
	SetColor(&b->squares[square_dest], GetColor(&to_move->position));
	ShowPoint(&to_move->position);
	
	return 1;
}

// given board and point, find pawn index
// later used to find index using mouse click coordinates
int FindPawnIndex(Board *b, Point *square)
{
	int i = 0;
	
	for( i = 0; i < b->num_pawns; ++i )
	{
		if( b->pawns[i].position.x == square->x && b->pawns[i].position.y == square->y )
		{
			return i;
		}
	}
	
	return -1;
}

Point CalculateDistance(Point *from, Point *to)			// to saves the distance between the points
{
	Point ret;
	
	ret.x = to->x - from->x;
	ret.y = to->y - from->y;
	
	return ret;
}

// find square index given x and y coordinate
int FindSquareIndex(Point *square)
{
	return (square->y * NUM_SQUARES) + square->x;
}

int ValidMove(Board *b, Player *p, Point *from, Point *to)
{
	Point dist = CalculateDistance(from, to);
	char lock = p->lock;
	
	// from contains information on if or not we have a pawn
	int index = FindPawnIndex(b, from);
	if( index < 0 )
	{
		return -1;
	}
	
	if( GetColor(&b->pawns[index].position) != lock )
	{
		fprintf(stderr, "Move your own pieces\n");
		
		return -1;
	}
	
	int dest_index = FindPawnIndex(b, to);
	if( dest_index > -1 )
	{
		fprintf(stderr, "There is a piece on the destination\n");
		
		return -1;
	}
	
	// `to` has to be empty otherwise we can not move
	// better to use other first condition ????
	// if( !GetColor(to) == ' ' )
	if( GetColor(to) == 'B' || GetColor(to) == 'W' )
	{
		return -1;
	}
	
	if( dist.x == 1 && dist.y == 1 )
	{
		return index;
	}
	else if( dist.x == -1 && dist.y == 1 )
	{
		return index;
	}
	else if( dist.x == 1 && dist.y == -1 )
	{
		return index;
	}
	else if( dist.x == -1 && dist.y == -1 )
	{
		return index;
	}
	else
	{
		return -1;
	}
}

void ShowBoard(Board *b, char color)
{
	int i = 0;
	
	for( i = 0; i < NUM_SQUARES; ++i )
	{
		printf(" %d ", i);
	}
	printf("\n");
	
	for( i = 0; i < NUM_SQUARES; ++i )
	{
		printf("___");
	}
	printf("\n\n");
	
	if( color == 'W' )
	{
		for( i = 0; i < BOARD_SIZE; ++i )
		{
		
			if( i > 1 && (i % 8 == 0) )
			{
				printf(" | %d \n", (i / 8) - 1);
			}
			if( IsEven(b->squares[i].x) && IsEven(b->squares[i].y) )
			{
				printf(" # ");
			}
			else if( !IsEven(b->squares[i].x) && !IsEven(b->squares[i].y) )
			{
				printf(" # ");
			}
			else
			{
				printf(" %c ", b->squares[i].color);
			}
		}
		printf(" | %d \n\n", (i / 8) - 1);
	}
	else
	{
		for( i = 0; i < BOARD_SIZE; ++i )
		{
			if( i > 1 && (i % 8 == 0) )
			{
				printf("\n");
			}
			if( !IsEven(b->squares[i].x) && IsEven(b->squares[i].y) )
			{
				printf(" # ");
			}
			else if( IsEven(b->squares[i].x) && !IsEven(b->squares[i].y) )
			{
				printf(" # ");
			}
			else
			{
				printf(" %c ", b->squares[i].color);
			}
		}
		printf("\n\n");
	}
	
	return;
}


int MovePawn(Board *b, Player *p)
{
	int pawn_index = -1;
	char lock = p->lock;
	Point from, to;
	
	fprintf(stderr, "Player %c: Which pawn to move?\n", lock);
	fscanf(stdin, "%d %d - %d %d", &from.x, &from.y, &to.x, &to.y);
	
	pawn_index = ValidMove(b, p, &from, &to);
	while( pawn_index < 0 )
	{
		fprintf(stderr, "Player %c\n", lock);
		fprintf(stderr, "your move was invalid, please try again\n");
		fscanf(stdin, "%d %d - %d %d", &from.x, &from.y, &to.x, &to.y);
		pawn_index = ValidMove(b, p, &from, &to);
	}
	
	fprintf(stderr, "moving pawn %d from (%d, %d) to (%d, %d)\n", pawn_index, from.x, from.y, to.x, to.y);
	int square_src = FindSquareIndex(&from);
	int square_dest = FindSquareIndex(&to);
	Pawn *to_move = &b->pawns[pawn_index];
	
	// update pawn
	printf("color of pawn is %c\n", GetColor(&to_move->position));
	printf("index of pawn is %d\n", pawn_index);
	UpdatePoint(&to_move->position, to.x, to.y, GetColor(&to_move->position));
	
	// update squares source and destination
	SetColor(&b->squares[square_src], ' ');
	SetColor(&b->squares[square_dest], GetColor(&to_move->position));
	ShowPoint(&to_move->position);
	
	// si il arrive ici, alors on envoie
	Move to_send;
	
	UpdatePoint(&to_send.from, from.x, from.y, GetColor(&b->squares[square_src]));
	UpdatePoint(&to_send.to, to.x, to.y, GetColor(&to_move->position));
	
	socket_send(p->csock, &to_send, sizeof(to_send));
	
	return 1;
}


/////// j'ai pris cette partie de ton code


// me est une variable globale
Player ps;

