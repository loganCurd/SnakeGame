//
// Created by Logan on 6/7/2020.
//

#include "../include/SnakeGame.h"

SnakeGame::SnakeGame()
    : screen(new wchar_t[maxWidth * maxHeight]),
      hConsole(CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0,
                                         nullptr, CONSOLE_TEXTMODE_BUFFER,
                                         NULL)),
      dwBytesWritten(0) {
  SetConsoleActiveScreenBuffer(hConsole);

  // create empty screen
  for (int i = 0; i < maxWidth * maxHeight; ++i) {

    screen[i] = ' ';
  }

  screen[maxWidth * maxHeight - 1] = '\0';

  clearGameState();
  createSnake();
}

SnakeGame::~SnakeGame() {

  destroySnake();
  delete[] screen;
}

void SnakeGame::move() {

  // change for each coordinate
  int deltaY = direction == 0 ? -1 : direction == 1 ? 1 : 0;
  int deltaX = direction == 2 ? 1 : direction == 3 ? -1 : 0;

  // add new segment in desired direction
  snakePtr nextSegment = new SnakeSeg{snake->x + deltaX, snake->y + deltaY};

  nextSegment->next = snake;

  snake = nextSegment;

  //checks if snake is in bounds and then maps position to different coordinate depending on result
  snake->x =
      snake->x < 31 ? maxWidth - 31 : snake->x == maxWidth - 30 ? 31 : snake->x;
  snake->y =
      snake->y < 8 ? maxHeight - 4 : snake->y == maxHeight - 3 ? 8 : snake->y;

  // send pointer to last node in length of snake
  for (int i = 0; i < length; ++i) {

    nextSegment = nextSegment->next;
  }

  // if there is an excess node, delete it
  snakePtr endNode = nextSegment->next;
  nextSegment->next = nullptr;

  if (endNode != nullptr) {

    delete[] endNode;
  }
}

void SnakeGame::grow() {

  // increase length of snake
  length += 1;
}

void SnakeGame::updateGameState() {

  move();

  // get coordinates of head
  int headX = snake->x;
  int headY = snake->y;

  snakePtr snakeBody = snake->next;

  // check if snake has eaten apple
  if (headX == appleX && headY == appleY) {

    grow();
    moveApple();
  } else { // check if snake hit itself

    while (snakeBody != nullptr) {

      if (snakeBody->x == headX && snakeBody->y == headY) {

        alive = false;
      }

      snakeBody = snakeBody->next;
    }
  }
}

void SnakeGame::drawBoard() {

  // clears screen
  for (int i = 0; i < maxWidth * maxHeight; ++i) {

    screen[i] = ' ';
  }

  // draw borders
  for (int i = 0; i < gameWidth; ++i) {

    screen[7 * maxWidth + i + 30] = '*';
    screen[(7 + gameHeight) * maxWidth + i + 30] = '*';
  }
  for (int i = 0; i < gameHeight; ++i) {

    screen[maxWidth * (i + 7) + 30] = '*';
    screen[maxWidth * (i + 7) + 90] = '*';
  }

  // draws title and score
  for (int i = 0; i < header.length(); ++i) {

    screen[maxWidth * 5 + i + (60 - header.length() / 2)] = header[i];
  }

  score = std::to_string(getScore());

  for (int i = 0; i < scoreHeader.length(); ++i) {

    screen[maxWidth * 6 + i +
           (60 - scoreHeader.length() / 2 - score.length())] = scoreHeader[i];
  }

  for (int i = 0; i < score.length(); ++i) {

    screen[maxWidth * 6 + i + 60 + scoreHeader.length() / 2 + score.length()] =
        score[i];
  }

  // draw snake body
  for (snakePtr tempPointer = snake->next; tempPointer != nullptr;
       tempPointer = tempPointer->next) {

    screen[tempPointer->y * maxWidth + tempPointer->x] = alive ? 'O' : '*';
  }

  // draw snake head
  if (!alive) {

    screen[snake->y * maxWidth + snake->x] = 'X';
  } else if (direction <= 1) {

    screen[snake->y * maxWidth + snake->x] = direction == 0 ? '^' : 'v';
  } else {
    screen[snake->y * maxWidth + snake->x] = direction == 2 ? '>' : '<';
  }

  // draw apple
  screen[appleY * maxWidth + appleX] = '@';

  // draw gameOver
  if (!alive) {

    for (int i = 0; i < gameOverHeader.length(); ++i) {

      screen[maxWidth * maxHeight / 2 + 60 - gameOverHeader.length() / 2 + i] =
          gameOverHeader[i];
    }
  }

  WriteConsoleOutputCharacterW(hConsole, screen, maxWidth * maxHeight, {0, 0},
                               &dwBytesWritten);
}

