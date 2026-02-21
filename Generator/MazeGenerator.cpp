#include "MazeGenerator.h"
#include <string>
MazeGenerator::MazeGenerator() :rows(0), cols(0), grid(nullptr), genType(GenerateType::DFS), entrance(0, 0), exit(0, 0){}
MazeGenerator::MazeGenerator(int rows, int cols)
	:rows(rows), cols(cols),genType(GenerateType::DFS)
{
	grid = new int* [rows];
	for (int i = 0; i < rows; i++) {
		grid[i] = new int[cols];
	}
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1) {
				grid[i][j] = 1;
			}
			else {
				grid[i][j] = 0;
			}
		}
	}
	entrance = Position(0, 0);
	exit = Position(rows - 2, cols - 2);
	grid[entrance.x][entrance.y] = 0;
	grid[exit.x][exit.y] = 0;
	srand(time(0));
}
bool MazeGenerator::isValid(int x, int y)
{
	return x > 0 && y > 0 && x < rows - 1 && y < cols - 1;
}
void MazeGenerator::loadConfig(const string& json)
{
	int idx = 0;
	int len = json.length();

	auto skipNonDigit = [&]() {
		while (idx < len && (json[idx] < '0' || json[idx] > '9')) idx++;
		};

	auto readInt = [&]() -> int {
		skipNonDigit();
		int val = 0;
		bool found = false;
		while (idx < len && json[idx] >= '0' && json[idx] <= '9') {
			val = val * 10 + (json[idx] - '0');
			idx++;
			found = true;
		}
		return found ? val : -1;
		};
	int r = readInt();
	int c = readInt();
	if (r > 0 && c > 0) {
		this->rows = r;
		this->cols = c;
	}
	initGrid();
	size_t entIdx = json.find("\"entrance\"");
	if (entIdx != std::string::npos) {
		idx = entIdx + 10;
		int ex = readInt();
		int ey = readInt();
		this->entrance = Position(ex, ey);
	}
	else {
		this->entrance = Position(1, 1);
	}
	size_t extIdx = json.find("\"exit\"");
	if (extIdx != std::string::npos) {
		idx = extIdx + 6;
		int ex = readInt();
		int ey = readInt();
		this->exit = Position(ex, ey);
	}
	else {
		this->exit = Position(rows - 2, cols - 2);
	}
	if (json.find("\"generateType\":\"DFS\"") != string::npos)
		genType = GenerateType::DFS;
	else if (json.find("\"generateType\":\"Prim\"") != std::string::npos)
		genType = GenerateType::Prim;
	else if (json.find("\"generateType\":\"Kruskal\"") != std::string::npos)
		genType = GenerateType::Kruskal;
	else
		genType = GenerateType::DFS;
	if (entrance.x >= 0 && entrance.x < rows && entrance.y >= 0 && entrance.y < cols)
		grid[entrance.x][entrance.y] = 0;
	if (exit.x >= 0 && exit.x < rows && exit.y >= 0 && exit.y < cols)
		grid[exit.x][exit.y] = 0;
}
void MazeGenerator::setEntrance(int x, int y)
{
	if (isValid(x, y)) {
		grid[x][y] = 0;
		entrance = Position(x, y);
	}
}
void MazeGenerator::setExit(int x, int y)
{
	if (isValid(x, y)) {
		grid[x][y] = 0;
		exit = Position(x, y);
	}
}
string MazeGenerator::gridToJson()const
{
	string json = "{";

	json += "\"rows\":";
	json += to_string(rows);
	json += ",";

	json += "\"cols\":";
	json += to_string(cols);
	json += ",";
	json += "\"entrance\":[";
	json += to_string(entrance.x);
	json += ",";
	json += to_string(entrance.y);
	json += "],";
	json += "\"exit\":[";
	json += to_string(exit.x);
	json += ",";
	json += to_string(exit.y);
	json += "],";

	json += "\"grid\":[";
	for (int i = 0; i < rows; ++i) {
		json += "[";
		for (int j = 0; j < cols; ++j) {
			json += to_string(grid[i][j]);
			if (j != cols - 1)
				json += ",";
		}
		json += "]";
		if (i != rows - 1)
			json += ",";
	}
	json += "]";

	json += "}";
	return json;
}
void MazeGenerator::clearGrid()
{
	if (grid != nullptr) {
		for (int i = 0; i < rows; i++) {
			delete[] grid[i];
		}
		delete[] grid;
		grid = nullptr;
	}
}
void MazeGenerator::initGrid() {
	clearGrid();
	grid = new int* [rows];
	for (int i = 0; i < rows; i++) {
		grid[i] = new int[cols];
		for (int j = 0; j < cols; j++) {
			if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1)
				grid[i][j] = 1;
			else
				grid[i][j] = 0;
		}
	}
}
void MazeGenerator::generateDFS()
{
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			grid[i][j] = 1;
		}
	}

	MyStack<Position> stack;
	Position start(1, 1);
	grid[start.x][start.y] = 0;
	stack.push(start);

	while (!stack.empty()) {
		Position current = stack.top();
		MyVector<Position> neighbors;
		int directions[4][2] = { {-2, 0}, {2, 0}, {0, -2}, {0, 2} };
		for (int i = 0; i < 4; i++) {
			int nx = current.x + directions[i][0];
			int ny = current.y + directions[i][1];
			if (isValid(nx, ny) && grid[nx][ny] == 1) {
				neighbors.push_back(Position(nx, ny));
			}
		}
		if (!neighbors.empty()) {
			int randIndex = rand() % neighbors.size();
			Position next = neighbors[randIndex];
			int wallX = (current.x + next.x) / 2;
			int wallY = (current.y + next.y) / 2;
			grid[wallX][wallY] = 0;
			grid[next.x][next.y] = 0;

			stack.push(next);
		}
		else {
			stack.pop();
		}
	}

	grid[entrance.x][entrance.y] = 0;
	grid[exit.x][exit.y] = 0;
}
void MazeGenerator::generatePrim()
{
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
			grid[i][j] = 1;
	}
	struct Wall {
		int x, y;
		int px, py;
	};
	MyVector<Wall> walls;
	int sx = 1, sy = 1;
	grid[sx][sy] = 0;
	int dx[] = { 1, -1, 0, 0 };
	int dy[] = { 0, 0, 1, -1 };
	for (int i = 0; i < 4; i++) {
		int wx = sx + dx[i], wy = sy + dy[i];
		if (isValid(wx, wy))
			walls.push_back({ wx, wy, sx, sy });
	}
	while (!walls.empty()) {
		int rIdx = rand() % walls.size();
		Wall w = walls[rIdx];
		int nx = w.x + (w.x - w.px);
		int ny = w.y + (w.y - w.py);
		if (isValid(nx, ny) && grid[nx][ny] == 1) {
			grid[w.x][w.y] = 0;
			grid[nx][ny] = 0;
			for (int i = 0; i < 4; i++) {
				int nwx = nx + dx[i], nwy = ny + dy[i];
				if (isValid(nwx, nwy))
					walls.push_back({ nwx, nwy, nx, ny });
			}
		}
		walls[rIdx] = walls[walls.size() - 1];
		walls.pop_back();
	}
	grid[entrance.x][entrance.y] = 0;
	grid[exit.x][exit.y] = 0;
}
struct DUS {
	int* parent;
	DUS(int n) {
		parent = new int[n];
		for (int i = 0; i < n; i++) {
			parent[i] = i;
		}
	}
	~DUS() {
		delete[] parent;
	}
	int find(int x) {
		if (parent[x] == x)
			return x;
		return parent[x] = find(parent[x]);
	}
	void unite(int i, int j) {
		int rootI = find(i);
		int rootJ = find(j);
		if (rootI != rootJ) {
			parent[rootJ] = rootI;
		}
	}
};
void MazeGenerator::generateKruskal()
{
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
			grid[i][j] = 1;
	}
	struct Edge {
		int x, y;
		int r1, c1;
		int r2, c2;
	};
	MyVector<Edge> edges;

	for (int i = 1; i < rows - 1; i += 2) {
		for (int j = 1; j < cols - 1; j += 2) {
			grid[i][j] = 0;
			if (i + 2 < rows - 1)
				edges.push_back({ i + 1, j, i, j, i + 2, j });
			if (j + 2 < cols - 1)
				edges.push_back({ i, j + 1, i, j, i, j + 2 });
		}
	}
	for (int i = edges.size() - 1; i > 0; i--) {
		int j = rand() % (i + 1);
		Edge temp = edges[i];
		edges[i] = edges[j];
		edges[j] = temp;
	}
	DUS dus(rows * cols);
	for (int i = 0; i < edges.size(); i++) {
		Edge& e = edges[i];
		int u = e.r1 * cols + e.c1;
		int v = e.r2 * cols + e.c2;

		if (dus.find(u) != dus.find(v)) {
			grid[e.x][e.y] = 0;
			dus.unite(u, v);
		}
	}
	grid[entrance.x][entrance.y] = 0;
	grid[exit.x][exit.y] = 0;
}
void MazeGenerator::generate()
{
	switch (genType) {
	case GenerateType::DFS:
		generateDFS();
		break;
	case GenerateType::Prim:
		generatePrim();
		break;
	case GenerateType::Kruskal:
		generateKruskal();
		break;
	default:
		generateDFS();
		break;
	}
}
MazeGenerator::~MazeGenerator()
{
	clearGrid();
}