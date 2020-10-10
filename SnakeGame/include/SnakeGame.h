//
// Created by Logan on 6/7/2020.
//

#ifndef SNAKEGAME_SNAKEGAME_H
#define SNAKEGAME_SNAKEGAME_H

#include <chrono>
#include <string>
#include <windows.h>

class SnakeGame {

private:
  /**
   * Node to represent one unit of the snake
   */
  struct SnakeSeg {

    int x;
    int y;

    SnakeSeg *next = nullptr;
  };

  typedef SnakeSeg *snakePtr;

  // holds linked list that represents snake
  snakePtr snake = nullptr;

private:
  // board dimensions
  int maxWidth = 120;
  int maxHeight = 30;
  int gameWidth = 60;
  int gameHeight = 20;

  // Window variables
  wchar_t *screen;
  HANDLE hConsole;
  DWORD dwBytesWritten;
  std::string header = "Logan Curd - S N A K E  G A M E";
  std::string gameOverHeader = "PRESS ENTER TO PLAY AGAIN OR ESC TO EXIT";
  std::string scoreHeader = "Score: ";
  std::string score;

  // snake variables
  bool alive;
  int startX;
  int startY;
  int startLength = 5;
  int length;

  // apple variables
  int appleX;
  int appleY;

  // direction handling N=0, S=1, E=2, W=3
  int defaultDirection = 1;
  int direction;

private:
  /**
   * move() - moves snake in desired direction by creating node with correct
   * coordinates and deleting tail node if snake has not grown
   */
  void move();

  /**
   * grow() - increases the length of snake by 1
   */
  void grow();

  /**
   * updateGameState() - moves snake and checks for collisions to update score
   * or end game
   */
  void updateGameState();

  /**
   * drawBoard() - draws the entire board for each frame
   */
  void drawBoard();

  /**
   * getScore() - subtracts original length from current length to find total
   * number of apples eaten
   * @return user score
   */
  int getScore();

  /**
   * clearGameState() - resets variables to original state and creates a new
   * snake so that the user can play again
   */
  void clearGameState();

  /**
   * createSnake() - deletes old snake and then creates a new one in the default
   * position.
   */
  void createSnake();

  /**
   * destroySnake() - deletes each node in snakePtr linked list.
   */
  void destroySnake();

  /**
   * moveApple() - generates a random coordinate and assigns that coordinate to
   * apple position if the position is not occupied by another object
   */
  void moveApple();

  /**
   * getInput() - checks the wasd keys to check what has been pressed each
   * frame. Uses that information to update snake direction
   */
  void getInput();

  /**
   * playAgain() - checks to see if the user inputs enter or escape to determine
   * if game should be replayed
   * @return true if key pressed is return, else if escape is pressed returns
   * false
   */
  bool playAgain();

public:
  /**
   * Constructor - configures screen in preperation for game, creates snake and
   * sets game state
   */
  SnakeGame();

  /**
   * Destructor - deletes snake nodes and screen pointer
   */
  ~SnakeGame();

  /**
   * start() - game loop
   */
  void start();
};

#endif // UNTITLED_SNAKEGAME_H
