# 3D Tic-Tac-Toe

A 3D implementation of Tic-Tac-Toe using OpenGL and FreeGLUT.

## Requirements

- OpenGL
- FreeGLUT
- C++ compiler with C++11 support

## Compilation

### Windows (using MinGW)

1. Install MinGW and MSYS
2. Install the required libraries:
   ```
   pacman -S mingw-w64-x86_64-freeglut
   ```

3. Compile the program:
   ```
   g++ -o tictactoe asst2.cpp -lfreeglut -lglu32 -lopengl32
   ```

### Linux

1. Install the required libraries:
   ```
   sudo apt-get install freeglut3-dev
   ```

2. Compile the program:
   ```
   g++ -o tictactoe asst2.cpp -lglut -lGLU -lGL
   ```

## Running the Game

Run the executable:
```
./tictactoe
```

## Controls

- Left Mouse Button: Rotate the camera
- Left Mouse Click: Place a piece
- '+' key: Zoom in
- '-' key: Zoom out
- 'r' key: Reset the game

## Game Rules

1. The game is played on a 3D board (3x3x3)
2. Players take turns placing their pieces (red for Player 1, blue for Player 2)
3. The first player to get three pieces in a row (horizontally, vertically, or diagonally) wins
4. The winning combination can be in any plane or along any diagonal
5. If all spaces are filled without a winner, the game is a draw

## Features

- 3D visualization with camera controls
- Colored game pieces (red for Player 1, blue for Player 2)
- Animation for winning combinations
- Clear display of current player and game status
- Multiple winning combinations across all planes and diagonals 