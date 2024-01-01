#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// Global constants used throughout the program
const char EMPTY = '.', HIT = 'X', MISS = '/';
const int BOARD_WIDTH = 9;
const int BOARD_HEIGHT = 9;

// ===============================================================================================
// ------------------------------------ Function prototypes --------------------------------------
// ===============================================================================================

// Checks if a coordinate is inside the grid
bool InBounds(int x, int y);

// Gets a random number between minVal and maxVal
int Random(int minVal, int maxVal);

// Checks if a ship of a certain size and rotation fits at a position on the board
bool CanFit(int x, int y, int size, bool horizontal, char board[BOARD_HEIGHT][BOARD_WIDTH]);

// Chooses a random position and places a ship (shipType is the character A - E)
// A size = 2
// B size = 3
// C size = 4
// D size = 4
// E size = 5
void PlaceShipRandom(char shipType, char board[BOARD_HEIGHT][BOARD_WIDTH]);

// Places ships on a board
void GenerateShipBoard(char board[BOARD_HEIGHT][BOARD_WIDTH]); // Matthew

void FillBoard(char board[BOARD_HEIGHT][BOARD_WIDTH], char value);

// Outputs a board with the row and column numbers
void ShowBoard(char board[BOARD_HEIGHT][BOARD_WIDTH]);

// Shows line of sunken ships
void ShowSunkShips(bool shipIsSunk[], int numShips); // Matthew

// Subprogram that either write highscore on txt file or displays it
void HighScoreFile(int score, string usrName, int choose); // Imran

void UpdateScore(int& score, int round, bool hit); // Imran

void ShowHowToPlay(); // Fares

void StartGame(); // Fares, Matthew

void ShowDivider();

// ===============================================================================================
// ---------------------------------- Main menu (main funcion) -----------------------------------
// ===============================================================================================

int main() {

	// Show title
	cout << " - BATTLESHIP GAME - " << endl;
	cout << "---------------------" << endl;

	while(1) {
		// Show the menu (Ruaa)
		cout << "Menu:" << endl;
		cout << "1: How to play:" << endl;
		cout << "2: Start game" << endl;
		cout << "3: Highscore table:" << endl;
		cout << "4: Quit" << endl;

		// Get menu input (Ruaa)
		bool validInput = false;
		do {
			cout << "Enter your choice: ";
			char guess;
			guess = cin.get();
			cin.ignore(9999, '\n'); //Ignores excessive inputs

			switch (guess) {
				case '1': ShowDivider(); ShowHowToPlay(); validInput = true;
					break;
				case '2': ShowDivider(); StartGame(); validInput = true;
					break;
				case '3': ShowDivider(); HighScoreFile(0, "", 2); validInput = true;
					break;
				case '4': return 0;
				default:
					cout << "Invalid input! Please enter 1, 2, 3 or 4" << endl;
			}
		} while (!validInput);
		ShowDivider();
	}
}

// ===============================================================================================
// ------------------------------------- Menu Functions ------------------------------------------
// ===============================================================================================

void ShowDivider() {
	cout << endl << "------------------------------------------------------------------------------" << endl;
}

void ShowHowToPlay() {
	string text;
	ifstream HTPFile;
	HTPFile.open("HowToPlay.txt");

	if (HTPFile.fail())
		cout << "ERROR: HowToPlay file could not be found or opened." << endl;
	else {
		while (getline(HTPFile, text))
			cout << text << endl;
	}

	HTPFile.close();
}

void HighScoreFile(int score, string usrName, int choose) {
	string lines;
	// write score and username to highscore file
	if (choose == 1) {
		ofstream writeFile;
		writeFile.open("Highscore.txt", ios_base::app); // ios_base::app   appends instead of overwrites

		if (writeFile.fail())
			cout << "ERROR: Highscore file could not be found or opened." << endl;
		else
			writeFile << "\n" + usrName + ": " << score << "Pts";
		writeFile.close();
	}
	// read and output highscore file
	else if (choose == 2) {
		ifstream readFromFile;
		readFromFile.open("Highscore.txt");

		if (readFromFile.fail())
			cout << "ERROR: Highscore file could not be found or opened." << endl;
		else {
			cout << "HIGHSCORES:";
			cout << "\n-----------------\n";
			while (getline(readFromFile, lines)) {
				cout << lines << endl;
			}
		}
		readFromFile.close();
	}
}

