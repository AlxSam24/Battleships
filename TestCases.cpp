/**
 * tests.cpp
 * Google Test unit tests for Battleships game logic.
 * Tests cover: validation, grid initialisation, ship placement,
 * overlap detection, firing mechanics, and win condition.
 */
#include <gtest/gtest.h>
#include "main.cpp"

// ============================================================
// Helper: produce a clean initialised grid for each test
// ============================================================
static void makeGrid(char grid[GRID_SIZE][GRID_SIZE]) {
    initGrid(grid);
}

// ============================================================
// isValidRow
// ============================================================
TEST(IsValidRow, AcceptsMinBoundary) {
    EXPECT_TRUE(isValidRow(1));
}

TEST(IsValidRow, AcceptsMaxBoundary) {
    EXPECT_TRUE(isValidRow(10));
}

TEST(IsValidRow, AcceptsMidRange) {
    EXPECT_TRUE(isValidRow(5));
}

TEST(IsValidRow, RejectsZero) {
    EXPECT_FALSE(isValidRow(0));
}

TEST(IsValidRow, RejectsNegative) {
    EXPECT_FALSE(isValidRow(-1));
}

TEST(IsValidRow, RejectsAboveMax) {
    EXPECT_FALSE(isValidRow(11));
}

// ============================================================
// isValidCol
// ============================================================
TEST(IsValidCol, AcceptsA) {
    EXPECT_TRUE(isValidCol('A'));
}

TEST(IsValidCol, AcceptsJ) {
    EXPECT_TRUE(isValidCol('J'));
}

TEST(IsValidCol, AcceptsMidRange) {
    EXPECT_TRUE(isValidCol('E'));
}

TEST(IsValidCol, AcceptsLowercase) {
    EXPECT_TRUE(isValidCol('a'));
}

TEST(IsValidCol, RejectsK) {
    EXPECT_FALSE(isValidCol('K'));
}

TEST(IsValidCol, RejectsDigit) {
    EXPECT_FALSE(isValidCol('1'));
}

TEST(IsValidCol, RejectsSymbol) {
    EXPECT_FALSE(isValidCol('!'));
}

// ============================================================
// isValidCoordinate
// ============================================================
TEST(IsValidCoordinate, AcceptsValidCorner) {
    EXPECT_TRUE(isValidCoordinate('A', 1));
}

TEST(IsValidCoordinate, AcceptsValidOppositeCorner) {
    EXPECT_TRUE(isValidCoordinate('J', 10));
}

TEST(IsValidCoordinate, RejectsInvalidCol) {
    EXPECT_FALSE(isValidCoordinate('Z', 5));
}

TEST(IsValidCoordinate, RejectsInvalidRow) {
    EXPECT_FALSE(isValidCoordinate('A', 0));
}

TEST(IsValidCoordinate, RejectsBothInvalid) {
    EXPECT_FALSE(isValidCoordinate('Z', 11));
}

// ============================================================
// initGrid  (was the source of the random-characters bug)
// ============================================================
TEST(InitGrid, AllCellsAreWater) {
    char grid[GRID_SIZE][GRID_SIZE];
    // Intentionally leave uninitialised before calling initGrid
    initGrid(grid);
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            EXPECT_EQ(grid[r][c], '~')
                << "Cell [" << r << "][" << c << "] was not '~'";
}

// ============================================================
// shipFits
// ============================================================
TEST(ShipFits, HorizontalFitsExactly) {
    // Size-3 from H (index 7) → H I J — last index 9, fits
    EXPECT_TRUE(shipFits('H', 1, 3, true));
}

TEST(ShipFits, HorizontalDoesNotFit) {
    // Size-3 from I (index 8) → I J K — K is out of bounds
    EXPECT_FALSE(shipFits('I', 1, 3, true));
}

TEST(ShipFits, HorizontalSingleCellAlwaysFits) {
    EXPECT_TRUE(shipFits('J', 10, 1, true));
}

TEST(ShipFits, VerticalFitsExactly) {
    // Size-3 from row 8 → rows 8,9,10 — fits
    EXPECT_TRUE(shipFits('A', 8, 3, false));
}

TEST(ShipFits, VerticalDoesNotFit) {
    // Size-3 from row 9 → rows 9,10,11 — out of bounds
    EXPECT_FALSE(shipFits('A', 9, 3, false));
}

TEST(ShipFits, VerticalCarrierFitsFromRow1) {
    EXPECT_TRUE(shipFits('A', 1, 5, false));
}

TEST(ShipFits, VerticalCarrierDoesNotFitFromRow7) {
    // rows 7..11 — out of bounds
    EXPECT_FALSE(shipFits('A', 7, 5, false));
}

TEST(ShipFits, LowercaseColAccepted) {
    EXPECT_TRUE(shipFits('a', 1, 3, true));
}

// ============================================================
// noOverlap
// ============================================================
TEST(NoOverlap, EmptyGridAlwaysNoOverlap) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    EXPECT_TRUE(noOverlap(grid, 'A', 1, 5, true));
}

TEST(NoOverlap, DetectsDirectOverlap) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 3, true, 'C');
    // Placing another ship on the same cells
    EXPECT_FALSE(noOverlap(grid, 'A', 1, 3, true));
}

TEST(NoOverlap, DetectsPartialOverlapHorizontal) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 3, true, 'C'); // occupies A1 B1 C1
    // New ship starts at C1 — partial overlap
    EXPECT_FALSE(noOverlap(grid, 'C', 1, 2, true));
}

