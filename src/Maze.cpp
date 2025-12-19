#include "Maze.h"
#include <fstream>
#include <sstream>
#include <raylib.h>
#include <algorithm>
#include <tuple>
#include <random>
#include <ctime>


bool LoadMazeFromFile(Maze& maze, const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        TraceLog(LOG_ERROR, "Failed to open maze file!");
        return false;
    }

    string line;
    // 读取首行行列数
    if (getline(file, line)) {
        istringstream iss(line);
        if (!(iss >> maze.rows >> maze.cols)) {
            TraceLog(LOG_ERROR, "Maze file first line format error!");
            return false;
        }
    } else {
        TraceLog(LOG_ERROR, "Maze file is empty!");
        return false;
    }

    // 读取迷宫数据
    maze.data.resize(maze.rows, vector<int>(maze.cols));
    for (int i = 0; i < maze.rows; i++) {
        if (!getline(file, line)) {
            TraceLog(LOG_WARNING, "Maze data is incomplete!");
            break;
        }
        istringstream iss(line);
        for (int j = 0; j < maze.cols; j++) {
            iss >> maze.data[i][j];
        }
    }

    file.close();
    // 查找起点终点并生成各算法路径
    FindStartEnd(maze);
    DFSFindPath(maze);
    BFSFindPath(maze);
    DijkstraFindPath(maze);
    LavaDijkstraFindPath(maze);
    return true;
}

void LoadMazeTextures(Maze& maze, const string& imagePath) {
    maze.texStart = LoadTexture((imagePath + "start.png").c_str());
    maze.texEnd = LoadTexture((imagePath + "end.png").c_str());
    maze.texFloor = LoadTexture((imagePath + "floor.png").c_str());
    maze.texWall = LoadTexture((imagePath + "wall.png").c_str());
    maze.texGrass = LoadTexture((imagePath + "grass.png").c_str());
    maze.texLava = LoadTexture((imagePath + "lava.png").c_str());
}

void FindStartEnd(Maze& maze) {
    for (int i = 0; i < maze.rows; i++) {
        for (int j = 0; j < maze.cols; j++) {
            if (maze.data[i][j] == -1) maze.startPos = {j, i}; // (x,y)对应(col,row)
            if (maze.data[i][j] == -2) maze.endPos = {j, i};
        }
    }
}

// DFS路径查找（递归实现）
bool DFSHelper(Maze& maze, int x, int y, vector<vector<bool>>& visited, vector<pair<int, int>>& path) {
    // 越界、墙、已访问则返回false
    if (x < 0 || x >= maze.cols || y < 0 || y >= maze.rows || maze.data[y][x] == 1 || visited[y][x]) {
        return false;
    }

    visited[y][x] = true;
    path.push_back({x, y});

    // 到达终点
    if (x == maze.endPos.first && y == maze.endPos.second) {
        return true;
    }

    // 上下左右遍历
    if (DFSHelper(maze, x-1, y, visited, path) || DFSHelper(maze, x+1, y, visited, path) ||
        DFSHelper(maze, x, y-1, visited, path) || DFSHelper(maze, x, y+1, visited, path)) {
        return true;
    }

    // 回溯
    path.pop_back();
    return false;
}

void DFSFindPath(Maze& maze) {
    vector<vector<bool>> visited(maze.rows, vector<bool>(maze.cols, false));
    DFSHelper(maze, maze.startPos.first, maze.startPos.second, visited, maze.dfsPath);
}

