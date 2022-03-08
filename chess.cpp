/*
Created by Oscar Bergstr�m.
Last update 2022-03-02.
*/

#include <iostream>
#include <stdlib.h>
#include <vector>

int main();
bool getMove(int*, int*, int*, int*, bool, char[8][8], bool*);
void printBoard(char[8][8], bool);
std::vector <char> checkMove(int, int, int, int, char[8][8]);
bool moveAction(int, int, int, int, char[8][8], bool, char, char, std::vector <char>);
bool pawnRules(int, int, int, int, std::vector <char>, char, bool);
bool rookRules(int, int, int, int, std::vector <char>, char, bool);
bool knightRules(int, int, int, int, char, char, bool);
bool bishopRules(int, int, int, int, std::vector <char>, char, bool);
bool queenRules(int, int, int, int, std::vector <char>, char, bool);
bool kingRules(int, int, int, int, std::vector <char>, char, bool);
bool checkChess(char board[8][8], bool);
bool lookForMove(char[8][8], int, int, int, int, char, bool, char);
void castling(bool, char[8][8], bool*, bool*);  

int main() {
    bool is_Running = true, playerTurn = true, validInput = false, chess = false, chessMate = false, opponent, is_castling = false;
    int selectY, selectX, moveY, moveX, kingX, kingY;
    std::vector <char> movement;
    char piece, target, king;
    char board[8][8] = {'R', 'K', 'B', 'Q','W', 'B', 'K', 'R',
                        'P', 'P', 'P', 'P','P', 'P', 'P', 'P',
                        ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',
                        ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',
                        ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',
                        ' ', ' ', ' ', ' ',' ', ' ', ' ', ' ',
                        'p', 'p', 'p', 'p','p', 'p', 'p', 'p',
                        'r', 'k', 'b', 'q','w', 'b', 'k', 'r' };

    while (is_Running) {
        printBoard(board, playerTurn);

        // At the start of a turn find the kings position. 
        if (playerTurn == true) king = 'W';
        else king = 'w';
        for (int i = 0; i < 8; i++)
            for (int n = 0; n < 8; n++)
                if (board[i][n] == king){ 
                    kingX = n;
                    kingY = i;
                }

        // Is the king under attack? 
        if (playerTurn == true) opponent = false;
        else opponent = true;
        for (int i = 0; i < 8; i++) {
            for (int n = 0; n < 8; n++) {
                if (board[i][n] != ' ') chess = lookForMove(board, i, n, kingY, kingX, board[i][n], opponent, king);
                if (chess == true) break;
            }
            if (chess == true) break;
        }
                
        if (chess == true) {
            chessMate = checkChess(board, playerTurn);
            if (chessMate == true) {
                std::cout << "\n Game over "; 
                if (playerTurn == true) std::cout << "white player king is chess mate";
                else std::cout << "black player king is chess mate"; 
                system("pause"); 
                break;
            }
        }

        // Move input and store its path. 
        while(validInput == false) validInput = getMove(&selectY, &selectX, &moveY, &moveX, playerTurn, board, &is_castling);
        if(is_castling == false)
            movement = checkMove(selectY, selectX, moveY, moveX, board);

        if(is_castling == false) {
            piece = board[selectY][selectX];
            target = board[moveY][moveX];
        }

        if(is_castling == false)
            playerTurn = moveAction(selectY, selectX, moveY, moveX, board, playerTurn, target, piece, movement);
        else {
            if (playerTurn == true) playerTurn = false;
            else playerTurn == true;       
        }

        // End of turn. 
        validInput = false;
        is_castling = false;
        movement.clear();
        system("cls");
    }

    return 0;
}

