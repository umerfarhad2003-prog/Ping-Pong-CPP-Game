#include <iostream>
#include <conio.h>  //For _kbhit() and _getch()
#include <unistd.h> // For usleep()
#include <windows.h> // For Windows-specific console functions
#include <fstream>    // For file handling
using namespace std;
// Game dimensions
const int WIDTH = 40;
const int HEIGHT = 20;
// Variables to store history
int highestScore = 0;
int lowestScore = 0;
// Function to set the cursor position
void SetCursorPosition(int x, int y) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD position = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hConsole, position);
}
// Function to hide the cursor for smoother experience
void HideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = FALSE; // Hide cursor
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}
// Function to save the history to a file
void saveHistory() {
    ofstream outputFile("history.txt");
    if (outputFile.is_open()) {
        outputFile << highestScore << " " << lowestScore;
        outputFile.close();
    }
}
// Function to load history from a file
void loadHistory() {
    ifstream inputFile("history.txt");
    if (inputFile.is_open()) {
        inputFile >> highestScore >> lowestScore;
        inputFile.close();
    }
}
// Function to draw the game board
void DrawBoard(int ballX, int ballY, int paddle1Y, int paddle2Y, int score1, int score2) {
    SetCursorPosition(0, 0); // Move cursor to top-left
    cout << "\t\t\t##########################################################\n";
    cout << " \n";
    cout << "\t\t\t\t\tWELCOME TO MY PING PONG GAME                         \n";
    cout << " \n";
    cout << "\t\t\t##########################################################\n";
    cout << "\n";
    cout << "Left Paddle: 'W' to Move Up, 'S' to Move Down." ;
    cout << "Right Paddle: 'I' to Move Up, 'K' to Move Down." << endl;
    // Draw game board
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                cout << "*"; // Boundary
            } else if (i == ballY && j == ballX) {
                cout << "O"; // Ball
            } else if (j == 2 && (i == paddle1Y || i == paddle1Y + 1 || i == paddle1Y + 2)) {
                cout << "|"; // Left paddle
            } else if (j == WIDTH - 2 && (i == paddle2Y || i == paddle2Y + 1 || i == paddle2Y + 2)) {
                cout << "|"; // Right paddle
            } else {
                cout << " "; // Empty space
            }
        }
        cout << endl;
    }
    cout << "Player 1: " << score1 << "   Player 2: " << score2 << endl;
    cout << "Press 'H' to view history, 'P' to pause, or 'Q' to quit." << endl;  // Show history, pause and quit prompt
}
// Function to handle ball movement and check for collisions
void MoveBall(int &ballX, int &ballY, int &ballDirX, int &ballDirY, int paddle1Y, int paddle2Y, int &score1, int &score2) {
    ballX += ballDirX;
    ballY += ballDirY;
    // Ball collision with top or bottom walls
    if (ballY == 1 || ballY == HEIGHT - 2) 
        ballDirY *= -1;
    // Ball collision with left paddle
    if (ballX == 3 && (ballY == paddle1Y || ballY == paddle1Y + 1 || ballY == paddle1Y + 2)) {
        ballDirX *= -1;
    }
    // Ball collision with right paddle
    if (ballX == WIDTH - 4 && (ballY == paddle2Y || ballY == paddle2Y + 1 || ballY == paddle2Y + 2)) {
        ballDirX *= -1;  
    }
    // Ball out of bounds (score)
    if (ballX == 1) {
        score2++; // Right player scores
        ballX = WIDTH / 2; ballY = HEIGHT / 2; // Reset ball position
        ballDirX = 1; ballDirY = 1; // Reset direction 
    } else if (ballX == WIDTH - 2) {
        score1++; // Left player scores
        ballX = WIDTH / 2; ballY = HEIGHT / 2; // Reset ball position
        ballDirX = -1; ballDirY = 1; // Reset direction
    }
}
// Function to update the highest and lowest score
void UpdateScoreHistory(int score1, int score2) {
    if (score1 > highestScore) highestScore = score1;
    if (score2 > highestScore) highestScore = score2;
    if (score1 < lowestScore || lowestScore == 0) lowestScore = score1;
    if (score2 < lowestScore || lowestScore == 0) lowestScore = score2;
}
// Function to handle paddle movement and controls
void HandleControls(int &paddle1Y, int &paddle2Y, bool &gameRunning, bool &gamePaused) {
    if (_kbhit()) {
        char ch = _getch();
        // Left paddle controls
        if (ch == 'w' && paddle1Y > 1) paddle1Y--;
        if (ch == 's' && paddle1Y < HEIGHT - 4) paddle1Y++;
        // Right paddle controls
        if (ch == 'i' && paddle2Y > 1) paddle2Y--;
        if (ch == 'k' && paddle2Y < HEIGHT - 4) paddle2Y++;
        // Quit game
        if (ch == 'q' || ch == 'Q') 
            gameRunning = false;
        // View history
        if (ch == 'h' || ch == 'H') {
            cout << "\nHighest Score: " << highestScore << "\n";
            cout << "Lowest Score: " << lowestScore << "\n";
            cout << "Press any key to return to the game.\n";
            _getch(); // Wait for user input to return
        }
        // Pause game
        if (ch == 'p' || ch == 'P') {
            gamePaused = true; // Pause game
        }
    }
}
// Function to handle game pause
void HandlePause(bool &gamePaused) {
    if (gamePaused) {
        SetCursorPosition(0, HEIGHT + 1);
        cout << "Game Paused. Press 'P' to resume..." << endl;
        while (gamePaused) {
            if (_kbhit()) {
                char ch = _getch();
                if (ch == 'p' || ch == 'P') {
                    gamePaused = false; // Resume game when 'P' is pressed
                }
            }
        }
    }
}
// Main game loop
int main() {
    // Ball position and movement direction
    int ballX = WIDTH / 2, ballY = HEIGHT / 2;
    int ballDirX = 1, ballDirY = 1;
    int paddle1Y = HEIGHT / 2 - 1;
    int paddle2Y = HEIGHT / 2 - 1;
    int score1 = 0, score2 = 0;
    bool gameRunning = true;
    bool gamePaused = false;
    loadHistory(); // Load history at the start
    HideCursor(); // Hide cursor for smooth rendering
    while (gameRunning) {
        HandlePause(gamePaused);
        DrawBoard(ballX, ballY, paddle1Y, paddle2Y, score1, score2);
        MoveBall(ballX, ballY, ballDirX, ballDirY, paddle1Y, paddle2Y, score1, score2);
        HandleControls(paddle1Y, paddle2Y, gameRunning, gamePaused);
        // Check if game is over (score limit)
        if (score1 == 10 || score2 == 10) {
            cout << "Game Over! ";
            cout << (score1 == 10 ? "Player 1 Wins!" : "Player 2 Wins!") << endl;
            cout << "Press R to restart or Q to quit: ";
            while (true) {
                char choice = _getch();
                if (choice == 'r' || choice == 'R') {
                    score1 = score2 = 0; // Reset scores
                    ballX = WIDTH / 2; ballY = HEIGHT / 2; // Reset ball
                    break;
                } else if (choice == 'q' || choice == 'Q') {
                    gameRunning = false;
                    break;
                }
            }
        }
        // Update score history
        UpdateScoreHistory(score1, score2);
        // Delay for ball movement
        usleep(40000);
    }
    // Save history before exiting
    saveHistory();
return 0;
}
