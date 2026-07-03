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
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

constexpr int GRID_SIZE = 10;

enum CellState {
    UNKNOWN,
    HIT,
    MISS,
    SUNK,
};

struct Ship {
    int length;
    bool isSunk;
};

/**
 * Clears the terminal screen.
 */
void clearScreen() {
    system("cls");
}

/**
 * Checks whether a given row number is within the valid grid range (1–10).
 * @param row the row number to validate
 * @return true if the row is between 1 and GRID_SIZE inclusive
 */
bool isValidRow(int row) {
    return row >= 1 && row <= GRID_SIZE;
}

/**
 * Checks whether a given column character is within the valid grid range (A–J).
 * @param col the column character to validate (case-insensitive)
 * @return true if the column is between 'A' and 'J' inclusive
 */
bool isValidCol(char col) {
    col = static_cast<char>(toupper(col));
    return col >= 'A' && col <= 'J';
}

/**
 * Checks whether a given column and row combination forms a valid grid coordinate.
 * @param col the column character (case-insensitive)
 * @param row the row number
 * @return true if both the column and row are within valid grid bounds
 */
bool isValidCoordinate(char col, int row) {
    return isValidRow(row) && isValidCol(col);
}

/**
 * Checks whether a ship of a given size fits on the grid from a starting position.
 * @param col the starting column character (case-insensitive)
 * @param row the starting row number
 * @param size the length of the ship
 * @param horizontal true if the ship is placed horizontally, false if vertically
 * @return true if the ship fits entirely within the grid boundaries
 */
bool shipFits(char col, int row, int size, bool horizontal) {
    col = static_cast<char>(toupper(col));
    if (horizontal) {
        int endCol = (col - 'A') + size - 1;
        return endCol < GRID_SIZE;
    }
    int endRow = row + size - 1;
    return endRow <= GRID_SIZE;
}

/**
 * Checks whether a ship placement would overlap any existing ships on the grid.
 * @param gameGrid the grid to check for existing ship placements
 * @param col the starting column character (case-insensitive)
 * @param row the starting row number
 * @param size the length of the ship
 * @param horizontal true if the ship is placed horizontally, false if vertically
 * @return true if there is no overlap with existing ships
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
 * Places a ship on the grid by marking each occupied cell with the given symbol.
 * @param gameGrid the grid on which to place the ship
 * @param col the starting column character (case-insensitive)
 * @param row the starting row number
 * @param size the length of the ship
 * @param horizontal true if the ship is placed horizontally, false if vertically
 * @param symbol the character used to represent this ship on the grid
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
 * Displays the current state of a grid to standard output.
 * Column headers are printed as letters (A–J) and row numbers as integers (1–10).
 * @param gameGrid the grid to display
 */
void displayGrid(char gameGrid[GRID_SIZE][GRID_SIZE]) {
    cout << "   A B C D E F G H I J\n";
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

/**
 * Prompts the current player to place a single named ship on their grid.
 * Validates the coordinate, orientation, fit, and overlap before placing.
 * @param gameGrid the player's grid on which to place the ship
 * @param shipName the display name of the ship (e.g. "Carrier")
 * @param size the length of the ship in cells
 * @param symbol the character used to represent the ship on the grid
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

/**
 * Guides a player through placing all five ships on their grid.
 * Ships placed are: Carrier (5), Battleship (4), Cruiser (3), Submarine (3), Destroyer (2).
 * @param playerNum the player number (1 or 2), used for display purposes
 * @param gameGrid the player's grid on which all ships will be placed
 */
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

/**
 * Randomly selects which player (1 or 2) takes the first turn.
 * Uses a non-deterministic random device as the seed.
 * @return 1 or 2, chosen at random
 */
int whoGoesFirst() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 2);
    return dist(gen);
}

/**
 * Prints a message to standard output one character at a time with a configurable delay,
 * creating a typewriter effect.
 * @param message the string to print character by character
 * @param delayMs the delay in milliseconds between each character (default: 30)
 */
void typewrite(const string& message, int delayMs = 30) {
    for (char c : message) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }
    cout << "\n";
}

/**
 * Prompts the user to press Enter and then waits for them to do so.
 * @param delayMs the delay in milliseconds used for the typewrite prompt
 */
void enterToContinue(int delayMs) {
    typewrite("Press Enter to continue...", delayMs);
        cin.ignore(numeric_limits<streamsize>::max(), '\n');  // wait for user to press Enter
    }

/**
 * Handles a single firing action for the current player.
 * Prompts for a target coordinate, validates it, checks it has not already been fired at,
 * then marks the result as a hit ('X') or miss ('O') on both the opponent's grid
 * and the current player's tracking grid.
 * @param opponentGrid the opponent's game grid, updated in place on a hit or miss
 * @param trackingGrid the current player's tracking grid, updated in place with 'X' or 'O'
 * @return true if the shot was a hit, false if it was a miss
 */
