#include "GameManager.h"
#include <raylib.h>

int main(void) {
    GameManager game;

    // 初始化游戏
    if (!game.Init()) {
        TraceLog(LOG_ERROR, "Game initialization failed!");
        return 1;
    }

    // 运行游戏主循环
    game.Run();

    // 清理资源
    game.Cleanup();

    return 0;
}