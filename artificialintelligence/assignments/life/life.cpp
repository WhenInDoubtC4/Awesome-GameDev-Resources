#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

constexpr char DEAD_SIGN = '.';
constexpr char ALIVE_SIGN = '#';

vector<vector<bool>> board;

void printBoard()
{
  for (const vector<bool>& line : board)
  {
    for (const bool& point : line)
    {
      cout << (point ? ALIVE_SIGN : DEAD_SIGN);
    }
    cout << '\n';
  }
}

int clampIndex(int index, int limit)
{
  return index - floor(float(index) / float(limit)) * limit;
}

void execTurn(int columns, int lines)
{
  //Copy board
  vector<vector<bool>> nextBoard = board;

  for (int y = 0; y < lines; y++)
  {
    for (int x = 0; x < columns; x++)
    {
      //Get neighbors
      int liveNeighbors = 0;
      for (int dy = -1; dy <= 1; dy++)
      {
        for (int dx = -1; dx <= 1; dx++)
        {
          if (dx == 0 && dy == 0) continue;

          liveNeighbors += board[clampIndex(y + dy, lines)][clampIndex(x + dx, columns)];
        }
      }

      if (board[y][x])
      {
        //Any live cell with fewer than two live neighbours dies, as if by underpopulation.
        if (liveNeighbors < 2) nextBoard[y][x] = false;

        //Any live cell with two or three live neighbours lives on to the next generation. (do nothing)

        //Any live cell with more than three live neighbours dies, as if by overpopulation.
        else if (liveNeighbors > 3) nextBoard[y][x] = false;
      }
      //Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
      else if (liveNeighbors == 3) nextBoard[y][x] = true;
    }
  }

  //Swap boards
  board = nextBoard;
}

int main()
{
  //Input numbers
  int columns, lines, turns;
  cin >> columns >> lines >> turns;

  //Create board
  board.resize(lines);
  for (int i = 0; i < lines; i++) board[i].resize(columns);

  //Input board
  for (int i = 0; i < lines; i++)
  {
    string line;
    cin >> line;
    for (int j = 0; j < columns; j++)
    {
      board[i][j] = line[j] == ALIVE_SIGN;
    }
  }

  for (int i = 0; i < turns; i++) execTurn(columns, lines);

  printBoard();
}