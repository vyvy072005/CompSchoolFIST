// WindowsProject3.cpp : Псевдо-3D Раннер - УЛУЧШЕННАЯ ГРАФИКА
//

#include "framework.h"
#include "WindowsProject3.h"
#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#define MAX_LOADSTRING 100

// === НАСТРОЙКИ ЭКРАНА И ПРОЕКЦИИ ===
const int SCREEN_W = 800;
const int SCREEN_H = 600;
const float HORIZON_Y = 180.0f;
const float GROUND_Y = 520.0f;
const float FOCAL_LENGTH = 300.0f;
const float LANE_SPACING = 160.0f;

enum ObjType { TRAIN, BARRIER, COIN };

struct GameObject {
    ObjType type;
    int lane;
    float z;
    bool active;
};

struct Player {
    int lane;
    float x, y, vy;
    bool isJumping;
} player;

// === НОВЫЕ СТРУКТУРЫ ДЛЯ КРАСОТЫ ===
struct Cloud {
    float x, y, size, speed;
};

struct Building {
    float z;
    int side; // 0 = лево, 1 = право
    int height;
    int width;
    COLORREF color;
};

struct Particle {
    float x, y, vx, vy;
    float life;
    DWORD color;
};

std::vector<GameObject> objects;
std::vector<Cloud> clouds;
std::vector<Building> buildings;
std::vector<Particle> particles;

float gameSpeed = 400.0f;
float spawnTimer = 0.0f;
float globalTime = 0.0f; // Для анимаций
float tieOffset = 0.0f;  // Смещение шпал
int score = 0;
int coins = 0;
int bestScore = 0;
bool isGameOver = false;

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hWndMain;

HBITMAP hDIB;
void* pPixels = nullptr;

// Функции
ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitGame();
void Update(float dt);
void Render();
void DrawPixel(int x, int y, DWORD color);
void DrawRect(int x, int y, int w, int h, DWORD color);
void DrawEllipse(int cx, int cy, int rx, int ry, DWORD color);
void DrawFilledCircle(int cx, int cy, int r, DWORD color);
void DrawGradientRect(int x, int y, int w, int h, DWORD topColor, DWORD bottomColor);
void SpawnParticles(float x, float y, DWORD color, int count);
float Lerp(float a, float b, float t);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    srand((unsigned)time(NULL));

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT3));
    MSG msg;
    DWORD lastTime = GetTickCount();

    while (true) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) return (int)msg.wParam;
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }

        DWORD currentTime = GetTickCount();
        float dt = (currentTime - lastTime) / 1000.0f;
        if (dt > 0.1f) dt = 0.1f;
        lastTime = currentTime;

        if (!isGameOver) Update(dt);
        Render();
    }
    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT3));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;

    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = SCREEN_W;
    bmi.bmiHeader.biHeight = -SCREEN_H;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC hdcScreen = GetDC(nullptr);
    hDIB = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &pPixels, nullptr, 0);
    ReleaseDC(nullptr, hdcScreen);

    RECT rc = { 0, 0, SCREEN_W, SCREEN_H };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, TRUE);

    hWndMain = CreateWindowW(szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWndMain) return FALSE;

    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);
    SetFocus(hWndMain);

    InitGame();
    return TRUE;
}

void InitGame() {
    player.lane = 1;
    player.x = 0.0f;
    player.y = 0.0f;
    player.vy = 0.0f;
    player.isJumping = false;

    objects.clear();
    particles.clear();
    gameSpeed = 400.0f;
    spawnTimer = 0.0f;
    globalTime = 0.0f;
    score = 0;
    coins = 0;
    isGameOver = false;

    // Создаём облака
    clouds.clear();
    for (int i = 0; i < 6; i++) {
        Cloud c;
        c.x = (float)(rand() % SCREEN_W);
        c.y = (float)(30 + rand() % 100);
        c.size = 20.0f + (float)(rand() % 40);
        c.speed = 10.0f + (float)(rand() % 20);
        clouds.push_back(c);
    }

    // Создаём здания
    buildings.clear();
    for (int i = 0; i < 20; i++) {
        Building b;
        b.z = 300.0f + i * 200.0f;
        b.side = rand() % 2;
        b.height = 100 + rand() % 150;
        b.width = 80 + rand() % 60;
        int shade = 60 + rand() % 80;
        b.color = RGB(shade, shade + 20, shade + 40);
        buildings.push_back(b);
    }
}

float Lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

bool IsKeyPressed(UINT scanCode) {
    UINT vKey = MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK);
    return (GetAsyncKeyState(vKey) & 0x8000) != 0;
}

void SpawnParticles(float x, float y, DWORD color, int count) {
    for (int i = 0; i < count; i++) {
        Particle p;
        p.x = x;
        p.y = y;
        float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
        float speed = 50.0f + (float)(rand() % 150);
        p.vx = cos(angle) * speed;
        p.vy = sin(angle) * speed - 100.0f;
        p.life = 0.5f + (float)(rand() % 50) / 100.0f;
        p.color = color;
        particles.push_back(p);
    }
}

void Update(float dt) {
    globalTime += dt;
    tieOffset += gameSpeed * dt * 0.5f; // Движение шпал
    if (tieOffset > 50.0f) tieOffset -= 50.0f;

    // === УПРАВЛЕНИЕ ===
    static bool keyLeftPrev = false, keyRightPrev = false, keyUpPrev = false;

    bool keyLeft = IsKeyPressed(0x1E);
    bool keyRight = IsKeyPressed(0x20);
    bool keyUp = IsKeyPressed(0x11);

    if (keyLeft && !keyLeftPrev && player.lane > 0) player.lane--;
    if (keyRight && !keyRightPrev && player.lane < 2) player.lane++;
    if (keyUp && !keyUpPrev && !player.isJumping) {
        player.isJumping = true;
        player.vy = -500.0f;
    }

    keyLeftPrev = keyLeft;
    keyRightPrev = keyRight;
    keyUpPrev = keyUp;

    if (player.isJumping) {
        player.y += player.vy * dt;
        player.vy += 1400.0f * dt;
        if (player.y >= 0.0f) {
            player.y = 0.0f;
            player.isJumping = false;
            player.vy = 0.0f;
        }
    }

    float targetX = (player.lane - 1) * LANE_SPACING;
    player.x = Lerp(player.x, targetX, 12.0f * dt);

    // === ОБЛАКА ===
    for (auto& c : clouds) {
        c.x -= c.speed * dt;
        if (c.x < -c.size * 3) {
            c.x = SCREEN_W + c.size * 2;
            c.y = 30.0f + (float)(rand() % 100);
        }
    }

    // === ЗДАНИЯ ===
    for (auto& b : buildings) {
        b.z -= gameSpeed * dt * 0.3f;
        if (b.z < 50.0f) {
            b.z += 4000.0f;
            b.height = 100 + rand() % 150;
            b.width = 80 + rand() % 60;
            int shade = 60 + rand() % 80;
            b.color = RGB(shade, shade + 20, shade + 40);
        }
    }

    // === ЧАСТИЦЫ ===
    for (auto& p : particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.vy += 400.0f * dt;
        p.life -= dt;
    }
    particles.erase(std::remove_if(particles.begin(), particles.end(),
        [](const Particle& p) { return p.life <= 0; }), particles.end());

    // === ИГРОВАЯ ЛОГИКА ===
    gameSpeed += 12.0f * dt;
    spawnTimer -= dt;

    if (spawnTimer <= 0.0f) {
        spawnTimer = 1.2f - (gameSpeed / 3000.0f);
        if (spawnTimer < 0.4f) spawnTimer = 0.4f;

        int lane = rand() % 3;
        int typeRoll = rand() % 10;

        if (typeRoll < 4) objects.push_back({ TRAIN, lane, 2000.0f, true });
        else if (typeRoll < 7) objects.push_back({ BARRIER, lane, 2000.0f, true });
        else objects.push_back({ COIN, lane, 2000.0f, true });
    }

    for (auto& obj : objects) {
        if (!obj.active) continue;
        obj.z -= gameSpeed * dt;

        if (obj.z < 80.0f && obj.z > 20.0f && obj.lane == player.lane) {
            if (obj.type == TRAIN) {
                isGameOver = true;
                if (score > bestScore) bestScore = score;
            }
            else if (obj.type == BARRIER) {
                if (player.y > -60.0f) {
                    isGameOver = true;
                    if (score > bestScore) bestScore = score;
                }
            }
            else if (obj.type == COIN) {
                obj.active = false;
                coins++;
                score += 50;
                // Частицы при сборе монеты
                float perspective = FOCAL_LENGTH / (FOCAL_LENGTH + obj.z);
                float sx = (SCREEN_W / 2.0f) + ((obj.lane - 1) * LANE_SPACING * perspective);
                float sy = HORIZON_Y + ((GROUND_Y - HORIZON_Y) * perspective);
                SpawnParticles(sx, sy - 30.0f, RGB(255, 215, 0), 12);
            }
        }

        if (obj.z < -100.0f) {
            obj.active = false;
            if (obj.type != COIN) score += 10;
        }
    }

    objects.erase(std::remove_if(objects.begin(), objects.end(),
        [](const GameObject& o) { return !o.active; }), objects.end());

    std::sort(objects.begin(), objects.end(),
        [](const GameObject& a, const GameObject& b) { return a.z > b.z; });
}

