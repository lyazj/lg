#include "GLMaze.h"

#include <algorithm>

#include "Utils.h"

using namespace std;

namespace {

// Knuth shuffle.
void Shuffle(GLint a[], GLint n)
{
  for(GLint i = n - 1; i > 0; --i) {
    GLint j = RandInt(i + 1);
    std::swap(a[i], a[j]);
  }
}

// A queue that can rapidly pops a random element.
class RandomizedQueue {
public:
  void Push(GLint x) { data.push_back(x); }
  GLint Pop();
  bool IsEmpty() const { return data.empty(); }

private:
  vector<GLint> data;
};

GLint RandomizedQueue::Pop()
{
  GLint i = RandInt((GLint)data.size());
  GLint x = data[i];
  data[i] = data.back();
  data.pop_back();
  return x;
}

class DisjointSet {
public:
  DisjointSet(GLint n);
  GLint Find(GLint x);
  bool Union(GLint x, GLint y);

private:
  vector<GLint> parent;
  vector<GLint> rank;
};

DisjointSet::DisjointSet(GLint n) : parent(n), rank(n)
{
  for(GLint i = 0; i < n; ++i) parent[i] = i;
}

GLint DisjointSet::Find(GLint x)
{
  if(parent[x] == x) return x;
  return parent[x] = Find(parent[x]);
}

bool DisjointSet::Union(GLint x, GLint y)
{
  x = Find(x), y = Find(y);
  if(x == y) return false;
  if(rank[x] < rank[y]) {
    parent[x] = y;
  } else if(rank[x] > rank[y]) {
    parent[y] = x;
  } else {
    parent[y] = x;
    ++rank[x];
  }
  return true;
}

class GLMazeGenerator {
public:
  GLMazeGenerator(GLint &entry, GLint &exit, vector<vector<GLMazeCell>> &cells);

  static void GetEntry(GLint width, GLint height, GLint entry, GLint &x, GLint &y);
  static void GetExit(GLint w, GLint h, GLint e, GLint &x, GLint &y) { GetEntry(w, h, e, x, y); }

  void GenerateBacktracking();
  void GeneratePrim();
  void GenerateKruskal();

private:
  GLint width, height;
  GLint &entry, &exit;
  vector<vector<GLMazeCell>> &cells;
  vector<vector<bool>> visited;
  GLint depth;
  GLint xs, ys, xt, yt;

