/*
 * This is a chess game.
 * Author: Oscar Bergström.
 * Last edited: 2022-06-1BOARD_SIZE. 
 */

#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <SDL2/SDL.h>
//#include <SDL2/SDL.h>

#define UNDEF 65    // Value used if nothing is selected.
#define START_X 110 // Start of X value of the chess board. 
#define START_Y	50	// Start of Y value of the chess board.
#define BOARD_SIZE 8

int main(int argc, char* argv[]);
std::vector <char> checkMove(int, int, int, int, char[BOARD_SIZE][BOARD_SIZE]);
bool moveAction(int, int, int, int, char[BOARD_SIZE][BOARD_SIZE], bool, char, char, std::vector <char>);
bool pawnRules(int, int, int, int, std::vector <char>, char, bool);
bool rookRules(int, int, int, int, std::vector <char>, char, bool);
bool knightRules(int, int, int, int, char, char, bool);
bool bishopRules(int, int, int, int, std::vector <char>, char, bool);
bool queenRules(int, int, int, int, std::vector <char>, char, bool);
bool kingRules(int, int, int, int, std::vector <char>, char, bool); 
bool is_check(char board[BOARD_SIZE][BOARD_SIZE], bool);
bool lookForMove(char[BOARD_SIZE][BOARD_SIZE], int, int, int, int, char, bool, char);
void castling(bool, char[BOARD_SIZE][BOARD_SIZE], bool*, bool*, bool*, bool*);
void castlingAction(char[BOARD_SIZE][BOARD_SIZE], bool*, bool*, bool*, bool*, bool*, int, int, int, int, bool);
void createSquares(SDL_Rect[BOARD_SIZE * BOARD_SIZE], const int, const int);
void drawBackground(SDL_Renderer* rend_Board);
void drawBoard(SDL_Renderer*, SDL_Rect[64], char[BOARD_SIZE][BOARD_SIZE], int, int, bool);
void drawRook(SDL_Renderer*, int, int);
void drawKnight(SDL_Renderer*, int, int);
void drawBishop(SDL_Renderer*, int, int);
void drawQueen(SDL_Renderer*, int, int);
void drawKing(SDL_Renderer*, int, int);
void drawPawn(SDL_Renderer*, int, int); 

