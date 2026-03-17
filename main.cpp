/**
 *Currently the main class of battleships.
 *This is the basic version of the battleships game.
 *This version is the simplest version of the game
 *@version 1.0
 *@author Alex Samuel
*/
#include <iostream>
#include <limits>
using namespace std;
constexpr int GRID_SIZE = 10;

/**
 *Checks whether the user has entered the correct row
 * @param row the row number
 * @return whether the row is valid (true or false).
 */
bool isValidRow(int row) {
    return row >= 1 && row <= GRID_SIZE;
}

/**
 * Checks whether the user has entered the correct column
 * @param col the column letter
 * @return whether the column is valid (true or false).
 */
bool isValidCol(char col) {
    col = toupper(col);
    return col >= 'A' && col <= 'J';
}

/**
 *
 * @param col the column number
 * @param row the row letter
 * @return whether the coordinate is valid or not.
 */
bool isValidCoordinate(char col, int row) {
    return isValidRow(row) && isValidCol(col);
}

/**
 * The main program which runs the battleship program
 * Creates a list of user prompts.
 * Player selection is here.
 * @return 0
 */
int main() {

    cout << "Welcome to Battleships !";
    cout << "(Game created by Alex Samuel) \n";
    cout << "Press Enter to Continue:";
    cin.get();
    cout.flush();
    bool inputError = false;
    do {
        cout << "Player select required \n";
        cout << "Would you like to play against the computer? \n";
        cout << "1. Yes \n2. No \n";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        if (choice == 1) {
            inputError = false;
        } else if (choice == 2) {
            inputError = false;
            cout << "Player 1 assign ships (10 * 10 grid) \n";
            char gameGrid [GRID_SIZE][GRID_SIZE];
            for (auto & row : gameGrid) {
                for (char & col : row) {
                    col = '~';
                }
            }
            for (auto & row : gameGrid) {
                for (const char col : row) {
                    cout << col << " ";
                }
                cout << endl;
            }
            bool coordinateError = true;
            while (coordinateError) {
                cout << "Enter the start coordinates of the carrier(5): ";
                char col;
                int row;
                cin >> col >> row;
                if (isValidCoordinate(col, row)) {
                    gameGrid[row][col] = 'C';
                    coordinateError = false;

                }
                else {
                    cout << "Invalid coordinates, please try again\n";
                    cout << "Press Enter to continue...";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin.get();
                    coordinateError = true;
                }

            }
        } else {
            inputError = true;
            cout << "Invalid input, please enter 1 (Yes) or 2 (No)\n";
            cout << "Press Enter to continue...";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }
    } while (inputError);
    return 0;
}