/*	
 	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#include "chess_engine.h"

static move getUserInput(move m_data);
static move isCastlingOk(char chessBoard[8][8], move m_data);
static bool isCastlingMove(move m_data);
static bool isCastlingPathOk(char chessBoard[8][8], move m_data);
static bool tryCastlingMove(char chessBoard[8][8], move m_data);
static int getPath(char chessBoard[8][8], move m_data, int pathX[8], int pathY[8]);
static int translateLetter(char letter);
static bool isUpperOrLower(char letter);
static bool isTargetUnderThreat(char chessBoard[8][8], move m_data, int targetX, int targetY);
static bool gameRules(char[8][8], move m_data);
static bool pawn(char[8][8], move m_data);
static bool executeEnPassant(char chessBoard[8][8], move m_data);
static bool rook(char[8][8], move m_data);
static bool knight(char[8][8], move m_data);
static bool bishop(char[8][8], move m_data);
static bool queen(char[8][8], move m_data);
static bool king(char[8][8], move m_data);
static bool targetStatus(char[8][8], move m_data);
static bool findTheKing(char chessBoard[8][8], int *kingX, int *kingY, bool playerTurn);
static bool isKingLocked(char chessBoard[8][8], move m_data, int kingX, int kingY);
static bool isThreatRemoveable(char chessBoard[8][8], move m_data, int kingX, int kingY);
static bool removalofThreat(char chessBoard[8][8], move m_data, int pathX[8], int pathY[8], int pathSize);
static bool tryMoveAtPath(char chessBoard[8][8], int pathX[8], int pathY[8], int pathSize, int x, int y, move m_data);
static inline bool isKingInCheck(char chessBoard[8][8], move m_data, int kingX, int kingY);
static void findThreat(char chessBoard[8][8], bool playerTurn, int kingX, int kingY, int *foundThreatX);
static void drawConsole(char chessBoard[8][8]);
static void failed_allocation(void);

static void failed_allocation(void)
{

	printf("Memory allocation failed:\nError exit with return code 1");
	exit(EXIT_FAILURE);
}

move initMove(void)
{
	move m_data;
	m_data.playerTurn = true;
	m_data.isCastlingFlow = false, m_data.enPassant = false;
	m_data.p1_shortCast = true, m_data.p1_longCast = true;
	m_data.p2_shortCast = true, m_data.p2_longCast = true;
	return m_data;
}

static void drawConsole(char chessBoard[8][8])
{
	int board_numbers = 1;

	(void)system(SYSTEM);

	for (int i = 0; i < 8; ++i)
	{
		printf("%d.", board_numbers++);
		for (int j = 0; j < 8; ++j)
		{
			printf("[%c]", chessBoard[i][j]);
		}
		printf("\n");
	}
	printf("   A  B  C  D  E  F  G  H\n");
}

static move getUserInput(move m_data)
{
	char *userInput = malloc(sizeof(char));
	if (userInput == NULL)
	{
		failed_allocation();
	}

	int key_pressed = 0, sizeOfArray = 0;

	m_data.playerTurn ? printf("Player one enter a move:") : printf("Player two enter a move:");

	while (key_pressed != ENTER)
	{
		key_pressed = getchar();

		if (key_pressed != ENTER)
		{
			userInput[sizeOfArray++] = (char)key_pressed;
			userInput = realloc(userInput, (sizeOfArray + 1) * sizeof(char));
			if (userInput == NULL)
			{
				failed_allocation();
			}
		}
		else
		{
			userInput[sizeOfArray] = '\0';
		}
	}

	if (strlen(userInput) > 4 || strlen(userInput) < 4)
	{
		printf("\nWrong input format, it should be:\n[LETTER][SINGLE DIGIT][LETTER][SINGLE DIGIT]\nPress any key to continue...");
		free(userInput);
		getchar();
		return m_data;
	}

	sizeOfArray = 0;
	while (userInput[sizeOfArray] != '\0')
	{
		switch (sizeOfArray)
		{
			case 0:
				if (((int)userInput[sizeOfArray] >= 97 && (int)userInput[sizeOfArray] <= 104) ||
						((int)userInput[sizeOfArray] >= 65 && (int)userInput[sizeOfArray] <= 72))
				{
					m_data.x_sel = translateLetter(userInput[sizeOfArray]);
				}
				break;
			case 1:
				if (userInput[sizeOfArray] - '0' >= 1 && userInput[sizeOfArray] - '0' <= 9)
				{
					m_data.y_sel = userInput[sizeOfArray] - '0' - 1;
				}
				break;
			case 2:
				if (((int)userInput[sizeOfArray] >= 97 && (int)userInput[sizeOfArray] <= 104) ||
						((int)userInput[sizeOfArray] >= 65 && (int)userInput[sizeOfArray] <= 72))
				{
					m_data.x_mov = translateLetter(userInput[sizeOfArray]);
				}
				break;
			case 3:
				if (userInput[sizeOfArray] - '0' >= 1 && userInput[sizeOfArray] - '0' <= 9)
				{
					m_data.y_mov = userInput[sizeOfArray] - '0' - 1;
				}
				break;
		}
		++sizeOfArray;
	}

	free(userInput);
	return m_data;
}

move castlingController(char chessBoard[8][8], move m_data)
{
	m_data = isCastlingOk(chessBoard, m_data);
	m_data.isCastlingFlow = false;

	if (chessBoard[m_data.y_sel][m_data.x_sel] != 'W' &&
			chessBoard[m_data.y_sel][m_data.x_sel] != 'w')
	{
		return m_data;
	}

	if (!isCastlingMove(m_data))
	{
		return m_data;
	}

	if (!isCastlingPathOk(chessBoard, m_data))
	{
		return m_data;
	}

	if (!tryCastlingMove(chessBoard, m_data))
	{
		return m_data;
	}
	else
	{
		m_data.isCastlingFlow = true;
		m_data.playerTurn = m_data.playerTurn == true ? false : true;
	}

	return m_data;
}

static move isCastlingOk(char chessBoard[8][8], move m_data)
{
	// Player 1, set castling to false if a move has been made.

	if (chessBoard[0][4] != 'W')
	{
		m_data.p1_shortCast = false;
		m_data.p1_longCast = false;
	}

	if (chessBoard[0][7] != 'R')
	{
		m_data.p1_shortCast = false;
	}

	if (chessBoard[0][0] != 'R')
	{
		m_data.p1_longCast = true;
	}

	// Player 2, set castling to false if a move has been made.

	if (chessBoard[0][4] != 'w')
	{
		m_data.p2_shortCast = false;
		m_data.p2_longCast = false;
	}

	if (chessBoard[0][7] != 'r')
	{
		m_data.p2_shortCast = false;
	}

	if (chessBoard[0][0] != 'r')
	{
		m_data.p2_longCast = true;
	}

	return m_data;
}

static bool isCastlingMove(move m_data)
{
	const int shortC = 7, longC = 0;
	int column = m_data.playerTurn == true ? 0 : 7;

	// Make sure castling is allowed by checking if any of the required pieces have been moved.
	if (m_data.playerTurn)
	{
		if (m_data.x_mov == shortC && !m_data.p1_shortCast)
		{
			return false;
		}

		if (m_data.x_mov == longC && !m_data.p1_longCast)
		{
			return false;
		}
	}
	else
	{
		if (m_data.x_mov == shortC && !m_data.p2_shortCast)
		{
			return false;
		}

		if (m_data.x_mov == longC && !m_data.p2_longCast)
		{
			return false;
		}
	}

	if (m_data.x_mov == shortC && m_data.y_mov == column)
	{
		return true;
	}
	else if (m_data.x_mov == longC && m_data.y_mov == column)
	{
		return true;
	}

	return false;
}

static bool isCastlingPathOk(char chessBoard[8][8], move m_data)
{
	const int shortC = 7, longC = 0;

	if (chessBoard[m_data.y_sel][m_data.x_sel + 1] == ' ' &&
			chessBoard[m_data.y_sel][m_data.x_sel + 2] == ' ' &&
			m_data.x_mov == shortC)
	{
		return true;
	}

	if (chessBoard[m_data.y_sel][m_data.x_sel - 1] == ' ' &&
			chessBoard[m_data.y_sel][m_data.x_sel - 2] == ' ' &&
			chessBoard[m_data.y_sel][m_data.x_sel - 3] == ' ' &&
			m_data.x_mov == longC)
	{
		return true;
	}

	return false;
}

static bool tryCastlingMove(char chessBoard[8][8], move m_data)
{
	const int shortC = 7, longC = 0;
	int column = m_data.playerTurn == true ? 0 : 7;

	char king = m_data.playerTurn == true ? 'W' : 'w';
	char rook = m_data.playerTurn == true ? 'R' : 'r';

	// Execute castling move.
	if (m_data.x_mov == shortC)
	{
		chessBoard[column][shortC] = ' ';
		chessBoard[column][shortC - 2] = rook;
		chessBoard[m_data.y_sel][m_data.x_sel] = ' ';
		chessBoard[column][shortC - 1] = king;
	}
	if (m_data.x_mov == longC)
	{
		chessBoard[column][longC] = ' ';
		chessBoard[column][longC + 3] = rook;
		chessBoard[m_data.y_sel][m_data.x_sel] = ' ';
		chessBoard[column][longC + 2] = king;
	}

	// If castling result in check the move must be reseted.
	m_data.playerTurn = m_data.playerTurn == true ? false : true;
	if (isTargetUnderThreat(chessBoard, m_data, m_data.x_sel, m_data.y_sel))
	{
		if (m_data.x_mov == shortC)
		{
			chessBoard[column][shortC] = rook;
			chessBoard[column][shortC - 2] = ' ';
			chessBoard[m_data.y_sel][m_data.x_sel] = king;
			chessBoard[column][shortC - 1] = ' ';
		}
		if (m_data.x_mov == longC)
		{
			chessBoard[column][longC] = rook;
			chessBoard[column][longC + 3] = ' ';
			chessBoard[m_data.y_sel][m_data.x_sel] = king;
			chessBoard[column][longC + 2] = ' ';
		}

		return false;
	}

	return true;
}

move executeMove(char chessBoard[8][8], move m_data)
{
	int kingX = 0, kingY = 0;
	char target = chessBoard[m_data.y_mov][m_data.x_mov];

	if (gameRules(chessBoard, m_data) && !m_data.blocked && !m_data.isCastlingFlow)
	{
		chessBoard[m_data.y_mov][m_data.x_mov] = chessBoard[m_data.y_sel][m_data.x_sel];
		chessBoard[m_data.y_sel][m_data.x_sel] = ' ';

		if (!findTheKing(chessBoard, &kingX, &kingY, m_data.playerTurn))
		{
			puts("checkmate: Error Couldn't find the king.");
			exit(EXIT_FAILURE);
		}

		m_data.playerTurn = m_data.playerTurn == true ? false : true;
		if (isTargetUnderThreat(chessBoard, m_data, kingX, kingY))
		{
			// Reverse the move, since it resulted in the king being under threat.
			m_data.playerTurn = m_data.playerTurn == true ? false : true;

			chessBoard[m_data.y_sel][m_data.x_sel] = chessBoard[m_data.y_mov][m_data.x_mov];
			chessBoard[m_data.y_mov][m_data.x_mov] = target;

			return m_data;
		}
	}

	return m_data;
}

move enPassant(char chessBoard[8][8], move m_data)
{
	char pawn = m_data.playerTurn ? 'p' : 'P';
	bool lastPlayer = m_data.playerTurn ? false : true;
	int diff = 0;

	// Reset EnPassant at the end of each turn.
	if (m_data.enPassant)
	{
		m_data.enPassant = false;
	}

	// Is not a piece of type pawn.
	if (chessBoard[m_data.y_mov][m_data.x_mov] != pawn)
	{
		return m_data;
	}

	if (lastPlayer)
	{
		diff = m_data.y_mov - m_data.y_sel;
	}
	else
	{
		diff = m_data.y_sel - m_data.y_mov;
	}

	// Is not a 2 step advance.
	if (diff != 2 && diff != -2)
	{
		return m_data;
	}

	m_data.enPassant = true;
	m_data.pawnX = m_data.x_mov;
	m_data.pawnY = m_data.y_mov;

	return m_data;
}

static int translateLetter(char letter)
{
	if (letter == 'a' || letter == 'A')
	{
		return 0;
	}
	else if (letter == 'b' || letter == 'B')
	{
		return 1;
	}
	else if (letter == 'c' || letter == 'C')
	{
		return 2;
	}
	else if (letter == 'd' || letter == 'D')
	{
		return 3;
	}
	else if (letter == 'e' || letter == 'E')
	{
		return 4;
	}
	else if (letter == 'f' || letter == 'F')
	{
		return 5;
	}
	else if (letter == 'g' || letter == 'G')
	{
		return 6;
	}
	else if (letter == 'h' || letter == 'H')
	{
		return 7;
	}
	else
	{
		return -1;
	}
}

move isPathBlocked(char chessBoard[8][8], move m_data)
{
	int index = 0;
	int x = m_data.x_sel, y = m_data.y_sel;
	int pathY[8], pathX[8];

	if (chessBoard[m_data.y_sel][m_data.x_sel] == 'K' || chessBoard[m_data.y_sel][m_data.x_sel] == 'k')
	{
		m_data.blocked = false;
		return m_data;
	}

	while (x != m_data.x_mov || y != m_data.y_mov)
	{
		pathY[index] = y;
		pathX[index] = x;
		++index;

		if (m_data.x_mov > x)
		{
			++x;
		}
		else if (m_data.x_mov < x)
		{
			--x;
		}

		if (m_data.y_mov > y)
		{
			++y;
		}
		else if (m_data.y_mov < y)
		{
			--y;
		}
	}

	for (int i = 1; i + 1 <= index; ++i)
	{
		if (chessBoard[pathY[i]][pathX[i]] != ' ')
		{
			m_data.blocked = true;
			return m_data;
		}
	}

	m_data.blocked = false;
	return m_data;
}

static bool gameRules(char chessBoard[8][8], move m_data)
{
	if ((m_data.playerTurn && isUpperOrLower(chessBoard[m_data.y_sel][m_data.x_sel])) ||
			(!m_data.playerTurn && !isUpperOrLower(chessBoard[m_data.y_sel][m_data.x_sel])))
	{
		if (chessBoard[m_data.y_sel][m_data.x_sel] == 'P' || chessBoard[m_data.y_sel][m_data.x_sel] == 'p')
		{
			return pawn(chessBoard, m_data);
		}
		else if (chessBoard[m_data.y_sel][m_data.x_sel] == 'R' || chessBoard[m_data.y_sel][m_data.x_sel] == 'r')
		{
			return rook(chessBoard, m_data);
		}
		else if (chessBoard[m_data.y_sel][m_data.x_sel] == 'K' || chessBoard[m_data.y_sel][m_data.x_sel] == 'k')
		{
			return knight(chessBoard, m_data);
		}
		else if (chessBoard[m_data.y_sel][m_data.x_sel] == 'B' || chessBoard[m_data.y_sel][m_data.x_sel] == 'b')
		{
			return bishop(chessBoard, m_data);
		}
		else if (chessBoard[m_data.y_sel][m_data.x_sel] == 'Q' || chessBoard[m_data.y_sel][m_data.x_sel] == 'q')
		{
			return queen(chessBoard, m_data);
		}
		else if (chessBoard[m_data.y_sel][m_data.x_sel] == 'W' || chessBoard[m_data.y_sel][m_data.x_sel] == 'w')
		{
			return king(chessBoard, m_data);
		}
	}

	return false;
}

static bool pawn(char chessBoard[8][8], move m_data)
{
	if (m_data.playerTurn)
	{
		if (m_data.y_sel == 1 && m_data.y_mov == m_data.y_sel + 2 && m_data.x_sel == m_data.x_mov)
		{
			if (chessBoard[m_data.y_mov][m_data.x_mov] == ' ')
			{
				return true;
			}
		}
		else if (m_data.y_mov == m_data.y_sel + 1 && m_data.x_sel == m_data.x_mov)
		{
			if (chessBoard[m_data.y_mov][m_data.x_mov] == ' ')
			{
				return true;
			}
		}
		else if (m_data.y_mov == m_data.y_sel + 1 && (m_data.x_mov == m_data.x_sel + 1 || m_data.x_mov == m_data.x_sel - 1) &&
				chessBoard[m_data.y_mov][m_data.x_mov] != ' ')
		{
			if (isUpperOrLower(chessBoard[m_data.y_mov][m_data.x_mov]) == false)
			{
				return true;
			}
		}
	}
	else
	{

		if (m_data.y_sel == 6 && m_data.y_mov == m_data.y_sel - 2 && m_data.x_sel == m_data.x_mov)
		{
			if (chessBoard[m_data.y_mov][m_data.x_mov] == ' ')
			{
				return true;
			}
		}
		else if (m_data.y_mov == m_data.y_sel - 1 && m_data.x_sel == m_data.x_mov)
		{
			if (chessBoard[m_data.y_mov][m_data.x_mov] == ' ')
			{
				return true;
			}
		}
		else if (m_data.y_mov == m_data.y_sel - 1 && m_data.x_sel != m_data.x_mov && (m_data.x_mov == m_data.x_sel + 1 || m_data.x_mov == m_data.x_sel - 1) &&
				chessBoard[m_data.y_mov][m_data.x_mov] != ' ')
		{
			if (isUpperOrLower(chessBoard[m_data.y_mov][m_data.x_mov]))
			{
				return true;
			}
		}
	}

	return executeEnPassant(chessBoard, m_data);
}

static bool executeEnPassant(char chessBoard[8][8], move m_data)
{
	int y = m_data.playerTurn ? m_data.pawnY + 1 : m_data.pawnY - 1;

	// Move most target same x coordinate and y coordinate +/- 1.
	if (m_data.x_mov == m_data.pawnX && m_data.y_mov == y && m_data.enPassant)
	{
		chessBoard[m_data.pawnY][m_data.pawnX] = ' ';
		return true;
	}

	return false;
}

static bool rook(char chessBoard[8][8], move m_data)
{
	bool moveIsOK = false;

	if (m_data.x_sel > m_data.x_mov && m_data.y_sel == m_data.y_mov)
	{
		moveIsOK = true;
	}
	else if (m_data.x_sel < m_data.x_mov && m_data.y_sel == m_data.y_mov)
	{
		moveIsOK = true;
	}
	else if (m_data.y_sel > m_data.y_mov && m_data.x_sel == m_data.x_mov)
	{
		moveIsOK = true;
	}
	else if (m_data.y_sel < m_data.y_mov && m_data.x_sel == m_data.x_mov)
	{
		moveIsOK = true;
	}

	if (targetStatus(chessBoard, m_data) && moveIsOK)
	{
		return true;
	}

	return false;
}

static bool knight(char chessBoard[8][8], move m_data)
{
	if (m_data.y_mov == m_data.y_sel + 2 && (m_data.x_mov == m_data.x_sel + 1 || m_data.x_mov == m_data.x_sel - 1))
	{
		return targetStatus(chessBoard, m_data);
	}
	else if (m_data.y_mov == m_data.y_sel - 2 && (m_data.x_mov == m_data.x_sel + 1 || m_data.x_mov == m_data.x_sel - 1))
	{
		return targetStatus(chessBoard, m_data);
	}
	else if (m_data.x_mov == m_data.x_sel + 2 && (m_data.y_mov == m_data.y_sel + 1 || m_data.y_mov == m_data.y_sel - 1))
	{
		return targetStatus(chessBoard, m_data);
	}
	else if (m_data.x_mov == m_data.x_sel - 2 && (m_data.y_mov == m_data.y_sel + 1 || m_data.y_mov == m_data.y_sel - 1))
	{
		return targetStatus(chessBoard, m_data);
	}

	return false;
}

static bool bishop(char chessBoard[8][8], move m_data)
{
	int differenceX = 0, differenceY = 0;

	if (m_data.x_sel < m_data.x_mov)
	{
		differenceX = m_data.x_mov - m_data.x_sel;
	}
	else
	{
		differenceX = m_data.x_sel - m_data.x_mov;
	}

	if (m_data.y_sel < m_data.y_mov)
	{
		differenceY = m_data.y_mov - m_data.y_sel;
	}
	else
	{
		differenceY = m_data.y_sel - m_data.y_mov;
	}

	if (differenceX != differenceY)
	{
		return false;
	}

	return targetStatus(chessBoard, m_data);
}

static bool queen(char chessBoard[8][8], move m_data)
{
	int differenceX = 0, differenceY = 0;

	if (m_data.x_sel < m_data.x_mov)
	{
		differenceX = m_data.x_mov - m_data.x_sel;
	}
	else
	{
		differenceX = m_data.x_sel - m_data.x_mov;
	}

	if (m_data.y_sel < m_data.y_mov)
	{
		differenceY = m_data.y_mov - m_data.y_sel;
	}
	else
	{
		differenceY = m_data.y_sel - m_data.y_mov;
	}

	if (differenceX == differenceY)
	{
		return targetStatus(chessBoard, m_data);
	}

	if (m_data.x_sel > m_data.x_mov && m_data.y_sel == m_data.y_mov)
	{
		return targetStatus(chessBoard, m_data);
	}
	else if (m_data.x_sel < m_data.x_mov && m_data.y_sel == m_data.y_mov)
	{
		return targetStatus(chessBoard, m_data);
	}
	else if (m_data.y_sel > m_data.y_mov && m_data.x_sel == m_data.x_mov)
	{
		return targetStatus(chessBoard, m_data);
	}
	else if (m_data.y_sel < m_data.y_mov && m_data.x_sel == m_data.x_mov)
	{
		return targetStatus(chessBoard, m_data);
	}

	return false;
}

static bool king(char chessBoard[8][8], move m_data)
{
	bool moveIsOK = false;

	if (m_data.y_mov == m_data.y_sel - 1 && m_data.x_mov == m_data.x_sel)
	{
		moveIsOK = true;
	}
	else if (m_data.y_mov == m_data.y_sel - 1 && m_data.x_mov == m_data.x_sel + 1)
	{
		moveIsOK = true;
	}
	else if (m_data.y_mov == m_data.y_sel && m_data.x_mov == m_data.x_sel + 1)
	{
		moveIsOK = true;
	}
	else if (m_data.y_mov == m_data.y_sel + 1 && m_data.x_mov == m_data.x_sel + 1)
	{
		moveIsOK = true;
	}
	else if (m_data.y_mov == m_data.y_sel + 1 && m_data.x_mov == m_data.x_sel)
	{
		moveIsOK = true;
	}
	else if (m_data.y_mov == m_data.y_sel + 1 && m_data.x_mov == m_data.x_sel - 1)
	{
		moveIsOK = true;
	}
	else if (m_data.y_mov == m_data.y_sel && m_data.x_mov == m_data.x_sel - 1)
	{
		moveIsOK = true;
	}
	else if (m_data.y_mov == m_data.y_sel - 1 && m_data.x_mov == m_data.x_sel - 1)
	{
		moveIsOK = true;
	}

	if (targetStatus(chessBoard, m_data) && moveIsOK)
	{
		return true;
	}

	return false;
}

static bool isTargetUnderThreat(char chessBoard[8][8], move m_data, int x, int y)
{
	m_data.x_mov = x, m_data.y_mov = y;
	m_data.blocked = true;

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			m_data.y_sel = i, m_data.x_sel = j;

			if (chessBoard[i][j] != ' ')
			{
				if (gameRules(chessBoard, m_data) == true)
				{
					m_data = isPathBlocked(chessBoard, m_data);

					if (m_data.blocked == false)
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

static bool targetStatus(char chessBoard[8][8], move m_data)
{
	if (m_data.playerTurn == true)
	{
		if (chessBoard[m_data.y_mov][m_data.x_mov] == ' ' || isUpperOrLower(chessBoard[m_data.y_mov][m_data.x_mov]) == false)
		{
			return true;
		}
	}
	else if (m_data.playerTurn == false)
	{
		if (chessBoard[m_data.y_mov][m_data.x_mov] == ' ' || isUpperOrLower(chessBoard[m_data.y_mov][m_data.x_mov]) == true)
		{
			return true;
		}
	}

	return false;
}

static bool isUpperOrLower(char letter)
{
	bool result = false;

	if (letter >= 'A' && letter <= 'Z')
	{
		result = true;
	}

	if (letter >= 'a' && letter <= 'z')
	{
		result = false;
	}

	return result;
}

bool checkmate(char chessBoard[8][8], move m_data)
{
	int x = 0, y = 0;

	if (!findTheKing(chessBoard, &x, &y, m_data.playerTurn))
	{
		return false;
	}

	if (!isKingInCheck(chessBoard, m_data, x, y))
	{
		return false;
	}

	if (!isKingLocked(chessBoard, m_data, x, y))
	{
		return false;
	}

	if (!isThreatRemoveable(chessBoard, m_data, x, y))
	{
		return false;
	}

	return true;
}

static bool findTheKing(char chessBoard[8][8], int *kingX, int *kingY, bool playerTurn)
{
	char king = playerTurn == true ? 'W' : 'w';

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (chessBoard[i][j] == king)
			{
				*kingX = j;
				*kingY = i;
				return true;
			}
		}
	}

	return false;
}

static inline bool isKingInCheck(char chessBoard[8][8], move m_data,
		int kingX, int kingY)
{
	m_data.playerTurn = m_data.playerTurn == true ? false : true;
	return isTargetUnderThreat(chessBoard, m_data, kingX, kingY);
}

static bool isKingLocked(char chessBoard[8][8], move m_data, int kingX, int kingY)
{
	for (int i = 0; i < 8; ++i)
	{
		switch (i)
		{
			case 0:
				--kingY;
				break;
			case 1:
				++kingX;
				break;
			case 2:
				++kingY;
				break;
			case 3:
				++kingY;
				break;
			case 4:
				--kingX;
				break;
			case 5:
				--kingX;
				break;
			case 6:
				--kingY;
				break;
			case 7:
				--kingY;
				break;
		}

		if (m_data.playerTurn == true)
		{
			if ((chessBoard[kingY][kingX] == ' ' || !isUpperOrLower(chessBoard[kingY][kingX])) &&
					kingY >= 0 && kingY <= 7 && kingX >= 0 && kingX <= 7)
			{
				if (!isTargetUnderThreat(chessBoard, m_data, kingX, kingY))
				{
					return false;
				}
			}
		}
		else
		{
			if ((chessBoard[kingY][kingX] == ' ' || isUpperOrLower(chessBoard[kingY][kingX])) &&
					kingY >= 0 && kingY <= 7 && kingX >= 0 && kingX <= 7)
			{
				if (!isTargetUnderThreat(chessBoard, m_data, kingX, kingY))
				{
					return false;
				}
			}
		}
	}

	return true;
}

static bool isThreatRemoveable(char chessBoard[8][8], move m_data, int kingX, int kingY)
{
	int threatX = 0, threatY = 0;
	int pathY[8], pathX[8];
	int pathSize = 0;

	findThreat(chessBoard, m_data.playerTurn, kingX, kingY, &threatX);

	m_data.x_sel = threatX, m_data.y_sel = threatY;

	pathSize = getPath(chessBoard, m_data, pathX, pathY);

	return removalofThreat(chessBoard, m_data,
			pathX, pathY, pathSize);
}

static void findThreat(char chessBoard[8][8], bool playerTurn, int kingX, int kingY, int *threatX)
{
	move m_data;

	m_data.blocked = false;
	m_data.playerTurn = playerTurn == true ? true : false;
	m_data.x_mov = kingX;
	m_data.y_mov = kingY;

	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			m_data.x_sel = j;
			m_data.y_sel = i;

			if (gameRules(chessBoard, m_data))
			{
				m_data = isPathBlocked(chessBoard, m_data);
				if (!m_data.blocked)
				{
					*threatX = j;
					*threatX = i;
					return;
				}
			}
		}
	}
}

static int getPath(char chessBoard[8][8], move m_data, int pathX[8], int pathY[8])
{
	int pathSize = 0;
	int x = m_data.x_sel, y = m_data.y_sel;

	if (chessBoard[m_data.y_sel][m_data.x_sel] == 'K' || chessBoard[m_data.y_sel][m_data.x_sel] == 'k')
	{
		return pathSize;
	}

	while (x != m_data.x_mov || y != m_data.y_mov)
	{
		pathY[pathSize] = y;
		pathX[pathSize] = x;
		++pathSize;

		if (m_data.x_mov > x)
		{
			++x;
		}
		else if (m_data.x_mov < x)
		{
			--x;
		}

		if (m_data.y_mov > y)
		{
			++y;
		}
		else if (m_data.y_mov < y)
		{
			--y;
		}
	}

	return pathSize;
}

static bool removalofThreat(char chessBoard[8][8], move m_data, int pathX[8], int pathY[8], int pathSize)
{
	for (int i = 0; i < 8; ++i)
	{
		for (int j = 0; j < 8; ++j)
		{
			if (isUpperOrLower(chessBoard[i][j]) == m_data.playerTurn)
			{
				tryMoveAtPath(chessBoard, pathX, pathY,
						pathSize, i, j, m_data);
			}
		}
	}

	return true;
}

static bool tryMoveAtPath(char chessBoard[8][8], int pathX[8], int pathY[8], int pathSize, int x, int y, move m_data)
{
	m_data.playerTurn = m_data.playerTurn == true ? true : false;
	m_data.x_sel = x;
	m_data.y_sel = y;

	for (int i = 0; i < pathSize; ++i)
	{
		m_data.x_mov = pathX[i];
		m_data.y_mov = pathY[i];

		if (gameRules(chessBoard, m_data))
		{
			return true;
		}
	}

	return false;
}


/**
 * Will run just the base engine in console. 
 */
void runFromConsole(void)
{
	char chessBoard[8][8] =
	{
		{'R', 'K', 'B', 'Q', 'W', 'B', 'K', 'R'},
		{'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'},
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
		{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
		{'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'},
		{'r', 'k', 'b', 'q', 'w', 'b', 'k', 'r'}
	};	

	move m_data = initMove();
	while (true)
	{
		drawConsole(chessBoard);
		m_data = getUserInput(m_data);
		m_data = isPathBlocked(chessBoard, m_data);
		m_data = castlingController(chessBoard, m_data);
		m_data = executeMove(chessBoard, m_data);
		m_data = enPassant(chessBoard, m_data);
		if(checkmate(chessBoard, m_data))
		{
			break;
		}
	}
	printf("\n###### Checkmate ######\n");
}