int main(int argc, char* argv[]) {

    bool is_Running = true;                                                                // Track if game is running.                                                                
    bool playerTurn = true, opponent = false;                                              // Stores data about game turns.
    bool check = false, checkmate = false;                                                 // Check status.
    bool shortCastp1 = false, longCastp1 = false, shortCastp2 = false, longCastp2 = false; // Castling.
    bool selected = false, can_move = false;                                               // Is a piece selected and can it move. 
	
    int selectY = UNDEF, selectX = UNDEF, moveY = UNDEF, moveX = UNDEF;          // Location of a selected piece and its movement target.
    int kingX = UNDEF, kingY = UNDEF;                                            // Stores the x and y position of the king.
    int hoovering = UNDEF, mouseX, mouseY, clicked = UNDEF, clickedMove = UNDEF; // Mouse interaction, such as hoovering, position and click. 
    int startX = START_X, startY = START_Y;                                      // Current X and Y location of the chess board squares. 
	
	std::vector <char> movement; // Pattern of a move. 

    char piece = ' ', target = ' ', king = ' '; // Letter corresponding to a piece, target and king. 
     
	char board[BOARD_SIZE][BOARD_SIZE] = { 'R', 'K', 'B', 'Q','W', 'B', 'K', 'R',                            
						 'P', 'P', 'P', 'P','P', 'P', 'P', 'P',                            
						 ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',                            
						 ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',                            
						 ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',                            
						 ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',                            
						 'p', 'p', 'p', 'p','p', 'p', 'p', 'p',                            
						 'r', 'k', 'b', 'q','w', 'b', 'k', 'r' };                         


    // Graphics variabels. 
	SDL_Window* chess_Board;           // Window variable. 
	SDL_Renderer* rend_Board = NULL;   // Renderer.
	SDL_Rect rect_Board[64];           // Array of squares: Used when writing the chess board. 
	SDL_Event event;                   // Event handler. 
	Uint32 button;                     // Stores mouse and button values.
    const int width = 50, height = 50; // Size of a square. 

    // Initgraphic, create and add window render settings. 
	chess_Board = SDL_CreateWindow("OSCARJFB-Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 500, SDL_WINDOW_RESIZABLE);
	rend_Board = SDL_CreateRenderer(chess_Board, -1, SDL_RENDERER_ACCELERATED);

    createSquares(rect_Board, width, height);

	while (is_Running == true) {
        // Find the king corresponding to the current player turn. 
		if (playerTurn == true) king = 'W';
		else king = 'w';
		for (int i = 0; i < BOARD_SIZE; i++)
			for (int n = 0; n < BOARD_SIZE; n++)
				if (board[i][n] == king) {
					kingX = n;
					kingY = i;
				}

		// Search for any threat to piece of type king. 
		if (playerTurn == true) opponent = false;
		else opponent = true;
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int n = 0; n < BOARD_SIZE; n++) {
				if (board[i][n] != ' ') check = lookForMove(board, i, n, kingY, kingX, board[i][n], opponent, king);
				if (check == true) break;
			}
			if (check == true) break;
		}

        // If the king is under threat check status must be controlled. 
		if (check == true) {
			checkmate = is_check(board, playerTurn);
            if (checkmate == true) {
                std::cout << "checkmate"; 
                system("pause");
                break;
            }
		}

        // Scan if castling is possible.
        castling(playerTurn, board, &shortCastp1, &longCastp1, &shortCastp2, &longCastp2);

        // If a piece is selected get mouse state when hoovering.
        if (selected == true) {
            int k = 0; // Reset column.
            int n = 0; // Reset row. 

            SDL_GetMouseState(&mouseX, &mouseY); // Current mouse position.
            piece = board[selectY][selectX];     // Store the letter corresponding to a selected piece. 
            
            // Iterate over the game board. 
            for (int i = 0; i < 64; i++) {
               
                target = board[k][n]; // Target is the current hoovering posistion. 
               
                // Mouse position compared to the squares. 
                if (mouseX <= rect_Board[i].x + width && mouseX >= rect_Board[i].x &&
                    mouseY <= rect_Board[i].y + height && mouseY >= rect_Board[i].y) {
                    movement = checkMove(selectY, selectX, k, n, board);

                    // Castling, hoovering over rook when a king is selected. 
                    if (playerTurn == true && board[k][n] == 'R' && board[selectY][selectX] == 'W' && 
                        shortCastp1 == true && k == 0 && n == 7) { // Shortcast.
                        can_move = true;
                        hoovering = i;
                        movement.clear();
                        break;
                    }   
                    if (playerTurn == false && board[k][n] == 'r' && board[selectY][selectX] == 'w' && 
                        shortCastp2 == true && k == 7 && n == 7) { // Shortcast.
                        can_move = true;
                        hoovering = i;
                        movement.clear();
                        break;
                    }
                    if (playerTurn == true && board[k][n] == 'R' && board[selectY][selectX] == 'W' && 
                        longCastp1 == true && k == 0 && n == 0) { // Longcast.
                        can_move = true;
                        hoovering = i;
                        movement.clear();
                        break;
                    }
                    if (playerTurn == false && board[k][n] == 'r' && board[selectY][selectX] == 'w' &&
                        longCastp2 == true && k == 7 && n == 0) { // Longcast.
                        can_move = true;
                        hoovering = i;
                        movement.clear();
                        break;
                    }

                    // Hoovering over a the chess board.
                    if (lookForMove(board, selectY, selectX, k, n, piece, playerTurn, board[k][n]) == true) { // If a move is possible. 
                        can_move = true;
                        hoovering = i;
                        movement.clear();
                        break;
                    }
                    else if (lookForMove(board, selectY, selectX, k, n, piece, playerTurn, board[k][n]) == false) { // if a move is not possible. 
                        can_move = false;
                        hoovering = i;
                        movement.clear();
                        break;
                    }
                }

                // Iteration of 2d array using a while loop.
                n++; 
                if (n == BOARD_SIZE) {   // Row.
                    n = 0;      // Reset.
                    k++;        // Column.
                }
            }
        }

        // Event control. 
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_QUIT) is_Running = false; // Exit the game loop.
			if (event.type == SDL_MOUSEBUTTONDOWN) {        // If event = mouse button, down. 

				button = SDL_GetMouseState(&mouseX, &mouseY); // Get mouse position in event of a click(x, y), stored the clicked button. 
				if (button == SDL_BUTTON(1)) {                // In case of button = left. 
					
                    int k = 0; // Reset column.
					int n = 0; // Reset row. 

                    // Iterate over the game board. 
					for (int i = 0; i < 64; i++) {
                        
                        // Mouse position compared to the squares. 
						if (mouseX <= rect_Board[i].x + width && mouseX >= rect_Board[i].x &&
							mouseY <= rect_Board[i].y + height && mouseY >= rect_Board[i].y) {
                            if(playerTurn == true) // P1. 
								if ((board[k][n] == 'P' || board[k][n] == 'R' || board[k][n] == 'K' || 
                                     board[k][n] == 'B' || board[k][n] == 'Q' || board[k][n] == 'W') 
                                     && selected == false) {
									clicked = i;     // Number of the clicked square. 
                                    selectX = n;     // X position of the clicked piece.
                                    selectY = k;     // Y position of the clicked piece.
                                    selected = true; // If true a piece has been selected. 
									break;
								}

							if (playerTurn == false) // P2.
								if ((board[k][n] == 'p' || board[k][n] == 'r' || board[k][n] == 'k' ||
                                     board[k][n] == 'b' || board[k][n] == 'q' || board[k][n] == 'w') 
                                     && selected == false) {

									clicked = i;     // Number of the clicked square.
                                    selectX = n;     // X position of the clicked piece.
                                    selectY = k;     // Y position of the clicked piece.
                                    selected = true; // If true a piece has been selected. 
									break;
								}

                            // Try to make a game move.
                            if (selected == true) {
                                hoovering = false; // No longer hoovering, reset = false. 
                                moveX = n;         // X target.
                                moveY = k;         // Y target.
                                movement = checkMove(selectY, selectX, moveY, moveX, board); // Movement pattern. 
                                piece = board[selectY][selectX];                             // Type of selcted piece.
                                target = board[moveY][moveX];                                // Type of selcted target.
                                playerTurn = moveAction(selectY, selectX, moveY, moveX, board, playerTurn, target, piece, movement);
                                clicked = UNDEF;   // Reset.
                                hoovering = UNDEF; // Reset.
                                selected = false;  // Reset.
                                movement.clear();  // Reset. 
                            }
						}

                        // Iteration of 2d array using a while loop.
						n++;
						if (n == BOARD_SIZE) { // Row.
							n = 0;    // Reset.
							k++;      // Column.
						}
					}
				}
			}
		}

        // Castling move action. 
        if ((shortCastp1 || longCastp1 || shortCastp2 || longCastp2) == true)  
        castlingAction(board, &playerTurn, &shortCastp1, &longCastp1, &shortCastp2, 
                       &longCastp2, selectX, selectY, moveX, moveY, check);

        // Draw graphics. 
		drawBackground(rend_Board);
		drawBoard(rend_Board, rect_Board, board, clicked, hoovering, can_move);
		SDL_RenderPresent(rend_Board);
	}


	// EXIT.
	SDL_DestroyRenderer(rend_Board); // Destroy the renderer.
	SDL_DestroyWindow(chess_Board);  // Destroy the window.
	SDL_Quit();                      // Quit SDL
    movement.clear();                // Reset.
	return 0;                        // Exit. 
}

std::vector <char> checkMove(int selectY, int  selectX, 
                             int  moveY,  int  moveX, 
                             char board[BOARD_SIZE][BOARD_SIZE]) {

    int trackX = selectX; // Used when tracking the movement. 

    std::vector <char> movement; // Store the registered pattern.

    // Register if the path is down in the direction of right or left. 
    if (moveY > selectY && selectX != moveX) {
        for (int i = selectY; i <= moveY; i++) {
            if (selectX < moveX) { // Right. 
                movement.push_back(board[i][trackX]);
                trackX++;
            }
            else if (selectX > moveX) { // Left. 
                movement.push_back(board[i][trackX]);
                trackX--;
            }
        }
    }

    // Register if the path is up in the direction of right or left. 
    if (moveY < selectY && selectX != moveX) {
        for (int i = selectY; i >= moveY; i--) {
            if (selectX < moveX) { // Right. 
                movement.push_back(board[i][trackX]);
                trackX++;
            }
            else if (selectX > moveX) { // Left.
                movement.push_back(board[i][trackX]);
                trackX--;
            }
        }
    }

    // Register if the path is in the direction of up or down. 
    if (moveY != selectY && selectX == moveX) {
        if (selectY < moveY) { // Down.
            for (int i = selectY; i <= moveY; i++)
                movement.push_back(board[i][selectX]);
        }
        else if (selectY > moveY) { // Up.
            for (int i = selectY; i >= moveY; i--)
                movement.push_back(board[i][selectX]);
        }
    }

    // Register if the path is in the direction of right or left. 
    if (moveY == selectY && selectX != moveX) {
        if (selectX < moveX) { // Right.
            for (int i = selectX; i <= moveX; i++)
                movement.push_back(board[selectY][i]);
        }
        else if (selectX > moveX) { // Left.
            for (int i = selectX; i >= moveX; i--)
                movement.push_back(board[selectY][i]);
        }
    }

    return movement;
}

