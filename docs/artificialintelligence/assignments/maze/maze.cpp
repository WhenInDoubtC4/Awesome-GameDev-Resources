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
  for (const vector<Node>& lines : maze)
  {
    for (const Node& node : lines)
    {
      cout << (node.wallVer ? '_' : ' ');
      cout << (node.wallHor ? '|' : ' ');
    }
    cout << '\n';
  }
}

vector<pair<Node&, bool&>> getVisitableNeighbors(vector<vector<Node>>& maze, int x, int y, int boundx, int boundy)
{
  vector<pair<Node&, bool&>> neighbors;

  //Up
  if (y > 0)
  {
    if (!maze[y - 1][x].isVisited) {
      printf("UP x=%i y=%i wall: VER x=%i, y=%i\n", x, y - 1, x, y -1);
      neighbors.emplace_back(maze[y - 1][x], maze[y - 1][x].wallVer);
    }
  }

  //Right
  if (x < boundx - 1)
  {
    if (!maze[y][x + 1].isVisited) {
      printf("RI x=%i y=%i wall: HOR x=%i, y=%i\n", x + 1, y, x, y);
      neighbors.emplace_back(maze[y][x + 1], maze[y][x].wallHor);
    }
  }

  //Down
  if (y < boundy - 1)
  {
    if (!maze[y + 1][x].isVisited) {
      printf("DN x=%i y=%i wall: VER x=%i, y=%i\n", x, y + 1, x, y);
      neighbors.emplace_back(maze[y + 1][x], maze[y][x].wallVer);
    }
  }

  //Left
  if (x > 0)
  {
    if (!maze[y][x - 1].isVisited) {
      printf("LE x=%i y=%i wall: HOR x=%i, y=%i\n0", x - 1, y, x - 1, y);
      neighbors.emplace_back(maze[y][x - 1], maze[y][x - 1].wallHor);
    }
  }

  return neighbors;
}

int main()
{
  uint32_t columns, lines, index;
  cin >> columns >> lines >> index;
  randomIndex = index;

  //Generate maze and init indices
  vector<vector<Node>> maze(lines, vector<Node>(columns));
  for (uint32_t y = 0; y < lines; y++)
  {
    for (uint32_t x = 0; x < columns; x++)
    {
      maze[y][x].x = x;
      maze[y][x].y = y;
    }
  }

  printMaze(maze, columns, lines);

  stack<Node*> nodeStack;

  //Add top cell to stack
  nodeStack.push(&maze[0][0]);

  while (!nodeStack.empty())
  {
    Node* topNode = nodeStack.top();

    printf("Top node: x=%i y=%i\n", topNode->x, topNode->y);

    //Get all visitable neighbors
    vector<pair<Node&, bool&>> neighbors = getVisitableNeighbors(maze, topNode->x, topNode->y, columns, lines);

    if (!neighbors.empty())
    {
      nodeStack.pop();

      //Mark the top cell as visited
      topNode->isVisited = true;

      //Choose a neighbor
      pair<Node&, bool&> selectedNeighbor = neighbors[generateRandom(neighbors.size())];

      //Remove the wall between cell and neighbors
      selectedNeighbor.second = false;

      //Add the neighbor to the stack
      nodeStack.push(&selectedNeighbor.first);
    }
    else
    {
      //Backtrack
      nodeStack.pop();
    }

    printMaze(maze, columns, lines);
  }

  cout << "Done\n";

  return 0;
}