void DrawPixel(int x, int y, DWORD color) {
    if (x >= 0 && x < SCREEN_W && y >= 0 && y < SCREEN_H) {
        ((DWORD*)pPixels)[y * SCREEN_W + x] = color;
    }
}

void DrawRect(int x, int y, int w, int h, DWORD color) {
    for (int j = y; j < y + h; j++) {
        for (int i = x; i < x + w; i++) {
            DrawPixel(i, j, color);
        }
    }
}

void DrawFilledCircle(int cx, int cy, int r, DWORD color) {
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                DrawPixel(cx + x, cy + y, color);
            }
        }
    }
}

void DrawEllipse(int cx, int cy, int rx, int ry, DWORD color) {
    for (int y = -ry; y <= ry; y++) {
        for (int x = -rx; x <= rx; x++) {
            if ((x * x * ry * ry) + (y * y * rx * rx) <= (rx * rx * ry * ry)) {
                DrawPixel(cx + x, cy + y, color);
            }
        }
    }
}

void DrawGradientRect(int x, int y, int w, int h, DWORD topColor, DWORD bottomColor) {
    BYTE r1 = GetRValue(topColor), g1 = GetGValue(topColor), b1 = GetBValue(topColor);
    BYTE r2 = GetRValue(bottomColor), g2 = GetGValue(bottomColor), b2 = GetBValue(bottomColor);
    for (int j = 0; j < h; j++) {
        float t = (float)j / h;
        BYTE r = (BYTE)(r1 + (r2 - r1) * t);
        BYTE g = (BYTE)(g1 + (g2 - g1) * t);
        BYTE b = (BYTE)(b1 + (b2 - b1) * t);
        for (int i = 0; i < w; i++) {
            DrawPixel(x + i, y + j, RGB(r, g, b));
        }
    }
}

