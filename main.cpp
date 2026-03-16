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
            cout << "Assign Your carrier(5) \n";
            int rowError = true;
            while (rowError) {
                int row;
                cout << "Enter the row number: ";
                cin >> row;
                if (row == 1 |row == 2 | row == 3 | row == 4 | row == 5
                    | row == 6 | row == 7 | row == 8 | row == 9 | row == 10) {
                    rowError = false;
                } else {
                    rowError = true;
                    cout << "Invalid input, please enter a number between 1 and 10\n";
                    cout << "Press Enter to continue...";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cin.get();
                }
            }
            cout << "Enter the column letter: ";
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