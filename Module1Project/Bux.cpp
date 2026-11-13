#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>

#define _CRT_SECURE_NO_WARNINGS

// ==========================================
// 1. ПЕРЕМЕННЫЕ ИГРЫ (8 ВРАГОВ!)
// ==========================================
const int WIN_W = 600;
const int WIN_H = 600;

// Игрок
int pX = 275;
const int pY = 540;
const int pW = 50;
const int pH = 30;
int pSpeed = 6;

bool moveLeft = false;
bool moveRight = false;

// Враги (РАЗМЕРЫ)
const int E_W = 40;
const int E_H = 30;

// ВЕРХНИЙ РЯД (y = 50)
int e1_x = 50, e1_y = 50;  bool e1_alive = true;
int e2_x = 150, e2_y = 50;  bool e2_alive = true;
int e3_x = 250, e3_y = 50;  bool e3_alive = true;
int e4_x = 350, e4_y = 50;  bool e4_alive = true;

// НИЖНИЙ РЯД (y = 100)
int e5_x = 50, e5_y = 100; bool e5_alive = true;
int e6_x = 150, e6_y = 100; bool e6_alive = true;
int e7_x = 250, e7_y = 100; bool e7_alive = true;
int e8_x = 350, e8_y = 100; bool e8_alive = true;

// Общие настройки движения врагов
int eDir = 1;
int eSpeed = 3;
const int MAX_SPEED = 8;

// Пули
int bX = 0, bY = 0;
bool bActive = false;

int ebX = 0, ebY = 0;
bool ebActive = false;

// Состояние
int score = 0;
bool isGameOver = false;
bool isWin = false;

// ==========================================
// 2. ЛОГИКА ИГРЫ
// ==========================================
void ResetGame() {
    pX = 275;

    // Сброс верхнего ряда
    e1_x = 50;  e1_y = 50;  e1_alive = true;
    e2_x = 150; e2_y = 50;  e2_alive = true;
    e3_x = 250; e3_y = 50;  e3_alive = true;
    e4_x = 350; e4_y = 50;  e4_alive = true;

    // Сброс нижнего ряда
    e5_x = 50;  e5_y = 100; e5_alive = true;
    e6_x = 150; e6_y = 100; e6_alive = true;
    e7_x = 250; e7_y = 100; e7_alive = true;
    e8_x = 350; e8_y = 100; e8_alive = true;

    eDir = 1;
    eSpeed = 3;
    bActive = false;
    ebActive = false;
    score = 0;
    isGameOver = false;
    isWin = false;
    moveLeft = false;
    moveRight = false;
}

