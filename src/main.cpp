#include "GameManager.h"
#include <raylib.h>

int main() {
    // ===== 模式1：生成随机迷宫 =====
    // 参数说明：图片路径, 迷宫文件路径（无用）, 是否随机, 行数, 列数
    GameManager game("../../resources/images/", "", true, 15, 15);

    // ===== 模式2：加载文件迷宫（注释上面一行，启用下面一行） =====
    // GameManager game("../../resources/images/", "../../resources/data/maze0.txt");

    if (!game.Init()) {
        TraceLog(LOG_ERROR, "Game initialization failed!");
        return -1;
    }
    game.Run();
    game.Cleanup();

    return 0;
}