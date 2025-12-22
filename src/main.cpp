#include "GameManager.h"
#include <raylib.h>

int main() {
    // 构造时传入 maze0.txt 的路径（即使初始是随机模式）
    GameManager game("../../resources/images/", "../../resources/data/maze0.txt", true, 15, 15);
    if (!game.Init()) {
        TraceLog(LOG_ERROR, "Game initialization failed!");
        return -1;
    }
    game.Run();
    game.Cleanup();

    return 0;
}