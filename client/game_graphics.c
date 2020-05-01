#include "game_graphics.h"
#include "game_sockets.h"

void SetCoordinates(SDL_Rect *coords, int x, int y)
{
	coords->x = x;
	coords->y = y;
	
	return;
}

void GetCoordinates(SDL_Rect *coords, int *x, int *y)
{
	*x = coords->x;
	*y = coords->y;
	
	return;
}

void SetDimensions(Dimensions *dim, int w, int h)
{
	dim->width = w;
	dim->height = h;
	
	return;
}

void GetDimensions(Dimensions *dim, int *w, int *h)
{
	*w = dim->width;
	*h = dim->height;
	
	return;
}

void InitializeGameWindow(GameSurface *gs)
{
	// initialize SDL video mode
	SDL_Init(SDL_INIT_VIDEO);
	
	// window to return
	SDL_Surface *ret = SDL_SetVideoMode(gs->dimensions.width, gs->dimensions.height, 32, SDL_HWSURFACE);
	if( !ret )
	{
		fprintf(stderr, "error: %s\n", SDL_GetError());
		exit(-1);
	}
	
	// window color
	SDL_FillRect(ret, NULL, SDL_MapRGB(ret->format, 0, 0, 0));
	
	gs->surface = ret;
	
	return;
}

Uint32 CreateColor(GameSurface *gs, int r, int g, int b)
{
	gs->r = r;
	gs->g = g;
	gs->b = b;
	
	return SDL_MapRGB(gs->surface->format, gs->r, gs->g, gs->b);
}

void ColorSurface(GameSurface *gs, Uint32 color)
{
	SDL_FillRect(gs->surface, NULL, color);
}

void CreateRect(GameSurface *gs)
{
	SDL_Surface *ret = SDL_CreateRGBSurface(0, gs->dimensions.width, gs->dimensions.height, 32, 0, 0, 0, 0);
	
	gs->surface = ret;
	// color the rectangle
	ColorSurface(gs, gs->color);
	
	return;
}

// object to draw in src
// where to draw in dest
void DrawObject(GameSurface *src, GameSurface *dest)
{
	SDL_BlitSurface(src->surface, NULL, dest->surface, &src->coordinates);
	
	return;
}

void LoadFromImage(GameSurface *gs, const char *img, int x_factor, int y_factor)
{
	gs->surface = IMG_Load(img);
	
	ResizeImage(gs, x_factor, y_factor);
}

void ResizeImage(GameSurface *img, int x_factor, int y_factor)
{
	// CODE TO REDUCE SIZE OF PAWNS TO FIT SQUARES
	SDL_PixelFormat *pixelFmt = img->surface->format;
	SDL_Surface *tmp = SDL_CreateRGBSurface(SDL_SWSURFACE | SDL_SRCALPHA, img->surface->w / x_factor, img->surface->h / y_factor, pixelFmt->BitsPerPixel, pixelFmt->Rmask, pixelFmt->Gmask, pixelFmt->Bmask, pixelFmt->Amask);
	
	SDL_FillRect(tmp, NULL, SDL_MapRGBA(tmp->format, 0, 0, 0, 0));
	SDL_SoftStretch(img->surface, NULL, tmp, NULL);
	
	SDL_FreeSurface(img->surface);
	
	img->surface = tmp;
	
	return;
}

void Update(GameSurface *gs)
{
	SDL_Flip(gs->surface);
	
	return;
}

// board creation and initialization graphics functions
void CreateGraphicsBoard(GameSurface *window, GameSurface *square, Uint32 color1, Uint32 color2)
{
	int i = 0, j = 0;
	
	for( i = 0; i < NUM_SQUARES; ++i )
	{
		for( j = 0; j < NUM_SQUARES; ++j )
		{
			SetCoordinates(&square->coordinates, (j * SQUARE_WIDTH), (i * SQUARE_HEIGHT));
			
			if( !IsEven(i) )
			{
				if( IsEven(j) )
				{
					ColorSurface(square, color1);
				}
				else
				{
					ColorSurface(square, color2);
				}
			}
			else
			{
				if( !IsEven(j) )
				{
					ColorSurface(square, color1);
				}
				else
				{
					ColorSurface(square, color2);
				}
			}
			
			DrawObject(square, window);
		}
	}
	
	return;
}

void ShowGraphicsBoard(GameSurface *window, GameSurface *white_pawns, GameSurface *black_pawns, Board *b, char color)
{
	int i = 0;
	
	for( i = 0; i < BOARD_SIZE; ++i )
	{
		if( b->squares[i].color == 'W' )
		{
			SetCoordinates(&white_pawns->coordinates, b->squares[i].x * SQUARE_WIDTH, b->squares[i].y * SQUARE_HEIGHT);
			DrawObject(white_pawns, window);
		}
		else if( b->squares[i].color == 'B' )
		{
			SetCoordinates(&black_pawns->coordinates, b->squares[i].x * SQUARE_WIDTH, b->squares[i].y * SQUARE_HEIGHT);
			DrawObject(black_pawns, window);
		}
	}
	
	if( color == 'W' )
	{
		for( i = 0; i < BOARD_SIZE; ++i )
		{
			if( i > 1 && (i % 8 == 0) )
			{
				printf(" | %d %d \n", (i / 8) - 1, b->squares[i].x);	
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
				// draw pawns here
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

void ClearSquare(GameSurface *window, GameSurface *square, Point *coordinates, Uint32 color)
{
	// multiply by SQUARE_WIDTH and height the index in x and y
	int x_coord = coordinates->x * SQUARE_WIDTH;
	int y_coord = coordinates->y * SQUARE_HEIGHT;
	
	SetCoordinates(&square->coordinates, x_coord, y_coord);
	ColorSurface(square, color);
	
	// draw the square on the window
	DrawObject(square, window);
	Update(window);
	
	return;
}

void BoxClicked(Point *click_coords, Point *out_coords)
{
	int x = click_coords->x / SQUARE_WIDTH;
	int y = click_coords->y / SQUARE_HEIGHT;
	
	UpdatePoint(out_coords, x, y, ' ');
	
	return;
}

int MovePawnWithPoint(Board *b, Player *p, Move *playerMove)
{
	int pawn_index = -1;
	char lock = p->lock;
	Point *from = &playerMove->from, *to = &playerMove->to;
	
	pawn_index = ValidMove(b, p, from, to);
	if( pawn_index < 0 )
	{
		return 0;
	}
	
	fprintf(stderr, "pawn_index: %d\n", pawn_index);
	
	fprintf(stderr, "moving pawn %d from (%d, %d) to (%d, %d)\n", pawn_index, from->x, from->y, to->x, to->y);
	int square_src = FindSquareIndex(from);
	int square_dest = FindSquareIndex(to);
	Pawn *to_move = &b->pawns[pawn_index];
	
	// update pawn
	printf("color of pawn is %c\n", GetColor(&to_move->position));
	printf("index of pawn is %d\n", pawn_index);
	UpdatePoint(&to_move->position, to->x, to->y, GetColor(&to_move->position));
	
	// update squares source and destination
	SetColor(&b->squares[square_src], ' ');
	SetColor(&b->squares[square_dest], GetColor(&to_move->position));
	ShowPoint(&to_move->position);
	
	// si il arrive ici, alors on envoie
	Move to_send;
	
	UpdatePoint(&to_send.from, from->x, from->y, GetColor(&b->squares[square_src]));
	UpdatePoint(&to_send.to, to->x, to->y, GetColor(&to_move->position));
	
	socket_send(p->csock, &to_send, sizeof(to_send));
	
	return 1;
}

