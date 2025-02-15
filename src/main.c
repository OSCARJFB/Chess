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

	printBoard(board);
	return 0;
}