void Render() {
    memset(pPixels, 0, SCREEN_W * SCREEN_H * 4);

    // 1. НЕБО С ГРАДИЕНТОМ (рассвет/закат)
    for (int y = 0; y < (int)HORIZON_Y; y++) {
        float t = y / HORIZON_Y;
        // От тёмно-синего к оранжево-розовому у горизонта
        BYTE r = (BYTE)(80 + t * 175);
        BYTE g = (BYTE)(120 + t * 100);
        BYTE b = (BYTE)(220 - t * 100);
        DWORD color = RGB(r, g, b);
        for (int x = 0; x < SCREEN_W; x++) {
            DrawPixel(x, y, color);
        }
    }

    // 2. СОЛНЦЕ
    int sunX = 650, sunY = 80;
    // Свечение
    for (int r = 60; r > 0; r -= 2) {
        BYTE alpha = (BYTE)(255 - r * 3);
        DWORD glowColor = RGB(255, 200 + (60 - r), 100 + (60 - r));
        DrawFilledCircle(sunX, sunY, r, glowColor);
    }
    // Само солнце
    DrawFilledCircle(sunX, sunY, 30, RGB(255, 240, 180));

    // 3. ОБЛАКА
    for (const auto& c : clouds) {
        int cx = (int)c.x, cy = (int)c.y;
        int s = (int)c.size;
        // Несколько эллипсов для пушистости
        DrawEllipse(cx, cy, s, s / 2, RGB(255, 255, 255));
        DrawEllipse(cx - s / 2, cy + 5, s / 2, s / 3, RGB(255, 255, 255));
        DrawEllipse(cx + s / 2, cy + 5, s / 2, s / 3, RGB(255, 255, 255));
        DrawEllipse(cx, cy - 5, s / 2, s / 3, RGB(255, 255, 255));
        // Тень снизу
        DrawEllipse(cx, cy + s / 3, s, s / 4, RGB(220, 220, 235));
    }

    // 4. ЗДАНИЯ НА ГОРИЗОНТЕ
    for (const auto& b : buildings) {
        if (b.z < 50.0f) continue;
        float perspective = FOCAL_LENGTH / (FOCAL_LENGTH + b.z);
        float screenX;
        if (b.side == 0) {
            screenX = (SCREEN_W / 2.0f) - (350.0f * perspective);
        }
        else {
            screenX = (SCREEN_W / 2.0f) + (350.0f * perspective);
        }
        float screenY = HORIZON_Y;
        float w = b.width * perspective;
        float h = b.height * perspective;
        if (w < 2 || h < 2) continue;

        int bx = (int)(screenX - w / 2);
        int by = (int)(screenY - h);
        int bw = (int)w;
        int bh = (int)h;

        // Здание
        DrawRect(bx, by, bw, bh, b.color);
        // Крыша темнее
        DrawRect(bx, by, bw, 3, RGB(30, 30, 50));
        // Окна (сетка)
        int windowSize = max(2, (int)(w / 6));
        int windowGap = windowSize + 2;
        for (int wy = by + 8; wy < by + bh - windowSize; wy += windowGap) {
            for (int wx = bx + 4; wx < bx + bw - windowSize; wx += windowGap) {
                // Некоторые окна светятся
                bool lit = ((wx * 7 + wy * 13) % 5) < 2;
                DWORD winColor = lit ? RGB(255, 230, 150) : RGB(40, 50, 70);
                DrawRect(wx, wy, windowSize, windowSize, winColor);
            }
        }
    }

    // 5. ЗЕМЛЯ С ТЕКСТУРОЙ
    for (int y = (int)HORIZON_Y; y < SCREEN_H; y++) {
        float t = (float)(y - HORIZON_Y) / (SCREEN_H - HORIZON_Y);
        // Градиент от светло-серого (вдали) к тёмному (вблизи)
        BYTE base = (BYTE)(120 - t * 60);
        DWORD roadColor = RGB(base, base, base + 10);

        // Трава по бокам
        bool isGrass = false;
        float roadHalfWidth = 280.0f * (1.0f - t * 0.6f);
        float centerX = SCREEN_W / 2.0f;
        for (int x = 0; x < SCREEN_W; x++) {
            if (abs(x - centerX) > roadHalfWidth) {
                // Трава с полосками
                BYTE grassShade = (BYTE)(60 + ((x + y) % 8) * 3);
                DrawPixel(x, y, RGB(40, grassShade, 30));
                isGrass = true;
            }
        }
        if (!isGrass) {
            for (int x = (int)(centerX - roadHalfWidth); x < (int)(centerX + roadHalfWidth); x++) {
                DrawPixel(x, y, roadColor);
            }
        }
    }

    // 6. ДВИЖУЩИЕСЯ ШПАЛЫ
    for (float z = 10.0f; z < 2000.0f; z += 50.0f) {
        float adjustedZ = z - tieOffset;
        if (adjustedZ < 10.0f) adjustedZ += 2000.0f;
        float perspective = FOCAL_LENGTH / (FOCAL_LENGTH + adjustedZ);
        float screenY = HORIZON_Y + ((GROUND_Y - HORIZON_Y) * perspective);
        float halfWidth = 240.0f * perspective;
        float centerX = SCREEN_W / 2.0f;
        int tieH = max(1, (int)(4 * perspective));
        int tieY = (int)screenY;

        // Шпала (тёмно-коричневая)
        DrawRect((int)(centerX - halfWidth), tieY, (int)(halfWidth * 2), tieH,
            RGB(80, 50, 30));
    }

    // 7. ЛИНИИ ПОЛОС
    for (int i = -1; i <= 2; i++) {
        float laneX = (i - 0.5f) * LANE_SPACING;
        float p1x = (SCREEN_W / 2) + laneX;
        float p1y = GROUND_Y;
        float p2x = (SCREEN_W / 2);
        float p2y = HORIZON_Y;

        int steps = 100;
        for (int s = 0; s <= steps; s++) {
            float t = s / (float)steps;
            int px = (int)(p1x + (p2x - p1x) * t);
            int py = (int)(p1y + (p2y - p1y) * t);
            // Жёлтые пунктирные линии
            bool dash = ((int)(s * 2 + globalTime * 10) % 4) < 2;
            if (dash) {
                DrawRect(px - 1, py, 2, 2, RGB(255, 220, 50));
            }
        }
    }

    // 8. ОБЪЕКТЫ (От дальних к ближним)
    for (const auto& obj : objects) {
        if (!obj.active || obj.z < 10.0f) continue;

        float perspective = FOCAL_LENGTH / (FOCAL_LENGTH + obj.z);
        float screenX = (SCREEN_W / 2.0f) + ((obj.lane - 1) * LANE_SPACING * perspective);
        float screenY = HORIZON_Y + ((GROUND_Y - HORIZON_Y) * perspective);

        float w = 0.0f, h = 0.0f;

        if (obj.type == TRAIN) {
            w = 110.0f * perspective;
            h = 150.0f * perspective;

            // Тень под поездом
            DrawEllipse((int)screenX, (int)screenY, (int)(w * 0.6f), (int)(h * 0.1f), RGB(0, 0, 0));

            // Корпус поезда (градиент)
            int trainX = (int)(screenX - w / 2);
            int trainY = (int)(screenY - h);
            int trainW = (int)w;
            int trainH = (int)h;
            DrawGradientRect(trainX, trainY, trainW, trainH, RGB(220, 60, 60), RGB(140, 30, 30));

            // Крыша (тёмная)
            DrawRect(trainX, trainY, trainW, (int)(h * 0.1f), RGB(60, 20, 20));

            // Окна (светящиеся)
            int winW = (int)(w * 0.25f);
            int winH = (int)(h * 0.25f);
            int winY = trainY + (int)(h * 0.2f);
            DrawRect(trainX + (int)(w * 0.1f), winY, winW, winH, RGB(180, 220, 255));
            DrawRect(trainX + (int)(w * 0.65f), winY, winW, winH, RGB(180, 220, 255));
            // Блик на окнах
            DrawRect(trainX + (int)(w * 0.12f), winY + 2, winW / 3, winH / 3, RGB(255, 255, 255));
            DrawRect(trainX + (int)(w * 0.67f), winY + 2, winW / 3, winH / 3, RGB(255, 255, 255));

            // Жёлтая полоса
            DrawRect(trainX, trainY + (int)(h * 0.55f), trainW, (int)(h * 0.08f), RGB(255, 200, 0));

            // Дверь посередине
            DrawRect(trainX + (int)(w * 0.4f), trainY + (int)(h * 0.2f),
                (int)(w * 0.2f), (int)(h * 0.7f), RGB(100, 20, 20));

            // Колёса
            int wheelR = (int)(h * 0.08f);
            DrawFilledCircle(trainX + (int)(w * 0.2f), (int)screenY - wheelR, wheelR, RGB(30, 30, 30));
            DrawFilledCircle(trainX + (int)(w * 0.8f), (int)screenY - wheelR, wheelR, RGB(30, 30, 30));
            // Блик на колёсах
            DrawFilledCircle(trainX + (int)(w * 0.2f) - 1, (int)screenY - wheelR - 1, wheelR / 3, RGB(100, 100, 100));
            DrawFilledCircle(trainX + (int)(w * 0.8f) - 1, (int)screenY - wheelR - 1, wheelR / 3, RGB(100, 100, 100));

        }
        else if (obj.type == BARRIER) {
            w = 90.0f * perspective;
            h = 50.0f * perspective;

            // Тень
            DrawEllipse((int)screenX, (int)screenY, (int)(w * 0.5f), (int)(h * 0.15f), RGB(0, 0, 0));

            // Полосатый барьер (красно-белый)
            int barX = (int)(screenX - w / 2);
            int barY = (int)(screenY - h);
            int barW = (int)w;
            int barH = (int)h;
            int stripeW = max(2, barW / 6);
            for (int i = 0; i < 6; i++) {
                DWORD stripeColor = (i % 2 == 0) ? RGB(255, 80, 30) : RGB(255, 255, 255);
                DrawRect(barX + i * stripeW, barY, stripeW, barH, stripeColor);
            }
            // Обводка
            DrawRect(barX, barY, barW, 2, RGB(80, 30, 10));
            DrawRect(barX, barY + barH - 2, barW, 2, RGB(80, 30, 10));

        }
        else if (obj.type == COIN) {
            float coinSize = 28.0f * perspective;
            h = coinSize;

            // Вращение монеты (ширина меняется синусоидально)
            float rotation = sin(globalTime * 6.0f + obj.z * 0.01f);
            float coinW = coinSize * fabs(rotation);
            if (coinW < 2) coinW = 2;

            // Парение
            float floatY = screenY - h - (20.0f * perspective) + (10.0f * perspective * sin(globalTime * 3.0f));

            // Свечение вокруг монеты
            DrawEllipse((int)screenX, (int)floatY, (int)(coinW * 1.5f), (int)(coinSize * 1.2f),
                RGB(255, 240, 150));

            // Сама монета
            DrawEllipse((int)screenX, (int)floatY, (int)coinW, (int)coinSize, RGB(255, 200, 0));
            // Внутренний круг
            DrawEllipse((int)screenX, (int)floatY, (int)(coinW * 0.7f), (int)(coinSize * 0.7f),
                RGB(255, 230, 100));
            // Блик
            if (rotation > 0) {
                DrawEllipse((int)(screenX - coinW * 0.3f), (int)(floatY - coinSize * 0.3f),
                    (int)(coinW * 0.2f), (int)(coinSize * 0.2f), RGB(255, 255, 220));
            }
        }
    }

    // 9. ИГРОК С АНИМАЦИЕЙ БЕГА
    float playerPerspective = FOCAL_LENGTH / (FOCAL_LENGTH + 50.0f);
    float playerScreenX = (SCREEN_W / 2.0f) + (player.x * playerPerspective);
    float playerScreenY = GROUND_Y + player.y;

    float pW = 45.0f * playerPerspective;
    float pH = 75.0f * playerPerspective;

    // Тень
    float shadowScale = 1.0f + player.y / 200.0f; // Тень уменьшается при прыжке
    DrawEllipse((int)playerScreenX, (int)GROUND_Y,
        (int)(pW * 0.8f * shadowScale), (int)(pH * 0.15f * shadowScale), RGB(0, 0, 0));

    // Анимация бега (только если на земле)
    float runPhase = player.isJumping ? 0.0f : sin(globalTime * 15.0f);
    float legOffset = runPhase * 8.0f * playerPerspective;
    float armOffset = -runPhase * 6.0f * playerPerspective;

    // Ноги
    DrawRect((int)(playerScreenX - pW * 0.25f), (int)(playerScreenY - pH * 0.35f - legOffset),
        (int)(pW * 0.2f), (int)(pH * 0.35f + legOffset), RGB(50, 50, 100));
    DrawRect((int)(playerScreenX + pW * 0.05f), (int)(playerScreenY - pH * 0.35f + legOffset),
        (int)(pW * 0.2f), (int)(pH * 0.35f - legOffset), RGB(50, 50, 100));

    // Ботинки
    DrawRect((int)(playerScreenX - pW * 0.3f), (int)(playerScreenY - 5.0f - legOffset),
        (int)(pW * 0.3f), (int)(pH * 0.08f), RGB(200, 50, 50));
    DrawRect((int)(playerScreenX), (int)(playerScreenY - 5.0f + legOffset),
        (int)(pW * 0.3f), (int)(pH * 0.08f), RGB(200, 50, 50));

    // Тело (толстовка)
    DrawRect((int)(playerScreenX - pW * 0.4f), (int)(playerScreenY - pH * 0.75f),
        (int)(pW * 0.8f), (int)(pH * 0.4f), RGB(40, 100, 220));
    // Капюшон/воротник
    DrawRect((int)(playerScreenX - pW * 0.3f), (int)(playerScreenY - pH * 0.75f),
        (int)(pW * 0.6f), (int)(pH * 0.08f), RGB(30, 80, 180));

    // Руки
    DrawRect((int)(playerScreenX - pW * 0.55f), (int)(playerScreenY - pH * 0.7f + armOffset),
        (int)(pW * 0.18f), (int)(pH * 0.3f), RGB(40, 100, 220));
    DrawRect((int)(playerScreenX + pW * 0.37f), (int)(playerScreenY - pH * 0.7f - armOffset),
        (int)(pW * 0.18f), (int)(pH * 0.3f), RGB(40, 100, 220));

    // Голова
    DrawFilledCircle((int)playerScreenX, (int)(playerScreenY - pH * 0.85f),
        (int)(pW * 0.35f), RGB(255, 210, 170));

    // Волосы
    DrawEllipse((int)playerScreenX, (int)(playerScreenY - pH * 0.95f),
        (int)(pW * 0.38f), (int)(pW * 0.2f), RGB(60, 30, 10));

    // Кепка
    DrawRect((int)(playerScreenX - pW * 0.4f), (int)(playerScreenY - pH * 1.05f),
        (int)(pW * 0.8f), (int)(pW * 0.25f), RGB(220, 40, 40));
    // Козырёк
    DrawRect((int)(playerScreenX + pW * 0.1f), (int)(playerScreenY - pH * 0.9f),
        (int)(pW * 0.4f), (int)(pW * 0.1f), RGB(180, 30, 30));

    // Глаза
    DrawFilledCircle((int)(playerScreenX - pW * 0.12f), (int)(playerScreenY - pH * 0.85f),
        (int)(pW * 0.05f), RGB(0, 0, 0));
    DrawFilledCircle((int)(playerScreenX + pW * 0.12f), (int)(playerScreenY - pH * 0.85f),
        (int)(pW * 0.05f), RGB(0, 0, 0));

    // 10. ЧАСТИЦЫ
    for (const auto& p : particles) {
        int size = max(1, (int)(4 * p.life));
        DrawFilledCircle((int)p.x, (int)p.y, size, p.color);
    }

    // 11. КОПИРОВАНИЕ НА ЭКРАН
    HDC hdc = GetDC(hWndMain);
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(hdcMem, hDIB);
    BitBlt(hdc, 0, 0, SCREEN_W, SCREEN_H, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hOldBmp);
    DeleteDC(hdcMem);
    ReleaseDC(hWndMain, hdc);

    // 12. КРАСИВЫЙ HUD
    HDC hdcText = GetDC(hWndMain);
    SetBkMode(hdcText, TRANSPARENT);

    // Панель очков (полупрозрачная имитация)
    HBRUSH hPanelBrush = CreateSolidBrush(RGB(20, 20, 40));
    RECT rcPanel = { 10, 10, 280, 110 };
    FillRect(hdcText, &rcPanel, hPanelBrush);
    // Рамка
    HPEN hBorderPen = CreatePen(PS_SOLID, 2, RGB(255, 215, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdcText, hBorderPen);
    SelectObject(hdcText, GetStockObject(NULL_BRUSH));
    Rectangle(hdcText, 10, 10, 280, 110);
    SelectObject(hdcText, hOldPen);
    DeleteObject(hBorderPen);
    DeleteObject(hPanelBrush);

    HFONT hFont = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdcText, hFont);

    SetTextColor(hdcText, RGB(255, 255, 255));
    RECT rcScore = { 25, 20, 270, 50 };
    DrawTextW(hdcText, (L"Очки: " + std::to_wstring(score)).c_str(), -1, &rcScore, DT_LEFT);

    SetTextColor(hdcText, RGB(255, 215, 0));
    RECT rcCoin = { 25, 50, 270, 80 };
    DrawTextW(hdcText, (L"Монеты: " + std::to_wstring(coins)).c_str(), -1, &rcCoin, DT_LEFT);

    SetTextColor(hdcText, RGB(180, 180, 200));
    RECT rcBest = { 25, 80, 270, 105 };
    DrawTextW(hdcText, (L"Рекорд: " + std::to_wstring(bestScore)).c_str(), -1, &rcBest, DT_LEFT);

    // Скорость справа
    HBRUSH hSpeedPanel = CreateSolidBrush(RGB(20, 20, 40));
    RECT rcSpeedPanel = { SCREEN_W - 220, 10, SCREEN_W - 10, 60 };
    FillRect(hdcText, &rcSpeedPanel, hSpeedPanel);
    HPEN hSpeedBorder = CreatePen(PS_SOLID, 2, RGB(100, 200, 255));
    SelectObject(hdcText, hSpeedBorder);
    SelectObject(hdcText, GetStockObject(NULL_BRUSH));
    Rectangle(hdcText, SCREEN_W - 220, 10, SCREEN_W - 10, 60);
    SelectObject(hdcText, (HPEN)GetStockObject(BLACK_PEN));
    DeleteObject(hSpeedBorder);
    DeleteObject(hSpeedPanel);

    SetTextColor(hdcText, RGB(100, 200, 255));
    RECT rcSpeed = { SCREEN_W - 210, 20, SCREEN_W - 20, 50 };
    DrawTextW(hdcText, (std::to_wstring((int)(gameSpeed / 10)) + L" км/ч").c_str(), -1, &rcSpeed, DT_CENTER);

    if (isGameOver) {
        // Затемнение
        HBRUSH hOverlay = CreateSolidBrush(RGB(0, 0, 0));
        for (int oy = 0; oy < SCREEN_H; oy += 2) {
            RECT rcLine = { 0, oy, SCREEN_W, oy + 1 };
            FillRect(hdcText, &rcLine, hOverlay);
        }
        DeleteObject(hOverlay);

        HFONT hBigFont = CreateFontW(72, 0, 0, 0, FW_HEAVY, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Arial");
        SelectObject(hdcText, hBigFont);
        SetTextColor(hdcText, RGB(255, 50, 50));
        RECT rcOver = { 0, SCREEN_H / 2 - 100, SCREEN_W, SCREEN_H / 2 - 20 };
        DrawTextW(hdcText, L"GAME OVER", -1, &rcOver, DT_CENTER | DT_VCENTER);

        HFONT hMedFont = CreateFontW(28, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
            OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        SelectObject(hdcText, hMedFont);
        SetTextColor(hdcText, RGB(255, 215, 0));
        RECT rcFinal = { 0, SCREEN_H / 2, SCREEN_W, SCREEN_H / 2 + 40 };
        DrawTextW(hdcText, (L"Очки: " + std::to_wstring(score)).c_str(), -1, &rcFinal, DT_CENTER | DT_VCENTER);

        SetTextColor(hdcText, RGB(255, 255, 255));
        RECT rcRestart = { 0, SCREEN_H / 2 + 50, SCREEN_W, SCREEN_H / 2 + 90 };
        DrawTextW(hdcText, L"Нажми R чтобы начать заново", -1, &rcRestart, DT_CENTER | DT_VCENTER);

        DeleteObject(hBigFont);
        DeleteObject(hMedFont);
    }

    SelectObject(hdcText, hOldFont);
    DeleteObject(hFont);
    ReleaseDC(hWndMain, hdcText);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_KEYDOWN:
    {
        UINT scanCode = (lParam >> 16) & 0xFF;
        if (scanCode == 0x13 && isGameOver) {
            InitGame();
        }
        break;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        if (wmId == IDM_ABOUT) {
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, [](HWND hDlg, UINT msg, WPARAM wp, LPARAM) -> INT_PTR {
                if (msg == WM_COMMAND && (LOWORD(wp) == IDOK || LOWORD(wp) == IDCANCEL)) {
                    EndDialog(hDlg, LOWORD(wp)); return TRUE;
                }
                return FALSE;
                });
        }
        else if (wmId == IDM_EXIT) {
            DestroyWindow(hWnd);
        }
        break;
    }
    case WM_DESTROY:
        if (hDIB) DeleteObject(hDIB);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