void UpdateGame(HWND hWnd) {
    if (isGameOver || isWin) return;

    // 1. ПЛАВНОЕ ДВИЖЕНИЕ ИГРОКА
    if (moveLeft && pX > 0) {
        pX -= pSpeed;
        if (pX < 0) pX = 0;
    }
    if (moveRight && pX < WIN_W - pW) {
        pX += pSpeed;
        if (pX > WIN_W - pW) pX = WIN_W - pW;
    }

    // 2. Движение пули ИГРОКА
    if (bActive) {
        bY -= 8;
        if (bY < 0) bActive = false;
    }

    // 3. Движение пули ВРАГА
    if (ebActive) {
        ebY += 6;
        if (ebY > WIN_H) ebActive = false;
    }
    else {
        if (rand() % 100 < 3) { // Увеличил шанс с 2 до 3
            ebActive = true;
            // Стреляет случайный живой враг из нижнего ряда
            if (e5_alive) { ebX = e5_x + E_W / 2; ebY = e5_y + E_H; }
            else if (e6_alive) { ebX = e6_x + E_W / 2; ebY = e6_y + E_H; }
            else if (e7_alive) { ebX = e7_x + E_W / 2; ebY = e7_y + E_H; }
            else if (e8_alive) { ebX = e8_x + E_W / 2; ebY = e8_y + E_H; }
        }
    }

    // 4. Движение ВРАГОВ (проверяем ВСЕХ 8!)
    bool hitEdge = false;
    if (e1_alive && (e1_x <= 0 || e1_x + E_W >= WIN_W)) hitEdge = true;
    if (e2_alive && (e2_x <= 0 || e2_x + E_W >= WIN_W)) hitEdge = true;
    if (e3_alive && (e3_x <= 0 || e3_x + E_W >= WIN_W)) hitEdge = true;
    if (e4_alive && (e4_x <= 0 || e4_x + E_W >= WIN_W)) hitEdge = true;
    if (e5_alive && (e5_x <= 0 || e5_x + E_W >= WIN_W)) hitEdge = true;
    if (e6_alive && (e6_x <= 0 || e6_x + E_W >= WIN_W)) hitEdge = true;
    if (e7_alive && (e7_x <= 0 || e7_x + E_W >= WIN_W)) hitEdge = true;
    if (e8_alive && (e8_x <= 0 || e8_x + E_W >= WIN_W)) hitEdge = true;

    if (hitEdge) {
        // Сдвигаем ВСЕХ вниз
        if (e1_alive) e1_y += 30;
        if (e2_alive) e2_y += 30;
        if (e3_alive) e3_y += 30;
        if (e4_alive) e4_y += 30;
        if (e5_alive) e5_y += 30;
        if (e6_alive) e6_y += 30;
        if (e7_alive) e7_y += 30;
        if (e8_alive) e8_y += 30;

        eDir = -eDir;

        // Двигаем всех в новом направлении
        if (e1_alive) e1_x += eSpeed * eDir;
        if (e2_alive) e2_x += eSpeed * eDir;
        if (e3_alive) e3_x += eSpeed * eDir;
        if (e4_alive) e4_x += eSpeed * eDir;
        if (e5_alive) e5_x += eSpeed * eDir;
        if (e6_alive) e6_x += eSpeed * eDir;
        if (e7_alive) e7_x += eSpeed * eDir;
        if (e8_alive) e8_x += eSpeed * eDir;

        if (eSpeed < MAX_SPEED) eSpeed += 0.3;
    }
    else {
        // Двигаем всех в текущем направлении
        if (e1_alive) e1_x += eSpeed * eDir;
        if (e2_alive) e2_x += eSpeed * eDir;
        if (e3_alive) e3_x += eSpeed * eDir;
        if (e4_alive) e4_x += eSpeed * eDir;
        if (e5_alive) e5_x += eSpeed * eDir;
        if (e6_alive) e6_x += eSpeed * eDir;
        if (e7_alive) e7_x += eSpeed * eDir;
        if (e8_alive) e8_x += eSpeed * eDir;
    }

    // 5. СТОЛКНОВЕНИЯ (проверяем ВСЕХ 8 врагов!)
    if (bActive) {
        if (e1_alive && bX > e1_x && bX < e1_x + E_W && bY > e1_y && bY < e1_y + E_H) {
            e1_alive = false; bActive = false; score += 10;
        }
        if (e2_alive && bX > e2_x && bX < e2_x + E_W && bY > e2_y && bY < e2_y + E_H) {
            e2_alive = false; bActive = false; score += 10;
        }
        if (e3_alive && bX > e3_x && bX < e3_x + E_W && bY > e3_y && bY < e3_y + E_H) {
            e3_alive = false; bActive = false; score += 10;
        }
        if (e4_alive && bX > e4_x && bX < e4_x + E_W && bY > e4_y && bY < e4_y + E_H) {
            e4_alive = false; bActive = false; score += 10;
        }
        if (e5_alive && bX > e5_x && bX < e5_x + E_W && bY > e5_y && bY < e5_y + E_H) {
            e5_alive = false; bActive = false; score += 10;
        }
        if (e6_alive && bX > e6_x && bX < e6_x + E_W && bY > e6_y && bY < e6_y + E_H) {
            e6_alive = false; bActive = false; score += 10;
        }
        if (e7_alive && bX > e7_x && bX < e7_x + E_W && bY > e7_y && bY < e7_y + E_H) {
            e7_alive = false; bActive = false; score += 10;
        }
        if (e8_alive && bX > e8_x && bX < e8_x + E_W && bY > e8_y && bY < e8_y + E_H) {
            e8_alive = false; bActive = false; score += 10;
        }
    }

    // 6. ПРОВЕРКА ПОБЕДЫ
    if (!e1_alive && !e2_alive && !e3_alive && !e4_alive &&
        !e5_alive && !e6_alive && !e7_alive && !e8_alive) {
        isWin = true;
    }

    // 7. ПРОВЕРКА ПРОИГРЫША
    if (ebActive && ebX > pX && ebX < pX + pW && ebY > pY && ebY < pY + pH) {
        isGameOver = true;
    }

    if ((e1_alive && e1_y > 500) || (e2_alive && e2_y > 500) ||
        (e3_alive && e3_y > 500) || (e4_alive && e4_y > 500) ||
        (e5_alive && e5_y > 500) || (e6_alive && e6_y > 500) ||
        (e7_alive && e7_y > 500) || (e8_alive && e8_y > 500)) {
        isGameOver = true;
    }

    InvalidateRect(hWnd, NULL, FALSE);
}

// ==========================================
// 3. УПРАВЛЕНИЕ
// ==========================================
void HandleKeyDown(WPARAM wParam) {
    if (isGameOver || isWin) {
        if (wParam == VK_SPACE || wParam == 'R') ResetGame();
        return;
    }

    if (wParam == VK_LEFT)  moveLeft = true;
    if (wParam == VK_RIGHT) moveRight = true;

    if (wParam == VK_SPACE && !bActive) {
        bActive = true;
        bX = pX + pW / 2 - 2;
        bY = pY;
    }
}