bool fireAtGrid(char opponentGrid[GRID_SIZE][GRID_SIZE], char trackingGrid[GRID_SIZE][GRID_SIZE]) {
    bool shotFired = false;
    bool isHit = false;

    while (!shotFired) {
        if (cin.fail() || cin.eof()) {
            cerr << "\nError: ran out of input\n";
            exit(1);
        }
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

        // Fresh shot — check the opponent's ORIGINAL grid
        if (opponentGrid[rowIndex][colIndex] != '~') {   // anything not water = ship
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
char randomComputerShipColumn() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, 9);
    char random_coordinate = 'a' + distribution(gen);
    return random_coordinate;
}
int randomComputerShipRow() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, 10);
    int random_coordinate = distribution(gen);
    return random_coordinate;
}
bool randomComputerShipOrientation() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution distribution(0.5);
    bool random_orientation = distribution(gen);
    return random_orientation;
}

void assignComputerShips (char gameGrid [GRID_SIZE][GRID_SIZE], int delayMS) {
    typewrite("Computer Assigning Ships ...", delayMS);
    placeShip(gameGrid, randomComputerShipColumn(), randomComputerShipRow(), 5, randomComputerShipOrientation(), 'C');
    placeShip(gameGrid, randomComputerShipColumn(), randomComputerShipRow(), 4, randomComputerShipOrientation(), 'B');
    placeShip(gameGrid, randomComputerShipColumn(), randomComputerShipRow(), 3, randomComputerShipOrientation(), 'R');
    placeShip(gameGrid, randomComputerShipColumn(), randomComputerShipRow(), 2, randomComputerShipOrientation(), 'D');
    displayGrid(gameGrid);
    typewrite("Computer Ships Assigned ...", delayMS);
    enterToContinue(delayMS);
    clearScreen();
}

/**
 * Asks the players whether they would like to start a new game.
 * Displays a prompt with options 1 (Yes) and 2 (No).
 * @return true if the players choose to play again, false otherwise
 */
bool playAgainFunc () {
    typewrite("\nWould you like to play another game?\n", 5);
    typewrite("1. Yes\n2. No\n", 5);
    int again = 0;
    cin >> again;
    return (again == 1);
}

/**
 * Entry point for the Battleships game.
 * Manages the outer play-again loop, player-select menu, ship placement,
 * turn-based firing, and win detection for a two-player local game.
 * @return 0 on normal exit
 */
int main() {
    typewrite("Welcome to Battleships!\n", 20);
    typewrite("(Game created by Alex Samuel)\n", 20);
    typewrite("Press Enter to continue...", 10);
    cin.get();
    clearScreen();

    bool playAgain = true;

    while (playAgain) {
        int choice = 0;
        bool inputError = false;

        do {
            typewrite("\nPlayer select required\n", 5);
            typewrite("Would you like to play against the computer?\n", 5);
            typewrite("1. Yes\n2. No\n", 5);
            typewrite("Enter your choice: ", 5);

            cin >> choice;

            if (cin.fail()) {
                inputError = true;
                typewrite("Invalid input, please enter 1 (Yes) or 2 (No)\n", 10);
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');

            } else if (choice == 1) {
                inputError = false;
                typewrite("\nComputer opponent not yet implemented coming in version 2!\n", 10);
                playAgain = playAgainFunc();

            } else if (choice == 2) {
                inputError = false;
                char gameGridPlayerOne[GRID_SIZE][GRID_SIZE];
                initGrid(gameGridPlayerOne);
                assignShipPrompts(1, gameGridPlayerOne);
                typewrite("Pass Over to Player 2 to assign ships!\n", 5);
                enterToContinue(5);
                clearScreen();

                char gameGridPlayerTwo[GRID_SIZE][GRID_SIZE];
                initGrid(gameGridPlayerTwo);
                assignShipPrompts(2, gameGridPlayerTwo);
                typewrite("Pass the computer back to Player 1!\n", 5);
                enterToContinue(5);
                clearScreen();

                typewrite("Random player selection ...", 15);
                int firstPlayer = whoGoesFirst();
                string playerPrompt = "Player " + to_string(firstPlayer) + " goes first!";
                typewrite(playerPrompt, 10);
                enterToContinue(10);
                clearScreen();

                char trackingGridOne[GRID_SIZE][GRID_SIZE];
                char trackingGridTwo[GRID_SIZE][GRID_SIZE];
                initGrid(trackingGridOne);
                initGrid(trackingGridTwo);

                int currentPlayer = firstPlayer;
                bool playerWon = false;

                while (!playerWon) {
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

                    if (!playerWon) {
                        enterToContinue(10);
                        clearScreen();
                    }
                    currentPlayer = opponent;
                }
                playAgain = playAgainFunc();

            } else {
                inputError = true;
                cout << "Invalid input, please enter 1 (Yes) or 2 (No)\n";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

        } while (inputError);
    }

    typewrite("Thank you for playing Battleships!\n", 10);
    return 0;
}