  void GenerateEntry();
  void GenerateExit();
  void GenerateBacktracking(GLint x, GLint y, GLint d);
};

void GLMazeGenerator::GetEntry(GLint width, GLint height, GLint entry, GLint &x, GLint &y)
{
  if(entry < width) {  // Top border.
    x = 0;
    y = entry;
  } else if(entry < 2 * width) {  // Bottom border.
    x = height - 1;
    y = entry - width;
  } else if(entry < 2 * width + height) {  // Left border.
    x = entry - 2 * width;
    y = 0;
  } else {  // Right border.
    x = entry - 2 * width - height;
    y = width - 1;
  }
}

GLMazeGenerator::GLMazeGenerator(GLint &en, GLint &ex, vector<vector<GLMazeCell>> &c)
    : width((GLint)c[0].size()),
      height((GLint)c.size()),
      entry(en),
      exit(ex),
      cells(c),
      depth(0),
      xs(-1),
      ys(-1),
      xt(-1),
      yt(-1)
{
  // empty
}

void GLMazeGenerator::GenerateBacktracking()
{
  GenerateEntry();
  visited.assign(height, vector<bool>(width, false));
  GenerateBacktracking(xs, ys, 0);
  GenerateExit();
}

// The entry is evenly drawn from the border.
// The four corner cells have a higher probability to be selected as entry.
void GLMazeGenerator::GenerateEntry()
{
  entry = RandInt(2 * (width + height));
  GetEntry(width, height, entry, xs, ys);
}

// Convert the exit cell to the corresponding border segment.
void GLMazeGenerator::GenerateExit()
{
  // Top-left corner.
  if(xt == 0 && yt == 0) {
    exit = 0;
    if(RandInt(2)) exit = 2 * width;
  }

  // Top-right corner.
  else if(xt == 0 && yt == width - 1) {
    exit = width - 1;
    if(RandInt(2)) exit = 2 * width + height;
  }

  // Bottom-left corner.
  else if(xt == height - 1 && yt == 0) {
    exit = width;
    if(RandInt(2)) exit = 2 * width + height - 1;
  }

  // Bottom-right corner.
  else if(xt == height - 1 && yt == width - 1) {
    exit = 2 * width - 1;
    if(RandInt(2)) exit = 2 * width + 2 * height - 1;
  }

  else if(xt == 0) {  // Top border.
    exit = yt;
  } else if(xt == height - 1) {  // Bottom border.
    exit = width + yt;
  } else if(yt == 0) {  // Left border.
    exit = 2 * width + xt;
  } else if(yt == width - 1) {  // Right border.
    exit = 2 * width + height + xt;
  }
}

// Recursive backtracking algorithm.
// The exit is selected as the farthest border cell from the entry.
// This likely generates very difficult mazes.
// Warning: Stack overflow possible for very large mazes.
void GLMazeGenerator::GenerateBacktracking(GLint x, GLint y, GLint d)
{
  visited[x][y] = true;
  if(x == 0 || x == height - 1 || y == 0 || y == width - 1) {
    if(d > depth) {
      depth = d;
      xt = x, yt = y;
    }
  }

  GLint nxs[4] = { x - 1, x + 1, x, x };
  GLint nys[4] = { y, y, y - 1, y + 1 };
  GLint nis[4] = { 0, 1, 2, 3 };
  Shuffle(nis, 4);  // Randomize the visiting order.

  for(GLint j = 0; j < 4; ++j) {
    GLint nx = nxs[nis[j]], ny = nys[nis[j]];
    if(nx < 0 || nx >= height || ny < 0 || ny >= width) continue;
    if(visited[nx][ny]) continue;
    switch(nis[j]) {
    case 0: cells[x][y].up = cells[nx][ny].down = true; break;
    case 1: cells[x][y].down = cells[nx][ny].up = true; break;
    case 2: cells[x][y].left = cells[nx][ny].right = true; break;
    case 3: cells[x][y].right = cells[nx][ny].left = true; break;
    }
    GenerateBacktracking(nx, ny, d + 1);
  }
}

// Vertex-oriented random spanning tree algorithm.
// The exit is selected as the farthest border cell from the entry.
// It's more like a randomized BFS, thus generates mazes with shorter paths.
void GLMazeGenerator::GeneratePrim()
{
  GenerateEntry();

  GLint dxs[4] = { -1, 1, 0, 0 };
  GLint dys[4] = { 0, 0, -1, 1 };
  vector<vector<GLint>> depths(height, vector<GLint>(width, -1));
  RandomizedQueue queue;

  depths[xs][ys] = 0;
  queue.Push(xs * width + ys);

  while(!queue.IsEmpty()) {
    GLint id = queue.Pop();  // a randomly selected cell
    GLint x = id / width, y = id % width;
    for(GLint i = 0; i < 4; ++i) {
      GLint nx = x + dxs[i], ny = y + dys[i];
      if(nx < 0 || nx >= height || ny < 0 || ny >= width) continue;
      if(depths[nx][ny] >= 0) continue;

      switch(i) {
      case 0: cells[x][y].up = cells[nx][ny].down = true; break;
      case 1: cells[x][y].down = cells[nx][ny].up = true; break;
      case 2: cells[x][y].left = cells[nx][ny].right = true; break;
      case 3: cells[x][y].right = cells[nx][ny].left = true; break;
      }
      depths[nx][ny] = depths[x][y] + 1;
      queue.Push(nx * width + ny);

      if(nx == 0 || nx == height - 1 || ny == 0 || ny == width - 1) {
        if(depths[nx][ny] > depth) {
          depth = depths[nx][ny];
          xt = nx, yt = ny;
        }
      }
    }
  }

  GenerateExit();
}

// Edge-oriented random spanning tree algorithm.
// As randomly as it is, it generates mazes with short paths.
void GLMazeGenerator::GenerateKruskal()
{
  vector<GLint> seq;
  seq.reserve(width * (height - 1) + (width - 1) * height);
  for(GLint i = 0; i < height - 1; ++i) {  // up-down
    for(GLint j = 0; j < width; ++j) seq.push_back(i * width + j);
  }
  for(GLint i = 0; i < height; ++i) {  // left-right
    for(GLint j = 0; j < width - 1; ++j) seq.push_back(i * width + j + width * height);
  }
  Shuffle(seq.data(), (GLint)seq.size());

  GLint nclass = width * height;
  DisjointSet djset(nclass);
  for(GLint i = 0; nclass != 1 && i < (GLint)seq.size(); ++i) {
    GLint id = seq[i];
    bool isLeftRight = id >= width * height;
    if(isLeftRight) id -= width * height;
    GLint x = id / width, y = id % width, nx = x + !isLeftRight, ny = y + isLeftRight;
    if(!djset.Union(x * width + y, nx * width + ny)) continue;
    --nclass;
    if(isLeftRight) {
      cells[x][y].right = cells[nx][ny].left = true;
    } else {
      cells[x][y].down = cells[nx][ny].up = true;
    }
  }

  // Make it entirely random.
  // I'm tired and don't want to find the longest path.
  GenerateEntry();
  exit = entry, xt = xs, yt = ys;
  do {
    GenerateEntry();
  } while(entry == exit);
}

}  // namespace