bool moveAction(int selectY,      int  selectX, 
                int  moveY,       int  moveX, 
                char board[BOARD_SIZE][BOARD_SIZE], bool playerTurn, 
                char target,      char piece, 
                std::vector <char> movement) {

    bool validMove = false; // If true a move is valid.
    bool check = false;     // check control.
    bool opponent = false;  // Status of the opponent p1 = true, p2 = false. 

    int kingX = UNDEF, kingY = UNDEF; // King x and y position on the chess board.
 
    char king = ' ', selected = ' ', moved = ' '; // Store the king and selected piece and move. 

    // Resulting action depending on the selected type of chess piece. 
    if (board[selectY][selectX] == 'p' || board[selectY][selectX] == 'P')
        validMove = pawnRules(selectY, selectX, moveY, moveX, movement, piece, playerTurn);
    else if (board[selectY][selectX] == 'r' || board[selectY][selectX] == 'R')
        validMove = rookRules(selectY, selectX, moveY, moveX, movement, piece, playerTurn);
    else if (board[selectY][selectX] == 'k' || board[selectY][selectX] == 'K')
        validMove = knightRules(selectY, selectX, moveY, moveX, target, piece, playerTurn);
    else if (board[selectY][selectX] == 'b' || board[selectY][selectX] == 'B')
        validMove = bishopRules(selectY, selectX, moveY, moveX, movement, piece, playerTurn);
    else if (board[selectY][selectX] == 'q' || board[selectY][selectX] == 'Q')
        validMove = queenRules(selectY, selectX, moveY, moveX, movement, piece, playerTurn);
    else if (board[selectY][selectX] == 'w' || board[selectY][selectX] == 'W')
        validMove = kingRules(selectY, selectX, moveY, moveX, movement, piece, playerTurn);

    moved = board[moveY][moveX];        // Make move. 
    selected = board[selectY][selectX]; // Will change the board[BOARD_SIZE][BOARD_SIZE] in accordance to the game actions. 
    board[moveY][moveX] = board[selectY][selectX];
    board[selectY][selectX] = ' ';


    // Find the kings position. 
    if (playerTurn == true) king = 'W';
    else king = 'w';
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int n = 0; n < BOARD_SIZE; n++)
            if (board[i][n] == king) {
                kingX = n;
                kingY = i;
            }

    // If the king is under threat. 
    if (playerTurn == true) opponent = false;
    else opponent = true;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int n = 0; n < BOARD_SIZE; n++)
            if (board[i][n] != ' ') {
                check = lookForMove(board, i, n, kingY, kingX, board[i][n], opponent, king);
                if (check == true) break;
            }
        if (check == true) break;
    }

    // Check if move is valid.
    if (check == false && validMove == true) {
        if (playerTurn == true) playerTurn = false;
        else playerTurn = true;
    }
    else {
        board[selectY][selectX] = selected;
        board[moveY][moveX] = moved;
    }

    return playerTurn;
}

bool pawnRules(int selectY, int selectX, 
               int moveY,   int moveX, 
               std::vector <char> movement, 
               char piece,  bool playerTurn) {
    
    // White pawn. 
    if (piece == 'P' && playerTurn == true) {
        if (selectY == 1 && moveY == selectY + 2 && moveX == selectX) { // Start move.
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && movement[i] == ' ') return true;
                else if (movement[i] != ' ') return false;
            }
        }
        if (moveY == selectY + 1 && moveX == selectX) { // Regular move.
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && movement[i] == ' ') return true;
                else return false;
            }
        }
        if (moveY == selectY + 1 && (moveX == selectX + 1 || moveX == selectX - 1)) { // Attack move.
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] == 'w' || movement[i] == 'q' || movement[i] == 'k' || 
                    movement[i] == 'r' || movement[i] == 'p' || movement[i] == 'b') return true;
                else return false;
            }
        }
        
    }
    
    // Black pawn.
    if (piece == 'p' && playerTurn == false) {
        if (selectY == 6 && moveY == selectY - 2 && moveX == selectX) { // Start move.
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && movement[i] == ' ') return true;
                else if (movement[i] != ' ') return false;
            }
        }
        if (moveY == selectY - 1 && moveX == selectX) { // Regular move.
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && movement[i] == ' ') return true;
                else return false;
            }
        }
        if (moveY == selectY - 1 && (moveX == selectX + 1 || moveX == selectX - 1)) { // Attack move.
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] == 'W' || movement[i] == 'Q' || movement[i] == 'K' || 
                    movement[i] == 'R' || movement[i] == 'P' || movement[i] == 'B') return true;
                else return false;
            }
        }
    }

    return false;
}

bool rookRules(int selectY, int selectX, 
               int moveY,   int moveX, 
               std::vector <char> movement, 
               char piece,  bool playerTurn) {
   
    // White rook.
    if (piece == 'R' && playerTurn == true) {
        if (selectX != moveX && selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && 
                    movement[i] != 'R' && movement[i] != 'P' && movement[i] != 'B') return true;
                else if (movement[i] != ' ') return false;
            }
        }
        if (selectX == moveX && selectY != moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && 
                    movement[i] != 'R' && movement[i] != 'P' && movement[i] != 'B') return true;
                else if (movement[i] != ' ') return false;
            }
        }
    }

    // Black rook.
    if (piece == 'r' && playerTurn == false) {
        if (selectX != moveX && selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && 
                    movement[i] != 'r' && movement[i] != 'p' && movement[i] != 'b') return true;
                else if (movement[i] != ' ') return false;
            }
        }
        if (selectX == moveX && selectY != moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && 
                    movement[i] != 'r' && movement[i] != 'p' && movement[i] != 'b') return true;
                else if (movement[i] != ' ') return false;
            }
        }
    }

    return false;
}

bool knightRules(int selectY, int selectX, 
                 int moveY,   int moveX, 
                 char target, char piece, 
                 bool playerTurn) {
    
    // White knight.
    if (piece == 'K' && playerTurn == true)
        if (moveY == selectY + 2 && moveX == selectX + 1 ||
            moveY == selectY + 2 && moveX == selectX - 1 ||
            moveY == selectY - 2 && moveX == selectX + 1 ||
            moveY == selectY - 2 && moveX == selectX - 1 ||
            moveY == selectY + 1 && moveX == selectX + 2 ||
            moveY == selectY - 1 && moveX == selectX + 2 ||
            moveY == selectY + 1 && moveX == selectX - 2 ||
            moveY == selectY - 1 && moveX == selectX - 2)
            if (target == ' ' || target == 'p' || target == 'r' || 
                target == 'b' || target == 'w' || target == 'q' || 
                target == 'k') return true;


    // Black knight.
    if (piece == 'k' && playerTurn == false)
        if (moveY == selectY + 2 && moveX == selectX + 1 ||
            moveY == selectY + 2 && moveX == selectX - 1 ||
            moveY == selectY - 2 && moveX == selectX + 1 ||
            moveY == selectY - 2 && moveX == selectX - 1 ||
            moveY == selectY + 1 && moveX == selectX + 2 ||
            moveY == selectY - 1 && moveX == selectX + 2 ||
            moveY == selectY + 1 && moveX == selectX - 2 ||
            moveY == selectY - 1 && moveX == selectX - 2)
            if (target == ' ' || target == 'P' || target == 'R' || 
                target == 'B' || target == 'W' || target == 'Q' || 
                target == 'K') return true;

    return false;
}

