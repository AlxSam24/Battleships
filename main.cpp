/**
 *Currently the main class of battleships.
 *This is the basic version of the battleships game.
 *This version is the simplest version of the game
 *@version 1.0
 *@author Alex Samuel
*/
#include <iostream>
using namespace std;

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
    bool inputError = false;
    do {
        cout << "Player select required";
        cout << "Would you like to play against the computer?";
        cout << "1. Yes \n2. No";
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;
        cin.ignore();
        if (choice == 1 || choice == 2) {
            inputError = false;
        } else {
            inputError = true;
        }

    } while (inputError != true);
    return 0;
}