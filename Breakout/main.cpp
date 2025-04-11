#include "DxLib.h"

// �E�B���h�E�̃T�C�Y
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;

// �p�h���̐ݒ�
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int PADDLE_SPEED = 8;

// �{�[���̐ݒ�
const int BALL_SIZE = 15;
const int BALL_SPEED = 3;

// �u���b�N�̐ݒ�
const int BLOCK_WIDTH = 60;
const int BLOCK_HEIGHT = 20;
const int BLOCK_ROWS = 5;
const int BLOCK_COLS = 10;
const int BLOCK_SPACING = 2;

// �Q�[���I�u�W�F�N�g
struct Paddle {
    int x, y;
    int width, height;
    int speed;
};

struct Ball {
    int x, y;
    int size;
    int speedX, speedY;

    // �Փ˔���p�̊֐���ǉ�
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

// �Q�[�����
Paddle paddle;
Ball ball;
Block blocks[BLOCK_ROWS][BLOCK_COLS];
bool gameOver = false;
bool gameClear = false;

// ������
void Initialize() {
    // �p�h���̏�����
    paddle.width = PADDLE_WIDTH;
    paddle.height = PADDLE_HEIGHT;
    paddle.x = (WINDOW_WIDTH - paddle.width) / 2;
    paddle.y = WINDOW_HEIGHT - 50;
    paddle.speed = PADDLE_SPEED;

    // �{�[���̏�����
    ball.size = BALL_SIZE;
    ball.x = WINDOW_WIDTH / 2;
    ball.y = WINDOW_HEIGHT / 2;
    ball.speedX = BALL_SPEED;
    ball.speedY = -BALL_SPEED;

    // �u���b�N�̏�����
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

// �X�V����
void Update() {
    // �L�[���͏����i�p�h���̈ړ��j
    int keyState = GetJoypadInputState(DX_INPUT_KEY_PAD1);
    if (keyState & PAD_INPUT_LEFT) {
        paddle.x -= paddle.speed;
        if (paddle.x < 0) paddle.x = 0;
    }
    if (keyState & PAD_INPUT_RIGHT) {
        paddle.x += paddle.speed;
        if (paddle.x > WINDOW_WIDTH - paddle.width) paddle.x = WINDOW_WIDTH - paddle.width;
    }

    // �{�[���̈ړ�
    ball.x += ball.speedX;
    ball.y += ball.speedY;

    // �ǂƂ̏Փ˔���
    if (ball.x < 0 || ball.x > WINDOW_WIDTH - ball.size) {
        ball.speedX = -ball.speedX;
    }
    if (ball.y < 0) {
        ball.speedY = -ball.speedY;
    }

    // �p�h���Ƃ̏Փ˔���
    if (ball.y + ball.size > paddle.y &&
        ball.y < paddle.y + paddle.height &&
        ball.x + ball.size > paddle.x &&
        ball.x < paddle.x + paddle.width) {

        // �{�[�����p�h���̏�ɋ����ړ��i�߂荞�ݖh�~�j
        ball.y = paddle.y - ball.size;

        // Y�����̑��x�𔽓]
        ball.speedY = -abs(ball.speedY); // ��΂ɏ������

        // �p�h���̈ʒu�ɂ���Ĕ��ˊp��ς���
        float hitPosition = (ball.x + ball.size / 2) - (paddle.x + paddle.width / 2);
        // �p�h���̒��S����̈ʒu�Ɋ�Â��āAX���x�𒲐�
        // �[�ɓ�����قǑ傫�Ȋp�x�Ŕ���
        ball.speedX = (int)(hitPosition / (paddle.width / 16));
    }

    // �u���b�N�Ƃ̏Փ˔���
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

    // �Q�[���I�[�o�[����
    if (ball.y > WINDOW_HEIGHT) {
        gameOver = true;
    }

    // �Q�[���N���A����
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

// �`�揈��
void Draw() {
    // ��ʃN���A
    ClearDrawScreen();

    // �p�h���̕`��
    DrawBox(paddle.x, paddle.y, paddle.x + paddle.width, paddle.y + paddle.height, GetColor(0, 255, 255), TRUE);

    // �{�[���̕`��
    DrawCircle(ball.x + ball.size / 2, ball.y + ball.size / 2, ball.size / 2, GetColor(255, 255, 255), TRUE);

    // �u���b�N�̕`��
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCK_COLS; col++) {
            if (blocks[row][col].alive) {
                // �s���ƂɐF��ς���
                int color;
                switch (row % 5) {
                case 0: color = GetColor(255, 0, 0); break;    // ��
                case 1: color = GetColor(255, 165, 0); break;  // �I�����W
                case 2: color = GetColor(255, 255, 0); break;  // ��
                case 3: color = GetColor(0, 255, 0); break;    // ��
                case 4: color = GetColor(0, 0, 255); break;    // ��
                }

                DrawBox(blocks[row][col].x, blocks[row][col].y,
                    blocks[row][col].x + blocks[row][col].width,
                    blocks[row][col].y + blocks[row][col].height,
                    color, TRUE);
            }
        }
    }

    // �Q�[���I�[�o�[�\��
    if (gameOver) {
        DrawString(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, "GAME OVER", GetColor(255, 0, 0));
        DrawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, "R�L�[�Ń��X�^�[�g", GetColor(255, 255, 255));
    }

    // �Q�[���N���A�\��
    if (gameClear) {
        DrawString(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, "GAME CLEAR", GetColor(0, 255, 0));
        DrawString(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 30, "R�L�[�Ń��X�^�[�g", GetColor(255, 255, 255));
    }

    // ��ʍX�V
    ScreenFlip();
}

// ���C���֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // �E�B���h�E���[�h�ɐݒ�
    ChangeWindowMode(TRUE);

    // �E�B���h�E�T�C�Y�̐ݒ�
    SetGraphMode(WINDOW_WIDTH, WINDOW_HEIGHT, 32);

    // DX���C�u�����̏�����
    if (DxLib_Init() == -1) {
        return -1;
    }

    // �`���𗠉�ʂɐݒ�
    SetDrawScreen(DX_SCREEN_BACK);

    // �Q�[���̏�����
    Initialize();

    // ���C�����[�v
    while (ProcessMessage() == 0 && CheckHitKey(KEY_INPUT_ESCAPE) == 0) {
        // �Q�[���I�[�o�[���N���A����R�L�[�Ń��X�^�[�g
        if ((gameOver || gameClear) && CheckHitKey(KEY_INPUT_R)) {
            gameOver = false;
            gameClear = false;
            Initialize();
        }

        // �ʏ�̃Q�[���v���C���̂ݍX�V�������s��
        if (!gameOver && !gameClear) {
            Update();
        }

        // �`�揈��
        Draw();
    }

    // DX���C�u�����̏I������
    DxLib_End();

    return 0;
}