// BFS路径查找
void BFSFindPath(Maze& maze) {
    vector<vector<bool>> visited(maze.rows, vector<bool>(maze.cols, false));
    vector<vector<pair<int, int>>> parent(maze.rows, vector<pair<int, int>>(maze.cols, {-1, -1}));
    queue<pair<int, int>> q;

    q.push(maze.startPos);
    visited[maze.startPos.second][maze.startPos.first] = true;

    // 方向数组：上下左右
    int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};

    while (!q.empty()) {
        auto curr = q.front();
        q.pop();

        // 到达终点
        if (curr == maze.endPos) {
            // 回溯父节点生成路径
            for (pair<int, int> p = curr; p != make_pair(-1, -1); p = parent[p.second][p.first]) {
                maze.bfsPath.push_back(p);
            }
            reverse(maze.bfsPath.begin(), maze.bfsPath.end());
            return;
        }

        // 遍历四个方向
        for (auto& dir : dirs) {
            int nx = curr.first + dir[0];
            int ny = curr.second + dir[1];
            if (nx >=0 && nx < maze.cols && ny >=0 && ny < maze.rows && maze.data[ny][nx] != 1 && !visited[ny][nx]) {
                visited[ny][nx] = true;
                parent[ny][nx] = curr;
                q.push({nx, ny});
            }
        }
    }
}

// Dijkstra最短路径（所有可走地块权重为1）
void DijkstraFindPath(Maze& maze) {
    // 距离数组，初始为无穷大
    vector<vector<int>> dist(maze.rows, vector<int>(maze.cols, INT_MAX));
    vector<vector<pair<int, int>>> parent(maze.rows, vector<pair<int, int>>(maze.cols, {-1, -1}));
    vector<vector<bool>> visited(maze.rows, vector<bool>(maze.cols, false));

    // 优先队列：(距离, x, y)，小顶堆
    priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>, greater<>> pq;

    int startX = maze.startPos.first;
    int startY = maze.startPos.second;
    dist[startY][startX] = 0;
    pq.push({0, {startX, startY}});

    int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};

    while (!pq.empty()) {
        auto curr = pq.top();
        pq.pop();
        int d = curr.first;
        int x = curr.second.first;
        int y = curr.second.second;

        if (visited[y][x]) continue;
        visited[y][x] = true;

        // 到达终点
        if (x == maze.endPos.first && y == maze.endPos.second) break;

        // 遍历四个方向
        for (auto& dir : dirs) {
            int nx = x + dir[0];
            int ny = y + dir[1];
            if (nx >=0 && nx < maze.cols && ny >=0 && ny < maze.rows && maze.data[ny][nx] != 1 && !visited[ny][nx]) {
                if (dist[ny][nx] > d + 1) {
                    dist[ny][nx] = d + 1;
                    parent[ny][nx] = {x, y};
                    pq.push({dist[ny][nx], {nx, ny}});
                }
            }
        }
    }

    // 回溯生成路径
    for (pair<int, int> p = maze.endPos; p != make_pair(-1, -1); p = parent[p.second][p.first]) {
        maze.dijkstraPath.push_back(p);
    }
    reverse(maze.dijkstraPath.begin(), maze.dijkstraPath.end());
}

// 绘制路径
void DrawPath(const Maze& maze, const vector<pair<int, int>>& path, Color color) {
    for (auto& p : path) {
        int x = p.first * maze.tileSize;
        int y = p.second * maze.tileSize;
        // 绘制半透明路径矩形覆盖在地块上
        DrawRectangle(x + 5, y + 5, maze.tileSize - 10, maze.tileSize - 10, Fade(color, 0.6f));
    }
}

