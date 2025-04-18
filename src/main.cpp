#include <SDL2/SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <SDL_image.h>
#include "Camera2D.h"

// プレイヤーやオブジェクトの矩形
struct Rectangle {
    float x;
    float y;
    float Width;
    float Height;
};

// void playerRender(SDL_Renderer* renderer, SDL_Texture* texture, Camera &camera, SDL_Rect &rect) {
//     SDL_Rect destRect = { 100, 100, 50, 50 }; // 描画するオブジェクトの位置とサイズ

//     // カメラを適用
//     applyCamera(camera, destRect, rect);

//     // 画像を描画
//     SDL_RenderCopy(renderer, texture, NULL, &destRect);
// }

void drawText(SDL_Renderer* renderer, float R, float G, float B, TTF_Font* font, std::string Text, float x, float y)
{
    // テキストの色を指定
    SDL_Color color = {static_cast<Uint8>(R), static_cast<Uint8>(G), static_cast<Uint8>(B)};
    
    // テキストをサーフェスに変換
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, Text.c_str(), color);
    if (!surface) {
        std::cerr << "テキストのレンダリングに失敗しました: " << TTF_GetError() << std::endl;
        return;
    }

    // サーフェスからテクスチャを作成
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface); // サーフェスは不要になったので解放

    if (!texture) {
        std::cerr << "テクスチャの作成に失敗しました: " << SDL_GetError() << std::endl;
        return;
    }

    // テクスチャの描画位置とサイズを設定
    SDL_Rect dstRect = {static_cast<int>(x), static_cast<int>(y), surface->w, surface->h};

    // テクスチャをレンダラーに描画
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);

    // テクスチャを解放
    SDL_DestroyTexture(texture);
}