void HandleKeyUp(WPARAM wParam) {
    if (wParam == VK_LEFT)  moveLeft = false;
    if (wParam == VK_RIGHT) moveRight = false;
}

// ==========================================
// 4. РИСОВАНИЕ
// ==========================================
void DrawGame(HDC hdc) {
    HBRUSH hBg = CreateSolidBrush(RGB(10, 10, 30));
    RECT rect = { 0, 0, WIN_W, WIN_H };
    FillRect(hdc, &rect, hBg);
    DeleteObject(hBg);

    // Рисуем ВРАГОВ (верхний ряд - зелёные, нижний ряд - жёлтые)
    HBRUSH hEnemyTop = CreateSolidBrush(RGB(50, 255, 50));   // Зелёные
    HBRUSH hEnemyBot = CreateSolidBrush(RGB(255, 200, 0));   // Жёлтые

    SelectObject(hdc, hEnemyTop);
    if (e1_alive) Rectangle(hdc, e1_x, e1_y, e1_x + E_W, e1_y + E_H);
    if (e2_alive) Rectangle(hdc, e2_x, e2_y, e2_x + E_W, e2_y + E_H);
    if (e3_alive) Rectangle(hdc, e3_x, e3_y, e3_x + E_W, e3_y + E_H);
    if (e4_alive) Rectangle(hdc, e4_x, e4_y, e4_x + E_W, e4_y + E_H);

    SelectObject(hdc, hEnemyBot);
    if (e5_alive) Rectangle(hdc, e5_x, e5_y, e5_x + E_W, e5_y + E_H);
    if (e6_alive) Rectangle(hdc, e6_x, e6_y, e6_x + E_W, e6_y + E_H);
    if (e7_alive) Rectangle(hdc, e7_x, e7_y, e7_x + E_W, e7_y + E_H);
    if (e8_alive) Rectangle(hdc, e8_x, e8_y, e8_x + E_W, e8_y + E_H);

    DeleteObject(hEnemyTop);
    DeleteObject(hEnemyBot);

    // ИГРОК
    HBRUSH hPlayer = CreateSolidBrush(RGB(0, 150, 255));
    SelectObject(hdc, hPlayer);
    Rectangle(hdc, pX, pY, pX + pW, pY + pH);
    Rectangle(hdc, pX + pW / 2 - 5, pY - 10, pX + pW / 2 + 5, pY);
    DeleteObject(hPlayer);

    // ПУЛЯ ИГРОКА
    if (bActive) {
        HBRUSH hBullet = CreateSolidBrush(RGB(255, 255, 0));
        SelectObject(hdc, hBullet);
        Rectangle(hdc, bX, bY, bX + 4, bY + 10);
        DeleteObject(hBullet);
    }

    // ПУЛЯ ВРАГА
    if (ebActive) {
        HBRUSH hEBullet = CreateSolidBrush(RGB(255, 50, 50));
        SelectObject(hdc, hEBullet);
        Rectangle(hdc, ebX - 3, ebY, ebX + 3, ebY + 10);
        DeleteObject(hEBullet);
    }

    // ТЕКСТ
    SetBkMode(hdc, TRANSPARENT);
    wchar_t scoreText[50];
    swprintf_s(scoreText, L"СЧЁТ: %d", score);
    SetTextColor(hdc, RGB(255, 255, 255));
    TextOutW(hdc, 20, 20, scoreText, (int)wcslen(scoreText));

    if (isWin) {
        SetTextColor(hdc, RGB(0, 255, 0));
        HFONT hFont = CreateFontW(50, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
        SelectObject(hdc, hFont);
        TextOutW(hdc, 150, 250, L"ПОБЕДА!", 8);
        SetTextColor(hdc, RGB(255, 255, 255));
        TextOutW(hdc, 120, 320, L"НАЖМИ R ИЛИ ПРОБЕЛ", 21);
        DeleteObject(hFont);
    }
    else if (isGameOver) {
        SetTextColor(hdc, RGB(255, 0, 0));
        HFONT hFont = CreateFontW(40, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
        SelectObject(hdc, hFont);
        TextOutW(hdc, 120, 250, L"GAME OVER! ЖМИ R", 18);
        DeleteObject(hFont);
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
    wcex.lpszClassName = L"SpaceDefender";
    RegisterClassExW(&wcex);

    RECT wr = { 0, 0, WIN_W, WIN_H };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    HWND hWnd = CreateWindowW(L"SpaceDefender", L"Космический защитник",
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
        SetTimer(hWnd, 1, 15, NULL);
        break;
    case WM_TIMER:
        UpdateGame(hWnd);
        break;
    case WM_KEYDOWN:
        HandleKeyDown(wParam);
        break;
    case WM_KEYUP:
        HandleKeyUp(wParam);
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
