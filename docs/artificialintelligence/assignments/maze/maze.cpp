#include <iostream>
#include <cstdint>
#include <vector>
#include <stack>
#include <utility>

using namespace std;

//Copied form https://courses.tolstenko.net/artificialintelligence/assignments/maze/
constexpr int RANDOM_NUMBER_LIST[] = {72, 99, 56, 34, 43, 62, 31, 4, 70, 22, 6, 65, 96, 71, 29, 9, 98, 41, 90, 7, 30, 3, 97, 49, 63, 88, 47, 82, 91, 54, 74, 2, 86, 14, 58, 35, 89, 11, 10, 60, 28, 21, 52, 50, 55, 69, 76, 94, 23, 66, 15, 57, 44, 18, 67, 5, 24, 33, 77, 53, 51, 59, 20, 42, 80, 61, 1, 0, 38, 64, 45, 92, 46, 79, 93, 95, 37, 40, 83, 13, 12, 78, 75, 73, 84, 81, 8, 32, 27, 19, 87, 85, 16, 25, 17, 68, 26, 39, 48, 36};
int randomIndex = 0;

int generateRandom(int max)
{
  return RANDOM_NUMBER_LIST[randomIndex++] % max;
}

struct Node
{
  bool isVisited = false;

  int x = -1;
  int y = -1;

  bool wallVer = true;
  bool wallHor = true;
};

void printMaze(const vector<vector<Node>>& maze, int columns, int lines)
{
  //Top border
  for (int x = 0; x < columns; x++) cout << " _";
  cout << '\n';

  for (int x = 0; x < columns; x++)
  {
    //Left border
    cout << '|';
    for (int y = 0; y < lines; y++)
    {
      cout << (maze[x][y].wallVer ? '_' : ' ')
           << (maze[x][y].wallHor ? '|' : ' ');
    }

    cout << '\n';
  }
}

vector<pair<Node*, bool*>> getVisitableNeighbors(vector<vector<Node>>& maze, int x, int y, int boundx, int boundy)
{
  vector<pair<Node*, bool*>> neighbors;

  printf("x=%i, y=%i\n", x, y);

  //Up
  if (y > 0)
  {
    if (!maze[x][y - 1].isVisited) neighbors.emplace_back(&maze[x][y - 1], &maze[x][y - 1].wallVer);
  }
  cout << "up\n";

  //Right
  if (x < boundx - 1)
  {
    if (!maze[x + 1][y].isVisited) neighbors.emplace_back(&maze[x + 1][y], &maze[x][y].wallHor);
  }
  cout << "Right\n";

  //Down
  if (y < boundy - 1)
  {
    if (!maze[x][y + 1].isVisited) neighbors.emplace_back(&maze[x][y + 1], &maze[x][y].wallVer);
  }
  cout << "Down\n";

  //Left
  if (x > 0)
  {
    if (!maze[x - 1][x].isVisited) neighbors.emplace_back(&maze[x - 1][x], &maze[x - 1][y].wallHor);
  }
  cout << "Left\n";

  return neighbors;
}

int main()
{
  uint32_t columns, lines, index;
  cin >> columns >> lines >> index;
  randomIndex = index;

  //Generate maze and init indices
  vector<vector<Node>> maze(columns, vector<Node>(lines));
  for (uint32_t x = 0; x < columns; x++)
  {
    for (uint32_t y = 0; y < lines; y++)
    {
      maze[x][y].x = x;
      maze[x][y].y = y;
    }
  }

  printMaze(maze, columns, lines);

  stack<Node*> nodeStack;

  //Add top cell to stack
  nodeStack.push(&maze[0][0]);

  while (!nodeStack.empty())
  {
    Node* topNode = nodeStack.top();

    //Get all visitable neighbors
    vector<pair<Node*, bool*>> neighbors = getVisitableNeighbors(maze, topNode->x, topNode->y, columns, lines);

    if (!neighbors.empty())
    {
      //Mark the top cell as visited
      topNode->isVisited = true;

      //Choose a neighbor
      pair<Node*, bool*>& selectedNeighbor = neighbors[generateRandom(neighbors.size() - 1)];

      //Remove the wall between cell and neighbors
      *selectedNeighbor.second = false;

      //Add the neighbor to the stack
      nodeStack.push(selectedNeighbor.first);
    }
    else
    {
      //Backtrack
      nodeStack.pop();
    }

    printMaze(maze, columns, lines);
  }

  return 0;
}