#include <stdio.h>
#include "pieces.h"

static void printBoard(int board[8][8])
{
	for(int i = 0; i < 8; ++i) {
		for(int n = 0; n < 8; ++n)
			printf("%c", board[i][n]);
		printf("\n");
	}
}

static inline void clearScreen(void)
{	
	printf("\e[1;1H\e[2J");
}

static int validateInput(char *inputBuffer, int max)
{
	int l = 0, n = 0;
	char tmp;
	for(int i = 0; i < max; ++i) {
		switch(*inputBuffer) {
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
				break;
			default:
				return 0;
				
		}
		
	}
	return 1;
}

static int readInput(char *inputBuffer, int max)
{
	int c = 0;
	for(int i = 0; i < max && (c = getchar()) != EOF; ++i)
		*inputBuffer++ = c;
	inputBuffer -= max;
	return validateInput(inputBuffer, max) ? 1 : 0;
}

int main(void)
{
	int board[8][8] =
	{
		{ WHITE_ROOK, WHITE_KNIGHT, WHITE_BISHOP, WHITE_QUEEN, WHITE_KING,   WHITE_BISHOP, WHITE_KNIGHT, WHITE_ROOK },
		{ WHITE_PAWN, WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,  WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN,   WHITE_PAWN },
		{ EMPTY_SQR,  EMPTY_SQR,    EMPTY_SQR, 	  EMPTY_SQR,   EMPTY_SQR,    EMPTY_SQR,    EMPTY_SQR,    EMPTY_SQR  },
 		{ EMPTY_SQR,  EMPTY_SQR,    EMPTY_SQR, 	  EMPTY_SQR,   EMPTY_SQR,    EMPTY_SQR,    EMPTY_SQR,    EMPTY_SQR  },
   		{ EMPTY_SQR,  EMPTY_SQR,    EMPTY_SQR, 	  EMPTY_SQR,   EMPTY_SQR,    EMPTY_SQR,    EMPTY_SQR,    EMPTY_SQR  },
 		{ EMPTY_SQR,  EMPTY_SQR,    EMPTY_SQR, 	  EMPTY_SQR,   EMPTY_SQR,    EMPTY_SQR,    EMPTY_SQR,    EMPTY_SQR  },
 		{ BLACK_PAWN, BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,  BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN,   BLACK_PAWN },
 		{ BLACK_ROOK, BLACK_KNIGHT, BLACK_BISHOP, BLACK_QUEEN, BLACK_KING,   BLACK_BISHOP, BLACK_KNIGHT, BLACK_ROOK }
	};
	char inputBuffer[4];
	inputBuffer[4] = '\0';

	while(1) {
		printBoard(board);
		(void)readInput(inputBuffer, sizeof(inputBuffer) / sizeof(char));
		clearScreen();
	}

	return 0;
}