bool bishopRules(int selectY, int selectX, 
                 int moveY,   int moveX, 
                 std::vector <char> movement, 
                 char piece,  bool playerTurn) {
    
    // Values are used to control movement path.
    int stepsY = UNDEF, stepsX = UNDEF;

    // White bishop.
    if (piece == 'B' && playerTurn == true) {
        if (selectY < moveY)
            stepsY = moveY - selectY;
        else
            stepsY = selectY - moveY;

        if (selectX < moveX)
            stepsX = moveX - selectX;
        else
            stepsX = selectX - moveX;

        if (stepsY == stepsX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && 
                    movement[i] != 'R' && movement[i] != 'P' && movement[i] != 'B') return true;
                else if (movement[i] != ' ') return false;
            }
        }
    }


    // Black bishop.
    if (piece == 'b' && playerTurn == false) {
        if (selectY < moveY)
            stepsY = moveY - selectY;
        else
            stepsY = selectY - moveY;

        if (selectX < moveX)
            stepsX = moveX - selectX;
        else
            stepsX = selectX - moveX;

        if (stepsY == stepsX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && 
                    movement[i] != 'r' && movement[i] != 'p' && movement[i] != 'b') return true;
                else if (movement[i] != ' ') return false;
            }
        }
    }

    return false;
}

bool queenRules(int selectY, int selectX, 
                int moveY,   int moveX, std::vector <char> movement, 
                char piece,  bool playerTurn) {
    
    // Values are used to control movement path.
    int stepsY = UNDEF, stepsX = UNDEF;

    // White queen.
    if (piece == 'Q' && playerTurn == true) {
        if (selectY != moveY && selectX == moveX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && 
                    movement[i] != 'R' && movement[i] != 'P' && movement[i] != 'B') return true;
                else if (movement[i] != ' ') return false;
            }
        }

        if (selectX != moveX && selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && 
                    movement[i] != 'R' && movement[i] != 'P' && movement[i] != 'B') return true;
                else if (movement[i] != ' ') return false;
            }
        }

        if (selectY < moveY)
            stepsY = moveY - selectY;
        else
            stepsY = selectY - moveY;

        if (selectX < moveX)
            stepsX = moveX - selectX;
        else
            stepsX = selectX - moveX;

        if (stepsY == stepsX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && 
                    movement[i] != 'R' && movement[i] != 'P' && movement[i] != 'B') return true;
                else if (movement[i] != ' ') return false;
            }
        }
    }

    // Black queen.
    if (piece == 'q' && playerTurn == false) {
        if (selectY != moveY && selectX == moveX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && 
                    movement[i] != 'r' && movement[i] != 'p' && movement[i] != 'b') return true;
                else if (movement[i] != ' ') return false;
            }
        }

        if (selectX != moveX && selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && 
                    movement[i] != 'r' && movement[i] != 'p' && movement[i] != 'b') return true;
                else if (movement[i] != ' ') return false;
            }
        }

        if (selectY < moveY)
            stepsY = moveY - selectY;
        else
            stepsY = selectY - moveY;

        if (selectX < moveX)
            stepsX = moveX - selectX;
        else
            stepsX = selectX - moveX;

        if (stepsY == stepsX)
        {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && 
                    movement[i] != 'r' && movement[i] != 'p' && movement[i] != 'b') return true;
                else if (movement[i] != ' ') return false;
            }
        }
    }

    return false;
}

bool kingRules(int selectY, int selectX, 
               int moveY,   int moveX, 
               std::vector <char> movement, 
               char piece,  bool playerTurn) {
    
    // Values are used to control movement path.
    int stepsY = UNDEF, stepsX = UNDEF;

    // White king.
    if (piece == 'W' && playerTurn == true) {
        if (selectY != moveY && 
           (moveY == selectY + 1 || moveY == selectY - 1) && 
            selectX == moveX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && 
                    movement[i] != 'R' && movement[i] != 'P' && movement[i] != 'B') return true;
                else if (movement[i] != ' ') return false;
            }
        }

        if (selectX != moveX && 
           (moveX == selectX + 1 || moveX == selectX - 1) && 
            selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && 
                    movement[i] != 'R' && movement[i] != 'P' && movement[i] != 'B') return true;
                else if (movement[i] != ' ') return false;
            }
        }

        if (selectY < moveY)
            stepsY = moveY - selectY;
        else
            stepsY = selectY - moveY;

        if (selectX < moveX)
            stepsX = moveX - selectX;
        else
            stepsX = selectX - moveX;

        if (stepsY == stepsX && 
           (moveX == (selectX + 1) || moveX == (selectX - 1)) && 
           (moveY == selectY + 1 || moveY == selectY - 1)) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && 
                    movement[i] != 'R' && movement[i] != 'P' && movement[i] != 'B') return true;
                else if (movement[i] != ' ') return false;
            }
        }
    }

 
    // Black king.
    if (piece == 'w' && playerTurn == false) {
        if (selectY != moveY && 
           (moveY == selectY + 1 || moveY == selectY - 1) && 
            selectX == moveX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && 
                    movement[i] != 'r' && movement[i] != 'p' && movement[i] != 'b') return true;
                else if (movement[i] != ' ') return false;
            }
        }

        if (selectX != moveX && 
           (moveX == selectX + 1 || moveX == selectX - 1) && 
            selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && 
                    movement[i] != 'r' && movement[i] != 'p' && movement[i] != 'b') return true;
                else if (movement[i] != ' ') return false;
            }
        }

        if (selectY < moveY)
            stepsY = moveY - selectY;
        else
            stepsY = selectY - moveY;

        if (selectX < moveX)
            stepsX = moveX - selectX;
        else
            stepsX = selectX - moveX;

        if (stepsY == stepsX && 
           (moveX == selectX + 1 || moveX == selectX - 1) && 
           (moveY == selectY + 1 || moveY == selectY - 1)) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == movement.size() - 1 && 
                    movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && 
                    movement[i] != 'r' && movement[i] != 'p' && movement[i] != 'b') return true;
                else if (movement[i] != ' ') return false;
            }
        }
    }

    return false;
}

