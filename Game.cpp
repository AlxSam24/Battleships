/**
 * Entry point for the Battleships game.
 * Manages the outer play-again loop, player-select menu, ship placement,
 * turn-based firing, and win detection for a two-player local game or a
 * single-player game against the computer AI.
 * @return 0 on normal exit
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