void GLMaze::GetNormalizedPosition(GLfloat x, GLfloat y, GLfloat &nx, GLfloat &ny) const
{
  nx = 1.8f * y / (GLfloat)width - 0.9f;
  ny = 0.9f - 1.8f * x / (GLfloat)height;
}

void GLMaze::GenerateVertices()
{
  // Originally: (width + 1) * height + width * (height + 1)
  // Entry and exit: 2
  // Connecting cells: width * height - 1
  // Remaining segments: width * height + width + height - 1
  vertices.reserve(2 * (width * height + width + height - 1));

  // Top border.
  for(GLint j = 0; j < width; ++j) {
    GLint id = j;
    if(id == entry || id == exit) continue;
    GenerateVertex(0, j);
    GenerateVertex(0, j + 1);
  }

  // Bottom border.
  for(GLint j = 0; j < width; ++j) {
    GLint id = width + j;
    if(id == entry || id == exit) continue;
    GenerateVertex(height, j);
    GenerateVertex(height, j + 1);
  }

  // Left border.
  for(GLint i = 0; i < height; ++i) {
    GLint id = 2 * width + i;
    if(id == entry || id == exit) continue;
    GenerateVertex(i, 0);
    GenerateVertex(i + 1, 0);
  }

  // Right border.
  for(GLint i = 0; i < height; ++i) {
    GLint id = 2 * width + height + i;
    if(id == entry || id == exit) continue;
    GenerateVertex(i, width);
    GenerateVertex(i + 1, width);
  }

  // Lower walls.
  for(GLint i = 0; i < height - 1; ++i) {
    for(GLint j = 0; j < width; ++j) {
      if(cells[i][j].down) continue;
      GenerateVertex(i + 1, j);
      GenerateVertex(i + 1, j + 1);
    }
  }

  // Right walls.
  for(GLint i = 0; i < height; ++i) {
    for(GLint j = 0; j < width - 1; ++j) {
      if(cells[i][j].right) continue;
      GenerateVertex(i, j + 1);
      GenerateVertex(i + 1, j + 1);
    }
  }
}

void GLMaze::GenerateVertex(GLint x, GLint y)
{
  GLfloat nx, ny;
  GetNormalizedPosition((GLfloat)x, (GLfloat)y, nx, ny);
  vertices.emplace_back(nx, ny);
}

GLMaze::GLMaze(GLint w, GLint h, GLMazeType type)
    : width(max<GLint>(1, w)),
      height(max<GLint>(1, h)),
      cells(height, vector<GLMazeCell>(width, { false, false, false, false }))
{
  switch(type) {
  case GLMazeType::Backtracking: GLMazeGenerator(entry, exit, cells).GenerateBacktracking(); break;
  case GLMazeType::Prim: GLMazeGenerator(entry, exit, cells).GeneratePrim(); break;
  case GLMazeType::Kruskal: GLMazeGenerator(entry, exit, cells).GenerateKruskal(); break;
  default: abort();
  }
  GenerateVertices();
}

GLMaze::~GLMaze()
{
  // empty
}

void GLMaze::GetEntry(GLint &x, GLint &y) const { GLMazeGenerator::GetEntry(width, height, entry, x, y); }

void GLMaze::GetExit(GLint &x, GLint &y) const { GLMazeGenerator::GetExit(width, height, exit, x, y); }

void GLMaze::IssueDraw() const { GLDrawArrays(GL_LINES, 0, (GLsizei)vertices.size()); }