bool is_check(char board[BOARD_SIZE][BOARD_SIZE], bool playerTurn) {
    
    // x and y position for the king, any possible threat and available disruptors.
    int kingX = UNDEF, kingY = UNDEF; 
    int threatX = UNDEF, threatY = UNDEF; 
    int removeX = UNDEF, removeY = UNDEF;         

    // Store the type of piece. 
    char king = ' ', queen = ' ', bishhop = ' ', knight = ' ', rook = ' ', pawn = ' ';                        
    
    bool checkmate = false;                                                                                
    bool opponent = false; 
    bool removeThreat = false;
    
    bool threat[BOARD_SIZE] = { false, false, false, false, false, false, false, false };
    bool is_Safe[BOARD_SIZE] = { true, true, true, true, true, true, true, true };

    // Store piece type depending on player turn. 
    if (playerTurn == true) {
        king = 'W';
        queen = 'Q';
        bishhop = 'B';
        knight = 'K';
        rook = 'R';
        pawn = 'P';
    }
    else {
        king = 'w';
        queen = 'q';
        bishhop = 'b';
        knight = 'k';
        rook = 'r';
        pawn = 'p';
    }

    // Find the kings position. 
    for (int i = 0; i < BOARD_SIZE; i++)
        for (int n = 0; n < BOARD_SIZE; n++)
            if (board[i][n] == king) {
                kingX = n; // x.
                kingY = i; // y.
            }

    // Check if a piece threaten the king. 
    if (playerTurn == true) opponent = false;
    else opponent = true;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int n = 0; n < BOARD_SIZE; n++)
            if (board[i][n] != ' ') {
                checkmate = lookForMove(board, i, n, kingY, kingX, 
                                        board[i][n], opponent, king);
                if (checkmate == true) { // If true store the position of the threat. 
                    threatX = n; 
                    threatY = i;
                    break;               // Their can only be one threat, therefore break when found.
                }
            }
        if (checkmate == true) break;    // Their can only be one threat, therefore break when found.
    }

    // Scan for any piece that might be able to remove the threat. 
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int n = 0; n < BOARD_SIZE; n++)
            if (board[i][n] != ' ') {
                removeThreat = lookForMove(board, i, n, threatY, threatX, 
                                           board[i][n], playerTurn, board[threatY][threatX]);
                if (removeThreat == true) { // Position of the possible disruptor. 
                    removeX = n;
                    removeY = i;
                    break;                  // break if threat can be removed. 
                }
            }
        if (removeThreat == true) break;    // break if threat can be removed. 
    }
    if (removeThreat == true) checkmate = false;
    else checkmate = true;

    // Is the threatend king allowed to move anywhere? 
    // Temporarely remove the king as to enable a full scan of its perimeter.
    board[kingY][kingX] = ' '; // Removed king. 
    if (checkmate == true) {
        for (int i = 0; i < BOARD_SIZE; i++)
            for (int n = 0; n < BOARD_SIZE; n++)
                if (board[i][n] != ' ') {
                    threat[0] = lookForMove(board, i, n, kingY - 1, kingX + 1, 
                                            board[i][n], opponent, king);
                    threat[1] = lookForMove(board, i, n, kingY, kingX + 1, 
                                            board[i][n], opponent, king);
                    threat[2] = lookForMove(board, i, n, kingY + 1, kingX + 1, 
                                            board[i][n], opponent, king);
                    threat[3] = lookForMove(board, i, n, kingY + 1, kingX, 
                                            board[i][n], opponent, king);
                    threat[4] = lookForMove(board, i, n, kingY + 1, kingX - 1, 
                                            board[i][n], opponent, king);
                    threat[5] = lookForMove(board, i, n, kingY, kingX - 1, 
                                            board[i][n], opponent, king);
                    threat[6] = lookForMove(board, i, n, kingY - 1, kingX - 1, 
                                            board[i][n], opponent, king);
                    threat[7] = lookForMove(board, i, n, kingY - 1, kingX, 
                                            board[i][n], opponent, king);
                    for (int y = 0; y < BOARD_SIZE; y++)
                        if (threat[y] == true) is_Safe[y] = false;
                }
    }

    // Scan done add the king back to the board. 
    board[kingY][kingX] = king;

    // if true, make sure that the spot of allowed movement is empty. 
    if (is_Safe[0] == true && board[kingY - 1][kingX + 1] == ' ' && 
        board[kingY - 1][kingX + 1] != (queen || bishhop || knight || rook || pawn)) checkmate = false;
    if (is_Safe[1] == true && board[kingY][kingX + 1] == ' ' && 
        board[kingY][kingX + 1] != (queen || bishhop || knight || rook || pawn)) checkmate = false;
    if (is_Safe[2] == true && board[kingY + 1][kingX + 1] == ' ' && 
        board[kingY + 1][kingX + 1] != (queen || bishhop || knight || rook || pawn)) checkmate = false;
    if (is_Safe[3] == true && board[kingY + 1][kingX] == ' ' && 
        board[kingY + 1][kingX] != (queen || bishhop || knight || rook || pawn)) checkmate = false;
    if (is_Safe[4] == true && board[kingY + 1][kingX - 1] == ' ' && 
        board[kingY + 1][kingX - 1] != (queen || bishhop || knight || rook || pawn)) checkmate = false;
    if (is_Safe[5] == true && board[kingY][kingX - 1] == ' ' && 
        board[kingY][kingX - 1] != (queen || bishhop || knight || rook || pawn)) checkmate = false;
    if (is_Safe[6] == true && board[kingY - 1][kingX - 1] == ' ' && 
        board[kingY - 1][kingX - 1] != (queen || bishhop || knight || rook || pawn)) checkmate = false;
    if (is_Safe[7] == true && board[kingY - 1][kingX] == ' ' && 
        board[kingY - 1][kingX] != (queen || bishhop || knight || rook || pawn)) checkmate = false;

    return checkmate;
}

bool lookForMove(char board[BOARD_SIZE][BOARD_SIZE], 
                 int selectY, int selectX, 
                 int moveY,   int moveX, 
                 char piece,  bool playerTurn, 
                 char target) {
    
    // Control the movement path. 
    std::vector <char> movement = checkMove(selectY, selectX, moveY, moveX, board);
   
    // If true the scan resulted in a valid move. 
    bool validMove = false;

    // Depending on piece type, scan if a move is possible. 
    if (board[selectY][selectX] == 'p' || board[selectY][selectX] == 'P')
        validMove = pawnRules(selectY, selectX, moveY, moveX, movement, piece, playerTurn);
    else if (board[selectY][selectX] == 'r' || board[selectY][selectX] == 'R')
        validMove = rookRules(selectY, selectX, moveY, moveX, movement, piece, playerTurn);
    else if (board[selectY][selectX] == 'k' || board[selectY][selectX] == 'K')
        validMove = knightRules(selectY, selectX, moveY, moveX, target, piece, playerTurn);
    else if (board[selectY][selectX] == 'b' || board[selectY][selectX] == 'B')
        validMove = bishopRules(selectY, selectX, moveY, moveX, movement, piece, playerTurn);
    else if (board[selectY][selectX] == 'q' || board[selectY][selectX] == 'Q')
        validMove = queenRules(selectY, selectX, moveY, moveX, movement, piece, playerTurn);
    else if (board[selectY][selectX] == 'w' || board[selectY][selectX] == 'W')
        validMove = kingRules(selectY, selectX, moveY, moveX, movement, piece, playerTurn);

    // Result.
    if (validMove == true)
        return true;
    else
        return false;
}

