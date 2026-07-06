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
#include <sstream>
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
 * Clears the terminal screen using an ANSI escape sequence (portable across
 * Windows/Linux/macOS terminals, unlike system("cls")).
 */
void clearScreen() {
    cout << "\033[2J\033[1;1H";
}

// Forward declaration: typewrite is defined later in the file but used by
// several functions (assignShip, assignShipPrompts, etc.) defined earlier.
// Default arguments live here only — the definition below must not repeat them.
void typewrite(const string& message, int delayMs = 30, bool newline = true);

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
    ostringstream headerMsg;
    headerMsg << "\nAssign your " << shipName << " (size " << size << ")";
    typewrite(headerMsg.str(), 5);

    bool placed = false;
    while (!placed) {
        typewrite("Enter start coordinate (e.g. A5): ", 5, false);
        string input;
        cin >> input;

        if (input.length() < 2) {
            typewrite("Invalid input, please enter a coordinate like A5", 5);
            continue;
        }

        char col = static_cast<char>(toupper(input[0]));
        int row = -1;
        bool parseSuccess = true;

        try {
            row = stoi(input.substr(1));
        } catch (...) {
            parseSuccess = false;
            typewrite("Invalid input, please enter a coordinate like A5", 5);
        }

        if (!parseSuccess) continue;

        if (!isValidCoordinate(col, row)) {
            typewrite("Invalid coordinate, column must be A-J and row must be 1-10", 5);
            continue;
        }

        typewrite("Orientation - 1. Horizontal  2. Vertical: ", 5, false);
        int orientChoice;
        if (!(cin >> orientChoice)) {
            typewrite("Invalid choice, please enter 1 or 2", 5);
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (orientChoice != 1 && orientChoice != 2) {
            typewrite("Invalid choice, please enter 1 or 2", 5);
            continue;
        }

        bool horizontal = (orientChoice == 1);

        if (!shipFits(col, row, size, horizontal)) {
            typewrite("Ship does not fit on the grid from that position, please try again", 5);
        } else if (!noOverlap(gameGrid, col, row, size, horizontal)) {
            typewrite("Ship overlaps an existing ship, please try again", 5);
        } else {
            char endCol = horizontal
                ? static_cast<char>('A' + (col - 'A') + size - 1)
                : col;
            int endRow = horizontal ? row : row + size - 1;

            ostringstream placedMsg;
            placedMsg << shipName << " placed from " << col << row
                      << " to " << endCol << endRow;
            typewrite(placedMsg.str(), 5);

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
    ostringstream startMsg;
    startMsg << "Player " << playerNum << " - assign your ships";
    typewrite(startMsg.str(), 5);
    displayGrid(gameGrid);
    assignShip(gameGrid, "Carrier",    5, 'C');
    assignShip(gameGrid, "Battleship", 4, 'B');
    assignShip(gameGrid, "Cruiser",    3, 'R');
    assignShip(gameGrid, "Submarine",  3, 'S');
    assignShip(gameGrid, "Destroyer",  2, 'D');
    ostringstream doneMsg;
    doneMsg << "\nAll ships placed. Player " << playerNum << " Ready to play!";
    typewrite(doneMsg.str(), 5);
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
 * @param newline whether to print a trailing newline after the message (default: true).
 *                Pass false for inline prompts immediately followed by a cin read,
 *                so the user's typed input stays on the same visual line.
 */
void typewrite(const string& message, int delayMs, bool newline) {
    for (char c : message) {
        cout << c << flush;
        this_thread::sleep_for(chrono::milliseconds(delayMs));
    }
    if (newline) {
        cout << "\n";
    }
}

/**
 * Prompts the user to press Enter and then waits for them to do so.
 * Clears any leftover newline left in the input buffer by a prior cin >> read
 * before blocking on the user's actual next Enter press.
 * @param delayMs the delay in milliseconds used for the typewrite prompt
 */
void enterToContinue(int delayMs) {
    typewrite("Press Enter to continue...", delayMs);
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // discard leftover from prior input, if any
    cin.get(); // now actually wait for the user's next Enter press
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
        typewrite("Enter target coordinate (e.g. A5): ", 5, false);
        string input;
        cin >> input;

        if (input.length() < 2) {
            typewrite("Invalid input, please enter a coordinate like A5", 5);
            continue;
        }

        char col = static_cast<char>(toupper(input[0]));
        int row = -1;
        bool parseSuccess = true;

        try {
            row = stoi(input.substr(1));
        } catch (...) {
            parseSuccess = false;
            typewrite("Invalid input, please enter a coordinate like A5", 5);
        }

        if (!parseSuccess) continue;

        if (!isValidCoordinate(col, row)) {
            typewrite("Invalid coordinate, column must be A-J and row must be 1-10", 5);
            continue;
        }

        int colIndex = col - 'A';
        int rowIndex = row - 1;

        if (trackingGrid[rowIndex][colIndex] == 'X' || trackingGrid[rowIndex][colIndex] == 'O') {
            ostringstream alreadyMsg;
            alreadyMsg << "You have already fired at " << col << row << ", choose another coordinate";
            typewrite(alreadyMsg.str(), 5);
            continue;
        }

        // Fresh shot — check the opponent's ORIGINAL grid
        if (opponentGrid[rowIndex][colIndex] != '~') {   // anything not water = ship
            ostringstream hitMsg;
            hitMsg << "HIT at " << col << row << "!";
            typewrite(hitMsg.str(), 5);
            trackingGrid[rowIndex][colIndex] = 'X';
            opponentGrid[rowIndex][colIndex] = 'X';

            isHit = true;
        } else {
            ostringstream missMsg;
            missMsg << "MISS at " << col << row << ".";
            typewrite(missMsg.str(), 5);
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
    std::uniform_int_distribution<> distribution(1, 10);
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

void validateComputerShipAssignment(char gameGrid[GRID_SIZE][GRID_SIZE], char col, int row, int size, bool horizontal, char symbol) {
    while (!shipFits(col, row, size, horizontal) || !noOverlap(gameGrid, col, row, size, horizontal)) {
        col = randomComputerShipColumn();
        row = randomComputerShipRow();
        horizontal = randomComputerShipOrientation();
    }
    placeShip(gameGrid, col, row, size, horizontal, symbol);
}

void assignComputerShips(char gameGrid[GRID_SIZE][GRID_SIZE], int delayMS) {
    typewrite("Computer Assigning Ships ...", delayMS);
    validateComputerShipAssignment(gameGrid, randomComputerShipColumn(), randomComputerShipRow(), 5, randomComputerShipOrientation(), 'C');
    validateComputerShipAssignment(gameGrid, randomComputerShipColumn(), randomComputerShipRow(), 4, randomComputerShipOrientation(), 'B');
    validateComputerShipAssignment(gameGrid, randomComputerShipColumn(), randomComputerShipRow(), 3, randomComputerShipOrientation(), 'R');
    validateComputerShipAssignment(gameGrid, randomComputerShipColumn(), randomComputerShipRow(), 3, randomComputerShipOrientation(), 'S');
    validateComputerShipAssignment(gameGrid, randomComputerShipColumn(), randomComputerShipRow(), 2, randomComputerShipOrientation(), 'D');
}

/**
 * Tracks everything the computer AI knows about one specific opponent ship:
 * its symbol/length (so we know how many hits sink it), how many hits it has
 * taken so far, whether it has been confirmed sunk, and which cells were hit.
 */
struct ShipTarget {
    char symbol;
    int length;
    int hitsTaken = 0;
    bool sunk = false;
    vector<pair<int, int>> hitCells;
};

/**
 * Persistent state for the computer's targeting AI across an entire game.
 * candidateQueue holds cells queued up to try next because they are adjacent
 * to a known, not-yet-sunk hit ("Target mode"). ships tracks sink progress
 * for each of the five opponent ships by their placement symbol.
 */
struct ComputerAI {
    vector<pair<int, int>> candidateQueue;
    vector<ShipTarget> ships = {
        {'C', 5}, {'B', 4}, {'R', 3}, {'S', 3}, {'D', 2}
    };
};

/**
 * Checks whether a 0-indexed row/column pair lies within the grid bounds.
 * @param r the row index (0-based)
 * @param c the column index (0-based)
 * @return true if both indices are within [0, GRID_SIZE)
 */
bool inBounds(int r, int c) {
    return r >= 0 && r < GRID_SIZE && c >= 0 && c < GRID_SIZE;
}

/**
 * Checks whether a cell on a tracking grid has already been fired at.
 * @param trackingGrid the tracking grid to check
 * @param r the row index (0-based)
 * @param c the column index (0-based)
 * @return true if the cell is marked 'X' (hit) or 'O' (miss)
 */
bool alreadyTried(char trackingGrid[GRID_SIZE][GRID_SIZE], int r, int c) {
    return trackingGrid[r][c] == 'X' || trackingGrid[r][c] == 'O';
}

/**
 * Looks up a ship target record by its grid placement symbol.
 * @param ai the computer AI state to search
 * @param symbol the ship's placement symbol (e.g. 'C', 'B', 'R', 'S', 'D')
 * @return a pointer to the matching ShipTarget, or nullptr if not found
 */
ShipTarget* findShipBySymbol(ComputerAI& ai, char symbol) {
    for (auto& ship : ai.ships) {
        if (ship.symbol == symbol) return &ship;
    }
    return nullptr;
}

/**
 * Finds the still-unsunk ship target (if any) that owns a hit at the given cell.
 * @param ai the computer AI state to search
 * @param row the row index (0-based) of the hit cell
 * @param column the column index (0-based) of the hit cell
 * @return a pointer to the owning ShipTarget, or nullptr if none found
 */
ShipTarget* findShipOwningHit(ComputerAI& ai, int row, int column) {
    for (auto& ship : ai.ships) {
        if (ship.sunk) continue;
        for (const auto& [r, c] : ship.hitCells) {
            if (r == row && c == column) return &ship;
        }
    }
    return nullptr;
}

/**
 * Queues candidate cells around a freshly hit cell for the computer to try next.
 * If the owning ship has two or more hits, this checks whether those hits share
 * a row (horizontal ship) or column (vertical ship) and, if so, restricts the
 * queued candidates to that axis only — avoiding wasted perpendicular shots
 * once the ship's orientation is known.
 * @param ai the computer AI state whose candidateQueue will be appended to
 * @param trackingGrid the computer's tracking grid, used to skip already-tried cells
 * @param row the row index (0-based) of the cell whose neighbours to queue
 * @param column the column index (0-based) of the cell whose neighbours to queue
 */
void queueNeighbours(ComputerAI& ai, char trackingGrid[GRID_SIZE][GRID_SIZE], const int row, const int column) {
    ShipTarget* activeShip = findShipOwningHit(ai, row, column);

    bool restrictHorizontal = false; // ship confirmed running left-right: only queue left/right
    bool restrictVertical = false;   // ship confirmed running up-down: only queue up/down

    if (activeShip != nullptr && activeShip->hitCells.size() >= 2) {
        bool sameRow = true, sameCol = true;
        int r0 = activeShip->hitCells[0].first;
        int c0 = activeShip->hitCells[0].second;
        for (const auto& [r, c] : activeShip->hitCells) {
            if (r != r0) sameRow = false;
            if (c != c0) sameCol = false;
        }
        restrictHorizontal = sameRow;
        restrictVertical = sameCol;
    }

    constexpr int deltaColumn[] = {0, 0, -1, 1};
    constexpr int deltaRow[] = {-1, 1, 0, 0};

    for (int i = 0; i < 4; i++) {
        if (restrictHorizontal && deltaRow[i] != 0) continue;   // skip up/down
        if (restrictVertical && deltaColumn[i] != 0) continue;  // skip left/right

        int neighbourRow = row + deltaRow[i];
        if (int neighbourColumn = column + deltaColumn[i]; inBounds(neighbourRow, neighbourColumn)
            && !alreadyTried(trackingGrid, neighbourRow, neighbourColumn)) {
            ai.candidateQueue.emplace_back(neighbourRow, neighbourColumn);
        }
    }
}

/**
 * Rebuilds the candidate queue from scratch using only the hit cells of ships
 * that are still afloat, discarding any stale leads left over from a ship
 * that has since been confirmed sunk.
 * @param ai the computer AI state whose candidateQueue will be rebuilt
 * @param trackingGrid the computer's tracking grid, used to skip already-tried cells
 */
void rebuildCandidateQueue(ComputerAI& ai, char trackingGrid[GRID_SIZE][GRID_SIZE]) {
    ai.candidateQueue.clear();
    for (const auto& ship : ai.ships) {
        if (ship.sunk) continue;
        for (const auto& [r, c] : ship.hitCells) {
            queueNeighbours(ai, trackingGrid, r, c);
        }
    }
}

/**
 * Counts how many of the computer's tracked target ships have been sunk so far.
 * @param ai the computer AI state to inspect
 * @return the number of ships marked sunk
 */
int shipsSunkCount(const ComputerAI& ai) {
    return static_cast<int>(count_if(ai.ships.begin(), ai.ships.end(),
        [](const ShipTarget& s) { return s.sunk; }));
}

/**
 * Builds a probability density grid for Hunt-mode targeting: for every still-
 * unsunk ship, every legal horizontal and vertical placement (one that stays
 * on the board and doesn't cross a known miss) adds +1 to every cell it would
 * occupy. Cells that appear in more legal placements score higher, biasing
 * shots toward the board centre and toward shapes consistent with the ships
 * still afloat.
 * @param ai the computer AI state, used to find still-unsunk ship lengths
 * @param trackingGrid the computer's tracking grid, used to exclude known misses
 * @param probabilityGrid output grid of per-cell placement counts
 */
void buildProbabilityGrid(const ComputerAI& ai, char trackingGrid[GRID_SIZE][GRID_SIZE],
                           int probabilityGrid[GRID_SIZE][GRID_SIZE]) {
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            probabilityGrid[r][c] = 0;
        }
    }

    for (const auto& ship : ai.ships) {
        if (ship.sunk) continue;
        int len = ship.length;

        // Horizontal placements
        for (int r = 0; r < GRID_SIZE; r++) {
            for (int c = 0; c <= GRID_SIZE - len; c++) {
                bool legal = true;
                for (int i = 0; i < len && legal; i++) {
                    if (trackingGrid[r][c + i] == 'O') legal = false;
                }
                if (legal) {
                    for (int i = 0; i < len; i++) probabilityGrid[r][c + i]++;
                }
            }
        }

        // Vertical placements
        for (int c = 0; c < GRID_SIZE; c++) {
            for (int r = 0; r <= GRID_SIZE - len; r++) {
                bool legal = true;
                for (int i = 0; i < len && legal; i++) {
                    if (trackingGrid[r + i][c] == 'O') legal = false;
                }
                if (legal) {
                    for (int i = 0; i < len; i++) probabilityGrid[r + i][c]++;
                }
            }
        }
    }
}

/**
 * Picks the highest-scoring untried cell from a probability grid, for use as
 * the computer's next Hunt-mode shot.
 * @param trackingGrid the computer's tracking grid, used to skip already-tried cells
 * @param probabilityGrid the probability density grid built by buildProbabilityGrid
 * @return the (row, column) of the best untried cell to fire at
 */
pair<int, int> bestHuntCell(char trackingGrid[GRID_SIZE][GRID_SIZE],
                             int probabilityGrid[GRID_SIZE][GRID_SIZE]) {
    int bestScore = -1, bestR = 0, bestC = 0;
    for (int r = 0; r < GRID_SIZE; r++) {
        for (int c = 0; c < GRID_SIZE; c++) {
            if (!alreadyTried(trackingGrid, r, c) && probabilityGrid[r][c] > bestScore) {
                bestScore = probabilityGrid[r][c];
                bestR = r;
                bestC = c;
            }
        }
    }
    return {bestR, bestC};
}

string checkDestroyedShip (const int shipLength) {
    if (shipLength == 5) {
        return "Computer sank your battleship (length 5)!";
    }
    if (shipLength == 4) {
        return "Computer sank your cruiser (length 4)!";
    }
    if (shipLength == 3) {
        return "Computer sank your submarine (length 3)!";
    }
    if (shipLength == 2) {
        return "Computer sank your destroyer (length 2)!";
    }
    return "Error detecting destroyed ship report error to developers";
}

/**
 * Performs one computer-controlled firing action against the human player's grid.
 * Uses a Hunt/Target state machine: if there are queued candidate cells near a
 * known, unsunk hit ("Target mode"), it fires at one of those (restricted to
 * the ship's known orientation once two aligned hits reveal it); otherwise it
 * fires at the highest-scoring untried cell from a probability density grid
 * built from all legal placements of the ships still afloat ("Hunt mode").
 * Tracks which specific ship was hit (by its placement symbol) so it can detect
 * when that ship has been fully sunk and purge stale queued leads accordingly.
 * @param opponentGrid the human player's game grid, updated in place on a hit or miss
 * @param trackingGrid the computer's tracking grid, updated in place with 'X' or 'O'
 * @param ai the computer's persistent targeting AI state
 * @return true if the shot was a hit, false if it was a miss
 */
bool computerFireAtGrid(char opponentGrid[GRID_SIZE][GRID_SIZE],
                         char trackingGrid[GRID_SIZE][GRID_SIZE],
                         ComputerAI& ai) {
    int rowIndex = -1, colIndex = -1;

    if (!ai.candidateQueue.empty()) {
        // TARGET MODE: work through queued cells near a known, unsunk hit
        while (!ai.candidateQueue.empty()) {
            auto [row, column] = ai.candidateQueue.back();
            ai.candidateQueue.pop_back();
            if (!alreadyTried(trackingGrid, row, column)) {
                rowIndex = row; colIndex = column;
                break;
            }
        }
    }

    if (rowIndex == -1) {
        // HUNT MODE: fire at the highest-probability untried cell
        static int probabilityGrid[GRID_SIZE][GRID_SIZE];
        buildProbabilityGrid(ai, trackingGrid, probabilityGrid);
        auto [bestR, bestC] = bestHuntCell(trackingGrid, probabilityGrid);
        rowIndex = bestR;
        colIndex = bestC;
    }

    char hitSymbol = opponentGrid[rowIndex][colIndex];   // capture BEFORE it gets overwritten
    bool isHit = (hitSymbol != '~');
    char mark = isHit ? 'X' : 'O';
    trackingGrid[rowIndex][colIndex] = mark;
    opponentGrid[rowIndex][colIndex] = mark;

    ostringstream fireMsg;
    fireMsg << "Computer fires at " << static_cast<char>('A' + colIndex)
             << (rowIndex + 1) << " - " << (isHit ? "HIT!" : "MISS.");
    typewrite(fireMsg.str(), 5);

    if (isHit) {
        ShipTarget* ship = findShipBySymbol(ai, hitSymbol);
        if (ship != nullptr) {
            ship->hitsTaken++;
            ship->hitCells.emplace_back(rowIndex, colIndex);

            if (ship->hitsTaken >= ship->length) {
                ship->sunk = true;
                ostringstream sunkMsg;
                if (ship ->length == 5) {
                    sunkMsg << "Computer sank your battleship (length 5)!";
                } else if (ship -> length == 4) {
                    sunkMsg << "Computer sank your cruiser (length 4)!";
                } else if (ship -> length == 3) {
                    sunkMsg << "Computer sank your submarine (length 3)!";
                } else if (ship -> length == 2) {
                    sunkMsg << "Computer sank your destroyer (length 2)!";
                }
                typewrite(sunkMsg.str(), 5);
                rebuildCandidateQueue(ai, trackingGrid);
            } else {
                queueNeighbours(ai, trackingGrid, rowIndex, colIndex);
            }
        }
    }

    return isHit;
}
string checkDestroyedShip (const int shipLength) {
    if (shipLength == 5) {
        return "Computer sank your battleship (length 5)!";
    }
    if (shipLength == 4) {
        return "Computer sank your cruiser (length 4)!";
    }
    if (shipLength == 3) {
        return "Computer sank your submarine (length 3)!";
    }
    if (shipLength == 2) {
        return "Computer sank your destroyer (length 2)!";
    }
        return "Error detecting destroyed ship report error to developers";
}

/**
 * Asks the players whether they would like to start a new game.
 * Displays a prompt with options 1 (Yes) and 2 (No).
 * @return true if the players choose to play again, false otherwise
 */
bool playAgainFunc() {
    typewrite("\nWould you like to play another game?\n", 5);
    typewrite("1. Yes\n2. No\n", 5);
    int again = 0;
    cin >> again;
    return (again == 1);
}

/**
 * Entry point for the Battleships game.
 * Manages the outer play-again loop, player-select menu, ship placement,
 * turn-based firing, and win detection for a two-player local game or a
 * single-player game against the computer AI.
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

                // Set up the computer's grid and ships
                char gameGridComputer[GRID_SIZE][GRID_SIZE];
                initGrid(gameGridComputer);
                assignComputerShips(gameGridComputer, 5);
                typewrite("Computer is thinking ...\n");
                this_thread::sleep_for(chrono::milliseconds(1500));
                typewrite("Computer is ready!\n", 5);
                enterToContinue(5);
                clearScreen();

                // Set up the human player's grid and ships
                char gameGridPlayer[GRID_SIZE][GRID_SIZE];
                initGrid(gameGridPlayer);
                assignShipPrompts(1, gameGridPlayer);
                enterToContinue(5);
                clearScreen();

                // Tracking grids: what each side knows about the other
                char playerTrackingGrid[GRID_SIZE][GRID_SIZE];
                char computerTrackingGrid[GRID_SIZE][GRID_SIZE];
                initGrid(playerTrackingGrid);
                initGrid(computerTrackingGrid);

                ComputerAI ai;

                typewrite("Random player selection ...", 15);
                int firstPlayer = whoGoesFirst(); // 1 = human, 2 = computer
                string firstPrompt = (firstPlayer == 1) ? "You go first!" : "Computer goes first!";
                typewrite(firstPrompt, 10);
                enterToContinue(10);
                clearScreen();

                int currentTurn = firstPlayer;
                bool gameOver = false;

                while (!gameOver) {
                    if (currentTurn == 1) {
                        typewrite("\n--- Your turn ---", 5);
                        typewrite("Your tracking grid (shots fired at the computer):", 5);
                        displayGrid(playerTrackingGrid);

                        fireAtGrid(gameGridComputer, playerTrackingGrid);

                        typewrite("\nYour updated tracking grid:", 5);
                        displayGrid(playerTrackingGrid);

                        if (allShipsSunk(gameGridComputer)) {
                            typewrite("\n*** You win! All of the computer's ships have been sunk! ***", 10);
                            gameOver = true;
                        }
                    } else {
                        typewrite("\nComputer's turn ...", 5);
                        computerFireAtGrid(gameGridPlayer, computerTrackingGrid, ai);
                        this_thread::sleep_for(chrono::milliseconds(1200));

                        ostringstream lostMsg;
                        lostMsg << "Ships you have lost so far: " << shipsSunkCount(ai) << " / 5";
                        typewrite(lostMsg.str(), 5);
                        typewrite("\nYour grid:", 5);
                        displayGrid(gameGridPlayer);

                        if (allShipsSunk(gameGridPlayer)) {
                            typewrite("\n*** Computer wins! All of your ships have been sunk! ***", 10);
                            gameOver = true;
                        }
                    }

                    if (!gameOver) {
                        enterToContinue(10);
                        clearScreen();
                    }
                    currentTurn = (currentTurn == 1) ? 2 : 1;
                }
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

                    ostringstream turnMsg;
                    turnMsg << "\n--- Player " << currentPlayer << "'s turn ---";
                    typewrite(turnMsg.str(), 5);
                    typewrite("Your tracking grid:", 5);
                    displayGrid(reinterpret_cast<char(*)[GRID_SIZE]>(myTrackingGrid));

                    fireAtGrid(
                        reinterpret_cast<char(*)[GRID_SIZE]>(opponentGrid),
                        reinterpret_cast<char(*)[GRID_SIZE]>(myTrackingGrid)
                    );

                    typewrite("\nYour updated tracking grid:", 5);
                    displayGrid(reinterpret_cast<char(*)[GRID_SIZE]>(myTrackingGrid));

                    if (allShipsSunk(reinterpret_cast<char(*)[GRID_SIZE]>(opponentGrid))) {
                        ostringstream winMsg;
                        winMsg << "\n*** Player " << currentPlayer << " wins! All of Player "
                               << opponent << "'s ships have been sunk! ***";
                        typewrite(winMsg.str(), 10);
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
                typewrite("Invalid input, please enter 1 (Yes) or 2 (No)", 10);
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }

        } while (inputError);
    }

    typewrite("Thank you for playing Battleships!\n", 10);
    return 0;
}