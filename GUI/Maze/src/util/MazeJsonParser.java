package util;

import com.google.gson.JsonArray;
import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;
import model.Maze;
import model.Position;

import java.util.List;
import java.util.ArrayList;
public class MazeJsonParser {
    /**
     * json格式
     * {
     *   rows: int,
     *   cols: int,
     *   entrance: [x, y],
     *   exit: [x, y],
     *   grid: int[][],
     *   path?: [ [x,y], ... ],
     *   solveType?: "DFS" | "BFS" | "AStar",
     *   connected?: 4 | 8
     * }*/
    public static Maze parse(String json){
        if(json == null)
            throw new IllegalArgumentException("json is null");
        json = json.trim();
        if(json.isEmpty())
            throw new IllegalArgumentException("json is empty");
        JsonObject jsonObject = JsonParser.parseString(json).getAsJsonObject();
        Maze data = new Maze();
        data.rows = jsonObject.get("rows").getAsInt();
        data.cols = jsonObject.get("cols").getAsInt();
        data.entrance=readPos(jsonObject.getAsJsonArray("entrance"));
        data.exit=readPos(jsonObject.getAsJsonArray("exit"));
        JsonArray gridArr = jsonObject.getAsJsonArray("grid");
        data.grid = new int[data.rows][data.cols];
        for (int i = 0; i < data.rows; i++) {
            JsonArray rowArr = gridArr.get(i).getAsJsonArray();
            for (int j = 0; j < data.cols; j++) {
                data.grid[i][j] = rowArr.get(j).getAsInt();
            }
        }
        if (jsonObject.has("path") && !jsonObject.get("path").isJsonNull()) {
            JsonArray pathArr = jsonObject.getAsJsonArray("path");
            List<Position> path = new ArrayList<>(pathArr.size());
            for (JsonElement el : pathArr) {
                JsonArray p = el.getAsJsonArray();
                path.add(new Position(p.get(0).getAsInt(), p.get(1).getAsInt()));
            }
            data.path = path;
        } else {
            data.path = null;
        }
        data.solveType = jsonObject.has("solveType") ? jsonObject.get("solveType").getAsString() : null;
        data.connected = jsonObject.has("connected") ? jsonObject.get("connected").getAsInt() : 0;

        return data;
    }
    private static Position readPos(JsonArray array){
        return new Position(array.get(0).getAsInt(),array.get(1).getAsInt());
    }
}
