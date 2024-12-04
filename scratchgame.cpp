#include <iostream>
#include <thread>  // For sleep_for
#include <chrono>  // For milliseconds
#include <cstdlib> // For rand() and srand()
#include <ctime>   // For seeding rand()
#include <termios.h> // For Unix-based input handling
#include <unistd.h>  // For read()

using namespace std;

const int WIDTH = 20;
const int HEIGHT = 10;
char sea[HEIGHT][WIDTH];
int playerX = WIDTH / 2; // Player starts in the middle
int obstacleX, obstacleY;
bool gameOver = false;
int score = 0;

// Function to initialize the sea
void initializeSea() {
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            sea[i][j] = ' ';
        }
    }
    sea[HEIGHT - 1][playerX] = 'P'; // Player character
    obstacleY = 0;
    obstacleX = rand() % WIDTH; // Random position for the first obstacle
    sea[obstacleY][obstacleX] = 'O'; // Obstacle character
}

// Function to display the sea
void displaySea() {
    #ifdef _WIN32
        system("cls"); // Windows

    #endif

    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            cout << sea[i][j];
        }
        cout << endl;
    }
    cout << "Score: " << score << endl;
}

// Function to update the game
void updateSea() {
    // Move the obstacle down
    sea[obstacleY][obstacleX] = ' ';
    obstacleY++;
    if (obstacleY >= HEIGHT) {
        obstacleY = 0;
        obstacleX = rand() % WIDTH;
        score++;
    }
    sea[obstacleY][obstacleX] = 'O';

    // Check for collision
    if (obstacleY == HEIGHT - 1 && obstacleX == playerX) {
        gameOver = true;
    }

    // Update the player's position
    for (int j = 0; j < WIDTH; ++j) {
        sea[HEIGHT - 1][j] = ' ';
    }
    sea[HEIGHT - 1][playerX] = 'P';
}

// Non-blocking input handling (termios)
char getChar() {
    struct termios oldt, newt;
    char ch;
    tcgetattr(STDIN_FILENO, &oldt);          // Get current terminal attributes
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);        // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &newt); // Apply new attributes
    ch = getchar();                          // Read character
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // Restore old attributes
    return ch;
}

// Function to handle player input
void handleInput() {
    if (gameOver) return; // No input if the game is over

    char input;
    if (read(STDIN_FILENO, &input, 1) > 0) {
        if (input == 'a' && playerX > 0) { // Move left
            playerX--;
        } else if (input == 'd' && playerX < WIDTH - 1) { // Move right
            playerX++;
        }
    }
}

int main() {
    srand(static_cast<unsigned>(time(0))); // Seed random number generator
    initializeSea();

    while (!gameOver) {
        displaySea();
        handleInput();
        updateSea();
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // Delay for a smoother game experience
    }

    cout << "Game Over! Final Score: " << score << endl;
    return 0;
}