bool getMove(int* ptr_selectY, int* ptr_selectX, int* ptr_moveY, int* ptr_moveX, bool playerTurn, char board[8][8], bool* is_castling) {
    char letFrom, numFrom, letTo, numTo, makeCastling = ' ';
    bool shortCast = false, longCast = false; 

    castling(playerTurn, board, &shortCast, &longCast); 

    std::cin.clear();

    if(shortCast == true) std::cout << "\tWould you like to make a short Castling, write: s (skip by other input)" << std::endl; 
    if(longCast == true) std::cout << "\tWould you like to make a long Castling, write: l (skip by other input)" << std::endl; 
    if(shortCast == true ||longCast == true) {
        std::cout << "\t: ";
        std::cin >> makeCastling;
    }
    if(makeCastling == 's' && shortCast == true) {
        *is_castling = true;

        if(playerTurn == true) { 
            board[0][6] = 'W';
            board[0][5] = 'R'; 
            board[0][4] = ' ';
            board[0][7] = ' ';
        } 
        else {
            board[7][6] = 'w';
            board[7][5] = 'r'; 
            board[7][4] = ' ';
            board[7][7] = ' ';
        }
        return true;
    }
    else if(makeCastling == 'l' && longCast == true) {
        *is_castling = true;

        if(playerTurn == true) { 
            board[0][2] = 'W';
            board[0][3] = 'R'; 
            board[0][4] = ' ';
            board[0][0] = ' ';
        } 
        else {
            board[7][2] = 'w';
            board[7][3] = 'r'; 
            board[7][4] = ' ';
            board[7][0] = ' ';
        }
        return true;
    }

    // Move input values.
    std::cout << "\tSelect a piece: ";
    std::cin >> letFrom >> numFrom;
    std::cout << "\tMove the piece: ";
    std::cin >> letTo >> numTo;
 
    // Convert letter to integer value. 
    if (letFrom == 'A' || letFrom == 'a') *ptr_selectX = 0;
    if (letFrom == 'B' || letFrom == 'b') *ptr_selectX = 1;
    if (letFrom == 'C' || letFrom == 'c') *ptr_selectX = 2;
    if (letFrom == 'D' || letFrom == 'd') *ptr_selectX = 3;
    if (letFrom == 'E' || letFrom == 'e') *ptr_selectX = 4;
    if (letFrom == 'F' || letFrom == 'f') *ptr_selectX = 5;
    if (letFrom == 'G' || letFrom == 'g') *ptr_selectX = 6;
    if (letFrom == 'H' || letFrom == 'h') *ptr_selectX = 7;

    // Convert char number to integer value. 
    if (numFrom == '8') *ptr_selectY = 0;
    if (numFrom == '7') *ptr_selectY = 1;
    if (numFrom == '6') *ptr_selectY = 2;
    if (numFrom == '5') *ptr_selectY = 3;
    if (numFrom == '4') *ptr_selectY = 4;
    if (numFrom == '3') *ptr_selectY = 5;
    if (numFrom == '2') *ptr_selectY = 6;
    if (numFrom == '1') *ptr_selectY = 7;

    // Convert letter to integer value. 
    if (letTo == 'A' || letTo == 'a') *ptr_moveX = 0;
    if (letTo == 'B' || letTo == 'b') *ptr_moveX = 1;
    if (letTo == 'C' || letTo == 'c') *ptr_moveX = 2;
    if (letTo == 'D' || letTo == 'd') *ptr_moveX = 3;
    if (letTo == 'E' || letTo == 'e') *ptr_moveX = 4;
    if (letTo == 'F' || letTo == 'f') *ptr_moveX = 5;
    if (letTo == 'G' || letTo == 'g') *ptr_moveX = 6;
    if (letTo == 'H' || letTo == 'h') *ptr_moveX = 7;

    // Convert char number to integer value. 
    if (numTo == '8') *ptr_moveY = 0;
    if (numTo == '7') *ptr_moveY = 1;
    if (numTo == '6') *ptr_moveY = 2;
    if (numTo == '5') *ptr_moveY = 3;
    if (numTo == '4') *ptr_moveY = 4;
    if (numTo == '3') *ptr_moveY = 5;
    if (numTo == '2') *ptr_moveY = 6;
    if (numTo == '1') *ptr_moveY = 7;

    if ((*ptr_selectX <= 7 && *ptr_selectX >= 0) && (*ptr_selectY <= 7 && *ptr_selectY >= 0) &&
        (*ptr_moveX <= 7 && *ptr_moveX >= 0) && (*ptr_moveY <= 7 && *ptr_moveY >= 0)) return true;

    return false;
}