int SnakeGame::getScore() { return length - startLength; }

void SnakeGame::start() {

  bool playGame = true;

  while (playGame) {

    drawBoard();

    while (alive) {

      getInput();
      updateGameState();
      drawBoard();
    }

    playGame = playAgain();

    clearGameState();
  }
}

void SnakeGame::clearGameState() {

  // set variables to default conditions
  alive = true;
  startX = maxWidth / 2;
  startY = maxHeight / 2;
  length = startLength;
  appleX = maxWidth / 2;
  appleY = maxHeight / 2 + 5;
  direction = defaultDirection;

  createSnake();
}

void SnakeGame::createSnake() {

  destroySnake();

  // create snake linked list
  snake = new SnakeSeg{startX, startY};

  snakePtr curPointer = snake;

  for (int x = 1; x < startLength; ++x) {

    curPointer->next = new SnakeSeg{startX, startY - x};
    curPointer = curPointer->next;
  }
}

void SnakeGame::destroySnake() {

  snakePtr curPointer = snake;

  snakePtr nextPointer;

  // delete all nodes in snake
  while (curPointer != nullptr) {

    nextPointer = curPointer->next;
    delete[] curPointer;
    curPointer = nextPointer;
  }
}

void SnakeGame::moveApple() {

  while (screen[appleY * maxWidth + appleX] != L' ') {

    appleX = rand() % gameWidth + 30;
    appleY = (rand() % (gameHeight - 3)) + 8;
  }
}

void SnakeGame::getInput() {

  auto t1 = std::chrono::system_clock::now();

  bool inputReceived = false;

  //changes delay speed depending on direction so game feels smoother in both directions
  std::chrono::milliseconds delay = direction > 1
                                        ? (std::chrono::milliseconds)50
                                        : (std::chrono::milliseconds)100;

  while ((std::chrono::system_clock::now() - t1) < delay) {

    if (GetKeyState(0x57) & 0x8000 && direction != 1 && !inputReceived) {
      // W key is down.
      direction = 0;
      inputReceived = true;
    }
    if (GetKeyState(0x53) & 0x8000 && direction != 0 && !inputReceived) {
      // S key is down.
      direction = 1;
      inputReceived = true;
    }
    if (GetKeyState(0x44) & 0x8000 && direction != 3 && !inputReceived) {
      // D key is down.
      direction = 2;
      inputReceived = true;
    }
    if (GetKeyState(0x41) & 0x8000 && direction != 2 && !inputReceived) {
      // A key is down.
      direction = 3;
      inputReceived = true;
    }
  }
}

bool SnakeGame::playAgain() {

  auto t1 = std::chrono::system_clock::now();

  std::chrono::milliseconds delay = (std::chrono::milliseconds)5000;

  while ((std::chrono::system_clock::now() - t1) < delay) {

    if (GetKeyState(VK_RETURN) & 0x8000) {

      return true;
    }
    if (GetKeyState(VK_ESCAPE) & 0x8000) {

      return false;
    }
  }

  return true;
}
