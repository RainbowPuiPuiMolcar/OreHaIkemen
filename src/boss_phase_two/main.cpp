#include "AvoidGame.hpp"

int main(int argc, char* argv[]) {
    std::srand((unsigned)std::time(nullptr));
    AvoidGame game(640, 480, 100);

    if (!game.init("", "compiler/run/data/image/boss.png", "compiler/run/data/music/LETHAL_DEAL/LETHAL_DEAL_V2.wav")) {
        SDL_Log("SDL 初期化失敗: %s", SDL_GetError());
        return -1;
    }

    // プレイヤー用画像を指定（PNG 推奨）
    // if (!game.loadPlayerTexture("compiler/run/data/image/boss.png")) {
    //     SDL_Log("テクスチャ読み込み失敗: %s", SDL_GetError());
    //     // texture がない場合は矩形で描画されます
    // }

    game.run();
    return 0;
}