void UpdateScore(int &score, int round, bool hit) {
	if (hit)
		score += 1000 + (100 / round);              // incentivises them to win in less rounds           
	else
		score -= 500 + (100 / round);
}

// ===============================================================================================
// ------------------------------------------ Game Code ------------------------------------------
// ===============================================================================================

void StartGame() {
	char shipBoard[BOARD_HEIGHT][BOARD_WIDTH]; // show ship positions
	char guessBoard[BOARD_HEIGHT][BOARD_WIDTH]; // shows hits as 'X', shows misses as '/', and empty as '.'
	int coordx, coordy;
	int score = 0, round = 0;
	bool gameOver = false;
	const int NUM_SHIPS = 5;

	// Parallel arrays stores the ship data
	int shipDamage[NUM_SHIPS] = { 0, 0, 0, 0, 0 };	// array of ship hits
	int shipSizes[NUM_SHIPS] = { 2, 3, 3, 4, 5 };	// array of ship sizes
	bool shipIsSunk[NUM_SHIPS] = { 0, 0, 0, 0, 0 };	// array showing which ships have sunk

	// Fill boards
	FillBoard(guessBoard, EMPTY);
	GenerateShipBoard(shipBoard);

	// Game loop
	do {
		round++;

		// Show board and progress
		ShowBoard(guessBoard);
		ShowSunkShips(shipIsSunk, NUM_SHIPS);
		cout << "Score: " << score << endl;

		// get input (Fares)
		bool end;
		do {
			end = false;
			cout << "Enter the co-ordinates of attack (y, x) : ";
			cin >> coordy >> coordx;

			// Remove invalid inputs
			bool inputFailed = cin.fail();
			cin.clear();
			cin.ignore(9999, '\n');

			// Validate board positions
			if (inputFailed)
				cout << "Invalid input. Only number are accepted." << endl;
			else if (InBounds(--coordx, --coordy) == false)
				cout << "Invalid input. Out of the grid's bounds." << endl;
			else if (guessBoard[coordy][coordx] != EMPTY)
				cout << "You already fired at this position." << endl;
			else
				end = true;
		} while (!end);

		// Check for ships
		char ship = shipBoard[coordy][coordx];
		if (ship != EMPTY) {
			// Add damage to a ship and check if sunk
			switch (ship) {
				case 'A': shipIsSunk[0] = ++shipDamage[0] >= shipSizes[0]; break;
				case 'B': shipIsSunk[1] = ++shipDamage[1] >= shipSizes[1]; break;
				case 'C': shipIsSunk[2] = ++shipDamage[2] >= shipSizes[2]; break;
				case 'D': shipIsSunk[3] = ++shipDamage[3] >= shipSizes[3]; break;
				case 'E': shipIsSunk[4] = ++shipDamage[4] >= shipSizes[4]; break;
			}
			// Check if all ships are destroyed
			if (shipIsSunk[0] && shipIsSunk[1] && shipIsSunk[2] && shipIsSunk[3] && shipIsSunk[4]) {
				gameOver = true;
			}
			// Update board and score
			guessBoard[coordy][coordx] = HIT;
			UpdateScore(score, round, true);
		}
		else {
			// Update board and score
			guessBoard[coordy][coordx] = MISS;
			UpdateScore(score, round, false);
		}

	} while (!gameOver);

	// Show gameover screen
	ShowBoard(guessBoard);
	ShowSunkShips(shipIsSunk, NUM_SHIPS);
	cout << "All enemy ships are destroyed! You Win!" << endl << endl;
	cout << "Final Score: " << score << endl;

	// Player name entry
	string name;
	cout << "Enter your name: ";
	getline(cin, name);
	HighScoreFile(score, name, 1);
}

// ===============================================================================================
// ---------------------------------------- Game Functions ---------------------------------------
// ===============================================================================================

