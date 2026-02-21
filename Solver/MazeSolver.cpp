#include "MazeSolver.h"
#include <sstream>
#include<string>
static const string solveTypeToStr(SolveType t)
{
	switch (t) {
	case SolveType::DFS:
		return "DFS";
	case SolveType::BFS:
		return "BFS";
	case SolveType::AStar:
		return "AStar";
	default:
		return "Unknown";
	}
}
string MazeSolver::resultToJson() const
{
	string json = "{";
	json += "\"rows\":"; json += to_string(rows); json += ",";
	json += "\"cols\":"; json += to_string(cols); json += ",";
	json += "\"entrance\":["; json += to_string(entrance.x); json += ",";
	json += to_string(entrance.y);   json += "],";
	json += "\"exit\":[";   json += to_string(exit.x);   json += ",";
	json += to_string(exit.y);     json += "],";
	json += "\"grid\":[";
	for (int i = 0; i < rows; ++i) {
		json += "[";
		for (int j = 0; j < cols; ++j) {
			json += to_string(grid[i][j]);
			if (j != cols - 1) json += ",";
		}
		json += "]";
		if (i != rows - 1) json += ",";
	}
	json += "],";
	json += "\"path\":[";
	for (int i = 0; i < path.size(); ++i) {
		json += "[";
		json += to_string(path[i].x);
		json += ",";
		json += to_string(path[i].y);
		json += "]";
		if (i != path.size() - 1) json += ",";
	}
	json += "],";
	json += "\"connected\":"; json += (is4Connected ? "4" : "8"); json += ",";
	json += "\"solveType\":\""; json += solveTypeToStr(solveType); json += "\"";

	json += "}";
	return json;
}
MazeSolver::MazeSolver(): rows(0), cols(0), is4Connected(true),solveType(SolveType::AStar)
{
	grid = nullptr;
	srand(time(0));
}
MazeSolver::~MazeSolver()
{
	if (grid != nullptr) {
		for (int i = 0; i < rows; i++) {
			delete[] grid[i];
		}
		delete[] grid;
		grid = nullptr;
	}
}
void MazeSolver::modifyCell(int x, int y, int value)
{
	if (isValid(x, y))
		grid[x][y] = value;
}
bool MazeSolver::isValid(int x, int y) const
{
	return x >= 0 && y >= 0 && x < rows && y < cols;
}
bool MazeSolver::isRoad(int x, int y) const
{
	return isValid(x, y) && grid[x][y] == 0;
}
void MazeSolver::setConnectivity(bool is4Connected)
{
	this->is4Connected = is4Connected;
}
MyVector<Position> MazeSolver::getNeighbors(const Position& pos) const
{
	MyVector<Position> neighbors;
	if (is4Connected)
	{
		int dx[] = { -1, 1, 0, 0 };
		int dy[] = { 0, 0, -1, 1 };
		for (int i = 0; i < 4; i++) {
			int nx = pos.x + dx[i];
			int ny = pos.y + dy[i];
			if (isValid(nx, ny)) {
				neighbors.push_back(Position(nx, ny));
			}
		}
	}
	else
	{
		for (int dx = -1; dx <= 1; dx++) {
			for (int dy = -1; dy <= 1; dy++) {
				if (dx == 0 && dy == 0) continue;
				int nx = pos.x + dx;
				int ny = pos.y + dy;
				if (isValid(nx, ny)) {
					neighbors.push_back(Position(nx, ny));
				}
			}
		}
	}
	return neighbors;
}

