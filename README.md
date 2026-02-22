# 迷宫生成与求解

一个基于C++的数据结构课程设计项目，实现了多种算法的迷宫生成与求解功能，并配有Java图形界面。

## 项目结构

```
迷宫生成与求解/
├── Generator/          # 迷宫生成模块 (C++)
│   ├── generate.cpp    # 主程序入口
│   ├── MazeGenerator.h # 迷宫生成器头文件
│   ├── MazeGenerator.cpp
│   ├── Position.h      # 位置类定义
│   └── MySTL.h         # 自定义STL容器
├── Solver/             # 迷宫求解模块 (C++)
│   ├── solve.cpp       # 主程序入口
│   ├── MazeSolver.h    # 迷宫求解器头文件
│   ├── MazeSolver.cpp
│   ├── Position.h      # 位置类定义
│   └── MySTL.h         # 自定义STL容器
└── GUI/                # 图形界面模块 (Java)
    └── Maze/           # JavaFX应用
        └── src/
            ├── model/  # 数据模型
            ├── view/   # 用户界面
            ├── dao/    # 数据访问层
            └── util/   # 工具类
```

## 功能特性

### 迷宫生成 (Generator)

支持三种经典迷宫生成算法：

- **DFS (深度优先搜索)**: 使用回溯法生成迷宫，生成的迷宫路径较长，分支较少
- **Prim (普里姆算法)**: 基于最小生成树的随机化版本，生成的迷宫分支较多
- **Kruskal (克鲁斯卡尔算法)**: 使用并查集数据结构实现，生成的迷宫较为均匀

**配置参数**:
- `rows`: 迷宫行数
- `cols`: 迷宫列数
- `entrance`: 入口坐标 [x, y]
- `exit`: 出口坐标 [x, y]
- `generateType`: 生成算法 ("DFS", "Prim", "Kruskal")

**输入/输出**: 通过标准输入接收JSON配置，输出生成的迷宫网格JSON

### 迷宫求解 (Solver)

支持三种路径搜索算法：

- **DFS (深度优先搜索)**: 不保证最短路径，内存占用较少
- **BFS (广度优先搜索)**: 保证找到最短路径
- **AStar (A*算法)**: 使用启发式搜索，在保证最优性的同时提高效率

**配置参数**:
- `rows`, `cols`: 迷宫尺寸
- `entrance`, `exit`: 起点和终点坐标
- `grid`: 迷宫网格数据 (0=通路, 1=墙壁)
- `solveType`: 求解算法 ("DFS", "BFS", "AStar")
- `is4Connected`: 连通方式 (4方向/8方向)

**输入/输出**: 通过标准输入接收包含迷宫数据的JSON，输出求解路径JSON

### 图形界面 (GUI)

基于JavaFX开发的可视化界面，提供：

- 迷宫参数配置
- 算法选择
- 迷宫可视化显示
- 生成/求解过程动画演示
- 历史记录保存

## 技术实现

### C++ 核心模块

- **自定义STL实现**: 项目包含 `MySTL.h`，实现了基本的数据结构
  - `MyVector<T>`: 动态数组
  - `MyStack<T>`: 栈结构
  - `MyQueue<T>`: 队列结构

- **数据结构**: 使用二维动态数组 `int** grid` 表示迷宫
  - `0` 表示可通行的路径
  - `1` 表示墙壁

### Java GUI 模块

- **框架**: JavaFX
- **架构**: MVC模式
  - Model: `Maze.java`, `Position.java`
  - View: `MazeFrame.java`
  - Controller: 集成在View中
  - DAO: SQLite数据库存储历史记录
  - Util: `ProcessUtil.java` (调用C++程序), `MazeJsonParser.java`

## 编译与运行

### C++ 模块

项目使用Visual Studio项目文件 (`.sln`, `.vcxproj`):

**Generator**:
```bash
cd Generator
# 使用Visual Studio打开Generate.sln进行编译
# 或使用MSBuild命令行
msbuild Generate.vcxproj /p:Configuration=Release
```

**Solver**:
```bash
cd Solver
msbuild Solve.vcxproj /p:Configuration=Release
```

### GUI 模块

```bash
cd GUI/Maze
# 使用JavaFX SDK编译运行
javac --module-path $PATH_TO_FX --add-modules javafx.controls src/**/*.java
java --module-path $PATH_TO_FX --add-modules javafx.controls --module Maze/src
```

或使用提供的打包脚本:
```bash
cd GUI
package_maze.bat
```

## 使用示例

### 命令行使用

**生成迷宫**:
```bash
echo '{"rows":21,"cols":21,"entrance":[1,1],"exit":[19,19],"generateType":"DFS"}' | Generate.exe
```

**求解迷宫**:
```bash
echo '{"rows":21,"cols":21,"grid":[[...]],"entrance":[1,1],"exit":[19,19],"solveType":"BFS","is4Connected":true}' | Solve.exe
```

### GUI使用

1. 运行打包好的应用程序或从IDE启动
2. 在界面中设置迷宫参数（行数、列数）
3. 选择生成算法并点击"生成迷宫"
4. 选择求解算法并点击"求解迷宫"
5. 可视化查看生成和求解过程

## 算法复杂度

### 生成算法

| 算法 | 时间复杂度 | 空间复杂度 | 特点 |
|------|-----------|-----------|------|
| DFS | O(rows × cols) | O(rows × cols) | 长路径，少分支 |
| Prim | O(rows × cols × log(rows × cols)) | O(rows × cols) | 多分支，较复杂 |
| Kruskal | O(rows × cols × α(rows × cols)) | O(rows × cols) | 均匀分布 |

### 求解算法

| 算法 | 时间复杂度 | 空间复杂度 | 最优性 |
|------|-----------|-----------|--------|
| DFS | O(rows × cols) | O(rows × cols) | 否 |
| BFS | O(rows × cols) | O(rows × cols) | 是 |
| A* | O(rows × cols log(rows × cols)) | O(rows × cols) | 是 |

## 依赖项

- **C++**: C++11或更高标准
- **Java**: Java 11+
- **JavaFX**: JavaFX SDK 11+
- **构建工具**: Visual Studio (Windows) 或 GCC/Clang (跨平台)
- **数据库**: SQLite (GUI历史记录功能)

## 开发环境

- Windows 10/11
- Visual Studio 2019+
- Java JDK 11+
- JavaFX SDK 11+

## 课程设计说明

本项目为数据结构课程设计，重点实践：
- 栈与队列的应用 (DFS/BFS)
- 优先队列的应用 (A*算法)
- 并查集的应用 (Kruskal算法)
- 图论算法 (最小生成树)
- 跨语言进程通信
- MVC架构设计

## 许可证

仅供学习交流使用。