void castling(bool playerTurn,       char board[BOARD_SIZE][BOARD_SIZE], 
              bool* ptr_shortCastp1, bool* ptr_longCastp1, 
              bool* ptr_shortCastp2, bool* ptr_longCastp2) {
    
    char king = UNDEF, rook = UNDEF;

    // Store piece type according to player turn.
    if (playerTurn == true) {
        king = 'W';
        rook = 'R';
    }
    else {
        king = 'w';
        rook = 'r';
    }

    // Short castling, white.
    if (board[0][4] == king && board[0][7] == rook && playerTurn == true)
        if (board[0][5] == ' ' && board[0][6] == ' ') 
            *ptr_shortCastp1 = true;

    // long castling, white.
    if (board[0][4] == king && board[0][0] == rook && playerTurn == true)
        if (board[0][3] == ' ' && board[0][2] == ' ' && board[0][1] == ' ') 
            *ptr_longCastp1 = true;

    // Short castling, black.
    if (board[7][4] == king && board[7][7] == rook && playerTurn == false)
        if (board[7][5] == ' ' && board[7][6] == ' ') *ptr_shortCastp2 = true;

    // long castling, black.
    if (board[7][4] == king && board[7][0] == rook && playerTurn == false)
        if (board[7][3] == ' ' && board[7][2] == ' ' && board[7][1] == ' ') 
            *ptr_longCastp2 = true;

    return;
}

void castlingAction(char board[BOARD_SIZE][BOARD_SIZE], bool* ptr_playerTurn, 
                    bool* ptr_shortCastp1,              bool* ptr_longCastp1, 
                    bool* ptr_shortCastp2,              bool* ptr_longCastp2, 
                    int selectX,                        int selectY, 
                    int moveX,                          int moveY, 
                                                        bool checkStatus) {
   
    // P1 white.
    bool check = false;
    bool opponent = false;
    if (*ptr_playerTurn == true) opponent = false;
    else opponent = true;
    
    // Can't evade check by castling. 
    if (checkStatus == true)
        return;

    // Castling action.
    if (*ptr_playerTurn == true) { // P1 white.
        if (board[selectY][selectX] == 'W' && board[moveY][moveX] == 'R') { // Picked piece is king and move target is rook.
            if (*ptr_shortCastp1 == true && moveY == 0 && moveX == 7) {
                for (int i = 0; i < BOARD_SIZE; i++) {
                    for (int n = 0; n < BOARD_SIZE; n++)
                        if (lookForMove(board, i, n, 0, 6, board[i][n], opponent, 'W') == true) {
                            check = true; // If rockade results in check. 
                            break;
                        }
                    if (check == true) break;
                }
                    
                if (check == false) {  // Not in chess.
                    board[0][7] = ' '; // Remove rook.
                    board[0][4] = ' '; // Remove king.
                    board[0][5] = 'R'; // Add rook.
                    board[0][6] = 'W'; // Add king.
                    
                    *ptr_shortCastp1 = false;
                    *ptr_longCastp1 = false;
                    *ptr_playerTurn = false;
                    return; // Done.
                }
            }
            if (*ptr_longCastp1 == true && moveY == 0 && moveX == 0) {
                for (int i = 0; i < BOARD_SIZE; i++) {
                    for (int n = 0; n < BOARD_SIZE; n++)
                        if (lookForMove(board, i, n, 0, 2, board[i][n], opponent, 'W') == true) {
                            check = true; // If rockade results in check. 
                            break;
                        }
                    if (check == true) break;
                }
                if (check == false) {  // Not in chess.
                    board[0][0] = ' '; // Remove rook.
                    board[0][4] = ' '; // Remove king.
                    board[0][3] = 'R'; // Add rook.
                    board[0][2] = 'W'; // Add king.
                    
                    *ptr_shortCastp1 = false;
                    *ptr_longCastp1 = false;
                    *ptr_playerTurn = false;
                    return;  // Done.
                }
            }
        }

    }
    else { // P2 black.
        if (board[selectY][selectX] == 'w' && board[moveY][moveX] == 'r') { // Picked piece is king and move target is rook.
            if (*ptr_shortCastp2 == true && moveY == 7 && moveX == 7) {
                for (int i = 0; i < BOARD_SIZE; i++) {
                    for (int n = 0; n < BOARD_SIZE; n++) 
                        if (lookForMove(board, i, n, 7, 6, board[i][n], opponent, 'w') == true) {
                            check = true; // If rockade results in check. 
                            break;
                        }
                    if (check == true) break;
                }
                if (check == false) {  // Not in chess.
                    board[7][7] = ' '; // Remove rook.
                    board[7][4] = ' '; // Remove king.
                    board[7][5] = 'r'; // Add rook.
                    board[7][6] = 'w'; // Add king.

                    *ptr_shortCastp2 = false;
                    *ptr_longCastp2 = false;
                    *ptr_playerTurn = true;
                    return; // Done.
                }
            }
            if (*ptr_longCastp2 == true && moveY == 7 && moveX == 0) {
                for (int i = 0; i < BOARD_SIZE; i++)
                    for (int n = 0; n < BOARD_SIZE; n++) {
                        if (lookForMove(board, i, n, 7, 2, board[i][n], opponent, 'w') == true) {
                            check = true; // If rockade results in check. 
                            break;
                        }
                        if (check == true) break;
                    }
                if (check == false) {  // Not in chess.
                    board[7][0] = ' '; // Remove rook.
                    board[7][4] = ' '; // Remove king.
                    board[7][3] = 'r'; // Add rook.
                    board[7][2] = 'w'; // Add king.

                    *ptr_shortCastp2 = false;
                    *ptr_longCastp2 = false;
                    *ptr_playerTurn = true;
                    return; // Done.
                }
            }
        }
    }

    return;
}

void createSquares(SDL_Rect rect_board[BOARD_SIZE * BOARD_SIZE],
                   const int width, const int height) {

    int rowSize = 0;                   // Current row. 
    int startX = 110, startY = 50;     // Start points of drawing. 
    const int resetX = 110;            // Reset x posistion of squares. 

    for (int i = 0; i < 64; i++) { // Num of squares.
        rowSize++;                 // Num of rows. 

        // Size of the squares. 
        rect_board[i].x = startX;        
        rect_board[i].y = startY;
        rect_board[i].w = width;
        rect_board[i].h = height;

        startX += width; // X starting point of the next square

        if (rowSize == BOARD_SIZE) {   // End of a row. 
            rowSize = 0;      // Reset row size. 
            startY += height; // Y starting point of the next square
            startX = resetX;  // Rest the X starting point.
        }
    }

    return; 
}