void DrawMaze(const Maze& maze, PathAlgorithm currentAlgo) {
    for (int i = 0; i < maze.rows; i++) {
        for (int j = 0; j < maze.cols; j++) {
            int x = j * maze.tileSize;
            int y = i * maze.tileSize;
            Rectangle src = { 0, 0, (float)maze.tileSize, (float)maze.tileSize };
            Rectangle dst = { (float)x, (float)y, (float)maze.tileSize, (float)maze.tileSize };

            switch (maze.data[i][j]) {
                case -1: DrawTexturePro(maze.texStart, src, dst, {0,0}, 0, WHITE); break;
                case -2: DrawTexturePro(maze.texEnd, src, dst, {0,0}, 0, WHITE); break;
                case 0: DrawTexturePro(maze.texFloor, src, dst, {0,0}, 0, WHITE); break;
                case 1: DrawTexturePro(maze.texWall, src, dst, {0,0}, 0, WHITE); break;
                case 2: DrawTexturePro(maze.texGrass, src, dst, {0,0}, 0, WHITE); break;
                case 3: DrawTexturePro(maze.texLava, src, dst, {0,0}, 0, WHITE); break;
                default: DrawRectangle(x, y, maze.tileSize, maze.tileSize, GRAY); break;
            }
        }
    }

    // 根据当前算法绘制对应路径
    if (currentAlgo == PathAlgorithm::DFS && !maze.dfsPath.empty()) {
        DrawPath(maze, maze.dfsPath, RED);
    } else if (currentAlgo == PathAlgorithm::BFS && !maze.bfsPath.empty()) {
        DrawPath(maze, maze.bfsPath, BLUE);
    } else if (currentAlgo == PathAlgorithm::DIJKSTRA && !maze.dijkstraPath.empty()) {
        DrawPath(maze, maze.dijkstraPath, GREEN);
    } else if (currentAlgo == PathAlgorithm::LAVA_DIJKSTRA && !maze.lavaShortestPath.empty()) {
        DrawPath(maze, maze.lavaShortestPath, ORANGE);
    }

    // 绘制路径说明文字
    DrawPathInfo(currentAlgo);
}

// 绘制右上角路径说明
void DrawPathInfo(PathAlgorithm currentAlgo) {
    int x = GetScreenWidth() - 200;
    int y = 10;
    int fontSize = 14;

    if (currentAlgo == PathAlgorithm::DFS) {
        DrawText("Depth-First Search", x, y, fontSize, RED);
    } else if (currentAlgo == PathAlgorithm::BFS) {
        DrawText("Breadth-First Search", x, y, fontSize, BLUE);
    } else if (currentAlgo == PathAlgorithm::DIJKSTRA) {
        DrawText("Shortest Path (Dijkstra)", x, y, fontSize, GREEN);
    } else if (currentAlgo == PathAlgorithm::LAVA_DIJKSTRA) {
        DrawText("Lava Shortest Path", x, y, fontSize, ORANGE);
    } else {
        DrawText("Hide Path", x, y, fontSize, GRAY);
    }
}

