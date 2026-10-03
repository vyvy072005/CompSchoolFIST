// WindowsProject3.cpp : Приложение для рисования (Paint-like)
//

#include "framework.h"
#include "WindowsProject3.h"
#include <commdlg.h>
#include <vector>
#include <string>
#include <queue>
#include <cmath>

#define MAX_LOADSTRING 100

// === НАСТРОЙКИ ===
const int WIN_W = 1000;
const int WIN_H = 700;
const int PANEL_W = 140;          // Ширина панели инструментов
const int CANVAS_X = PANEL_W + 10;
const int CANVAS_Y = 10;
const int CANVAS_W = WIN_W - PANEL_W - 20;
const int CANVAS_H = WIN_H - 20;
const int MAX_UNDO = 10;

// === ИНСТРУМЕНТЫ ===
enum Tool { TOOL_PENCIL, TOOL_LINE, TOOL_RECT, TOOL_ELLIPSE, TOOL_ERASER, TOOL_FILL };

// === СОСТОЯНИЕ ===
struct AppState {
    Tool currentTool;
    COLORREF currentColor;
    int brushSize;
    bool isDrawing;
    int startX, startY;       // Начало рисования фигуры
    int lastX, lastY;         // Последняя позиция мыши
    HBITMAP hCanvas;          // Основной холст
    HBITMAP hBackBuffer;      // Для rubber-banding
    void* canvasPixels;
    void* backPixels;
    std::vector<std::vector<DWORD>> undoStack; // Снимки холста для Undo
} state;

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hWndMain;

// Палитра цветов
const COLORREF paletteColors[] = {
    RGB(0, 0, 0),       RGB(128, 128, 128), RGB(255, 255, 255),
    RGB(255, 0, 0),     RGB(255, 128, 0),   RGB(255, 255, 0),
    RGB(0, 200, 0),     RGB(0, 150, 255),   RGB(100, 50, 200),
    RGB(255, 100, 180), RGB(139, 69, 19),   RGB(0, 128, 128)
};
const int PALETTE_COUNT = 12;

// Толщины кисти
const int brushSizes[] = { 2, 4, 8, 14, 22 };
const int BRUSH_COUNT = 5;

// Функции
ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitApp();
void ClearCanvas();
void SaveUndoState();
void Undo();
void DrawPixelToCanvas(int x, int y, COLORREF color);
void DrawThickPixel(int cx, int cy, int r, COLORREF color);
void DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness);
void FloodFill(int x, int y, COLORREF newColor);
void DrawToolIcon(HDC hdc, int x, int y, int w, int h, Tool tool, bool selected);
void Render();
bool SaveToBmp(const std::wstring& filename);
bool LoadFromBmp(const std::wstring& filename);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow) {
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT3, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT3));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex = { sizeof(WNDCLASSEX) };
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT3));
    wcex.hCursor = LoadCursor(nullptr, IDC_CROSS);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;

    // Создаём DIB для холста
    BITMAPINFO bmi = { 0 };
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = CANVAS_W;
    bmi.bmiHeader.biHeight = -CANVAS_H;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    HDC hdcScreen = GetDC(nullptr);
    state.hCanvas = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &state.canvasPixels, nullptr, 0);
    state.hBackBuffer = CreateDIBSection(hdcScreen, &bmi, DIB_RGB_COLORS, &state.backPixels, nullptr, 0);
    ReleaseDC(nullptr, hdcScreen);

    RECT rc = { 0, 0, WIN_W, WIN_H };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, TRUE);

    hWndMain = CreateWindowW(szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWndMain) return FALSE;

    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);

    InitApp();
    return TRUE;
}

void InitApp() {
    state.currentTool = TOOL_PENCIL;
    state.currentColor = RGB(0, 0, 0);
    state.brushSize = 4;
    state.isDrawing = false;
    state.startX = state.startY = state.lastX = state.lastY = 0;
    state.undoStack.clear();
    ClearCanvas();
}

void ClearCanvas() {
    DWORD* pixels = (DWORD*)state.canvasPixels;
    for (int i = 0; i < CANVAS_W * CANVAS_H; i++) {
        pixels[i] = RGB(255, 255, 255); // Белый фон
    }
    InvalidateRect(hWndMain, nullptr, FALSE);
}