void drawBackground(SDL_Renderer* rend_Board) {
    
    // This is just the black background.
	SDL_SetRenderDrawColor(rend_Board, 0, 0, 0, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(rend_Board);

	return; 
}

void drawBoard(SDL_Renderer* rend_Board,            SDL_Rect rect_Board[64], 
               char board[BOARD_SIZE][BOARD_SIZE],  int clicked, 
               int hoovering,                       bool can_move) {
    
    // Status determines color of the squares. True = white, false = black.
	bool gridColor = false;

    // Iterates the game board. 
	int k = 0, n = 0;

    // Iterate all the squares. 
	for (int i = 0; i < 64; i++) {
		SDL_RenderDrawRect(rend_Board, &rect_Board[i]);

        // Color of the squares, true = white, false = black. 
		switch (i) {
		case 0:
			gridColor = true;
			break;
		case 8:
			gridColor = false;
			break;
		case 16:
			gridColor = true;
			break;
		case 24:
			gridColor = false;
			break;
		case 32:
			gridColor = true;
			break;
		case 40:
			gridColor = false;
			break;
		case 48:
			gridColor = true;
			break;
		case 56:
			gridColor = false;
			break;
		default:
			break;
		}

		if (gridColor == true) { // White
			SDL_SetRenderDrawColor(rend_Board, 255, 255, 255, SDL_ALPHA_OPAQUE);
			gridColor = false;
		}
		else { // Black.
			SDL_SetRenderDrawColor(rend_Board, 50, 50, 50, SDL_ALPHA_OPAQUE);
			gridColor = true;
		}
		
        // Color if a square has been clicked: color = green. 
		if(clicked == i)
			SDL_SetRenderDrawColor(rend_Board, 0, 255, 0, SDL_ALPHA_OPAQUE);

        // Color when hoovering and a move is not allowed: color = green.
        if(hoovering == i && can_move == true)
            SDL_SetRenderDrawColor(rend_Board, 0, 255, 0, SDL_ALPHA_OPAQUE);
        
        // Color when hoovering and a move is not allowed: color = red.
        if(hoovering == i && can_move == false)
            SDL_SetRenderDrawColor(rend_Board, 255, 0, 0, SDL_ALPHA_OPAQUE);

        // Paint!
		SDL_RenderFillRect(rend_Board, &rect_Board[i]);
		
        // Draw all the pieces on the squares that corresponds to their current location.

		// Draw pawn. 
		if (board[k][n] == 'P') {
			SDL_SetRenderDrawColor(rend_Board, 222, 126, 71, SDL_ALPHA_OPAQUE);
			drawPawn(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}
		else if (board[k][n] == 'p') {
			SDL_SetRenderDrawColor(rend_Board, 12, 12, 14, SDL_ALPHA_OPAQUE);
			drawPawn(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}
		
		//Draw Rook. 
		if (board[k][n] == 'R') {
			SDL_SetRenderDrawColor(rend_Board, 222, 126, 71, SDL_ALPHA_OPAQUE);
			drawRook(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}
		else if (board[k][n] == 'r') {
			SDL_SetRenderDrawColor(rend_Board, 12, 12, 14, SDL_ALPHA_OPAQUE);
			drawRook(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}

		// Draw Knight
		if (board[k][n] == 'K') {
			SDL_SetRenderDrawColor(rend_Board, 222, 126, 71, SDL_ALPHA_OPAQUE);
			drawKnight(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}
		else if (board[k][n] == 'k') {
			SDL_SetRenderDrawColor(rend_Board, 12, 12, 14, SDL_ALPHA_OPAQUE);
			drawKnight(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}

		// Draw Bishop.
		if (board[k][n] == 'B') {
			SDL_SetRenderDrawColor(rend_Board, 222, 126, 71, SDL_ALPHA_OPAQUE);
			drawBishop(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}
		else if (board[k][n] == 'b') {
			SDL_SetRenderDrawColor(rend_Board, 12, 12, 14, SDL_ALPHA_OPAQUE);
			drawBishop(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}

		// Draw Queen. 
		if (board[k][n] == 'Q') {
			SDL_SetRenderDrawColor(rend_Board, 222, 126, 71, SDL_ALPHA_OPAQUE);
			drawQueen(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}
		else if (board[k][n] == 'q') {
			SDL_SetRenderDrawColor(rend_Board, 12, 12, 14, SDL_ALPHA_OPAQUE);
			drawQueen(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}

		// Draw King.
		if (board[k][n] == 'W') {
			SDL_SetRenderDrawColor(rend_Board, 222, 126, 71, SDL_ALPHA_OPAQUE);
			drawKing(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}
		else if (board[k][n] == 'w') {
			SDL_SetRenderDrawColor(rend_Board, 12, 12, 14, SDL_ALPHA_OPAQUE);
			drawKing(rend_Board, rect_Board[i].x + 27, rect_Board[i].y + 17);
		}
			
		// Iterate the numbers used to control the status of the chess board.
		// k = x and n = y. 
		n++;
		if (n == BOARD_SIZE) {
			n = 0;
			k++; 
		}
	
	}

	return;
}

void drawRook(SDL_Renderer* rend_Board, int startX, int startY) {
	
    // Width and lenght of the piece. 
    int length = 20;
	int width = 6;

	// Body. 
	for (int i = 0; i < length; i++) {
		SDL_RenderDrawLine(rend_Board, startX - 4, startY + i, startX + width, startY + length);
		SDL_RenderDrawLine(rend_Board, startX - 4, startY + i, startX - width, startY + length);
		SDL_RenderDrawLine(rend_Board, startX - width, startY + length, startX + width, startY + length);
	}
	for (int i = 0; i < length; i++) {
		SDL_RenderDrawLine(rend_Board, startX, startY + i, startX + width, startY + length);
		SDL_RenderDrawLine(rend_Board, startX, startY + i, startX - width, startY + length);
		SDL_RenderDrawLine(rend_Board, startX - width, startY + length, startX + width, startY + length);
	}
	for (int i = 0; i < length; i++) {
		SDL_RenderDrawLine(rend_Board, startX + 4, startY + i, startX + width, startY + length);
		SDL_RenderDrawLine(rend_Board, startX + 4, startY + i, startX - width, startY + length);
		SDL_RenderDrawLine(rend_Board, startX - width, startY + length, startX + width, startY + length);
	}

	return;
}

void drawKnight(SDL_Renderer* rend_Board, int startX, int startY) {
    
    // Width and lenght of the piece. 
	int length = 20;
	int width = 5;
	int headLength = 6;
	int headStart = startY + 1;

	// Head.
	for (int i = 0; i < headLength; i++) {
		SDL_RenderDrawLine(rend_Board, startX + 1, headStart + i, startX + width, (headStart + headLength));
		SDL_RenderDrawLine(rend_Board, startX + 1, headStart + i, startX - (width + 4), (headStart + headLength));

		SDL_RenderDrawLine(rend_Board, startX - width, headStart + headLength, startX + width, headStart + headLength);
	}

	// Body. 
	for (int i = 0; i < length; i++) {
		SDL_RenderDrawLine(rend_Board, startX, startY + i, startX + width, startY + length);
		SDL_RenderDrawLine(rend_Board, startX, startY + i, startX - width, startY + length);
		SDL_RenderDrawLine(rend_Board, startX - width, startY + length, startX + width, startY + length);
	}
	return;
}

void drawBishop(SDL_Renderer* rend_Board, int startX, int startY) {
    
    // Width and lenght of the piece. 
	int length = 20;
	int width = 5;
	int headLength = 7;
	int headStart = startY - 5;

	// Head.
	for (int i = 0; i < length; i++) {
		SDL_RenderDrawLine(rend_Board, startX, headStart + i, startX + width, headStart + headLength);
		SDL_RenderDrawLine(rend_Board, startX, headStart + i, startX - width, headStart + headLength);
		SDL_RenderDrawLine(rend_Board, startX - width, headStart + headLength, startX + width, headStart + headLength);
	}

	// Body. 
	for (int i = 0; i < length; i++) {
		SDL_RenderDrawLine(rend_Board, startX, startY + i, (startX + width), (startY + length));
		SDL_RenderDrawLine(rend_Board, startX, startY + i, (startX - width), (startY + length));
		SDL_RenderDrawLine(rend_Board, (startX - width), (startY + length), (startX + width), (startY + length));
	}
	return;
}

void drawQueen(SDL_Renderer* rend_Board, int startX, int startY) {
    
    // Width and lenght of the piece. 
	int crownLength = 6;
	int crownWidth = 2;
	int bodyLength = 20;
	int bodyWidth = 5;

	// Crown. 
	for (int i = 0; i < crownLength; i++) {
		SDL_RenderDrawLine(rend_Board, startX - 4, startY + i, (startX + crownWidth), (startY + crownLength));
		SDL_RenderDrawLine(rend_Board, startX - 4, startY + i, (startX - crownWidth), (startY + crownLength));
		SDL_RenderDrawLine(rend_Board, (startX - crownWidth), (startY + crownLength), (startX + crownWidth), (startY + crownLength));
	}
	for (int i = 0; i < crownLength; i++) {
		SDL_RenderDrawLine(rend_Board, startX, startY + i, (startX + crownWidth), (startY + crownLength));
		SDL_RenderDrawLine(rend_Board, startX, startY + i, (startX - crownWidth), (startY + crownLength));
		SDL_RenderDrawLine(rend_Board, (startX - crownWidth), (startY + crownLength), (startX + crownWidth), (startY + crownLength));
	}
	for (int i = 0; i < crownLength; i++) {
		SDL_RenderDrawLine(rend_Board, startX + 4, startY + i, (startX  + crownWidth), (startY + crownLength));
		SDL_RenderDrawLine(rend_Board, startX + 4, startY + i, (startX - crownWidth), (startY + crownLength));
		SDL_RenderDrawLine(rend_Board, (startX - crownWidth), (startY + crownLength), (startX + crownWidth), (startY + crownLength));
	}

	// Body
	for (int i = 0; i < bodyLength; i++) {
		SDL_RenderDrawLine(rend_Board, startX, startY + i, (startX + bodyWidth), (startY + bodyLength));
		SDL_RenderDrawLine(rend_Board, startX, startY + i, (startX - bodyWidth), (startY + bodyLength));
		SDL_RenderDrawLine(rend_Board, (startX - bodyWidth), (startY + bodyLength), (startX + bodyWidth), (startY + bodyLength));
	}

	return;
}

void drawKing(SDL_Renderer* rend_Board, int startX, int startY) {
    
    // Width and lenght of the piece. 
	int length = 20;
	int width = BOARD_SIZE;
	int headStart = startY + 5;

	// Head.
	SDL_RenderDrawLine(rend_Board, (startX - 5), headStart, (startX + 5), headStart);
	for (int i = 6; i > 0; i--) {
		if (i <= 3)
			SDL_RenderDrawLine(rend_Board, (startX - 7), (headStart - i), (startX + 7), (headStart - i));
		else
			SDL_RenderDrawLine(rend_Board, (startX - (7 - i)), (headStart - i), (startX + (7 - i)), (headStart - i));

		if (i == 1) {
			SDL_RenderDrawLine(rend_Board, startX, headStart, startX, (headStart - 11));
			SDL_RenderDrawLine(rend_Board, startX - 3, (headStart - 9), startX + 3, (headStart - 9));
		}
	}

	// Body. 
	for (int i = 0; i < length; i++) {
		SDL_RenderDrawLine(rend_Board, startX, startY + i, (startX + width), (startY + length));
		SDL_RenderDrawLine(rend_Board, startX, startY + i, (startX - width), (startY + length));
		SDL_RenderDrawLine(rend_Board, (startX - width), (startY + length), (startX + width), (startY + length));
	}
	return;
}

void drawPawn(SDL_Renderer* rend_Board, int startX, int startY) {
    
    // Width and lenght of the piece. 
	int length = 10;
	int width = 5;
	int headStart = startY + 2;

	// Head.
	SDL_RenderDrawLine(rend_Board, (startX - 1), headStart, (startX + 1), headStart);
	SDL_RenderDrawLine(rend_Board, (startX - 2), (headStart - 1), (startX + 2), (headStart - 1));
	SDL_RenderDrawLine(rend_Board, (startX - 3), (headStart - 2), (startX + 3), (headStart - 2));
	SDL_RenderDrawLine(rend_Board, (startX - 4), (headStart - 3), (startX + 4), (headStart - 3));
	SDL_RenderDrawLine(rend_Board, (startX - 3), (headStart - 4), (startX + 3), (headStart - 4));
	SDL_RenderDrawLine(rend_Board, (startX - 2), (headStart - 5), (startX + 2), (headStart - 5));
	SDL_RenderDrawLine(rend_Board, (startX - 1), (headStart - 6), (startX + 1), (headStart - 6));


	// Body. 
	for (int i = 0; i < length; i++) {
		SDL_RenderDrawLine(rend_Board, startX, startY + i, (startX + width), (startY + length));
		SDL_RenderDrawLine(rend_Board, startX, startY + i, (startX - width), (startY + length));
		SDL_RenderDrawLine(rend_Board, (startX - width), (startY + length), (startX + width), (startY + length));
	}
	return;
}
