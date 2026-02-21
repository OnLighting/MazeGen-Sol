#pragma once
#include"MySTL.h"
#include"Position.h"
enum class SolveType {
    DFS,
    BFS,
    AStar
};
class MazeSolver
{
    int rows, cols;
    int** grid;
    Position entrance;
    Position exit;
    bool is4Connected;
    SolveType solveType;
    MyVector<Position> path;
    MyVector<Position> solveDFS();
    MyVector<Position> solveBFS();
    MyVector<Position> solveAStar();
public:
    MazeSolver();
    ~MazeSolver();
    void modifyCell(int x, int y, int value);
    bool isValid(int x, int y)const;
    bool isRoad(int x, int y)const;
    void setConnectivity(bool is4Connected);
    MyVector<Position> getNeighbors(const Position& pos)const;
    void solve();
    void loadConfig(const string& json);
    string resultToJson() const;
};