void printBoard(char board[8][8], bool playerTurn) {
    int num = 8;

    std::cout << "\n";
    for (int i = 0; i < 8; i++) {
        std::cout << "\t";
        std::cout << num << ": ";
        for (int n = 0; n < 8; n++) {
            if (board[i][n] == ' ') // A empty slot.
                std::cout << "[ ]";
            else // Game-slot contains a chess piece.
                std::cout << "[" << board[i][n] << "]";

            if (n == (8 - 1)) // Write a new line in console if k is iterated to the size of the board - 1.
                std::cout << "\n";
        }
        num -= 1;
    }

    std::cout << "\t";
    std::cout << "    A  B  C  D  E  F  G  H" << std::endl;

    if (playerTurn == true)
        std::cout << "\tWhite player turn (start out at the top of the board)!" << std::endl;
    else if (playerTurn == false)
        std::cout << "\tBlack player turn (start out at the bottom of the board)!" << std::endl;

    return;
}

std::vector <char> checkMove(int selectY, int  selectX, int  moveY, int  moveX, char board[8][8]) {
    int trackX = selectX;
    std::vector <char> movement;

    // Diagonal movement: right up, left up, right down and left down.
    if (moveY > selectY && selectX != moveX) {
        for (int i = selectY; i <= moveY; i++) {
            if (selectX < moveX) {
                movement.push_back(board[i][trackX]);
                trackX++;
            }
            else if (selectX > moveX) {
                movement.push_back(board[i][trackX]);
                trackX--;
            }
        }
    }

    if (moveY < selectY && selectX != moveX) {
        for (int i = selectY; i >= moveY; i--) {
            if (selectX < moveX) {
                movement.push_back(board[i][trackX]);
                trackX++;
            }
            else if (selectX > moveX) {
                movement.push_back(board[i][trackX]);
                trackX--;
            }
        }
    }

    // Vertical movement: right, left, up and down.
    if (moveY != selectY && selectX == moveX) {
        if (selectY < moveY) {
            for (int i = selectY; i <= moveY; i++)
                movement.push_back(board[i][selectX]);
        }
        else if (selectY > moveY) {
            for (int i = selectY; i >= moveY; i--)
                movement.push_back(board[i][selectX]);
        }
    }

    if (moveY == selectY && selectX != moveX) {
        if (selectX < moveX) {
            for (int i = selectX; i <= moveX; i++)
                movement.push_back(board[selectY][i]);
        }
        else if (selectX > moveX) {
            for (int i = selectX; i >= moveX; i--)
                movement.push_back(board[selectY][i]);
        }
    }

    return movement;
}

bool moveAction(int selectY, int  selectX, int  moveY, int  moveX, char board[8][8], bool playerTurn, char target, char piece, std::vector <char> movement) {
    bool validMove = false, chess = false, opponent;
    int kingX, kingY;
    char king, selected = ' ', moved = ' ';

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

    // Make move. 
    if (validMove == true) {
        moved = board[moveY][moveX];
        selected = board[selectY][selectX];
        board[moveY][moveX] = board[selectY][selectX];
        board[selectY][selectX] = ' ';
    }

    // Find the kings position. 
    if (playerTurn == true) king = 'W';
    else king = 'w';
    for (int i = 0; i < 8; i++)
        for (int n = 0; n < 8; n++)
            if (board[i][n] == king) {
                kingX = n;
                kingY = i;
            }

    // Check if the king is under threat. 
    if (playerTurn == true) opponent = false;
    else opponent = true;
    for (int i = 0; i < 8; i++) {
        for (int n = 0; n < 8; n++)
            if (board[i][n] != ' ') {
                chess = lookForMove(board, i, n, kingY, kingX, board[i][n], opponent, king);
                if (chess == true) break;
            }
        if (chess == true) break;
    }
        
    // Check if move is valid a move is not allowed to put king in chess. 
    if (chess == false) {
        if (playerTurn == true) playerTurn = false;
        else playerTurn = true;
    }
    else {
        board[selectY][selectX] = selected;
        board[moveY][moveX] = moved;
    }
    
    return playerTurn;
}