TEST(NoOverlap, DetectsOverlapCrossPattern) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 3, true, 'C');  // horizontal A1-C1
    // Vertical ship through B1
    EXPECT_FALSE(noOverlap(grid, 'B', 1, 3, false));
}

TEST(NoOverlap, AdjacentShipNotOverlap) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 3, true, 'C'); // A1 B1 C1
    // Adjacent row: A2 — no overlap
    EXPECT_TRUE(noOverlap(grid, 'A', 2, 3, true));
}

// ============================================================
// placeShip
// ============================================================
TEST(PlaceShip, HorizontalPlacesCorrectCells) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 3, true, 'C');
    EXPECT_EQ(grid[0][0], 'C'); // A1
    EXPECT_EQ(grid[0][1], 'C'); // B1
    EXPECT_EQ(grid[0][2], 'C'); // C1
    EXPECT_EQ(grid[0][3], '~'); // D1 untouched
}

TEST(PlaceShip, VerticalPlacesCorrectCells) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 3, false, 'B');
    EXPECT_EQ(grid[0][0], 'B'); // A1
    EXPECT_EQ(grid[1][0], 'B'); // A2
    EXPECT_EQ(grid[2][0], 'B'); // A3
    EXPECT_EQ(grid[3][0], '~'); // A4 untouched
}

TEST(PlaceShip, LowercaseColNormalisedCorrectly) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'a', 1, 2, true, 'D');
    EXPECT_EQ(grid[0][0], 'D');
    EXPECT_EQ(grid[0][1], 'D');
}

TEST(PlaceShip, SingleCellShip) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'E', 5, 1, true, 'S');
    EXPECT_EQ(grid[4][4], 'S');
    // Neighbours untouched
    EXPECT_EQ(grid[4][3], '~');
    EXPECT_EQ(grid[4][5], '~');
}

TEST(PlaceShip, DoesNotWriteOutsidePlacedCells) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 5, true, 'C');
    // Columns 5-9 in row 0 must still be water
    for (int c = 5; c < GRID_SIZE; c++)
        EXPECT_EQ(grid[0][c], '~') << "Column " << c << " should be untouched";
}

// ============================================================
// allShipsSunk
// ============================================================
TEST(AllShipsSunk, EmptyGridCountsAsSunk) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    EXPECT_TRUE(allShipsSunk(grid));
}

TEST(AllShipsSunk, GridWithShipNotSunk) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 3, true, 'C');
    EXPECT_FALSE(allShipsSunk(grid));
}

TEST(AllShipsSunk, AllShipsMarkedHit) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 3, true, 'C');
    // Mark all cells as hit
    grid[0][0] = 'X';
    grid[0][1] = 'X';
    grid[0][2] = 'X';
    EXPECT_TRUE(allShipsSunk(grid));
}

TEST(AllShipsSunk, PartiallyHitShipNotSunk) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 3, true, 'C');
    grid[0][0] = 'X'; // Only one cell hit
    EXPECT_FALSE(allShipsSunk(grid));
}

TEST(AllShipsSunk, MissCellsDoNotPreventSunk) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    // Place and fully hit a ship
    placeShip(grid, 'A', 1, 2, true, 'D');
    grid[0][0] = 'X';
    grid[0][1] = 'X';
    // Scatter some miss markers
    grid[5][5] = 'O';
    grid[9][9] = 'O';
    EXPECT_TRUE(allShipsSunk(grid));
}

// ============================================================
// Interaction between placeShip + noOverlap
// (regression for the "can place on existing ships" bug)
// ============================================================
TEST(PlacementRegression, CannotPlaceSecondShipOnFirst) {
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    placeShip(grid, 'A', 1, 5, true, 'C'); // Carrier A1-E1
    // Try placing Battleship starting at A1 — must report overlap
    EXPECT_FALSE(noOverlap(grid, 'A', 1, 4, true));
}

TEST(PlacementRegression, CannotPlaceOnPreviouslyFiredCell) {
    // After a hit, the cell becomes 'X'.  noOverlap should still reject it
    // because 'X' != '~'.
    char grid[GRID_SIZE][GRID_SIZE];
    makeGrid(grid);
    grid[0][0] = 'X'; // Simulate a hit cell
    EXPECT_FALSE(noOverlap(grid, 'A', 1, 1, true));
}

// ============================================================
// Tracking grid initialisation
// (regression for the "random characters in tracking grid" bug)
// ============================================================
TEST(TrackingGridRegression, TrackingGridMustBeInitialisedBeforeUse) {
    char trackingGrid[GRID_SIZE][GRID_SIZE];
    // Bug: forgetting initGrid() → garbage values
    initGrid(trackingGrid); // This MUST be called — test verifies it works
    for (int r = 0; r < GRID_SIZE; r++)
        for (int c = 0; c < GRID_SIZE; c++)
            EXPECT_EQ(trackingGrid[r][c], '~')
                << "Uninitialised tracking cell at [" << r << "][" << c << "]";
}

// ============================================================
// whoGoesFirst
// ============================================================
TEST(WhoGoesFirst, AlwaysReturnsOneOrTwo) {
    // Run many times to catch any out-of-range value
    for (int i = 0; i < 1000; i++) {
        int result = whoGoesFirst();
        EXPECT_TRUE(result == 1 || result == 2)
            << "whoGoesFirst() returned out-of-range value: " << result;
    }
}