void LavaDijkstraFindPath(Maze& maze) {
    // 方向数组：上下左右
    int dirs[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    int rows = maze.rows;
    int cols = maze.cols;

    // 距离数组：dist[y][x][usedLava]，初始化为无穷大
    const int INF = INT_MAX;
    vector<vector<vector<int>>> dist(rows, vector<vector<int>>(cols, vector<int>(2, INF)));
    // 父节点数组：记录每个状态的父节点 (px, py, pUsedLava)
    vector<vector<vector<tuple<int, int, int>>>> parent(rows, vector<vector<tuple<int, int, int>>>(cols, vector<tuple<int, int, int>>(2, {-1, -1, -1})));

    // 起点状态初始化
    int startX = maze.startPos.first;
    int startY = maze.startPos.second;
    dist[startY][startX][0] = 0;
    // 优先队列：(距离, x, y, usedLava)，小顶堆
    priority_queue<tuple<int, int, int, int>, vector<tuple<int, int, int, int>>, greater<tuple<int, int, int, int>>> pq;
    pq.push({0, startX, startY, 0});

    while (!pq.empty()) {
        auto [d, x, y, usedLava] = pq.top();
        pq.pop();

        // 到达终点，提前退出
        if (x == maze.endPos.first && y == maze.endPos.second) {
            break;
        }
        // 当前距离不是最短，跳过
        if (d > dist[y][x][usedLava]) {
            continue;
        }

        // 遍历四个方向
        for (auto& dir : dirs) {
            int nx = x + dir[0];
            int ny = y + dir[1];
            // 边界检测
            if (nx < 0 || nx >= cols || ny < 0 || ny >= rows) {
                continue;
            }
            // 墙不可走
            if (maze.data[ny][nx] == 1) {
                continue;
            }

            int newUsedLava = usedLava;
            // 判断是否是熔岩，且未使用过熔岩次数
            if (maze.data[ny][nx] == 3 && usedLava == 0) {
                newUsedLava = 1; // 踩熔岩，状态切换为1
            }
            // 已使用过熔岩次数，不能再踩熔岩
            else if (maze.data[ny][nx] == 3 && usedLava == 1) {
                continue;
            }

            // 新路径距离 = 当前距离 + 1（所有可走地块权重都是1）
            int newDist = d + 1;
            // 更新最短距离
            if (newDist < dist[ny][nx][newUsedLava]) {
                dist[ny][nx][newUsedLava] = newDist;
                parent[ny][nx][newUsedLava] = {x, y, usedLava};
                pq.push({newDist, nx, ny, newUsedLava});
            }
        }
    }

    // 回溯生成路径：选择到达终点的最优状态（usedLava=0 或 1 中距离更短的）
    int endX = maze.endPos.first;
    int endY = maze.endPos.second;
    int bestUsedLava = (dist[endY][endX][0] <= dist[endY][endX][1]) ? 0 : 1;
    if (dist[endY][endX][bestUsedLava] == INF) {
        TraceLog(LOG_WARNING, "No path found with one lava step allowed!");
        return;
    }

    // 从终点回溯到起点
    int currX = endX;
    int currY = endY;
    int currUsed = bestUsedLava;
    while (currX != -1 && currY != -1) {
        maze.lavaShortestPath.push_back({currX, currY});
        auto [px, py, pUsed] = parent[currY][currX][currUsed];
        currX = px;
        currY = py;
        currUsed = pUsed;
    }
    // 反转路径，得到起点到终点的顺序
    reverse(maze.lavaShortestPath.begin(), maze.lavaShortestPath.end());
}

void UnloadMazeTextures(const Maze& maze) {
    UnloadTexture(maze.texStart);
    UnloadTexture(maze.texEnd);
    UnloadTexture(maze.texFloor);
    UnloadTexture(maze.texWall);
    UnloadTexture(maze.texGrass);
    UnloadTexture(maze.texLava);
}


// 随机打乱方向数组
void ShuffleDirections(vector<pair<int, int>>& dirs) {
    static mt19937 rng((unsigned int)time(nullptr));
    shuffle(dirs.begin(), dirs.end(), rng);
}

// 递归回溯法生成迷宫核心函数
void RecursiveBacktracker(Maze& maze, int x, int y, vector<vector<bool>>& visited) {
    visited[y][x] = true;
    vector<pair<int, int>> dirs = {{-2, 0}, {2, 0}, {0, -2}, {0, 2}};
    ShuffleDirections(dirs);

    for (auto& dir : dirs) {
        int ny = y + dir.first;
        int nx = x + dir.second;
        // ========== 关键修改：限制 ny/nx 在 1 ~ rows-2 / cols-2 之间 ==========
        if (ny >= 1 && ny <= maze.rows-2 && nx >= 1 && nx <= maze.cols-2 && !visited[ny][nx]) {
            // 打通中间墙壁
            int wallY = y + dir.first / 2;
            int wallX = x + dir.second / 2;
            maze.data[wallY][wallX] = 0;
            maze.data[ny][nx] = 0;
            RecursiveBacktracker(maze, nx, ny, visited);
        }
    }
}

// 统计路径上的熔岩数量
int CountLavaOnPath(const Maze& maze, const vector<pair<int, int>>& path) {
    int lavaCount = 0;
    for (auto& p : path) {
        int x = p.first;
        int y = p.second;
        if (maze.data[y][x] == 3) {
            lavaCount++;
        }
    }
    return lavaCount;
}

// 生成随机迷宫入口函数
void GenerateRandomMaze(Maze& maze, int rows, int cols, int tileSize) {
    // 清空所有旧路径
    maze.dfsPath.clear();
    maze.bfsPath.clear();
    maze.dijkstraPath.clear();
    maze.lavaShortestPath.clear();

    maze.rows = rows;
    maze.cols = cols;
    maze.tileSize = tileSize;
    // 初始化全墙，包括外圈
    maze.data.resize(rows, vector<int>(cols, 1));

    mt19937 rng((unsigned int)time(nullptr));
    // 起点：内圈奇数坐标（1 ≤ x ≤ cols-2，1 ≤ y ≤ rows-2）
    uniform_int_distribution<int> oddDistX(0, (cols-3)/2);
    uniform_int_distribution<int> oddDistY(0, (rows-3)/2);
    int startX = 1 + oddDistX(rng) * 2;
    int startY = 1 + oddDistY(rng) * 2;
    maze.data[startY][startX] = -1;
    maze.startPos = {startX, startY};

    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    RecursiveBacktracker(maze, startX, startY, visited);

    // 终点：内圈边缘奇数坐标（y=1 / y=rows-2 / x=1 / x=cols-2）
    vector<pair<int, int>> edgePoints;
    // 上下内边缘（y=1 和 y=rows-2）
    for (int x = 1; x <= cols-2; x += 2) {
        edgePoints.emplace_back(x, 1);
        edgePoints.emplace_back(x, rows-2);
    }
    // 左右内边缘（x=1 和 x=cols-2）
    for (int y = 3; y <= rows-4; y += 2) { // 跳过上下边缘已添加的点
        edgePoints.emplace_back(1, y);
        edgePoints.emplace_back(cols-2, y);
    }
    // 随机选一个内边缘点作为终点
    uniform_int_distribution<int> edgeDist(0, edgePoints.size()-1);
    auto endPos = edgePoints[edgeDist(rng)];
    maze.data[endPos.second][endPos.first] = -2;
    maze.endPos = endPos;

    // 草地、熔岩生成
    // 1. 收集所有通路格子（类型0）
    vector<pair<int, int>> pathTiles;
    for (int y = 0; y < maze.rows; y++) {
        for (int x = 0; x < maze.cols; x++) {
            if (maze.data[y][x] == 0) {
                pathTiles.emplace_back(x, y);
            }
        }
    }

    // 2. 定义生成比例
    float grassRatio = 0.15f;
    float lavaRatio = 0.05f;
    int grassCount = (int)(pathTiles.size() * grassRatio);
    int lavaCountTarget = (int)(pathTiles.size() * lavaRatio);

    // 3. 生成草地（无需校验）
    shuffle(pathTiles.begin(), pathTiles.end(), rng);
    for (int i = 0; i < grassCount && i < pathTiles.size(); i++) {
        auto [x, y] = pathTiles[i];
        maze.data[y][x] = 2;
    }

    // 4. 生成熔岩：循环校验，确保最短路径（BFS）上熔岩数 ≤1
    vector<pair<int, int>> lavaCandidates;
    for (int i = grassCount; i < pathTiles.size(); i++) {
        lavaCandidates.push_back(pathTiles[i]);
    }

    int maxRetry = 10; // 最大重试次数，避免死循环
    while (maxRetry-- > 0) {
        // 临时生成熔岩
        shuffle(lavaCandidates.begin(), lavaCandidates.end(), rng);
        for (int i = 0; i < maze.rows; i++) {
            for (int j = 0; j < maze.cols; j++) {
                if (maze.data[i][j] == 3) maze.data[i][j] = 0; // 清空旧熔岩
            }
        }
        for (int i = 0; i < lavaCountTarget && i < lavaCandidates.size(); i++) {
            auto [x, y] = lavaCandidates[i];
            maze.data[y][x] = 3;
        }

        // 生成BFS路径并校验熔岩数
        BFSFindPath(maze); // 临时生成最短路径
        int pathLava = CountLavaOnPath(maze, maze.bfsPath);
        if (pathLava <= 1) {
            break; // 符合要求，退出循环
        }
    }
    maze.bfsPath.clear(); // 清空临时BFS路径，后续会重新生成

    // 5. 重新生成所有路径（此时路径上熔岩数≤1）
    DFSFindPath(maze);
    BFSFindPath(maze);
    DijkstraFindPath(maze);
    LavaDijkstraFindPath(maze);
}