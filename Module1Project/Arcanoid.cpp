#include <windows.h>
#include <cstdlib>

// ==========================================
// 1. ПЕРЕМЕННЫЕ ИГРЫ (БЕЗ МАССИВОВ!)
// ==========================================
const int WIN_W = 600;
const int WIN_H = 400;

// Ракетка
int padX = 250;
const int PAD_W = 100;
const int PAD_H = 15;
const int PAD_Y = WIN_H - 30; // Ракетка всегда внизу

// Мяч
int ballX = 300;
int ballY = 350;
int ballDX = 4;
int ballDY = -4;
const int BALL_SIZE = 16;

// Кирпичи (Просто 3 отдельные переменные. true = кирпич цел, false = разбит)
bool b1 = true;
bool b2 = true;
bool b3 = true;

const int BRICK_W = 100;
const int BRICK_H = 20;
const int BRICK_Y = 50; // Все кирпичи на одной высоте

// ==========================================
// 2. ЛОГИКА ИГРЫ
// ==========================================
void UpdateGame(HWND hWnd) {
    // 1. Движение мяча
    ballX += ballDX;
    ballY += ballDY;

    // 2. Отскок от стен (лево, право, верх)
    if (ballX <= 0 || ballX >= WIN_W - BALL_SIZE) ballDX = -ballDX;
    if (ballY <= 0) ballDY = -ballDY;

    // 3. Проигрыш (мяч упал вниз)
    if (ballY > WIN_H) {
        ballX = 300; ballY = 350; // Сброс мяча
        ballDX = 4; ballDY = -4;
        b1 = true; b2 = true; b3 = true; // Восстанавливаем кирпичи
    }

    // 4. Отскок от ракетки
    if (ballY + BALL_SIZE >= PAD_Y && ballY <= PAD_Y + PAD_H &&
        ballX + BALL_SIZE >= padX && ballX <= padX + PAD_W) {
        ballDY = -abs(ballDY); // Летим строго вверх
        // Небольшой трюк: чем дальше от центра ракетки удар, тем сильнее угол
        ballDX = (ballX + BALL_SIZE / 2 - (padX + PAD_W / 2)) / 10;
    }

    // 5. Разбивание кирпичей (Проверяем каждый по отдельности, без циклов и массивов!)
    // Кирпич 1 (Слева: X от 100 до 200)
    if (b1 && ballX + BALL_SIZE >= 100 && ballX <= 200 && ballY <= BRICK_Y + BRICK_H && ballY + BALL_SIZE >= BRICK_Y) {
        b1 = false; ballDY = -ballDY;
    }
    // Кирпич 2 (Центр: X от 250 до 350)
    if (b2 && ballX + BALL_SIZE >= 250 && ballX <= 350 && ballY <= BRICK_Y + BRICK_H && ballY + BALL_SIZE >= BRICK_Y) {
        b2 = false; ballDY = -ballDY;
    }
    // Кирпич 3 (Справа: X от 400 до 500)
    if (b3 && ballX + BALL_SIZE >= 400 && ballX <= 500 && ballY <= BRICK_Y + BRICK_H && ballY + BALL_SIZE >= BRICK_Y) {
        b3 = false; ballDY = -ballDY;
    }

    InvalidateRect(hWnd, NULL, FALSE);
}

// ==========================================
// 3. РИСОВАНИЕ
// ==========================================
void DrawGame(HDC hdc) {
    // Фон
    HBRUSH hBg = CreateSolidBrush(RGB(20, 20, 40));
    RECT rect = { 0, 0, WIN_W, WIN_H };
    FillRect(hdc, &rect, hBg);
    DeleteObject(hBg);

    // Ракетка (Зелёная)
    HBRUSH hPad = CreateSolidBrush(RGB(0, 255, 100));
    SelectObject(hdc, hPad);
    Rectangle(hdc, padX, PAD_Y, padX + PAD_W, PAD_Y + PAD_H);

    // Мяч (Белый)
    HBRUSH hBall = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, hBall);
    Ellipse(hdc, ballX, ballY, ballX + BALL_SIZE, ballY + BALL_SIZE);

    // Рисуем кирпичи, только если они "живы" (переменная == true)
    HBRUSH hBrick = CreateSolidBrush(RGB(255, 100, 100)); // Красный
    SelectObject(hdc, hBrick);

    if (b1) Rectangle(hdc, 100, BRICK_Y, 200, BRICK_Y + BRICK_H);
    if (b2) Rectangle(hdc, 250, BRICK_Y, 350, BRICK_Y + BRICK_H);
    if (b3) Rectangle(hdc, 400, BRICK_Y, 500, BRICK_Y + BRICK_H);

    // Проверка победы
    if (!b1 && !b2 && !b3) {
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 255, 0));
        TextOutW(hdc, 230, 150, L"ПОБЕДА! НАЖМИ R", 17);
    }

    DeleteObject(hPad);
    DeleteObject(hBall);
    DeleteObject(hBrick);
}

// ==========================================
// 4. WINDOWS КОД (Стандартная обвязка)
// ==========================================
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow) {
    // Правильная инициализация структуры с указанием всех полей
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;           // Иконка окна (не обязательна)
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);  // Курсор мыши
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // Цвет фона
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"Arkanoid";
    wcex.hIconSm = nullptr;         // Маленькая иконка

    RegisterClassExW(&wcex);

    RECT wr = { 0, 0, WIN_W, WIN_H };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    HWND hWnd = CreateWindowW(L"Arkanoid", L"Простой Арканоид",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, hInstance, nullptr);

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
        SetTimer(hWnd, 1, 15, NULL); // 60 FPS
        break;
    case WM_TIMER:
        UpdateGame(hWnd);
        break;
    case WM_KEYDOWN:
        if (wParam == VK_LEFT && padX > 0) padX -= 15;
        if (wParam == VK_RIGHT && padX < WIN_W - PAD_W) padX += 15;
        // Перезапуск по кнопке R
        if (wParam == 'R') { b1 = true; b2 = true; b3 = true; ballX = 300; ballY = 350; ballDX = 4; ballDY = -4; }
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
