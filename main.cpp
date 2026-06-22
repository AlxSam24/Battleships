/**
 * Currently the main class of battleships.
 * This is the basic version of the battleships game.
 * This version is the simplest version of the game.
 * @version 1.0
 * @author Alex Samuel
 */
#include <iostream>
#include <limits>
#include <string>
#include <random>
#include <chrono>
#include <thread>

using namespace std;

constexpr int GRID_SIZE = 10;

/**
 * Checks whether the user has entered a valid row.
 * @param row the row number
 * @return true if valid, false otherwise
 */

bool isValidRow(int row) {
    return row >= 1 && row <= GRID_SIZE;
}

/**
 * Checks whether the user has entered a valid column.
 * @param col the column letter
 * @return true if valid, false otherwise
 */

bool isValidCol(char col) {
    col = static_cast<char>(toupper(col));
    return col >= 'A' && col <= 'J';
}

/**
 * Checks whether a coordinate is valid.
 * @param col the column letter
 * @param row the row number
 * @return true if valid, false otherwise
 */

bool isValidCoordinate(char col, int row) {
    return isValidRow(row) && isValidCol(col);
}

/**
 * Checks whether a ship fits on the grid from a start coordinate
 * in the given orientation.
 * @param col        start column (A-J)
 * @param row        start row (1-10)
 * @param size       length of the ship
 * @param horizontal true = horizontal, false = vertical
 * @return true if the ship fits within the grid
 */

bool shipFits(char col, int row, int size, bool horizontal) {
    col = static_cast<char>(toupper(col));
    if (horizontal) {
        int endCol = (col - 'A') + size - 1;
        return endCol < GRID_SIZE;
    } else {
        int endRow = row + size - 1;
        return endRow <= GRID_SIZE;
    }
}

/**
 * Checks whether a ship overlaps any already-placed ships on the grid.
 * @param gameGrid   the current game grid
 * @param col        start column (A-J)
 * @param row        start row (1-10)
 * @param size       length of the ship
 * @param horizontal true = horizontal, false = vertical
 * @return true if no overlap, false if overlap detected
 */
bool noOverlap(char gameGrid[GRID_SIZE][GRID_SIZE], char col, int row, int size, bool horizontal) {
    col = static_cast<char>(toupper(col));
    int colIndex = col - 'A';
    int rowIndex = row - 1;

    for (int i = 0; i < size; i++) {
        int r = horizontal ? rowIndex     : rowIndex + i;
        int c = horizontal ? colIndex + i : colIndex;
        if (gameGrid[r][c] != '~') {
            return false;
        }
    }
    return true;
}

/**
 * Places a ship on the grid from the start coordinate.
 * @param gameGrid   the current game grid
 * @param col        start column (A-J)
 * @param row        start row (1-10)
 * @param size       length of the ship
 * @param horizontal true = horizontal, false = vertical
 * @param symbol     the character to mark the ship with
 */
void placeShip(char gameGrid[GRID_SIZE][GRID_SIZE], char col, int row, int size, bool horizontal, char symbol) {
    col = static_cast<char>(toupper(col));
    int colIndex = col - 'A';
    int rowIndex = row - 1;

    for (int i = 0; i < size; i++) {
        if (horizontal) {
            gameGrid[rowIndex][colIndex + i] = symbol;
        } else {
            gameGrid[rowIndex + i][colIndex] = symbol;
        }
    }
}

/**
 * Displays the game grid with column headers (A-J) and row numbers (1-10).
 * @param gameGrid the grid to display
 */
void displayGrid(char gameGrid[GRID_SIZE][GRID_SIZE]) {
    cout << "  A B C D E F G H I J\n";
    for (int r = 0; r < GRID_SIZE; r++) {
        cout << (r + 1);
        if (r + 1 < 10) cout << " ";
        for (int c = 0; c < GRID_SIZE; c++) {
            cout << " " << gameGrid[r][c];
        }
        cout << "\n";
    }
}

/**
 *Display the intial grid of the player before player has assigned their ships.
 *@param gameGrid the player map
 */