void SaveUndoState() {
    DWORD* pixels = (DWORD*)state.canvasPixels;
    std::vector<DWORD> snapshot(CANVAS_W * CANVAS_H);
    memcpy(snapshot.data(), pixels, CANVAS_W * CANVAS_H * sizeof(DWORD));
    state.undoStack.push_back(std::move(snapshot));
    if ((int)state.undoStack.size() > MAX_UNDO) {
        state.undoStack.erase(state.undoStack.begin());
    }
}

void Undo() {
    if (state.undoStack.empty()) return;
    auto& snapshot = state.undoStack.back();
    DWORD* pixels = (DWORD*)state.canvasPixels;
    memcpy(pixels, snapshot.data(), CANVAS_W * CANVAS_H * sizeof(DWORD));
    state.undoStack.pop_back();
    InvalidateRect(hWndMain, nullptr, FALSE);
}

void DrawPixelToCanvas(int x, int y, COLORREF color) {
    if (x < 0 || x >= CANVAS_W || y < 0 || y >= CANVAS_H) return;
    ((DWORD*)state.canvasPixels)[y * CANVAS_W + x] = color;
}

void DrawThickPixel(int cx, int cy, int r, COLORREF color) {
    if (r <= 1) {
        DrawPixelToCanvas(cx, cy, color);
        return;
    }
    for (int y = -r; y <= r; y++) {
        for (int x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                DrawPixelToCanvas(cx + x, cy + y, color);
            }
        }
    }
}

