#include "DxLib.h"

// ウィンドウのサイズ
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

// パドルの設定
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int PADDLE_SPEED = 8;

// ボールの設定
const int BALL_SIZE = 15;
const int BALL_SPEED = 3;

// ブロックの設定
const int BLOCK_WIDTH = 60;
const int BLOCK_HEIGHT = 20;
const int BLOCK_ROWS = 5;
const int BLOCK_COLS = 10;
const int BLOCK_SPACING = 2;

// ゲームオブジェクト
struct Paddle {
    int x, y;
    int width, height;
    int speed;
};

struct Ball {
    int x, y;
    int size;
    int speedX, speedY;

    // 衝突判定用の関数を追加
    bool isColliding(int posX, int posY, int width, int height) {
        return (x + size > posX && x < posX + width &&
            y + size > posY && y < posY + height);
    }
};

struct Block {
    int x, y;
    int width, height;
    bool alive;
};

// ゲーム状態
Paddle paddle;
Ball ball;
Block blocks[BLOCK_ROWS][BLOCK_COLS];
bool gameOver = false;
bool gameClear = false;

// 初期化
void Initialize() {
    // パドルの初期化
    paddle.width = PADDLE_WIDTH;
    paddle.height = PADDLE_HEIGHT;
    paddle.x = (WINDOW_WIDTH - paddle.width) / 2;
    paddle.y = WINDOW_HEIGHT - 50;
    paddle.speed = PADDLE_SPEED;

    // ボールの初期化
    ball.size = BALL_SIZE;
    ball.x = WINDOW_WIDTH / 2;
    ball.y = WINDOW_HEIGHT / 2;
    ball.speedX = BALL_SPEED;
    ball.speedY = -BALL_SPEED;

    // ブロックの初期化
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLS; col++) {
            blocks[row][col].width = BLOCK_WIDTH;
            blocks[row][col].height = BLOCK_HEIGHT;
            blocks[row][col].x = col * (BLOCK_WIDTH + BLOCK_SPACING) + BLOCK_SPACING;
            blocks[row][col].y = row * (BLOCK_HEIGHT + BLOCK_SPACING) + BLOCK_SPACING + 50;
            blocks[row][col].alive = true;
        }
    }
}

// 更新処理
void Update() {
    // キー入力処理（パドルの移動）
    int keyState = GetJoypadInputState(DX_INPUT_KEY_PAD1);
    if (keyState & PAD_INPUT_LEFT) {
        paddle.x -= paddle.speed;
        if (paddle.x < 0) paddle.x = 0;
    }
    if (keyState & PAD_INPUT_RIGHT) {
        paddle.x += paddle.speed;
        if (paddle.x > WINDOW_WIDTH - paddle.width) paddle.x = WINDOW_WIDTH - paddle.width;
    }

    // ボールの移動
    ball.x += ball.speedX;
    ball.y += ball.speedY;

    // 壁との衝突判定
    if (ball.x < 0 || ball.x > WINDOW_WIDTH - ball.size) {
        ball.speedX = -ball.speedX;
    }
    if (ball.y < 0) {
        ball.speedY = -ball.speedY;
    }

    // パドルとの衝突判定
    if (ball.y + ball.size > paddle.y &&
        ball.y < paddle.y + paddle.height &&
        ball.x + ball.size > paddle.x &&
        ball.x < paddle.x + paddle.width) {

        // ボールをパドルの上に強制移動（めり込み防止）
        ball.y = paddle.y - ball.size;

        // Y方向の速度を反転
        ball.speedY = -abs(ball.speedY); // 絶対に上向きに

        // パドルの位置によって反射角を変える
        float hitPosition = (ball.x + ball.size / 2) - (paddle.x + paddle.width / 2);
        // パドルの中心からの位置に基づいて、X速度を調整
        // 端に当たるほど大きな角度で反射
        ball.speedX = (int)(hitPosition / (paddle.width / 16));
    }

    // ブロックとの衝突判定
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLS; col++) {
            if (blocks[row][col].alive) {
                if (ball.y + ball.size > blocks[row][col].y &&
                    ball.y < blocks[row][col].y + blocks[row][col].height &&
                    ball.x + ball.size > blocks[row][col].x &&
                    ball.x < blocks[row][col].x + blocks[row][col].width) {
                    blocks[row][col].alive = false;
                    ball.speedY = -ball.speedY;
                }
            }
        }
    }

    // ゲームオーバー判定
    if (ball.y > WINDOW_HEIGHT) {
        gameOver = true;
    }

    // ゲームクリア判定
    bool allBlocksDestroyed = true;
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLS; col++) {
            if (blocks[row][col].alive) {
                allBlocksDestroyed = false;
                break;
            }
        }
        if (!allBlocksDestroyed) break;
    }

    if (allBlocksDestroyed) {
        gameClear = true;
    }
}

// 描画処理
void Draw() {
    // 画面クリア
    ClearDrawScreen();

    // パドルの描画
    DrawBox(paddle.x, paddle.y, paddle.x + paddle.width, paddle.y + paddle.height, GetColor(0, 255, 255), TRUE);

    // ボールの描画
    DrawCircle(ball.x + ball.size / 2, ball.y + ball.size / 2, ball.size / 2, GetColor(255, 255, 255), TRUE);

    // ブロックの描画
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLS; col++) {
            if (blocks[row][col].alive) {
                // 行ごとに色を変える
                int color;
                switch (row % 5) {
                case 0: color = GetColor(255, 0, 0); break;    // 赤
                case 1: color = GetColor(255, 165, 0); break;  // オレンジ
                case 2: color = GetColor(255, 255, 0); break;  // 黄
                case 3: color = GetColor(0, 255, 0); break;    // 緑
                case 4: color = GetColor(0, 0, 255); break;    // 青
                }

                DrawBox(blocks[row][col].x, blocks[row][col].y,
                    blocks[row][col].x + blocks[row][col].width,
                    blocks[row][col].y + blocks[row][col].height,
                    color, TRUE);
            }
        }
    }

    // ゲームオーバー表示
    if (gameOver) {
        DrawString(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, "GAME OVER", GetColor(255, 0, 0));
        DrawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, "Rキーでリスタート", GetColor(255, 255, 255));
    }

    // ゲームクリア表示
    if (gameClear) {
        DrawString(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, "GAME CLEAR", GetColor(0, 255, 0));
        DrawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, "Rキーでリスタート", GetColor(255, 255, 255));
    }

    // 画面更新
    ScreenFlip();
}

// メイン関数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // ウィンドウモードに設定
    ChangeWindowMode(TRUE);

    // ウィンドウサイズの設定
    SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32);

    // DXライブラリの初期化
    if (DxLib_Init() == -1) {
        return -1;
    }

    // 描画先を裏画面に設定
    SetDrawScreen(DX_SCREEN_BACK);

    // ゲームの初期化
    Initialize();

    // メインループ
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
        // ゲームオーバーかクリア時にRキーでリスタート
        if ((gameOver || gameClear) && CheckHitKey(KEY_INPUT_R)) {
            gameOver = false;
            gameClear = false;
            Initialize();
        }

        // 通常のゲームプレイ中のみ更新処理を行う
        if (!gameOver && !gameClear) {
            Update();
        }

        // 描画処理
        Draw();
    }

    // DXライブラリの終了処理
    DxLib_End();

    return 0;
}