bool pawnRules(int selectY, int selectX, int moveY, int moveX, std::vector <char> movement, char piece, bool playerTurn) {
    // PAWN start move 
    if (1 == selectY && moveY == (selectY + 2) && moveX == selectX && moveY > selectY && piece == 'P' && playerTurn == true) {
        for (unsigned int i = 1; i < movement.size(); i++)
            if (movement[i] != ' ') return false;

        return true;
    }
    else if (6 == selectY && moveY == (selectY - 2) && moveX == selectX && moveY < selectY && piece == 'p' && playerTurn == false) {
        for (unsigned int i = 1; i < movement.size(); i++)
            if (movement[i] != ' ')
                return false;

        return true;
    }

    // PAWN regular move 
    if (moveY == (selectY + 1) && moveX == selectX && piece == 'P' && playerTurn == true) {
        for (unsigned int i = 1; i < movement.size(); i++)
            if (movement[i] != ' ')
                return false;

        return true;
    }
    else if (moveY == (selectY - 1) && moveX == selectX && piece == 'p' && playerTurn == false) {
        for (unsigned int i = 1; i < movement.size(); i++)
            if (movement[i] != ' ')
                return false;

        return true;
    }

    /* PAWN attack move */
    if (moveY == (selectY + 1) && (moveX == (selectX + 1) || moveX == (selectX - 1)) && piece == 'P' && playerTurn == true) {
        for (unsigned int i = 1; i < movement.size(); i++)
            if (movement[i] == ' ')
                return false;

        return true;
    }
    else if (moveY == (selectY - 1) && (moveX == (selectX + 1) || moveX == (selectX - 1)) && piece == 'p' && playerTurn == false) {
        for (unsigned int i = 1; i < movement.size(); i++) {
            if (movement[i] == ' ')
                return false;

            return true;
        }
    }

    return false;
}

bool rookRules(int selectY, int selectX, int moveY, int moveX, std::vector <char> movement, char piece, bool playerTurn) {
    // White rook.
    if (piece == 'R' && playerTurn == true) {
        if (selectX != moveX && selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && movement[i] != 'R' && movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }
        if (selectX == moveX && selectY != moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && movement[i] != 'R' && movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }
    }

    // Black rook.
    if (piece == 'r' && playerTurn == false) {
        if (selectX != moveX && selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && movement[i] != 'r' && movement[i] != 'p'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }
        if (selectX == moveX && selectY != moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && movement[i] != 'r' && movement[i] != 'p'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }
    }

    return false;
}

bool knightRules(int selectY, int selectX, int moveY, int moveX, char target, char piece, bool playerTurn) {
    // White knight.
    if (piece == 'K' && playerTurn == true)
        if ((moveY == (selectY + 2) && moveX == (selectX + 1)) ||
            (moveY == (selectY + 2) && moveX == (selectX - 1)) ||
            (moveY == (selectY - 2) && moveX == (selectX + 1)) ||
            (moveY == (selectY - 2) && moveX == (selectX - 1)) ||
            (moveY == (selectY + 1) && moveX == (selectX + 2)) ||
            (moveY == (selectY - 1) && moveX == (selectX + 2)) ||
            (moveY == (selectY + 1) && moveX == (selectX - 2)) ||
            (moveY == (selectY - 1) && moveX == (selectX - 2)))
            if (target == ' ' || target == 'p' || target == 'r' || target == 'b' || target == 'w' || target == 'q')
                return true;


    // Black knight.
    if (piece == 'k' && playerTurn == false)
        if ((moveY == (selectY + 2) && moveX == (selectX + 1)) ||
            (moveY == (selectY + 2) && moveX == (selectX - 1)) ||
            (moveY == (selectY - 2) && moveX == (selectX + 1)) ||
            (moveY == (selectY - 2) && moveX == (selectX - 1)) ||
            (moveY == (selectY + 1) && moveX == (selectX + 2)) ||
            (moveY == (selectY - 1) && moveX == (selectX + 2)) ||
            (moveY == (selectY + 1) && moveX == (selectX - 2)) ||
            (moveY == (selectY - 1) && moveX == (selectX - 2)))
            if (target == ' ' || target == 'P' || target == 'R' || target == 'B' || target == 'W' || target == 'Q')
                return true;

    return false;
}