void DrawLine(int x1, int y1, int x2, int y2, COLORREF color, int thickness) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;
    int r = thickness / 2;

    while (true) {
        DrawThickPixel(x1, y1, r, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

void FloodFill(int x, int y, COLORREF newColor) {
    if (x < 0 || x >= CANVAS_W || y < 0 || y >= CANVAS_H) return;
    DWORD* pixels = (DWORD*)state.canvasPixels;
    COLORREF targetColor = pixels[y * CANVAS_W + x];
    if (targetColor == newColor) return;

    std::queue<std::pair<int, int>> q;
    q.push({ x, y });

    while (!q.empty()) {
        // ✅ ИСПРАВЛЕНО: вместо auto [cx, cy] используем обычный код
        std::pair<int, int> front = q.front();
        int cx = front.first;
        int cy = front.second;
        q.pop();

        if (cx < 0 || cx >= CANVAS_W || cy < 0 || cy >= CANVAS_H) continue;
        int idx = cy * CANVAS_W + cx;
        if (pixels[idx] != targetColor) continue;
        pixels[idx] = newColor;
        q.push({ cx + 1, cy });
        q.push({ cx - 1, cy });
        q.push({ cx, cy + 1 });
        q.push({ cx, cy - 1 });
    }
}

bool SaveToBmp(const std::wstring& filename) {
    BITMAPFILEHEADER bfh = { 0 };
    BITMAPINFOHEADER bih = { 0 };
    bfh.bfType = 0x4D42; // 'BM'
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biWidth = CANVAS_W;
    bih.biHeight = -CANVAS_H;
    bih.biPlanes = 1;
    bih.biBitCount = 32;
    bih.biCompression = BI_RGB;
    bfh.bfSize = bfh.bfOffBits + CANVAS_W * CANVAS_H * 4;

    HANDLE hFile = CreateFileW(filename.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, 0, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) return false;
    DWORD written;
    WriteFile(hFile, &bfh, sizeof(bfh), &written, nullptr);
    WriteFile(hFile, &bih, sizeof(bih), &written, nullptr);
    WriteFile(hFile, state.canvasPixels, CANVAS_W * CANVAS_H * 4, &written, nullptr);
    CloseHandle(hFile);
    return true;
}

bool LoadFromBmp(const std::wstring& filename) {
    HBITMAP hBmp = (HBITMAP)LoadImageW(nullptr, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBmp) return false;

    BITMAP bm;
    GetObject(hBmp, sizeof(bm), &bm);

    HDC hdcScreen = GetDC(nullptr);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hBmp);

    // Копируем в наш canvas (центрируем, если размеры разные)
    DWORD* pixels = (DWORD*)state.canvasPixels;
    for (int i = 0; i < CANVAS_W * CANVAS_H; i++) pixels[i] = RGB(255, 255, 255);

    int copyW = min(bm.bmWidth, CANVAS_W);
    int copyH = min(bm.bmHeight, CANVAS_H);
    int offsetX = (CANVAS_W - copyW) / 2;
    int offsetY = (CANVAS_H - copyH) / 2;

    for (int y = 0; y < copyH; y++) {
        for (int x = 0; x < copyW; x++) {
            COLORREF c = GetPixel(hdcMem, x, y);
            if (c != CLR_INVALID) {
                pixels[(offsetY + y) * CANVAS_W + (offsetX + x)] = c;
            }
        }
    }

    SelectObject(hdcMem, hOld);
    DeleteDC(hdcMem);
    DeleteObject(hBmp);
    ReleaseDC(nullptr, hdcScreen);
    InvalidateRect(hWndMain, nullptr, FALSE);
    return true;
}

void DrawToolIcon(HDC hdc, int x, int y, int w, int h, Tool tool, bool selected) {
    // Фон
    HBRUSH hBg = CreateSolidBrush(selected ? RGB(100, 150, 220) : RGB(230, 230, 240));
    RECT rc = { x, y, x + w, y + h };
    FillRect(hdc, &rc, hBg);
    DeleteObject(hBg);

    // Рамка
    HPEN hPen = CreatePen(PS_SOLID, selected ? 2 : 1, selected ? RGB(50, 100, 200) : RGB(150, 150, 170));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    SelectObject(hdc, GetStockObject(NULL_BRUSH));
    Rectangle(hdc, x, y, x + w, y + h);
    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);

    // Иконка
    int cx = x + w / 2, cy = y + h / 2;
    HPEN hDrawPen = CreatePen(PS_SOLID, 2, RGB(30, 30, 30));
    HPEN hOldP2 = (HPEN)SelectObject(hdc, hDrawPen);

    switch (tool) {
    case TOOL_PENCIL:
        MoveToEx(hdc, cx - 10, cy + 10, nullptr);
        LineTo(hdc, cx + 8, cy - 8);
        MoveToEx(hdc, cx + 8, cy - 8, nullptr);
        LineTo(hdc, cx + 12, cy - 12);
        break;
    case TOOL_LINE:
        MoveToEx(hdc, cx - 12, cy + 10, nullptr);
        LineTo(hdc, cx + 12, cy - 10);
        break;
    case TOOL_RECT:
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, cx - 12, cy - 8, cx + 12, cy + 8);
        break;
    case TOOL_ELLIPSE:
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Ellipse(hdc, cx - 12, cy - 8, cx + 12, cy + 8);
        break;
    case TOOL_ERASER:
        SelectObject(hdc, CreateSolidBrush(RGB(255, 200, 200)));
        Rectangle(hdc, cx - 10, cy - 8, cx + 10, cy + 8);
        DeleteObject((HBRUSH)GetCurrentObject(hdc, OBJ_BRUSH));
        break;
    case TOOL_FILL:
        // Ведро
        MoveToEx(hdc, cx - 8, cy - 6, nullptr);
        LineTo(hdc, cx + 8, cy - 6);
        LineTo(hdc, cx + 6, cy + 8);
        LineTo(hdc, cx - 6, cy + 8);
        LineTo(hdc, cx - 8, cy - 6);
        // Капля
        SelectObject(hdc, CreateSolidBrush(RGB(50, 150, 255)));
        Ellipse(hdc, cx + 6, cy - 10, cx + 12, cy - 4);
        DeleteObject((HBRUSH)GetCurrentObject(hdc, OBJ_BRUSH));
        break;
    }
    SelectObject(hdc, hOldP2);
    DeleteObject(hDrawPen);
}