void displayEmptyGrid(char gameGrid[GRID_SIZE][GRID_SIZE]) {
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            gameGrid[r][c] = '~';
        }
    }
}

/**
 * Prompts the player to place a single ship on the grid.
 * Validates start coordinate, orientation, fit, and overlap.
 * Automatically calculates and displays the end coordinate.
 * @param gameGrid  the current game grid
 * @param shipName  name of the ship
 * @param size      length of the ship
 * @param symbol    character to mark the ship with
 */

void assignShip(char gameGrid[GRID_SIZE][GRID_SIZE], const string& shipName, int size, char symbol) {
    cout << "\nAssign your " << shipName << " (size " << size << ")\n";

    bool placed = false;
    while (!placed) {
        cout << "Enter start coordinate (e.g. A5): ";
        string input;
        cin >> input;

        if (input.length() < 2) {
            cout << "Invalid input, please enter a coordinate like A5\n";
        } else {
            char col = static_cast<char>(toupper(input[0]));
            int row = -1;
            bool parseSuccess = true;

            try {
                row = stoi(input.substr(1));
            } catch (...) {
                parseSuccess = false;
                cout << "Invalid input, please enter a coordinate like A5\n";
            }

            if (parseSuccess) {
                if (!isValidCoordinate(col, row)) {
                    cout << "Invalid coordinate, column must be A-J and row must be 1-10\n";
                } else {
                    cout << "Orientation - 1. Horizontal  2. Vertical: ";
                    int orientChoice;
                    cin >> orientChoice;

                    if (orientChoice != 1 && orientChoice != 2) {
                        cout << "Invalid choice, please enter 1 or 2\n";
                    } else {
                        bool horizontal = (orientChoice == 1);

                        if (!shipFits(col, row, size, horizontal)) {
                            cout << "Ship does not fit on the grid from that position, please try again\n";
                        } else if (!noOverlap(gameGrid, col, row, size, horizontal)) {
                            cout << "Ship overlaps an existing ship, please try again\n";
                        } else {
                            char endCol = horizontal
                                ? static_cast<char>('A' + (col - 'A') + size - 1)
                                : col;
                            int endRow = horizontal ? row : row + size - 1;

                            cout << shipName << " placed from " << col << row
                                 << " to " << endCol << endRow << "\n";

                            placeShip(gameGrid, col, row, size, horizontal, symbol);
                            displayGrid(gameGrid);
                            placed = true;
                        }
                    }
                }
            }
        }
    }
}

/**
 *This method is used to stop repeated code and improve the readability of the code.
 *The returned grid of the player
 *@param playerNum the player number
 *@param gameGrid the game grid of the player
*/

void assignShipPrompts (const int playerNum, char gameGrid[GRID_SIZE][GRID_SIZE]) {
    cout << "Player " << playerNum << " - assign your ships\n";
    displayGrid(gameGrid);
    assignShip(gameGrid, "Carrier",    5, 'C');
    assignShip(gameGrid, "Battleship", 4, 'B');
    assignShip(gameGrid, "Cruiser",    3, 'R');
    assignShip(gameGrid, "Submarine",  3, 'S');
    assignShip(gameGrid, "Destroyer",  2, 'D');
    cout << "\nAll ships placed. Player "<< playerNum << " Ready to play!\n";
    displayGrid(gameGrid);
}
/**
 *Function to check who goes first by randomly selecting 1 or 2
 */

int whoGoesFirst() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 2);
    return dist(gen);
}

/**
 *Typewrite method to type messages to the user for effect
 * @param message The message being displayed
 * @param delayMs The delay for writing the message. Set at 30 but can be slower if needs be.
 */
void typewrite(const string& message, int delayMs = 30) {
    for (char c : message) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }
    cout << "\n";
}

/**
 * Method to display enter to continue prompt (as this is used a lot in the program)
 * @param delayMs typewriter delay
 */

