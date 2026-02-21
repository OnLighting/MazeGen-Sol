#pragma once
#include"MyStl.h"
#include"Position.h"
enum class GenerateType {
	DFS,
	Prim,
	Kruskal
};
class MazeGenerator
{
	int rows, cols;
	int** grid;
	Position entrance;
	Position exit;
	GenerateType genType;


	void generateDFS();
	void generatePrim();
	void generateKruskal();
	void initGrid();
	void clearGrid();
public:
	string gridToJson()const;
	MazeGenerator();
	~MazeGenerator();
	void loadConfig(const string& json);

	MazeGenerator(int rows, int cols);
	bool isValid(int x, int y);
	void setEntrance(int x, int y);
	void setExit(int x, int y);
	void generate();
	int** getGrid()const { return grid; }
	int getRows() const { return rows; }
	int getCols() const { return cols; }
	Position getEntrance() const { return entrance; }
	Position getExit() const { return exit; }
};

