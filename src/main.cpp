#include "GameManager.h"
#include <raylib.h>

int main(void) {
    GameManager game;

    if (!game.Init()) {
        TraceLog(LOG_ERROR, "Game initialization failed!");
        return 1;
    }

    game.Run();
    game.Cleanup();

    return 0;
}