void enterToContinue(int delayMs) {
    typewrite( "Press Enter to continue...",delayMs);
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

/**
 * Fires at a coordinate on the opponent's grid.
 * Marks the result on the attacking player's tracking grid.
 * @param opponentGrid  the opponent's ship grid (to check for hits)
 * @param trackingGrid  the current player's tracking grid (to record shots)
 * @return true if the shot was a hit, false if a miss
 */
bool fireAtGrid(char opponentGrid[GRID_SIZE][GRID_SIZE], char trackingGrid[GRID_SIZE][GRID_SIZE]) {
    bool shotFired = false;
    bool isHit = false;

    while (!shotFired) {
        cout << "Enter target coordinate (e.g. A5): ";
        string input;
        cin >> input;

        if (!isValidCoordinate(input[0], input[1])) {
            cout << "Invalid input, please enter a coordinate like A5\n";
            continue;
        }

        char col = static_cast<char>(toupper(input[0]));
        int row = -1;
        bool parseSuccess = true;

        try {
            row = stoi(input.substr(1));
        } catch (...) {
            parseSuccess = false;
            cout << "Invalid input, please enter a coordinate like A5\n";
        }

        if (!parseSuccess) continue;

        if (!isValidCoordinate(col, row)) {
            cout << "Invalid coordinate, column must be A-J and row must be 1-10\n";
            continue;
        }

        int colIndex = col - 'A';
        int rowIndex = row - 1;

        // Check if this coordinate has already been fired at
        if (trackingGrid[rowIndex][colIndex] == 'X' || trackingGrid[rowIndex][colIndex] == 'O') {
            cout << "You have already fired at " << col << row << ", choose another coordinate\n";
            continue;
        }

        // Check opponent's grid for a hit
        if (opponentGrid[rowIndex][colIndex] != '~') {
            cout << "HIT at " << col << row << "!\n";
            trackingGrid[rowIndex][colIndex] = 'X';
            opponentGrid[rowIndex][colIndex] = 'X'; // Mark hit on opponent's grid too
            isHit = true;
        } else {
            cout << "MISS at " << col << row << ".\n";
            trackingGrid[rowIndex][colIndex] = 'O';
        }

        shotFired = true;
    }

    return isHit;
}

/**
 * The main program which runs the battleship program.
 * Creates a list of user prompts.
 * Player selection is here.
 * @return 0
 */
int main() {
    typewrite("Welcome to Battleships!\n",20);
    typewrite("(Game created by Alex Samuel)\n",20);
    typewrite("Press Enter to continue...",10);
    cin.get();

    bool inputError = false;
    do {
        typewrite("\nPlayer select required\n",5);
        typewrite("Would you like to play against the computer?\n",5);
        typewrite("1. Yes\n2. No\n",5);
        typewrite("Enter your choice: ",5);

        int choice;
        cin >> choice;

        if (cin.fail()) {
            inputError = true;
            typewrite("Invalid input, please enter 1 (Yes) or 2 (No)\n",10);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else if (choice == 1) {
            inputError = false;
            // Computer opponent logic goes here

        } else if (choice == 2) {
            inputError = false;

            char gameGridPlayerOne[GRID_SIZE][GRID_SIZE];
            displayEmptyGrid(gameGridPlayerOne);
            assignShipPrompts(1, gameGridPlayerOne);
            typewrite("Pass Over to Player 2 to assign ships!\n",5);
            enterToContinue(5);


            char gameGridPlayerTwo[GRID_SIZE][GRID_SIZE];
            displayEmptyGrid(gameGridPlayerTwo);
            assignShipPrompts(2, gameGridPlayerTwo);

            typewrite("Random player selection ...", 15);
            int firstPlayer = whoGoesFirst();
            string playerPrompt = "Player " + to_string(firstPlayer) + " goes first!";
            typewrite(playerPrompt, 10);

            enterToContinue(10);

            char firstPlayerGrid[GRID_SIZE][GRID_SIZE];
            cout << "Player " << firstPlayer << " - your tracking grid:\n";
            displayGrid(firstPlayerGrid);

            bool hit = false;


            if (firstPlayer == 1) {
                hit = fireAtGrid(gameGridPlayerTwo, firstPlayerGrid);
            } else {
                hit = fireAtGrid(gameGridPlayerOne, firstPlayerGrid);
            }

            cout << "\nYour updated tracking grid:\n";
            displayGrid(firstPlayerGrid);



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