bool InBounds(int x, int y) {
	return ((x >= 0) && (x < BOARD_WIDTH) && (y >= 0) && (y < BOARD_HEIGHT));
}

int Random(int minVal, int maxVal) {
	return rand() % (maxVal - minVal + 1) + minVal;
}

bool CanFit(int x, int y, int size, bool horizontal, char board[BOARD_HEIGHT][BOARD_WIDTH]) {
	if (horizontal) {
		for (int i = 0; i < size; i++) {
			if (!InBounds(x + i, y) || board[y][x + i] != EMPTY)
				return false;
		}
	}
	else {
		for (int i = 0; i < size; i++) {
			if (!InBounds(x, y + i) || board[y + i][x] != EMPTY)
				return false;
		}
	}
	return true;
}

void PlaceShipRandom(char shipType, char board[BOARD_HEIGHT][BOARD_WIDTH]) {
	int shipX, shipY;
	int shipSize = 0;
	switch (shipType) {
		case 'A': shipSize = 2; break;
		case 'B': shipSize = 3; break;
		case 'C': shipSize = 3; break;
		case 'D': shipSize = 4; break;
		case 'E': shipSize = 5; break;
	}
	// Gets the random orientation of the ship
	bool isHorizontal = (rand() % 2) == 0;
	// Horizontal
	if (isHorizontal) {

		// get random position and checks if a ship can fit
		do {
			shipX = Random(0, BOARD_WIDTH - shipSize);
			shipY = Random(0, BOARD_HEIGHT - 1);
		} while (!CanFit(shipX, shipY, shipSize, isHorizontal, board));

		// Place ship "segemnts"
		for (int i = 0; i < shipSize; i++) {
			board[shipY][shipX + i] = shipType;
		}
	}
	// Vertical
	else {

		// get random position and checks if a ship can fit
		do {
			shipX = Random(0, BOARD_WIDTH - 1);
			shipY = Random(0, BOARD_HEIGHT - shipSize);
		} while (!CanFit(shipX, shipY, shipSize, isHorizontal, board));

		// Place ship "segemnts"
		for (int i = 0; i < shipSize; i++) {
			board[shipY + i][shipX] = shipType;
		}
	}
}

void FillBoard(char board[BOARD_HEIGHT][BOARD_WIDTH], char value) {
	for (int y = 0; y < BOARD_HEIGHT; y++) {
		for (int x = 0; x < BOARD_WIDTH; x++) {
			board[y][x] = value;
		}
	}
}

void GenerateShipBoard(char board[BOARD_HEIGHT][BOARD_WIDTH]) {
	srand(time(0));
	FillBoard(board, EMPTY);
	// Places ships (large ships are added first to make sure smaller ships can fit)
	// letter represents ship "type":
	// A size = 2
	// B size = 3
	// C size = 4
	// D size = 4
	// E size = 5
	PlaceShipRandom('E', board);
	PlaceShipRandom('D', board);
	PlaceShipRandom('C', board);
	PlaceShipRandom('B', board);
	PlaceShipRandom('A', board);
}

void ShowBoard(char board[BOARD_HEIGHT][BOARD_WIDTH]) {
	cout << endl;
	for (int y = -1; y < BOARD_HEIGHT; y++) {
		for (int x = -1; x < BOARD_WIDTH; x++) {
			if (y == -1)
				cout << ' ' << x + 1;
			else if (x == -1 && y != -1)
				cout << ' ' << y + 1;
			else if (x != -1 && y != -1)
				cout << ' ' << board[y][x];
		}
		cout << endl;
	}
}

void ShowSunkShips(bool shipIsSunk[], int numShips) {
	cout << "Sunk ships: ";
	char shipType = 'A';
	for (int ship = 0; ship < numShips; ship++) {
		if (shipIsSunk[ship]) {
			switch (shipType) {
				case 'A': cout << "    AA"; break;
				case 'B': cout << "    BBB"; break;
				case 'C': cout << "    CCC"; break;
				case 'D': cout << "    DDDD"; break;
				case 'E': cout << "    EEEEE"; break;
			}
		}
		shipType += 1;
	}
	cout << endl;
}