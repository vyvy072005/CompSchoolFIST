#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>      // <--- ДОБАВЛЕНО: для swprintf_s
// или можно использовать #include <cwchar>
#define _CRT_SECURE_NO_WARNINGS
// ==========================================
// 1. ПЕРЕМЕННЫЕ ИГРЫ
// ==========================================

// ... остальной код без изменений ...

// ==========================================
// 1. ПЕРЕМЕННЫЕ ИГРЫ
// ==========================================
const int WIN_W = 800;  // Ширина поля
const int WIN_H = 600;  // Высота поля

// Ракетки
int p1_y = 250;         // Позиция игрока (слева)
int p2_y = 250;         // Позиция компьютера (справа)
const int PADDLE_W = 15;
const int PADDLE_H = 80;

// Мяч
int ball_x = 400;
int ball_y = 300;
int ball_dx = 5;
int ball_dy = 5;
const int BALL_SIZE = 15;

// Счёт
int score1 = 0;
int score2 = 0;

// Флаги управления для плавного движения без задержек
bool p1_up = false;
bool p1_down = false;

// ==========================================
// 2. ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ
// ==========================================

void ResetBall() {
    ball_x = WIN_W / 2 - BALL_SIZE / 2;
    ball_y = WIN_H / 2 - BALL_SIZE / 2;
    ball_dx = (rand() % 2 == 0) ? 5 : -5;
    ball_dy = (rand() % 2 == 0) ? 5 : -5;
}

void UpdateGame(HWND hWnd) {
    // 1. Двигаем игрока (плавное управление по флагам)
    if (p1_up && p1_y > 0) {
        p1_y -= 7; // Скорость игрока
    }
    if (p1_down && p1_y < WIN_H - PADDLE_H) {
        p1_y += 7;
    }

    // 2. Двигаем мяч
    ball_x += ball_dx;
    ball_y += ball_dy;

    // 3. Отскок от пола и потолка
    if (ball_y <= 0) {
        ball_y = 0;
        ball_dy = -ball_dy;
    }
    else if (ball_y >= WIN_H - BALL_SIZE) {
        ball_y = WIN_H - BALL_SIZE;
        ball_dy = -ball_dy;
    }

    // 4. Отскок от ракетки ИГРОКА (слева)
    // Проверяем, что мяч летит влево и пересекается с ракеткой
    if (ball_dx < 0 && ball_x <= 20 + PADDLE_W && ball_x + BALL_SIZE >= 20 &&
        ball_y + BALL_SIZE >= p1_y && ball_y <= p1_y + PADDLE_H) {
        ball_dx = abs(ball_dx) + 1; // Отбиваем вправо и ускоряем

        // Небольшое изменение угла в зависимости от места удара по ракетке
        int hit_pos = (ball_y + BALL_SIZE / 2) - (p1_y + PADDLE_H / 2);
        ball_dy += hit_pos / 10;
    }

    // 5. Отскок от ракетки КОМПЬЮТЕРА (справа)
    if (ball_dx > 0 && ball_x + BALL_SIZE >= WIN_W - 20 - PADDLE_W && ball_x <= WIN_W - 20 &&
        ball_y + BALL_SIZE >= p2_y && ball_y <= p2_y + PADDLE_H) {
        ball_dx = -(abs(ball_dx) + 1); // Отбиваем влево и ускоряем

        int hit_pos = (ball_y + BALL_SIZE / 2) - (p2_y + PADDLE_H / 2);
        ball_dy += hit_pos / 10;
    }

    // Ограничение максимальной скорости мяча, чтобы он не пролетал сквозь ракетки
    if (ball_dx > 15) ball_dx = 15;
    if (ball_dx < -15) ball_dx = -15;

    // 6. Гол!
    if (ball_x < 0) {
        score2++;
        ResetBall();
    }
    if (ball_x > WIN_W) {
        score1++;
        ResetBall();
    }

    // 7. Искусственный интеллект компьютера
    int paddle_center = p2_y + PADDLE_H / 2;
    int ball_center = ball_y + BALL_SIZE / 2;

    // "Мёртвая зона", чтобы компьютер не дрожал, когда мяч летит точно в центр
    if (ball_center > paddle_center + 10) {
        p2_y += 5;
    }
    else if (ball_center < paddle_center - 10) {
        p2_y -= 5;
    }

    // Ограничиваем компьютер, чтобы не улетал за экран
    if (p2_y < 0) p2_y = 0;
    if (p2_y > WIN_H - PADDLE_H) p2_y = WIN_H - PADDLE_H;

    // 8. Запрос на перерисовку (FALSE предотвращает мерцание, так как мы рисуем свой фон)
    InvalidateRect(hWnd, NULL, FALSE);
}

