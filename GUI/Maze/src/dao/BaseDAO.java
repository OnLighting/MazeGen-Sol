package dao;

import java.sql.*;
import java.util.*;

public class BaseDAO {

//    private static final String DB_URL = "jdbc:sqlite:maze.db";
//    private static final String USER = "";
//    private static final String PASSWORD = "";

    private static volatile boolean tablesInitialized = false;

    public static Connection getConnection() throws SQLException {
        try {
            Class.forName("org.sqlite.JDBC");
        } catch (ClassNotFoundException e) {
            throw new SQLException("SQLite JDBC 驱动未找到: " + e.getMessage());
        }
        String workDir = System.getProperty("user.dir");
        String dbFilePath = workDir + java.io.File.separator + "maze.db";
        String dynamicUrl = "jdbc:sqlite:" + dbFilePath;
        Connection conn = DriverManager.getConnection(dynamicUrl, "", "");
        try (Statement stmt = conn.createStatement()) {
            stmt.execute("PRAGMA foreign_keys = ON;");
        }
        if (!tablesInitialized) {
            synchronized (BaseDAO.class) {
                if (!tablesInitialized) {
                    createTables(conn);
                    tablesInitialized = true;
                }
            }
        }
        return conn;
    }
    /**
     * 创建数据表
     * maze_base,maze_solve
     */
    private static void createTables(Connection conn) throws SQLException {
        String createMazeBase = """
            CREATE TABLE IF NOT EXISTS maze_base (
                maze_id        INTEGER PRIMARY KEY AUTOINCREMENT,
                row_count      INTEGER NOT NULL,
                col_count      INTEGER NOT NULL,
                entrance_x     INTEGER NOT NULL,
                entrance_y     INTEGER NOT NULL,
                exit_x         INTEGER NOT NULL,
                exit_y         INTEGER NOT NULL,
                grid_json      TEXT    NOT NULL,
                generate_type  TEXT    NOT NULL,
                create_time    TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            );
        """;

        String createMazeSolve = """
            CREATE TABLE IF NOT EXISTS maze_solve (
                solve_id       INTEGER PRIMARY KEY AUTOINCREMENT,
                maze_id        INTEGER NOT NULL,
                solve_type     TEXT    NOT NULL,
                connected      INTEGER NOT NULL,
                path_json      TEXT,
                path_length    INTEGER NOT NULL,
                solve_time_ms  INTEGER,
                create_time    TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                FOREIGN KEY (maze_id) REFERENCES maze_base(maze_id) ON DELETE CASCADE
            );
        """;

        try (Statement stmt = conn.createStatement()) {
            stmt.execute(createMazeBase);
            stmt.execute(createMazeSolve);
        }
    }

    /**
     * 插入迷宫基本信息，返回自动生成的 maze_id
     */
    public static long insertMazeBase(int rowCount, int colCount,
                                      int entranceX, int entranceY,
                                      int exitX, int exitY,
                                      String gridJson, String generateType) throws SQLException {
        String sql = """
            INSERT INTO maze_base
            (row_count, col_count, entrance_x, entrance_y, exit_x, exit_y, grid_json, generate_type)
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)
        """;

        try (Connection conn = getConnection();
             PreparedStatement ps = conn.prepareStatement(sql,Statement.RETURN_GENERATED_KEYS)) {

            ps.setInt(1, rowCount);
            ps.setInt(2, colCount);
            ps.setInt(3, entranceX);
            ps.setInt(4, entranceY);
            ps.setInt(5, exitX);
            ps.setInt(6, exitY);
            ps.setString(7, gridJson);
            ps.setString(8, generateType);

            int affectedRows = ps.executeUpdate();
            if (affectedRows == 0) {
                throw new SQLException("插入 maze_base 失败");
            }
            try (ResultSet rs = ps.getGeneratedKeys()) {
                if (rs.next()) {
                    return rs.getLong(1);
                }
            }
        }
        throw new SQLException("插入 maze_base 失败，无法获取生成的主键");
    }

    /**
     * 插入迷宫求解记录，返回自动生成的 solve_id
     */
    public static long insertMazeSolve(long mazeId, String solveType,
                                       int connected, String pathJson,
                                       int pathLength, Integer solveTimeMs) throws SQLException {
        String sql = """
            INSERT INTO maze_solve
            (maze_id, solve_type, connected, path_json, path_length, solve_time_ms)
            VALUES (?, ?, ?, ?, ?, ?)
        """;

        try (Connection conn = getConnection();
             PreparedStatement ps = conn.prepareStatement(sql,Statement.RETURN_GENERATED_KEYS)) {

            ps.setLong(1, mazeId);
            ps.setString(2, solveType);
            ps.setInt(3, connected);
            ps.setString(4, pathJson);
            ps.setInt(5, pathLength);

            if (solveTimeMs == null) {
                ps.setNull(6, Types.INTEGER);
            } else {
                ps.setInt(6, solveTimeMs);
            }

            int affectedRows = ps.executeUpdate();
            if (affectedRows == 0) {
                throw new SQLException("插入 maze_solve 失败");
            }
            try (ResultSet rs = ps.getGeneratedKeys()) {
                if (rs.next()) {
                    return rs.getLong(1);
                }
            }
        }
        throw new SQLException("插入 maze_solve 失败，无法获取生成的主键");
    }
    /**
     * 获取迷宫ID与描述信息
     */
    public static Map<Long,String> getMazeInfo() throws SQLException {
        Map<Long,String> infos = new LinkedHashMap<>();
        String sql = "SELECT maze_id, generate_type FROM maze_base ORDER BY maze_id ASC";
        try (Connection conn = getConnection();
        Statement stmt = conn.createStatement();
        ResultSet rs = stmt.executeQuery(sql)) {
            while (rs.next()) {
                infos.put(rs.getLong("maze_id"),rs.getString("generate_type"));
            }
        }
        return infos;
    }
    /**
     * 根据迷宫ID查询 grid_json
     */
    public static String getMazeJsonById(long inputId) throws SQLException {
        String sql = "SELECT grid_json FROM maze_base WHERE maze_id = ?";
        try (Connection conn = getConnection();
             PreparedStatement ps = conn.prepareStatement(sql)) {
            ps.setLong(1, inputId);
            try (ResultSet rs = ps.executeQuery()) {
                if (rs.next()) {
                    return rs.getString("grid_json");
                }
            }
        }
        return null;
    }
    /**
     * 重置数据表 maze_base,maze_solve
     * 保留 maze_base的前7条测试用例
     */
    public static void resetDatabase() throws SQLException {
        try(Connection conn = getConnection();
        Statement stmt = conn.createStatement()) {
            stmt.execute("DELETE FROM maze_base " +
                    "WHERE maze_id > 7;");
            stmt.execute("UPDATE sqlite_sequence SET seq = 7 " +
                    "WHERE name = 'maze_base';");
            stmt.execute("UPDATE sqlite_sequence SET seq = COALESCE((SELECT MAX(solve_id) FROM maze_solve), 0) " +
                    "WHERE name = 'maze_solve';");
        }
    }
}