bool bishopRules(int selectY, int selectX, int moveY, int moveX, std::vector <char> movement, char piece, bool playerTurn) {
    int stepsY, stepsX;

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
                if (i == (movement.size() - 1) && (movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && movement[i] != 'R' && movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
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
                if (i == (movement.size() - 1) && (movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && movement[i] != 'r' && movement[i] != 'p'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }
    }

    return false;
}

bool queenRules(int selectY, int selectX, int moveY, int moveX, std::vector <char> movement, char piece, bool playerTurn) {
    int stepsY, stepsX;

    // White queen.
    if (piece == 'Q' && playerTurn == true) {
        if (selectY != moveY && selectX == moveX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && movement[i] != 'R' && movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }

        if (selectX != moveX && selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && movement[i] != 'R' && movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
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
                if (i == (movement.size() - 1) && (movement[i] != 'W' || movement[i] != 'Q' || movement[i] != 'K' || movement[i] != 'R' || movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }
    }

    // Black queen.
    if (piece == 'q' && playerTurn == false) {
        if (selectY != moveY && selectX == moveX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'w' || movement[i] != 'q' || movement[i] != 'k' || movement[i] != 'r' || movement[i] != 'p'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }

        if (selectX != moveX && selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'w' || movement[i] != 'q' || movement[i] != 'k' || movement[i] != 'r' || movement[i] != 'p'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
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
                if (i == (movement.size() - 1) && (movement[i] != 'W' || movement[i] != 'Q' || movement[i] != 'K' || movement[i] != 'R' || movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }
    }

    return false;
}

bool kingRules(int selectY, int selectX, int moveY, int moveX, std::vector <char> movement, char piece, bool playerTurn) {
    int stepsY, stepsX;

    // White king.
    if (piece == 'W' && playerTurn == true) {
        if (selectY != moveY && selectX == moveX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && movement[i] != 'R' && movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }

        if (selectX != moveX && selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && movement[i] != 'R' && movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
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
                if (i == (movement.size() - 1) && (movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && movement[i] != 'R' && movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }
    }

    /*
        Black king.
    */
    if (piece == 'w' && playerTurn == true) {
        if (selectY != moveY && selectX == moveX) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && movement[i] != 'r' && movement[i] != 'p'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }

        if (selectX != moveX && selectY == moveY) {
            for (unsigned int i = 1; i <= movement.size(); i++) {
                if (i == (movement.size() - 1) && (movement[i] != 'w' && movement[i] != 'q' && movement[i] != 'k' && movement[i] != 'r' && movement[i] != 'p'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
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
                if (i == (movement.size() - 1) && (movement[i] != 'W' && movement[i] != 'Q' && movement[i] != 'K' && movement[i] != 'R' && movement[i] != 'P'))
                    return true;
                else if (movement[i] != ' ')
                    return false;
            }
        }
    }

    return false;
}

bool checkChess(char board[8][8], bool playerTurn) {
    int kingX, kingY, threatX, threatY, removeX, removeY; 
    char king;
    bool chessMate, opponent, removeThreat = false;
    bool threat[8] = { true, true, true, true, true, true, true, true };
    bool is_Safe[8] = { true, true, true, true, true, true, true, true };
    if (playerTurn == true) king = 'W';
    else king = 'w'; 

    // Find the kings position. 
    for(int i = 0; i < 8; i++)
        for (int n = 0; n < 8; n++) 
            if (board[i][n] == king) {
                kingX = n;
                kingY = i;
            }

    // Check if a piece threaten the king. 
    if (playerTurn == true) opponent = false;
    else opponent = true; 
    for (int i = 0; i < 8; i++) {
        for (int n = 0; n < 8; n++)
            if (board[i][n] != ' ') {
                chessMate = lookForMove(board, i, n, kingY, kingX, board[i][n], opponent, king);
                if (chessMate == true) {
                    threatX = n;
                    threatY = i;
                    break;
                }
            }
        if (chessMate == true) break;
    }

    // Check if any piece is able to remove the threat. 
    for (int i = 0; i < 8; i++) {
        for (int n = 0; n < 8; n++)
            if (board[i][n] != ' ' && board[i][n] != king) {
                removeThreat = lookForMove(board, i, n, threatY, threatX, board[i][n], playerTurn, board[threatY][threatX]);
                if (removeThreat == true) {
                    removeX = n;
                    removeY = i;
                    break;
                }
            }
        if (removeThreat == true) break;
    }
    if (removeThreat == true) chessMate = false;
    else chessMate = true;

    // Check if the threatend king is allowed to move anywhere. 
    if (chessMate == true) {
        for (int i = 0; i < 8; i++) 
            for (int n = 0; n < 8; n++)
                if (board[i][n] != ' ') {
                    threat[0] = lookForMove(board, i, n, kingY - 1, kingX + 1, board[i][n], playerTurn, king);
                    threat[1] = lookForMove(board, i, n, kingY, kingX + 1, board[i][n], playerTurn, king);
                    threat[2] = lookForMove(board, i, n, kingY + 1, kingX + 1, board[i][n], playerTurn, king);
                    threat[3] = lookForMove(board, i, n, kingY + 1, kingX, board[i][n], playerTurn, king);
                    threat[4] = lookForMove(board, i, n, kingY + 1, kingX - 1, board[i][n], playerTurn, king);
                    threat[5] = lookForMove(board, i, n, kingY, kingX - 1, board[i][n], playerTurn, king);
                    threat[6] = lookForMove(board, i, n, kingY - 1, kingX - 1, board[i][n], playerTurn, king);
                    threat[7] = lookForMove(board, i, n, kingY - 1, kingX, board[i][n], playerTurn, king);
                    for (int y = 0; y < 8; y++)
                        if (threat[y] == true) is_Safe[y] = false;
                        
                }
    }

    // if true, make sure that the spot of allowed movement is empty. 
    if (is_Safe[0] == true && board[kingY - 1][kingX + 1] == ' ') chessMate = false;
    if (is_Safe[1] == true && board[kingY][kingX + 1] == ' ') chessMate = false;
    if (is_Safe[2] == true && board[kingY + 1][kingX + 1] == ' ') chessMate = false;
    if (is_Safe[3] == true && board[kingY + 1][kingX] == ' ') chessMate = false;
    if (is_Safe[4] == true && board[kingY + 1][kingX - 1] == ' ') chessMate = false;
    if (is_Safe[5] == true && board[kingY][kingX - 1] == ' ') chessMate = false;
    if (is_Safe[6] == true && board[kingY - 1][kingX - 1] == ' ') chessMate = false;
    if (is_Safe[7] == true && board[kingY - 1][kingX] == ' ') chessMate = false; 

    return chessMate; 
}

bool lookForMove(char board[8][8], int selectY, int selectX, int moveY, int moveX, char piece, bool playerTurn, char target) {
    std::vector <char> movement = checkMove(selectY, selectX, moveY, moveX, board);
    bool validMove = false; 

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
    
    if (validMove == true)
        return true;
    else
        return false;
}

void castling(bool playerTurn, char board[8][8], bool* shortCast, bool* longCast) {
    char king, rook;

    // Set piece type according to player turn.
    if(playerTurn == true) {
        king = 'W';
        rook = 'R';
    } 
    else {
       king = 'w';
       rook = 'r';
    } 

    // Short castling white.
    if(board[0][4] == king && board[0][7] == rook && playerTurn == true) 
        if (board[0][5] == ' ' && board[0][6] == ' ') *shortCast = true;
            
    // long castling white.
    if(board[0][4] == king && board[0][0] == rook && playerTurn == true) 
        if (board[0][3] == ' ' && board[0][2] == ' ' && board[0][1] == ' ') *longCast = true;

    // Short castling black.
    if (board[7][4] == king && board[7][7] == rook && playerTurn == true)
        if (board[7][5] == ' ' && board[7][6] == ' ') *shortCast = true;

    // long castling black.
    if(board[7][4] == king && board[7][0] == rook && playerTurn == true)
        if (board[7][3] == ' ' && board[7][2] == ' ' && board[7][1] == ' ') *longCast = true;

    return;
}  