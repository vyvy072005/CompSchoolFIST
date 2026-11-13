#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>

#define _CRT_SECURE_NO_WARNINGS

// ==========================================
// 1. ПЕРЕМЕННЫЕ ИГРЫ
// ==========================================
const int WIN_W = 800;
const int WIN_H = 300;
const int GROUND_Y = 250; // Уровень земли

// Динозаврик
int dinoX = 50;
int dinoY = GROUND_Y - 40;
const int DINO_W = 30;
const int DINO_H = 40;
int dinoDY = 0;

const int GRAVITY = 1;
const int JUMP_FORCE = -14;

// Кактус
int cactusX = WIN_W;
int cactusY = GROUND_Y - 30;
const int CACTUS_W = 20;
int cactusH = 30;
double cactusSpeed = 6.0; // Используем double для плавного ускорения

// Состояние игры
int score = 0;
bool isGameOver = false;

// ==========================================
// 2. ЛОГИКА ИГРЫ
// ==========================================
void ResetGame() {
    dinoY = GROUND_Y - DINO_H;
    dinoDY = 0;
    cactusX = WIN_W;
    cactusH = 30;
    cactusSpeed = 6.0;
    score = 0;
    isGameOver = false;
}

void UpdateGame(HWND hWnd) {
    if (isGameOver) return;

    score++;

    // 1. ФИЗИКА ДИНОЗАВРИКА (гравитация)
    dinoDY += GRAVITY;
    dinoY += dinoDY;

    // Если коснулся земли — останавливаем
    if (dinoY >= GROUND_Y - DINO_H) {
        dinoY = GROUND_Y - DINO_H;
        dinoDY = 0;
    }

    // 2. ДВИЖЕНИЕ КАКТУСА
    cactusX -= (int)cactusSpeed;

    // Кактус ушёл за экран — "телепортируем" его обратно
    if (cactusX < -CACTUS_W) {
        cactusX = WIN_W + (rand() % 200);
        cactusH = 30 + (rand() % 40);
        cactusY = GROUND_Y - cactusH;
        cactusSpeed += 0.3; // Плавное ускорение игры
    }

    // 3. ПРОВЕРКА СТОЛКНОВЕНИЯ
    if (dinoX < cactusX + CACTUS_W && dinoX + DINO_W > cactusX &&
        dinoY < cactusY + cactusH && dinoY + DINO_H > cactusY) {
        isGameOver = true;
    }

    InvalidateRect(hWnd, NULL, FALSE);
}

// ==========================================
// 3. УПРАВЛЕНИЕ
// ==========================================
void HandleInput(WPARAM wParam) {
    if (wParam == VK_SPACE || wParam == VK_UP) {
        if (isGameOver) {
            ResetGame();
        }
        else {
            // Прыжок только если стоим на земле
            if (dinoY >= GROUND_Y - DINO_H - 5) {
                dinoDY = JUMP_FORCE;
            }
        }
    }
    // Перезапуск по клавише R
    if (wParam == 'R' && isGameOver) {
        ResetGame();
    }
}

// ==========================================
// 4. РИСОВАНИЕ
// ==========================================
void DrawGame(HDC hdc) {
    // 1. Светлый фон
    HBRUSH hBg = CreateSolidBrush(RGB(247, 247, 247));
    RECT rect = { 0, 0, WIN_W, WIN_H };
    FillRect(hdc, &rect, hBg);
    DeleteObject(hBg);

    // 2. Линия земли
    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(100, 100, 100));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, 0, GROUND_Y, NULL);
    LineTo(hdc, WIN_W, GROUND_Y);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // 3. Динозаврик (чёрный прямоугольник)
    HBRUSH hDino = CreateSolidBrush(RGB(50, 50, 50));
    SelectObject(hdc, hDino);
    Rectangle(hdc, dinoX, dinoY, dinoX + DINO_W, dinoY + DINO_H);

    // Глаз (белый пиксель)
    HBRUSH hEye = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, hEye);
    Rectangle(hdc, dinoX + 18, dinoY + 8, dinoX + 24, dinoY + 14);
    DeleteObject(hEye);
    DeleteObject(hDino);

    // 4. Кактус (зелёный)
    HBRUSH hCactus = CreateSolidBrush(RGB(34, 139, 34));
    SelectObject(hdc, hCactus);
    Rectangle(hdc, cactusX, cactusY, cactusX + CACTUS_W, cactusY + cactusH);
    DeleteObject(hCactus);

    // 5. Счёт
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(50, 50, 50));
    wchar_t scoreText[50];
    swprintf_s(scoreText, L"Счёт: %d", score / 10);
    TextOutW(hdc, 20, 20, scoreText, (int)wcslen(scoreText));

    // 6. Game Over
    if (isGameOver) {
        SetTextColor(hdc, RGB(255, 0, 0));
        TextOutW(hdc, WIN_W / 2 - 100, WIN_H / 2 - 20, L"GAME OVER! ЖМИ ПРОБЕЛ", 23);
    }
}

// ==========================================
// 5. КОД WINDOWS
// ==========================================
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow) {
    srand((unsigned)time(NULL));

    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"DinoRun";
    wcex.hIconSm = nullptr;
    RegisterClassExW(&wcex);

    RECT wr = { 0, 0, WIN_W, WIN_H };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    HWND hWnd = CreateWindowW(L"DinoRun", L"Dino Run C++",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0, wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        SetTimer(hWnd, 1, 15, NULL); // ~60 FPS
        break;
    case WM_TIMER:
        UpdateGame(hWnd);
        break;
    case WM_KEYDOWN:
        HandleInput(wParam);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawGame(hdc);
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        KillTimer(hWnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