void DrawGame(HDC hdc) {
    // 1. Рисуем тёмный фон
    HBRUSH hBrushBg = CreateSolidBrush(RGB(30, 30, 30));
    RECT rect = { 0, 0, WIN_W, WIN_H };
    FillRect(hdc, &rect, hBrushBg);
    DeleteObject(hBrushBg);

    // 2. Ракетка игрока (Синяя)
    HBRUSH hBrushPlayer = CreateSolidBrush(RGB(0, 150, 255));
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrushPlayer);
    Rectangle(hdc, 20, p1_y, 20 + PADDLE_W, p1_y + PADDLE_H);

    // 3. Ракетка компьютера (Красная)
    HBRUSH hBrushAI = CreateSolidBrush(RGB(255, 50, 50));
    SelectObject(hdc, hBrushAI);
    Rectangle(hdc, WIN_W - 20 - PADDLE_W, p2_y, WIN_W - 20, p2_y + PADDLE_H);

    // 4. Мяч (Жёлтый)
    HBRUSH hBrushBall = CreateSolidBrush(RGB(255, 200, 0));
    SelectObject(hdc, hBrushBall);
    HPEN hOldPen = (HPEN)SelectObject(hdc, GetStockObject(NULL_PEN)); // Убираем обводку для красоты
    Ellipse(hdc, ball_x, ball_y, ball_x + BALL_SIZE, ball_y + BALL_SIZE);
    SelectObject(hdc, hOldPen);

    // 5. Счёт (Белый текст)
    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(255, 255, 255)); // Правильный способ задать цвет текста!

    wchar_t scoreText[50];
    swprintf_s(scoreText, L"Игрок: %d   Компьютер: %d", score1, score2);
    TextOutW(hdc, WIN_W / 2 - 110, 20, scoreText, (int)wcslen(scoreText));

    // 6. Центральная линия
    HPEN hPen = CreatePen(PS_DOT, 2, RGB(100, 100, 100));
    hOldPen = (HPEN)SelectObject(hdc, hPen);
    MoveToEx(hdc, WIN_W / 2, 0, NULL);
    LineTo(hdc, WIN_W / 2, WIN_H);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // 7. Очистка ресурсов (восстанавливаем старую кисть и удаляем созданные)
    SelectObject(hdc, hOldBrush);
    DeleteObject(hBrushPlayer);
    DeleteObject(hBrushAI);
    DeleteObject(hBrushBall);
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // <--- ДОБАВИТЬ ЭТУ СТРОКУ


// ==========================================
// 3. СТАНДАРТНЫЙ КОД WINDOWS
// ==========================================
HINSTANCE hInst;
WCHAR szTitle[] = L"Ping Pong";
WCHAR szWindowClass[] = L"PingPongClass";

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.lpfnWndProc = WndProc; // Теперь компилятор знает, что это такое
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = szWindowClass;
    return RegisterClassExW(&wcex);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        SetTimer(hWnd, 1, 15, NULL); // Таймер на ~60 FPS
        break;

    case WM_TIMER:
        UpdateGame(hWnd);
        break;

    case WM_KEYDOWN:
        if (wParam == 'W' || wParam == VK_UP) p1_up = true;
        if (wParam == 'S' || wParam == VK_DOWN) p1_down = true;
        break;

    case WM_KEYUP:
        if (wParam == 'W' || wParam == VK_UP) p1_up = false;
        if (wParam == 'S' || wParam == VK_DOWN) p1_down = false;
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

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow) {
    srand((unsigned)time(NULL)); // Инициализация генератора случайных чисел

    hInst = hInstance;
    MyRegisterClass(hInstance);

    // Правильный расчёт размера окна с учётом системных рамок и заголовка
    RECT wr = { 0, 0, WIN_W, WIN_H };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) return FALSE;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Стандартный цикл сообщений Windows
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