void Render() {
    HDC hdc = GetDC(hWndMain);
    RECT rcClient;
    GetClientRect(hWndMain, &rcClient);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    // === ФОН ОКНА ===
    HBRUSH hBg = CreateSolidBrush(RGB(220, 220, 230));
    FillRect(hdcMem, &rcClient, hBg);
    DeleteObject(hBg);

    // === ПАНЕЛЬ ИНСТРУМЕНТОВ ===
    RECT rcPanel = { 0, 0, PANEL_W, WIN_H };
    HBRUSH hPanel = CreateSolidBrush(RGB(240, 240, 245));
    FillRect(hdcMem, &rcPanel, hPanel);
    DeleteObject(hPanel);

    // Заголовок панели
    HFONT hTitleFont = CreateFontW(16, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdcMem, hTitleFont);
    SetBkMode(hdcMem, TRANSPARENT);
    SetTextColor(hdcMem, RGB(50, 50, 80));
    RECT rcTitle = { 10, 8, PANEL_W - 10, 30 };
    DrawTextW(hdcMem, L"🎨 Инструменты", -1, &rcTitle, DT_LEFT | DT_VCENTER);

    // Инструменты (2 колонки)
    int iconW = 55, iconH = 40;
    int iconX1 = 8, iconX2 = 8 + iconW + 4;
    int iconY = 38;
    Tool tools[] = { TOOL_PENCIL, TOOL_LINE, TOOL_RECT, TOOL_ELLIPSE, TOOL_ERASER, TOOL_FILL };
    for (int i = 0; i < 6; i++) {
        int x = (i % 2 == 0) ? iconX1 : iconX2;
        int y = iconY + (i / 2) * (iconH + 4);
        DrawToolIcon(hdcMem, x, y, iconW, iconH, tools[i], state.currentTool == tools[i]);
    }

    // === ЦВЕТ И РАЗМЕР ===
    int infoY = iconY + 3 * (iconH + 4) + 10;

    // Текущий цвет
    SetTextColor(hdcMem, RGB(50, 50, 80));
    RECT rcColorLabel = { 10, infoY, PANEL_W - 10, infoY + 18 };
    DrawTextW(hdcMem, L"Цвет:", -1, &rcColorLabel, DT_LEFT);

    RECT rcColorBox = { 10, infoY + 20, 60, infoY + 50 };
    HBRUSH hColorBox = CreateSolidBrush(state.currentColor);
    FillRect(hdcMem, &rcColorBox, hColorBox);
    DeleteObject(hColorBox);
    HPEN hColorBorder = CreatePen(PS_SOLID, 2, RGB(50, 50, 50));
    HPEN hOldPen = (HPEN)SelectObject(hdcMem, hColorBorder);
    SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
    Rectangle(hdcMem, 10, infoY + 20, 60, infoY + 50);
    SelectObject(hdcMem, hOldPen);
    DeleteObject(hColorBorder);

    // Кнопка выбора цвета
    RECT rcPickBtn = { 65, infoY + 20, PANEL_W - 10, infoY + 50 };
    HBRUSH hPickBtn = CreateSolidBrush(RGB(200, 200, 220));
    FillRect(hdcMem, &rcPickBtn, hPickBtn);
    DeleteObject(hPickBtn);
    SelectObject(hdcMem, hColorBorder);
    Rectangle(hdcMem, 65, infoY + 20, PANEL_W - 10, infoY + 50);
    SetTextColor(hdcMem, RGB(30, 30, 30));
    HFONT hSmallFont = CreateFontW(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    SelectObject(hdcMem, hSmallFont);
    RECT rcPickText = { 65, infoY + 20, PANEL_W - 10, infoY + 50 };
    DrawTextW(hdcMem, L"Другой", -1, &rcPickText, DT_CENTER | DT_VCENTER);
    DeleteObject(hSmallFont);

    // Палитра цветов (3x4)
    int palY = infoY + 58;
    int palCellW = (PANEL_W - 20) / 3;
    int palCellH = 22;
    for (int i = 0; i < PALETTE_COUNT; i++) {
        int px = 10 + (i % 3) * palCellW;
        int py = palY + (i / 3) * palCellH;
        RECT rcPal = { px, py, px + palCellW - 2, py + palCellH - 2 };
        HBRUSH hPal = CreateSolidBrush(paletteColors[i]);
        FillRect(hdcMem, &rcPal, hPal);
        DeleteObject(hPal);
        HPEN hPalBorder = CreatePen(PS_SOLID, 1,
            (paletteColors[i] == state.currentColor) ? RGB(255, 215, 0) : RGB(100, 100, 100));
        HPEN hOldP = (HPEN)SelectObject(hdcMem, hPalBorder);
        SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
        int borderWidth = (paletteColors[i] == state.currentColor) ? 3 : 1;
        HPEN hThick = CreatePen(PS_SOLID, borderWidth, (paletteColors[i] == state.currentColor) ? RGB(255, 215, 0) : RGB(100, 100, 100));
        SelectObject(hdcMem, hThick);
        Rectangle(hdcMem, px, py, px + palCellW - 2, py + palCellH - 2);
        SelectObject(hdcMem, hOldP);
        DeleteObject(hThick);
        DeleteObject(hPalBorder);
    }

    // Толщина кисти
    int brushY = palY + 4 * palCellH + 8;
    SetTextColor(hdcMem, RGB(50, 50, 80));
    SelectObject(hdcMem, hTitleFont);
    RECT rcBrushLabel = { 10, brushY, PANEL_W - 10, brushY + 18 };
    DrawTextW(hdcMem, L"Размер:", -1, &rcBrushLabel, DT_LEFT);

    int brushBtnY = brushY + 22;
    int brushBtnW = (PANEL_W - 20) / BRUSH_COUNT;
    for (int i = 0; i < BRUSH_COUNT; i++) {
        int bx = 10 + i * brushBtnW;
        RECT rcBrush = { bx, brushBtnY, bx + brushBtnW - 2, brushBtnY + 30 };
        HBRUSH hBBg = CreateSolidBrush((brushSizes[i] == state.brushSize) ? RGB(100, 150, 220) : RGB(230, 230, 240));
        FillRect(hdcMem, &rcBrush, hBBg);
        DeleteObject(hBBg);
        HPEN hBBorder = CreatePen(PS_SOLID, 1, RGB(100, 100, 120));
        HPEN hOldP = (HPEN)SelectObject(hdcMem, hBBorder);
        SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
        Rectangle(hdcMem, bx, brushBtnY, bx + brushBtnW - 2, brushBtnY + 30);
        SelectObject(hdcMem, hOldP);
        DeleteObject(hBBorder);
        // Кружок размера
        int r = max(2, brushSizes[i] / 3);
        HBRUSH hDot = CreateSolidBrush(RGB(30, 30, 30));
        int cx = bx + (brushBtnW - 2) / 2;
        int cy = brushBtnY + 15;
        SelectObject(hdcMem, hDot);
        SelectObject(hdcMem, GetStockObject(NULL_PEN));
        Ellipse(hdcMem, cx - r, cy - r, cx + r, cy + r);
        DeleteObject(hDot);
    }

    // Кнопки действий
    int actY = brushBtnY + 40;
    auto DrawActionButton = [&](int y, const WCHAR* text, COLORREF bgColor) {
        RECT rc = { 10, y, PANEL_W - 10, y + 30 };
        HBRUSH hBtn = CreateSolidBrush(bgColor);
        FillRect(hdcMem, &rc, hBtn);
        DeleteObject(hBtn);
        HPEN hBorder = CreatePen(PS_SOLID, 1, RGB(80, 80, 100));
        HPEN hOldP = (HPEN)SelectObject(hdcMem, hBorder);
        SelectObject(hdcMem, GetStockObject(NULL_BRUSH));
        Rectangle(hdcMem, 10, y, PANEL_W - 10, y + 30);
        SelectObject(hdcMem, hOldP);
        DeleteObject(hBorder);
        SetTextColor(hdcMem, RGB(255, 255, 255));
        SelectObject(hdcMem, hTitleFont);
        DrawTextW(hdcMem, text, -1, &rc, DT_CENTER | DT_VCENTER);
    };

    DrawActionButton(actY, L"💾 Сохранить", RGB(60, 140, 80));
    DrawActionButton(actY + 36, L"📂 Загрузить", RGB(60, 100, 180));
    DrawActionButton(actY + 72, L"🗑️ Очистить", RGB(180, 60, 60));
    DrawActionButton(actY + 108, L"↶ Отменить", RGB(120, 100, 160));

    // === ХОЛСТ ===
    // Рамка холста
    RECT rcCanvasFrame = { CANVAS_X - 3, CANVAS_Y - 3, CANVAS_X + CANVAS_W + 3, CANVAS_Y + CANVAS_H + 3 };
    HBRUSH hFrame = CreateSolidBrush(RGB(100, 100, 120));
    FillRect(hdcMem, &rcCanvasFrame, hFrame);
    DeleteObject(hFrame);

    // Рисуем холст (из DIB)
    HDC hdcCanvas = CreateCompatibleDC(hdcMem);
    HBITMAP hOldCanvas = (HBITMAP)SelectObject(hdcCanvas, state.hCanvas);

    // Если рисуем фигуру — используем backbuffer для rubber-banding
    if (state.isDrawing && (state.currentTool == TOOL_LINE || state.currentTool == TOOL_RECT || state.currentTool == TOOL_ELLIPSE)) {
        memcpy(state.backPixels, state.canvasPixels, CANVAS_W * CANVAS_H * 4);

        // Рисуем "призрачную" фигуру на backbuffer
        DWORD* bp = (DWORD*)state.backPixels;
        auto setPx = [&](int x, int y, COLORREF c) {
            if (x >= 0 && x < CANVAS_W && y >= 0 && y < CANVAS_H)
                bp[y * CANVAS_W + x] = c;
        };

        if (state.currentTool == TOOL_LINE) {
            int dx = abs(state.lastX - state.startX), dy = abs(state.lastY - state.startY);
            int sx = state.startX < state.lastX ? 1 : -1;
            int sy = state.startY < state.lastY ? 1 : -1;
            int err = dx - dy;
            int x1 = state.startX, y1 = state.startY;
            int r = state.brushSize / 2;
            while (true) {
                for (int yy = -r; yy <= r; yy++)
                    for (int xx = -r; xx <= r; xx++)
                        if (xx * xx + yy * yy <= r * r)
                            setPx(x1 + xx, y1 + yy, state.currentColor);
                if (x1 == state.lastX && y1 == state.lastY) break;
                int e2 = 2 * err;
                if (e2 > -dy) { err -= dy; x1 += sx; }
                if (e2 < dx) { err += dx; y1 += sy; }
            }
        }
        else if (state.currentTool == TOOL_RECT) {
            int x1 = min(state.startX, state.lastX);
            int y1 = min(state.startY, state.lastY);
            int x2 = max(state.startX, state.lastX);
            int y2 = max(state.startY, state.lastY);
            int t = state.brushSize / 2;
            for (int i = -t; i <= t; i++) {
                for (int x = x1; x <= x2; x++) { setPx(x, y1 + i, state.currentColor); setPx(x, y2 + i, state.currentColor); }
                for (int y = y1; y <= y2; y++) { setPx(x1 + i, y, state.currentColor); setPx(x2 + i, y, state.currentColor); }
            }
        }
        else if (state.currentTool == TOOL_ELLIPSE) {
            int x1 = min(state.startX, state.lastX);
            int y1 = min(state.startY, state.lastY);
            int x2 = max(state.startX, state.lastX);
            int y2 = max(state.startY, state.lastY);
            int cx = (x1 + x2) / 2, cy = (y1 + y2) / 2;
            int rx = (x2 - x1) / 2, ry = (y2 - y1) / 2;
            if (rx < 1) rx = 1;
            if (ry < 1) ry = 1;
            // Рисуем эллипс по углу
            int steps = max(rx, ry) * 8;
            int t = state.brushSize / 2;
            for (int i = 0; i <= steps; i++) {
                float angle = (float)i / steps * 2.0f * 3.14159f;
                int px = cx + (int)(rx * cos(angle));
                int py = cy + (int)(ry * sin(angle));
                for (int yy = -t; yy <= t; yy++)
                    for (int xx = -t; xx <= t; xx++)
                        if (xx * xx + yy * yy <= t * t)
                            setPx(px + xx, py + yy, state.currentColor);
            }
        }

        // Выводим backbuffer
        HDC hdcBack = CreateCompatibleDC(hdcMem);
        HBITMAP hOldBack = (HBITMAP)SelectObject(hdcBack, state.hBackBuffer);
        BitBlt(hdcMem, CANVAS_X, CANVAS_Y, CANVAS_W, CANVAS_H, hdcBack, 0, 0, SRCCOPY);
        SelectObject(hdcBack, hOldBack);
        DeleteDC(hdcBack);
    }
    else {
        // Просто выводим холст
        BitBlt(hdcMem, CANVAS_X, CANVAS_Y, CANVAS_W, CANVAS_H, hdcCanvas, 0, 0, SRCCOPY);
    }

    SelectObject(hdcCanvas, hOldCanvas);
    DeleteDC(hdcCanvas);

    // === СТАТУС-БАР ВНИЗУ ===
    // (не делаем, чтобы не загромождать)

    // === КОПИРОВАНИЕ НА ЭКРАН ===
    BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hOldFont);
    DeleteObject(hTitleFont);
    SelectObject(hdcMem, hOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    ReleaseDC(hWndMain, hdc);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_LBUTTONDOWN:
    {
        int mx = LOWORD(lParam);
        int my = HIWORD(lParam);

        // Клик по холсту — начало рисования
        if (mx >= CANVAS_X && mx < CANVAS_X + CANVAS_W &&
            my >= CANVAS_Y && my < CANVAS_Y + CANVAS_H) {
            int cx = mx - CANVAS_X;
            int cy = my - CANVAS_Y;

            if (state.currentTool == TOOL_FILL) {
                SaveUndoState();
                FloodFill(cx, cy, state.currentColor);
                InvalidateRect(hWnd, nullptr, FALSE);
            }
            else {
                SaveUndoState();
                state.isDrawing = true;
                state.startX = state.lastX = cx;
                state.startY = state.lastY = cy;

                if (state.currentTool == TOOL_PENCIL || state.currentTool == TOOL_ERASER) {
                    COLORREF c = (state.currentTool == TOOL_ERASER) ? RGB(255, 255, 255) : state.currentColor;
                    DrawThickPixel(cx, cy, state.brushSize / 2, c);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
            }
            SetCapture(hWnd);
        }
        // Клик по панели инструментов
        else if (mx < PANEL_W) {
            // Инструменты (2 колонки)
            int iconW = 55, iconH = 40;
            int iconX1 = 8, iconX2 = 8 + iconW + 4;
            int iconY = 38;
            Tool tools[] = { TOOL_PENCIL, TOOL_LINE, TOOL_RECT, TOOL_ELLIPSE, TOOL_ERASER, TOOL_FILL };
            for (int i = 0; i < 6; i++) {
                int x = (i % 2 == 0) ? iconX1 : iconX2;
                int y = iconY + (i / 2) * (iconH + 4);
                if (mx >= x && mx < x + iconW && my >= y && my < y + iconH) {
                    state.currentTool = tools[i];
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
            }

            // Палитра цветов
            int infoY = iconY + 3 * (iconH + 4) + 10;
            int palY = infoY + 58;
            int palCellW = (PANEL_W - 20) / 3;
            int palCellH = 22;
            for (int i = 0; i < PALETTE_COUNT; i++) {
                int px = 10 + (i % 3) * palCellW;
                int py = palY + (i / 3) * palCellH;
                if (mx >= px && mx < px + palCellW - 2 && my >= py && my < py + palCellH - 2) {
                    state.currentColor = paletteColors[i];
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
            }

            // Кнопка "Другой цвет"
            if (mx >= 65 && mx < PANEL_W - 10 && my >= infoY + 20 && my < infoY + 50) {
                CHOOSECOLORW cc = { 0 };
                cc.lStructSize = sizeof(cc);
                cc.hwndOwner = hWnd;
                COLORREF customColors[16] = { 0 };
                cc.lpCustColors = customColors;
                cc.rgbResult = state.currentColor;
                cc.Flags = CC_FULLOPEN | CC_RGBINIT;
                if (ChooseColorW(&cc)) {
                    state.currentColor = cc.rgbResult;
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                return 0;
            }

            // Толщина кисти
            int brushY = palY + 4 * palCellH + 8;
            int brushBtnY = brushY + 22;
            int brushBtnW = (PANEL_W - 20) / BRUSH_COUNT;
            for (int i = 0; i < BRUSH_COUNT; i++) {
                int bx = 10 + i * brushBtnW;
                if (mx >= bx && mx < bx + brushBtnW - 2 && my >= brushBtnY && my < brushBtnY + 30) {
                    state.brushSize = brushSizes[i];
                    InvalidateRect(hWnd, nullptr, FALSE);
                    return 0;
                }
            }

            // Кнопки действий
            int actY = brushBtnY + 40;
            if (mx >= 10 && mx < PANEL_W - 10) {
                if (my >= actY && my < actY + 30) {
                    // Сохранить
                    WCHAR szFile[MAX_PATH] = L"drawing.bmp";
                    OPENFILENAMEW ofn = { 0 };
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hWnd;
                    ofn.lpstrFilter = L"BMP Images\0*.bmp\0";
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_OVERWRITEPROMPT;
                    ofn.lpstrDefExt = L"bmp";
                    if (GetSaveFileNameW(&ofn)) {
                        if (SaveToBmp(szFile)) {
                            MessageBoxW(hWnd, L"Изображение сохранено!", L"Успех", MB_OK | MB_ICONINFORMATION);
                        }
                    }
                    return 0;
                }
                if (my >= actY + 36 && my < actY + 66) {
                    // Загрузить
                    WCHAR szFile[MAX_PATH] = L"";
                    OPENFILENAMEW ofn = { 0 };
                    ofn.lStructSize = sizeof(ofn);
                    ofn.hwndOwner = hWnd;
                    ofn.lpstrFilter = L"BMP Images\0*.bmp\0";
                    ofn.lpstrFile = szFile;
                    ofn.nMaxFile = MAX_PATH;
                    ofn.Flags = OFN_FILEMUSTEXIST;
                    if (GetOpenFileNameW(&ofn)) {
                        LoadFromBmp(szFile);
                    }
                    return 0;
                }
                if (my >= actY + 72 && my < actY + 102) {
                    // Очистить
                    if (MessageBoxW(hWnd, L"Очистить холст?", L"Подтверждение", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                        state.undoStack.clear();
                        ClearCanvas();
                    }
                    return 0;
                }
                if (my >= actY + 108 && my < actY + 138) {
                    // Отменить
                    Undo();
                    return 0;
                }
            }
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        if (state.isDrawing) {
            int mx = LOWORD(lParam);
            int my = HIWORD(lParam);
            int cx = mx - CANVAS_X;
            int cy = my - CANVAS_Y;

            if (cx < 0) cx = 0;
            if (cy < 0) cy = 0;
            if (cx >= CANVAS_W) cx = CANVAS_W - 1;
            if (cy >= CANVAS_H) cy = CANVAS_H - 1;

            if (state.currentTool == TOOL_PENCIL || state.currentTool == TOOL_ERASER) {
                COLORREF c = (state.currentTool == TOOL_ERASER) ? RGB(255, 255, 255) : state.currentColor;
                DrawLine(state.lastX, state.lastY, cx, cy, c, state.brushSize);
                state.lastX = cx;
                state.lastY = cy;
            }
            else {
                state.lastX = cx;
                state.lastY = cy;
            }
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        break;
    }
    case WM_LBUTTONUP:
    {
        if (state.isDrawing) {
            int mx = LOWORD(lParam);
            int my = HIWORD(lParam);
            int cx = mx - CANVAS_X;
            int cy = my - CANVAS_Y;
            if (cx < 0) cx = 0;
            if (cy < 0) cy = 0;
            if (cx >= CANVAS_W) cx = CANVAS_W - 1;
            if (cy >= CANVAS_H) cy = CANVAS_H - 1;

            // Дорисовываем фигуру на основном холсте
            if (state.currentTool == TOOL_LINE) {
                DrawLine(state.startX, state.startY, cx, cy, state.currentColor, state.brushSize);
            }
            else if (state.currentTool == TOOL_RECT) {
                int x1 = min(state.startX, cx), y1 = min(state.startY, cy);
                int x2 = max(state.startX, cx), y2 = max(state.startY, cy);
                int t = state.brushSize / 2;
                for (int i = -t; i <= t; i++) {
                    DrawLine(x1, y1 + i, x2, y1 + i, state.currentColor, 1);
                    DrawLine(x1, y2 + i, x2, y2 + i, state.currentColor, 1);
                    DrawLine(x1 + i, y1, x1 + i, y2, state.currentColor, 1);
                    DrawLine(x2 + i, y1, x2 + i, y2, state.currentColor, 1);
                }
            }
            else if (state.currentTool == TOOL_ELLIPSE) {
                int x1 = min(state.startX, cx), y1 = min(state.startY, cy);
                int x2 = max(state.startX, cx), y2 = max(state.startY, cy);
                int ccx = (x1 + x2) / 2, ccy = (y1 + y2) / 2;
                int rx = (x2 - x1) / 2, ry = (y2 - y1) / 2;
                if (rx < 1) rx = 1;
                if (ry < 1) ry = 1;
                int steps = max(rx, ry) * 8;
                int t = state.brushSize / 2;
                int prevPx = -1, prevPy = -1;
                for (int i = 0; i <= steps; i++) {
                    float angle = (float)i / steps * 2.0f * 3.14159f;
                    int px = ccx + (int)(rx * cos(angle));
                    int py = ccy + (int)(ry * sin(angle));
                    if (prevPx >= 0) {
                        DrawLine(prevPx, prevPy, px, py, state.currentColor, state.brushSize);
                    }
                    prevPx = px;
                    prevPy = py;
                }
            }

            state.isDrawing = false;
            ReleaseCapture();
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        break;
    }
    case WM_KEYDOWN:
    {
        // Ctrl+Z — отмена
        if (wParam == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000)) {
            Undo();
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
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        Render();
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        if (state.hCanvas) DeleteObject(state.hCanvas);
        if (state.hBackBuffer) DeleteObject(state.hBackBuffer);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