MyVector<Position> MazeSolver::solveDFS()
{
	bool** visited = new bool* [rows];
	Position** parent = new Position * [rows];
	MyStack<Position> stack;
	MyVector<Position> path;
	for (int i = 0; i < rows; i++) {
		visited[i] = new bool[cols];
		parent[i] = new Position[cols];
		for (int j = 0; j < cols; j++) {
			visited[i][j] = false;
			parent[i][j] = Position(-1, -1);
		}
	}
	stack.push(entrance);
	visited[entrance.x][entrance.y] = true;
	bool found = false;

	while (!stack.empty()) {
		Position current = stack.top();
		stack.pop();
		if (current.x == exit.x && current.y == exit.y) {
			found = true;
			break;
		}
		MyVector<Position> neighbors = getNeighbors(current);
		for (int i = neighbors.size() - 1; i > 0; i--) {
			int j = rand() % (i + 1);
			Position temp = neighbors[i];
			neighbors[i] = neighbors[j];
			neighbors[j] = temp;
		}
		for (int i = 0; i < neighbors.size(); i++) {
			Position neighbor = neighbors[i];
			if (isRoad(neighbor.x, neighbor.y) && !visited[neighbor.x][neighbor.y]) {
				visited[neighbor.x][neighbor.y] = true;
				parent[neighbor.x][neighbor.y] = current;
				stack.push(neighbor);
			}
		}
	}
	if (found) {
		Position curr = exit;
		MyVector<Position> tempPath;
		int safetyCounter = 0;
		int maxPossibleSteps = rows * cols;
		while (!(curr.x == entrance.x && curr.y == entrance.y) && safetyCounter < maxPossibleSteps) {
			tempPath.push_back(curr);
			Position p = parent[curr.x][curr.y];
			if (p.x == -1 || p.y == -1) {
				break;
			}
			curr = p;
			safetyCounter++;
		}
		if (curr.x == entrance.x && curr.y == entrance.y) {
			tempPath.push_back(entrance);
			for (int i = tempPath.size() - 1; i >= 0; i--) {
				path.push_back(tempPath[i]);
			}
		}
	}
	for (int i = 0; i < rows; i++) {
		delete[] visited[i];
		delete[] parent[i];
	}
	delete[] visited;
	delete[] parent;

	return path;
}
MyVector<Position> MazeSolver::solveBFS() {
	bool** visited = new bool* [rows];
	Position** parent = new Position * [rows];
	MyQueue<Position> queue;
	MyVector<Position> path;
	for (int i = 0; i < rows; i++) {
		visited[i] = new bool[cols];
		parent[i] = new Position[cols];
		for (int j = 0; j < cols; j++) {
			visited[i][j] = false;
		}
	}
	queue.push(entrance);
	visited[entrance.x][entrance.y] = true;
	while (!queue.empty()) {
		Position current = queue.front();
		queue.clear();
		if (current == exit) {
			Position p = exit;
			MyVector<Position> tempPath;
			while (!(p == entrance)) {
				tempPath.push_back(p);
				p = parent[p.x][p.y];
			}
			tempPath.push_back(entrance);
			for (int i = tempPath.size() - 1; i >= 0; i--) {
				path.push_back(tempPath[i]);
			}
			for (int i = 0; i < rows; i++) {
				delete[] visited[i];
				delete[] parent[i];
			}
			delete[] visited;
			delete[] parent;
			return path;
		}
		MyVector<Position> neighbors = getNeighbors(current);
		for (int i = 0; i < neighbors.size(); i++) {
			Position neighbor = neighbors[i];
			if (isRoad(neighbor.x, neighbor.y) && !visited[neighbor.x][neighbor.y]) {
				visited[neighbor.x][neighbor.y] = true;
				parent[neighbor.x][neighbor.y] = current;
				queue.push(neighbor);
			}
		}
	}
	for (int i = 0; i < rows; i++) {
		delete[] visited[i];
		delete[] parent[i];
	}
	delete[] visited;
	delete[] parent;
	return path;
}
MyVector<Position> MazeSolver::solveAStar() {
	struct AStarNode {
		Position pos;
		int g, f;
		AStarNode() : g(0), f(0) {}
		AStarNode(Position p, int g_v, int f_v) : pos(p), g(g_v), f(f_v) {}
		bool operator>(const AStarNode& other) const {
			return this->f > other.f;
		}
		bool operator<(const AStarNode& other) const {
			return this->f < other.f;
		}
	};
	auto getH = [&](const Position& p) {
		int dx = abs(p.x - exit.x);
		int dy = abs(p.y - exit.y);
		return is4Connected ? (dx + dy) : (dx > dy ? dx : dy);
		};
	int** gScore = new int* [rows];
	Position** parent = new Position * [rows];
	bool** closedSet = new bool* [rows];

	for (int i = 0; i < rows; i++) {
		gScore[i] = new int[cols];
		parent[i] = new Position[cols];
		closedSet[i] = new bool[cols];
		for (int j = 0; j < cols; j++) {
			gScore[i][j] = 2147483647;
			closedSet[i][j] = false;
			parent[i][j] = Position(-1, -1);
		}
	}
	MyPriorityQueue<AStarNode> openSet;
	MyVector<Position> path;
	gScore[entrance.x][entrance.y] = 0;
	openSet.push(AStarNode(entrance, 0, getH(entrance)));

	bool found = false;

	while (!openSet.empty()) {
		AStarNode current = openSet.top();
		openSet.pop();
		if (closedSet[current.pos.x][current.pos.y])
			continue;
		if (current.pos == exit) {
			found = true;
			break;
		}
		closedSet[current.pos.x][current.pos.y] = true;
		MyVector<Position> neighbors = getNeighbors(current.pos);
		for (int i = 0; i < neighbors.size(); i++) {
			Position next = neighbors[i];
			if (!isRoad(next.x, next.y) || closedSet[next.x][next.y])
				continue;
			int tentativeG = gScore[current.pos.x][current.pos.y] + 1;
			if (tentativeG < gScore[next.x][next.y]) {
				parent[next.x][next.y] = current.pos;
				gScore[next.x][next.y] = tentativeG;
				int f = tentativeG + getH(next);
				openSet.push(AStarNode(next, tentativeG, f));
			}
		}
	}
	if (found) {
		Position curr = exit;
		MyVector<Position> tempPath;
		while (!(curr == entrance) && isValid(curr.x, curr.y)) {
			tempPath.push_back(curr);
			curr = parent[curr.x][curr.y];
		}
		tempPath.push_back(entrance);
		for (int i = tempPath.size() - 1; i >= 0; i--) {
			path.push_back(tempPath[i]);
		}
	}
	for (int i = 0; i < rows; i++) {
		delete[] gScore[i];
		delete[] parent[i];
		delete[] closedSet[i];
	}
	delete[] gScore;
	delete[] parent;
	delete[] closedSet;
	return path;
}
void MazeSolver::solve()
{
	setConnectivity(is4Connected);
	switch (solveType) {
	case SolveType::DFS:
		path = solveDFS();
		break;
	case SolveType::BFS:
		path = solveBFS();
		break;
	case SolveType::AStar:
		path = solveAStar();
		break;
	}
}
void MazeSolver::loadConfig(const string& json)
{
	if (grid != nullptr) {
		for (int i = 0; i < this->rows; i++) {
			delete[] grid[i];
		}
		delete[] grid;
		grid = nullptr;
	}
	int idx = 0;
	int len = json.size();

	auto skipNonDigit = [&](void) {
		while (idx < len && (json[idx] < '0' || json[idx] > '9'))
			idx++;
		};

	auto readInt = [&](void) -> int {
		skipNonDigit();
		int val = 0;
		while (idx < len && json[idx] >= '0' && json[idx] <= '9') {
			val = val * 10 + (json[idx] - '0');
			idx++;
		}
		return val;
		};
	rows = readInt();
	cols = readInt();
	entrance.x = readInt();
	entrance.y = readInt();
	exit.x = readInt();
	exit.y = readInt();
	grid = new int* [rows];
	for (int i = 0; i < rows; ++i)
		grid[i] = new int[cols];
	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			grid[i][j] = readInt();
		}
	}
	if (json.find("\"solveType\":\"BFS\"") != string::npos)
		solveType = SolveType::BFS;
	else if (json.find("\"solveType\":\"AStar\"") != string::npos)
		solveType = SolveType::AStar;
	else
		solveType = SolveType::DFS;
	if (json.find("\"connected\":8") != string::npos)
		is4Connected = false;
	else
		is4Connected = true;
	if (entrance.x >= 0 && entrance.x < rows &&
		entrance.y >= 0 && entrance.y < cols) {
		grid[entrance.x][entrance.y] = 0;
	}

	if (exit.x >= 0 && exit.x < rows &&
		exit.y >= 0 && exit.y < cols) {
		grid[exit.x][exit.y] = 0;
	}
}