bool isKeyPressed(SDL_Event& event, SDL_Keycode key) {
    return (event.type == SDL_KEYDOWN && event.key.keysym.sym == key);
}

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // SDL_ttfの初期化
    if (TTF_Init() == -1) {
      std::cerr << "TTF_Init Error: " << TTF_GetError() << std::endl;
      SDL_Quit();
      return 1;
    }

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_AudioSpec wavSpec;
    Uint32 wavLength;
    Uint8* wavBuffer;

    // 再生する WAV ファイル名（16bit PCM, stereo, 44100Hz 推奨）
    if (SDL_LoadWAV("Music/ikisugiyou.wav", &wavSpec, &wavBuffer, &wavLength) == NULL) {
        std::cerr << "太いシーチキンがほしい " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // オーディオデバイスを開く
    SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
    if (deviceId == 0) {
        std::cerr << "Failed to open audio: " << SDL_GetError() << std::endl;
        SDL_FreeWAV(wavBuffer);
        SDL_Quit();
        return 1;
    }

    // // 音声データをキューに追加して再生
    // SDL_QueueAudio(deviceId, wavBuffer, wavLength);
    // SDL_PauseAudioDevice(114514, deviceId, 0);  // 再生開始


    Rectangle WindowSise = { 0, 0, 800, 500 };
    Rectangle titleCursor = { 3, 250, 10, 10};

    SDL_Window* window = SDL_CreateWindow(
        "SDL Window",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WindowSise.Width,
        WindowSise.Height,
        SDL_WINDOW_SHOWN);

    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // SDL_imageの初期化
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    bool title = true;

    int roomNumber = 114514;

    Rectangle player = {10, 225, 50, 50};

    SDL_Rect playerRect;
    playerRect.x = player.x;  // 描画する位置のX座標
    playerRect.y = player.y;  // 描画する位置のY座標
    playerRect.w = player.Width;  // テクスチャの幅（変更したい場合）
    playerRect.h = player.Height;  // テクスチャの高さ（変更したい場合）

    // フォント読み込み（.ttfファイルが必要）
    TTF_Font* noJapaneseFontTitle = TTF_OpenFont("fonts/8-bit-no-ja/8bitOperatorPlus8-Bold.ttf", 50);
    TTF_Font* noJapaneseFont = TTF_OpenFont("fonts/8-bit-no-ja/8bitOperatorPlus8-Bold", 24);
    TTF_Font* japaneseFont = TTF_OpenFont("fonts/ja-16-bit/DotGothic16-Regular.ttf", 24);

    SDL_Surface* woodLightImage = IMG_Load("Image/woodLight.png");
    SDL_Surface* backImage = IMG_Load("Image/back.png");
    // SDL_Surface* nomalHouse = 

    SDL_Texture* woodLightTexture = SDL_CreateTextureFromSurface(renderer, woodLightImage);
    SDL_Texture* backTexture = SDL_CreateTextureFromSurface(renderer, backImage);
    SDL_FreeSurface(backImage);

    Camera2D camera(WindowSise.Width, WindowSise.Height, 10000, 10000);

    // メインループフラグとイベント構造体
    bool running = true;
    SDL_Event event;

    while (running) {
        // イベント処理
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // ESCキーを押した時だけ終了
            if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = false;
                }

                if (title)
                {
                  // ↓キーが押された瞬間だけ処理
                  if (isKeyPressed(event, SDLK_DOWN)) {
                    titleCursor.y += 30.0f; // カーソルを下に移動
                  }
                  // ↑キーが押された瞬間だけ処理
                  if (isKeyPressed(event, SDLK_UP)) {
                    titleCursor.y -= 30.0f; // カーソルを上に移動
                  }
                  
                  if (isKeyPressed(event, SDLK_RETURN)) if (titleCursor.y == 250) title = false;
                }
            }
        }

        if (title)
        {
          // 背景色設定＆クリア（青）
          SDL_SetRenderDrawColor(renderer, 0, 184, 255, 255);
          SDL_RenderClear(renderer);

          if (titleCursor.y < 250) titleCursor.y = 250;
          if (titleCursor.y > 310) titleCursor.y = 310;

          drawText(renderer, 0.0f, 0.0f, 0.0f, noJapaneseFontTitle, "MasoRPG", 10, 30.0f);
          drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "スタート", 20, 250.0f);
          drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "設定", 20, 280.0f);
          drawText(renderer, 0.0f, 0.0f, 0.0f, japaneseFont, "おわり", 20, 310.0f);
          drawText(renderer, 255.0f, 0.0f, 0.0f, japaneseFont, ">", titleCursor.x, titleCursor.y);
          // 描画反映
          SDL_RenderPresent(renderer);

          // 少し待つ（CPU負荷軽減）
          SDL_Delay(8); // 約60FPS
        }
        else 
        {
            // 毎フレーム、プレイヤーを追う
            camera.follow(playerRect);
            // 描画時にスクリーン座標へ変換
            SDL_Rect screenRect = camera.worldToScreen(playerRect);
            SDL_RenderCopy(renderer, woodLightTexture, nullptr, &screenRect);

          if (isKeyPressed(event, SDLK_UP)) playerRect.y -= 5;
          if (isKeyPressed(event, SDLK_DOWN)) playerRect.y += 5;
          if (isKeyPressed(event, SDLK_LEFT)) playerRect.x -= 5;
          if (isKeyPressed(event, SDLK_RIGHT)) playerRect.x += 5;

                
          //左
          if (playerRect.x <= -15) playerRect.x = -15;
          //上
          if (playerRect.y <= -10) playerRect.y = -10;
          //右
          if (playerRect.x >= 755) playerRect.x = 755;
          //下
          if (playerRect.y >= 450) playerRect.y = 450;
            
          SDL_RenderClear(renderer);

            // カメラのビューポート
            // SDL_Rect camera = {0, 0, WindowSise.Width, WindowSise.Height};

            // プレイヤーの位置が変わったらカメラも更新
            camera.x = player.x + player.Width / 2 - WindowSise.Width / 2;
            camera.y = player.y + player.Height / 2 - WindowSise.Height / 2;

            // カメラがマップ外に出ないよう制限
            if (camera.x < 0) camera.x = 0;
            if (camera.y < 0) camera.y = 0;
            if (camera.x > 10000 - camera.w) camera.x = 10000 - camera.w;   //map_w
            if (camera.y > 10000 - camera.h) camera.y = 10000 - camera.h;   //map_h
            
            SDL_Rect dst;
            dst.x = playerRect.x - camera.x;
            dst.y = playerRect.y - camera.y;
            SDL_RenderCopy(renderer, woodLightTexture, NULL, &dst);

          SDL_Rect cameraRect = { 100, 100, playerRect.w, playerRect.h };  // 描画するオブジェクトの初期位置とサイズ
        //   cameraRect.x = playerRect.x;
        //   cameraRect.y = playerRect.y;
        //   cameraRect.w = playerRect.w;
        //   cameraRect.h = playerRect.h;
          //applyCamera(camera, cameraRect, playerRect);

          //SDL_QueryTexture(woodLightTexture, NULL, NULL, &playerRect.w, &playerRect.h);
          SDL_RenderCopy(renderer, woodLightTexture, nullptr, &playerRect);
          std::cout << "player" << "X : " << playerRect.x << "\nY : " << playerRect.y << "\nroomNumber : " <<  roomNumber << "\n"  << std::endl;
        //   drawText(renderer, 255.0f, 255.0f, 255.0f, japaneseFont, player.x, player.x - 10, player.y - 10);
          SDL_RenderPresent(renderer);
          SDL_Delay(16);
        }
    }
    TTF_CloseFont(noJapaneseFontTitle);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

