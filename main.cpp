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

bool isValidRow(int row) {
    return row >= 1 && row <= GRID_SIZE;
}

bool isValidCol(char col) {
    col = static_cast<char>(toupper(col));
    return col >= 'A' && col <= 'J';
}

bool isValidCoordinate(char col, int row) {
    return isValidRow(row) && isValidCol(col);
}

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
 * Initialises all cells on a grid to '~' (water).
 * Previously named displayEmptyGrid — renamed to reflect actual purpose.
 * @param gameGrid the grid to initialise
 */
void initGrid(char gameGrid[GRID_SIZE][GRID_SIZE]) {
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            gameGrid[r][c] = '~';
        }
    }
}

/**
 * Checks whether all ships on the given grid have been sunk.
 * A cell counts as a remaining ship if it is not '~', 'X', or 'O'.
 * @param grid the opponent's grid to check
 * @return true if no ship symbols remain
 */
bool allShipsSunk(char grid[GRID_SIZE][GRID_SIZE]) {
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            char cell = grid[r][c];
            if (cell != '~' && cell != 'X' && cell != 'O') {
                return false;
            }
        }
    }
    return true;
}

void assignShip(char gameGrid[GRID_SIZE][GRID_SIZE], const string& shipName, int size, char symbol) {
    cout << "\nAssign your " << shipName << " (size " << size << ")\n";

    bool placed = false;
    while (!placed) {
        cout << "Enter start coordinate (e.g. A5): ";
        string input;
        cin >> input;

        if (input.length() < 2) {
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

        cout << "Orientation - 1. Horizontal  2. Vertical: ";
        int orientChoice;
        if (!(cin >> orientChoice)) {
            cout << "Invalid choice, please enter 1 or 2\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (orientChoice != 1 && orientChoice != 2) {
            cout << "Invalid choice, please enter 1 or 2\n";
            continue;
        }

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

void assignShipPrompts(const int playerNum, char gameGrid[GRID_SIZE][GRID_SIZE]) {
    cout << "Player " << playerNum << " - assign your ships\n";
    displayGrid(gameGrid);
    assignShip(gameGrid, "Carrier",    5, 'C');
    assignShip(gameGrid, "Battleship", 4, 'B');
    assignShip(gameGrid, "Cruiser",    3, 'R');
    assignShip(gameGrid, "Submarine",  3, 'S');
    assignShip(gameGrid, "Destroyer",  2, 'D');
    cout << "\nAll ships placed. Player " << playerNum << " Ready to play!\n";
    displayGrid(gameGrid);
}

int whoGoesFirst() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 2);
    return dist(gen);
}

void typewrite(const string& message, int delayMs = 30) {
    for (char c : message) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }
    cout << "\n";
}

void enterToContinue(int delayMs) {
    typewrite("Press Enter to continue...", delayMs);
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

bool fireAtGrid(char opponentGrid[GRID_SIZE][GRID_SIZE], char trackingGrid[GRID_SIZE][GRID_SIZE]) {
    bool shotFired = false;
    bool isHit = false;

    while (!shotFired) {
        cout << "Enter target coordinate (e.g. A5): ";
        string input;
        cin >> input;

        if (input.length() < 2) {
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

        if (trackingGrid[rowIndex][colIndex] == 'X' || trackingGrid[rowIndex][colIndex] == 'O') {
            cout << "You have already fired at " << col << row << ", choose another coordinate\n";
            continue;
        }

        if (opponentGrid[rowIndex][colIndex] != '~' && opponentGrid[rowIndex][colIndex] != 'X') {
            cout << "HIT at " << col << row << "!\n";
            trackingGrid[rowIndex][colIndex] = 'X';
            opponentGrid[rowIndex][colIndex] = 'X';
            isHit = true;
        } else {
            cout << "MISS at " << col << row << ".\n";
            trackingGrid[rowIndex][colIndex] = 'O';
            opponentGrid[rowIndex][colIndex] = 'O';
        }

        shotFired = true;
    }

    return isHit;
}

#ifndef TESTING
int main() {
    typewrite("Welcome to Battleships!\n", 20);
    typewrite("(Game created by Alex Samuel)\n", 20);
    typewrite("Press Enter to continue...", 10);
    cin.get();

    bool inputError = false;
    do {
        typewrite("\nPlayer select required\n", 5);
        typewrite("Would you like to play against the computer?\n", 5);
        typewrite("1. Yes\n2. No\n", 5);
        typewrite("Enter your choice: ", 5);

        int choice;
        cin >> choice;

        if (cin.fail()) {
            inputError = true;
            typewrite("Invalid input, please enter 1 (Yes) or 2 (No)\n", 10);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } else if (choice == 1) {
            inputError = false;
            // Computer opponent logic goes here
        } else if (choice == 2) {
            inputError = false;

            char gameGridPlayerOne[GRID_SIZE][GRID_SIZE];
            initGrid(gameGridPlayerOne);
            assignShipPrompts(1, gameGridPlayerOne);
            typewrite("Pass Over to Player 2 to assign ships!\n", 5);
            enterToContinue(5);

            char gameGridPlayerTwo[GRID_SIZE][GRID_SIZE];
            initGrid(gameGridPlayerTwo);
            assignShipPrompts(2, gameGridPlayerTwo);

            typewrite("Random player selection ...", 15);
            int firstPlayer = whoGoesFirst();
            string playerPrompt = "Player " + to_string(firstPlayer) + " goes first!";
            typewrite(playerPrompt, 10);
            enterToContinue(10);

            char trackingGridOne[GRID_SIZE][GRID_SIZE];
            char trackingGridTwo[GRID_SIZE][GRID_SIZE];
            initGrid(trackingGridOne);
            initGrid(trackingGridTwo);

            int currentPlayer = firstPlayer;
            bool playerWon = false;

            while (!playerWon) {
                playerWon = false;
                int opponent = (currentPlayer == 1) ? 2 : 1;
                char* opponentGrid   = (currentPlayer == 1)
                    ? &gameGridPlayerTwo[0][0] : &gameGridPlayerOne[0][0];
                char* myTrackingGrid = (currentPlayer == 1)
                    ? &trackingGridOne[0][0]   : &trackingGridTwo[0][0];

                cout << "\n--- Player " << currentPlayer << "'s turn ---\n";
                cout << "Your tracking grid:\n";
                displayGrid(reinterpret_cast<char(*)[GRID_SIZE]>(myTrackingGrid));

                fireAtGrid(
                    reinterpret_cast<char(*)[GRID_SIZE]>(opponentGrid),
                    reinterpret_cast<char(*)[GRID_SIZE]>(myTrackingGrid)
                );

                cout << "\nYour updated tracking grid:\n";
                displayGrid(reinterpret_cast<char(*)[GRID_SIZE]>(myTrackingGrid));

                if (allShipsSunk(reinterpret_cast<char(*)[GRID_SIZE]>(opponentGrid))) {
                    cout << "\n*** Player " << currentPlayer << " wins! All of Player "
                         << opponent << "'s ships have been sunk! ***\n";
                    playerWon = true;
                }

                enterToContinue(10);
                currentPlayer = opponent;
            }
        } else {
            inputError = true;
            cout << "Invalid input, please enter 1 (Yes) or 2 (No)\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cin.get();
        }
    } while (inputError);

    return 0;
}
#endif