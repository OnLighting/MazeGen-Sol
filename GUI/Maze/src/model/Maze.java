package model;

import java.util.List;

public class Maze {
    public int rows;
    public int cols;
    public Position entrance;
    public Position exit;
    public  int[][] grid;
    public List<Position> path;
    public String solveType;
    public String generateType;
    public int connected;

}
