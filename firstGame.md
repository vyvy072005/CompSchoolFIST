Вот готовый, красочный и понятный Markdown-файл, специально созданный для юного программиста. Ты можешь сохранить этот текст как файл `README.md` в своём проекте на GitHub или просто читать его как инструкцию.

---

# 🧩 Создай свою собственную игру "Три в ряд" (как Royal Match!) на C++! 🚀

Привет, будущий создатель игр! 👋 
Ты когда-нибудь играл в **Royal Match**, **Candy Crush** или подобные головоломки и думал: *"А что, если я сделаю свою собственную, где правила и цвета придумываю Я?"* 

Сегодня мы это сделаем! Мы напишем настоящую игру на языке **C++** с использованием **WinAPI** (это как волшебная палочка для создания окон и рисования в Windows). 

Не бойся, если код кажется большим. Мы разберём его по кусочкам, как конструктор LEGO! 🧱

---

## 🛠️ Что тебе понадобится?
1. **Visual Studio** (бесплатная версия Community отлично подойдёт).
2. Желание экспериментировать!
3. Этот гайд.

### 🚀 Быстрый старт:
1. Открой Visual Studio и нажми **"Создание проекта"**.
2. Выбери шаблон **"Приложение Windows (Win32)"** или **"Пустой проект"**.
3. Назови его, например, `MyMatch3Game`.
4. Найди файл `Основной_файл.cpp` (или создай его) и **вставь туда весь код из конца этого файла**.
5. Нажми зелёную кнопку **"Локальный отладчик Windows"** (или `F5`) и играй!

---

## 🧠 Как это работает? (Разбираем по кусочкам)

### 1. Настройки игрового поля 📏
В самом начале кода есть числа, которые определяют размер игры. Это как выбрать размер листа для рисования.

```cpp
const int ROWS = 8, COLS = 8, CELL = 62; // 8 строк, 8 столбцов, клетка 62 пикселя
const int NUM_TYPES = 6; // Сколько разных фигур у нас есть
```
> 💡 **ТВОЁ ЗАДАНИЕ №1:** Попробуй изменить `ROWS` и `COLS` на `10`. Сделай поле огромным! Не забудь поменять `CELL` на `50`, чтобы оно поместилось на экране.

### 2. Волшебные цвета 🎨
Компьютер понимает цвета как смесь Красного, Зелёного и Синего (RGB) от 0 до 255.

```cpp
const COLORREF typeColors[NUM_TYPES] = {
    RGB(220, 50, 50),   // 0: Ярко-красный
    RGB(50, 100, 230),  // 1: Синий
    RGB(40, 190, 60),   // 2: Зелёный
    RGB(240, 200, 30),  // 3: Жёлтый
    RGB(160, 50, 210),  // 4: Фиолетовый
    RGB(240, 130, 30)   // 5: Оранжевый
};
```
> 💡 **ТВОЁ ЗАДАНИЕ №2:** Придумай свои цвета! Поменяй цифры в `RGB()`. Например, `RGB(255, 0, 255)` сделает фигуру ярко-розовой. Сделай "Радужный режим"!

### 3. Рисование фигур (Самое интересное!) ✏️
Функция `DrawShape` — это твой художественный набор. Здесь мы говорим компьютеру, как рисовать каждую фигуру.

```cpp
switch (type) {
    case 0: // Круг
        Ellipse(hdc, cx - sr, cy - sr, cx + sr, cy + sr);
        break;
    case 1: // Квадрат (скруглённый)
        RoundRect(hdc, cx - sr, cy - sr, cx + sr, cy + sr, sr / 3, sr / 3);
        break;
    // ... и так далее
}
```
> 💡 **ТВОЁ ЗАДАНИЕ №3 (Для смелых!):** Давай добавим **Сердечко**! 
> Найди `case 5:` (или добавь новый `case 6`, если изменил `NUM_TYPES` на 7) и вставь туда этот код:
> ```cpp
> // Рисуем сердечко из двух кругов и треугольника внизу
> Ellipse(hdc, cx - sr/2, cy - sr/2, cx, cy); // Левая половинка
> Ellipse(hdc, cx, cy - sr/2, cx + sr/2, cy); // Правая половинка
> POINT heart[] = { {cx - sr/2, cy - sr/4}, {cx + sr/2, cy - sr/4}, {cx, cy + sr/2} };
> Polygon(hdc, heart, 3); // Нижняя часть
> ```

### 4. Мозг игры: Поиск совпадений 🕵️‍♂️
Функция `FindMatches` работает как детектив. Она проверяет каждую строку и столбец: *"Эй, тут три одинаковых фигуры стоят подряд? Если да, запоминаем их!"*

### 5. Гравитация и заполнение 🍎
Когда фигуры исчезают, функция `DoGravity` заставляет верхние фигуры падать вниз (как настоящие яблоки!), а `DoFill` создаёт новые случайные фигуры сверху, чтобы поле никогда не пустовало.

---

## 🌟 4 Супер-Челленджа для тебя!

Попробуй выполнить хотя бы один из них. Это сделает игру **по-настоящему твоей**:

1. **Читерский режим:** Найди строку `int moves = 30;` и поменяй на `999`. Теперь у тебя бесконечные ходы, чтобы набрать максимум очков!
2. **Легкая победа:** Найди `int target = 2000;` и сделай `500`. Ты будешь выигрывать мгновенно и чувствовать себя супергероем.
3. **Новая фигура:** Как описано выше, добавь 7-й тип фигуры (например, сердечко или молнию). Не забудь добавить для неё цвет в массив `typeColors`!
4. **Измени текст:** Найди строку `L"🧩 Match-3 | Уровень "` и напиши там название СВОЕЙ игры! Например: `L"👾 Супер Пазл Макса 👾"`.

---

## 📜 Полный код игры (Скопируй и вставь!)

Просто выдели всё от `// WindowsProject3.cpp` до самого конца и вставь в свой файл в Visual Studio.

```cpp
// WindowsProject3.cpp : Match-3 Головоломка (Твоя собственная игра!)
//

#include "framework.h"
#include "WindowsProject3.h"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <ctime>

#define MAX_LOADSTRING 100
#define TIMER_CASCADE 1

// === 🛠️ НАСТРОЙКИ ИГРЫ (Попробуй их поменять!) ===
const int ROWS = 8, COLS = 8, CELL = 62;
const int NUM_TYPES = 6; // Количество разных фигур
const int BOARD_X = 22, BOARD_Y = 100;
const int WIN_W = BOARD_X * 2 + COLS * CELL;
const int WIN_H = BOARD_Y + ROWS * CELL + 55;

// === СОСТОЯНИЯ ИГРЫ ===
enum Phase { IDLE, SWAPPED, SWAP_BACK, REMOVING, GRAVITY, FILLING, CHECKING, GAMEOVER, WINSTATE };

// === ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ ===
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];
HWND hWndMain;

int grid[ROWS][COLS];
Phase phase = IDLE;
int selR = -1, selC = -1;
int score = 0, moves = 30, target = 2000, level = 1, combo = 0;
bool animating = false;
std::vector<std::pair<int, int>> matched;

// === 🎨 ЦВЕТА ЭЛЕМЕНТОВ (Поменяй цифры на свои любимые цвета!) ===
const COLORREF typeColors[NUM_TYPES] = {
    RGB(220, 50, 50),   // 0 красный
    RGB(50, 100, 230),  // 1 синий
    RGB(40, 190, 60),   // 2 зеленый
    RGB(240, 200, 30),  // 3 желтый
    RGB(160, 50, 210),  // 4 фиолетовый
    RGB(240, 130, 30)   // 5 оранжевый
};

// === ОБЪЯВЛЕНИЯ ФУНКЦИЙ ===
ATOM MyRegisterClass(HINSTANCE);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void InitBoard();
bool FindMatches();
void DoGravity();
void DoFill();
bool HasValidMoves();
void ShuffleBoard();
void StartCascade();
void DrawShape(HDC hdc, int cx, int cy, int r, int type, float scale);
void Render(HWND hWnd);

// === 1. ИНИЦИАЛИЗАЦИЯ ДОСКИ (Создаём поле без случайных совпадений на старте) ===
void InitBoard() {
    srand((unsigned)time(NULL));
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            int t;
            do {
                t = rand() % NUM_TYPES;
                grid[r][c] = t;
            } while (
                (c >= 2 && grid[r][c - 1] == t && grid[r][c - 2] == t) ||
                (r >= 2 && grid[r - 1][c] == t && grid[r - 2][c] == t)
                );
        }
    }
    if (!HasValidMoves()) ShuffleBoard();
}

// === 2. ПОИСК СОВПАДЕНИЙ (Детектив ищет 3+ одинаковых фигуры) ===
bool FindMatches() {
    matched.clear();
    bool found = false;

    // Горизонтальные
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS - 2; c++) {
            int t = grid[r][c];
            if (t < 0) continue;
            int len = 1;
            while (c + len < COLS && grid[r][c + len] == t) len++;
            if (len >= 3) {
                found = true;
                for (int i = 0; i < len; i++) matched.push_back({ r, c + i });
            }
            c += len - 1;
        }
    }
    // Вертикальные
    for (int c = 0; c < COLS; c++) {
        for (int r = 0; r < ROWS - 2; r++) {
            int t = grid[r][c];
            if (t < 0) continue;
            int len = 1;
            while (r + len < ROWS && grid[r + len][c] == t) len++;
            if (len >= 3) {
                found = true;
                for (int i = 0; i < len; i++) matched.push_back({ r + i, c });
            }
            r += len - 1;
        }
    }

    // Убираем дубликаты, если фигура часть и горизонтального, и вертикального ряда
    std::sort(matched.begin(), matched.end());
    matched.erase(std::unique(matched.begin(), matched.end()), matched.end());
    return found;
}

// === 3. ГРАВИТАЦИЯ (Фигуры падают вниз, как настоящие!) ===
void DoGravity() {
    for (int c = 0; c < COLS; c++) {
        int writeRow = ROWS - 1;
        for (int r = ROWS - 1; r >= 0; r--) {
            if (grid[r][c] >= 0) {
                grid[writeRow][c] = grid[r][c];
                if (writeRow != r) grid[r][c] = -1;
                writeRow--;
            }
        }
        for (int r = writeRow; r >= 0; r--) {
            grid[r][c] = -1; // Очищаем верх
        }
    }
}

// === 4. ЗАПОЛНЕНИЕ ПУСТЫХ КЛЕТОК (Спавним новые фигуры сверху) ===
void DoFill() {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (grid[r][c] < 0)
                grid[r][c] = rand() % NUM_TYPES;
}

// === ПРОВЕРКА: ЕСТЬ ЛИ ВОЗМОЖНЫЕ ХОДЫ? ===
bool HasValidMoves() {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (c + 1 < COLS) {
                std::swap(grid[r][c], grid[r][c + 1]);
                bool ok = FindMatches();
                std::swap(grid[r][c], grid[r][c + 1]);
                if (ok) return true;
            }
            if (r + 1 < ROWS) {
                std::swap(grid[r][c], grid[r + 1][c]);
                bool ok = FindMatches();
                std::swap(grid[r][c], grid[r + 1][c]);
                if (ok) return true;
            }
        }
    }
    return false;
}

// === ПЕРЕМЕШИВАНИЕ (Если ходов нет, игра сама всё перемешает) ===
void ShuffleBoard() {
    do {
        for (int r = 0; r < ROWS; r++)
            for (int c = 0; c < COLS; c++)
                grid[r][c] = rand() % NUM_TYPES;
        while (FindMatches()) {
            for (auto& p : matched) grid[p.first][p.second] = rand() % NUM_TYPES;
        }
    } while (!HasValidMoves());
}

// === ЗАПУСК КАСКАДА (Анимация исчезновения) ===
void StartCascade() {
    combo = 0;
    phase = SWAPPED;
    animating = true;
    SetTimer(hWndMain, TIMER_CASCADE, 180, NULL);
}

// === 5. РИСОВАНИЕ ФИГУР (ТВОРЧЕСКАЯ МАСТЕРСКАЯ!) ===
void DrawShape(HDC hdc, int cx, int cy, int r, int type, float scale) {
    if (type < 0 || type >= NUM_TYPES) return;
    int sr = (int)(r * scale);
    if (sr < 2) return;

    COLORREF mainColor = typeColors[type];
    BYTE mr = GetRValue(mainColor), mg = GetGValue(mainColor), mb = GetBValue(mainColor);
    COLORREF lightColor = RGB(min(255, mr + 80), min(255, mg + 80), min(255, mb + 80));
    COLORREF darkColor = RGB(mr * 2 / 3, mg * 2 / 3, mb * 2 / 3);

    HBRUSH hBrush = CreateSolidBrush(mainColor);
    HBRUSH hLightBrush = CreateSolidBrush(lightColor);
    HPEN hPen = CreatePen(PS_SOLID, 2, darkColor);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    switch (type) {
    case 0: // 🔴 Круг
        Ellipse(hdc, cx - sr, cy - sr, cx + sr, cy + sr);
        SelectObject(hdc, hLightBrush);
        Ellipse(hdc, cx - sr / 2, cy - sr / 2 - sr / 4, cx - sr / 6, cy - sr / 6); // Блик
        break;

    case 1: // 🟦 Квадрат (скруглённый)
        RoundRect(hdc, cx - sr, cy - sr, cx + sr, cy + sr, sr / 3, sr / 3);
        SelectObject(hdc, hLightBrush);
        RoundRect(hdc, cx - sr + 4, cy - sr + 4, cx - sr / 3, cy - sr / 3, 4, 4);
        break;

    case 2: // 🔷 Ромб
    {
        POINT pts[] = { {cx, cy - sr}, {cx + sr, cy}, {cx, cy + sr}, {cx - sr, cy} };
        Polygon(hdc, pts, 4);
        SelectObject(hdc, hLightBrush);
        POINT hl[] = { {cx, cy - sr + 5}, {cx + sr / 3, cy - 2}, {cx, cy - 2}, {cx - sr / 3, cy - 2} };
        Polygon(hdc, hl, 4);
        break;
    }

    case 3: // 🔺 Треугольник
    {
        POINT pts[] = { {cx, cy - sr}, {cx + sr, cy + sr * 3 / 4}, {cx - sr, cy + sr * 3 / 4} };
        Polygon(hdc, pts, 3);
        SelectObject(hdc, hLightBrush);
        POINT hl[] = { {cx, cy - sr + 6}, {cx + sr / 3, cy}, {cx - sr / 3, cy} };
        Polygon(hdc, hl, 3);
        break;
    }

    case 4: // ⭐ Звезда (5 лучей)
    {
        POINT pts[10];
        for (int i = 0; i < 10; i++) {
            float angle = (float)(i * 36 - 90) * 3.14159f / 180.0f;
            float rad = (i % 2 == 0) ? (float)sr : (float)sr * 0.45f;
            pts[i] = { cx + (int)(rad * cos(angle)), cy + (int)(rad * sin(angle)) };
        }
        Polygon(hdc, pts, 10);
        SelectObject(hdc, hLightBrush);
        Ellipse(hdc, cx - sr / 5, cy - sr / 5, cx + sr / 5, cy + sr / 5);
        break;
    }

    case 5: // ➕ Крест (Попробуй заменить это на Сердечко! См. инструкцию выше)
    {
        int t = sr / 3;
        POINT pts[] = {
            {cx - t, cy - sr}, {cx + t, cy - sr}, {cx + t, cy - t}, {cx + sr, cy - t},
            {cx + sr, cy + t}, {cx + t, cy + t}, {cx + t, cy + sr}, {cx - t, cy + sr},
            {cx - t, cy + t}, {cx - sr, cy + t}, {cx - sr, cy - t}, {cx - t, cy - t}
        };
        Polygon(hdc, pts, 12);
        SelectObject(hdc, hLightBrush);
        Rectangle(hdc, cx - t + 3, cy - sr + 3, cx + t - 3, cy - t);
        break;
    }
    }

    SelectObject(hdc, hOldBrush);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hLightBrush);
    DeleteObject(hPen);
}

// === 6. ОСНОВНАЯ ОТРИСОВКА ЭКРАНА ===
void Render(HWND hWnd) {
    HDC hdc = GetDC(hWnd);
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    // Двойная буферизация (чтобы не мерцало)
    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rcClient.right, rcClient.bottom);
    HBITMAP hOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    // Фон — красивый градиент
    for (int y = 0; y < rcClient.bottom; y++) {
        float t = (float)y / rcClient.bottom;
        BYTE r = (BYTE)(30 + t * 20);
        BYTE g = (BYTE)(20 + t * 30);
        BYTE b = (BYTE)(60 + t * 40);
        HPEN hPen = CreatePen(PS_SOLID, 1, RGB(r, g, b));
        HPEN hOldP = (HPEN)SelectObject(hdcMem, hPen);
        MoveToEx(hdcMem, 0, y, NULL);
        LineTo(hdcMem, rcClient.right, y);
        SelectObject(hdcMem, hOldP);
        DeleteObject(hPen);
    }

    // Заголовок
    SetBkMode(hdcMem, TRANSPARENT);
    HFONT hTitleFont = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdcMem, hTitleFont);
    SetTextColor(hdcMem, RGB(255, 220, 100));
    std::wstring titleStr = L"🧩 Твоя Match-3 Игра | Уровень " + std::to_wstring(level); // 👈 ПОМЕНЯЙ НАЗВАНИЕ ЗДЕСЬ!
    RECT rcTitle = { 10, 8, WIN_W, 35 };
    DrawTextW(hdcMem, titleStr.c_str(), -1, &rcTitle, DT_LEFT | DT_VCENTER);

    // Очки, ходы, цель
    HFONT hStatFont = CreateFontW(17, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    SelectObject(hdcMem, hStatFont);

    std::wstring scoreStr = L"Очки: " + std::to_wstring(score);
    std::wstring movesStr = L"Ходы: " + std::to_wstring(moves);
    std::wstring targetStr = L"Цель: " + std::to_wstring(target);

    SetTextColor(hdcMem, RGB(255, 255, 255));
    RECT rcScore = { 10, 38, 170, 60 };
    DrawTextW(hdcMem, scoreStr.c_str(), -1, &rcScore, DT_LEFT);

    SetTextColor(hdcMem, moves <= 5 ? RGB(255, 80, 80) : RGB(200, 255, 200));
    RECT rcMoves = { 170, 38, 330, 60 };
    DrawTextW(hdcMem, movesStr.c_str(), -1, &rcMoves, DT_LEFT);

    SetTextColor(hdcMem, RGB(255, 220, 100));
    RECT rcTarget = { 330, 38, WIN_W, 60 };
    DrawTextW(hdcMem, targetStr.c_str(), -1, &rcTarget, DT_LEFT);

    // Комбо!
    if (combo > 1 && animating) {
        SetTextColor(hdcMem, RGB(255, 100, 100));
        std::wstring comboStr = L"🔥 COMBO x" + std::to_wstring(combo) + L"!";
        RECT rcCombo = { 10, 62, WIN_W, 85 };
        DrawTextW(hdcMem, comboStr.c_str(), -1, &rcCombo, DT_CENTER);
    }

    // Прогресс-бар
    RECT rcBar = { BOARD_X, 82, BOARD_X + COLS * CELL, 92 };
    HBRUSH hBarBg = CreateSolidBrush(RGB(40, 40, 60));
    FillRect(hdcMem, &rcBar, hBarBg);
    DeleteObject(hBarBg);
    float progress = (float)score / target;
    if (progress > 1.0f) progress = 1.0f;
    int barW = (int)(progress * (COLS * CELL));
    if (barW > 0) {
        RECT rcFill = { BOARD_X, 82, BOARD_X + barW, 92 };
        HBRUSH hBarFill = CreateSolidBrush(progress >= 1.0f ? RGB(50, 220, 50) : RGB(50, 150, 255));
        FillRect(hdcMem, &rcFill, hBarFill);
        DeleteObject(hBarFill);
    }

    // Фон доски
    RECT rcBoard = { BOARD_X - 4, BOARD_Y - 4, BOARD_X + COLS * CELL + 4, BOARD_Y + ROWS * CELL + 4 };
    HBRUSH hBoardBg = CreateSolidBrush(RGB(20, 15, 40));
    FillRect(hdcMem, &rcBoard, hBoardBg);
    DeleteObject(hBoardBg);

    // Сетка и клетки
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            int x = BOARD_X + c * CELL;
            int y = BOARD_Y + r * CELL;
            RECT cellRect = { x + 1, y + 1, x + CELL - 1, y + CELL - 1 };

            COLORREF cellBg = ((r + c) % 2 == 0) ? RGB(50, 45, 75) : RGB(40, 35, 65);
            HBRUSH hCellBg = CreateSolidBrush(cellBg);
            FillRect(hdcMem, &cellRect, hCellBg);
            DeleteObject(hCellBg);

            // Подсветка выбранной клетки
            if (r == selR && c == selC) {
                HPEN hSelPen = CreatePen(PS_SOLID, 3, RGB(255, 255, 100));
                HPEN hOldP = (HPEN)SelectObject(hdcMem, hSelPen);
                HBRUSH hNull = (HBRUSH)GetStockObject(NULL_BRUSH);
                HBRUSH hOldB = (HBRUSH)SelectObject(hdcMem, hNull);
                Rectangle(hdcMem, x + 2, y + 2, x + CELL - 2, y + CELL - 2);
                SelectObject(hdcMem, hOldP);
                SelectObject(hdcMem, hOldB);
                DeleteObject(hSelPen);
            }

            // Подсветка совпавших клеток (вспышка)
            bool isMatched = false;
            for (auto& m : matched) {
                if (m.first == r && m.second == c) { isMatched = true; break; }
            }
            if (isMatched && phase == REMOVING) {
                HBRUSH hFlash = CreateSolidBrush(RGB(255, 255, 255));
                FillRect(hdcMem, &cellRect, hFlash);
                DeleteObject(hFlash);
            }

            // Рисуем фигуру
            if (grid[r][c] >= 0) {
                float scale = isMatched && phase == REMOVING ? 0.5f : 1.0f;
                DrawShape(hdcMem, x + CELL / 2, y + CELL / 2, CELL / 2 - 6, grid[r][c], scale);
            }
        }
    }

    // Линии сетки
    HPEN hGridPen = CreatePen(PS_SOLID, 1, RGB(70, 65, 100));
    HPEN hOldGridPen = (HPEN)SelectObject(hdcMem, hGridPen);
    for (int r = 0; r <= ROWS; r++) {
        MoveToEx(hdcMem, BOARD_X, BOARD_Y + r * CELL, NULL);
        LineTo(hdcMem, BOARD_X + COLS * CELL, BOARD_Y + r * CELL);
    }
    for (int c = 0; c <= COLS; c++) {
        MoveToEx(hdcMem, BOARD_X + c * CELL, BOARD_Y, NULL);
        LineTo(hdcMem, BOARD_X + c * CELL, BOARD_Y + ROWS * CELL);
    }
    SelectObject(hdcMem, hOldGridPen);
    DeleteObject(hGridPen);

    // Подсказка внизу
    HFONT hHintFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
    SelectObject(hdcMem, hHintFont);
    SetTextColor(hdcMem, RGB(150, 150, 180));
    RECT rcHint = { 10, WIN_H - 30, WIN_W, WIN_H - 5 };
    DrawTextW(hdcMem, L"Кликни на элемент, затем на соседний для обмена | R — новая игра", -1, &rcHint, DT_CENTER);

    // Экран победы / поражения
    if (phase == WINSTATE || phase == GAMEOVER) {
        HBRUSH hOverlay = CreateSolidBrush(RGB(0, 0, 0));
        for (int oy = 0; oy < WIN_H; oy += 2) { // Полупрозрачный эффект
            RECT rcLine = { 0, oy, WIN_W, oy + 1 };
            FillRect(hdcMem, &rcLine, hOverlay);
        }
        DeleteObject(hOverlay);

        HFONT hBigFont = CreateFontW(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        SelectObject(hdcMem, hBigFont);

        if (phase == WINSTATE) {
            SetTextColor(hdcMem, RGB(50, 255, 50));
            RECT rcMsg = { 0, WIN_H / 2 - 60, WIN_W, WIN_H / 2 };
            DrawTextW(hdcMem, L"🎉 ПОБЕДА! 🎉", -1, &rcMsg, DT_CENTER | DT_VCENTER);
        } else {
            SetTextColor(hdcMem, RGB(255, 50, 50));
            RECT rcMsg = { 0, WIN_H / 2 - 60, WIN_W, WIN_H / 2 };
            DrawTextW(hdcMem, L"💔 ПОРАЖЕНИЕ", -1, &rcMsg, DT_CENTER | DT_VCENTER);
        }

        HFONT hMedFont = CreateFontW(22, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, DEFAULT_PITCH, L"Segoe UI");
        SelectObject(hdcMem, hMedFont);
        SetTextColor(hdcMem, RGB(255, 255, 255));
        std::wstring finalStr = L"Очки: " + std::to_wstring(score) + L"  |  Нажми R для продолжения";
        RECT rcFinal = { 0, WIN_H / 2, WIN_W, WIN_H / 2 + 40 };
        DrawTextW(hdcMem, finalStr.c_str(), -1, &rcFinal, DT_CENTER | DT_VCENTER);

        DeleteObject(hBigFont);
        DeleteObject(hMedFont);
    }

    // Копирование на экран (магия двойной буферизации)
    BitBlt(hdc, 0, 0, rcClient.right, rcClient.bottom, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hOldFont);
    DeleteObject(hTitleFont);
    DeleteObject(hStatFont);
    DeleteObject(hHintFont);
    SelectObject(hdcMem, hOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    ReleaseDC(hWnd, hdc);
}

// === ОБРАБОТКА КЛИКА МЫШКОЙ ===
void HandleClick(int mx, int my) {
    if (animating || phase == WINSTATE || phase == GAMEOVER) return;

    int c = (mx - BOARD_X) / CELL;
    int r = (my - BOARD_Y) / CELL;

    if (r < 0 || r >= ROWS || c < 0 || c >= COLS) {
        selR = selC = -1;
        return;
    }

    if (selR < 0) {
        selR = r; selC = c; // Выбрали первую фигуру
    } else {
        int dr = abs(r - selR), dc = abs(c - selC);
        if ((dr == 1 && dc == 0) || (dr == 0 && dc == 1)) { // Если кликнули на соседнюю
            std::swap(grid[selR][selC], grid[r][c]);
            if (FindMatches()) {
                moves--;
                StartCascade();
            } else {
                std::swap(grid[selR][selC], grid[r][c]); // Возвращаем обратно, если совпадений нет
                phase = SWAP_BACK;
                animating = true;
                SetTimer(hWndMain, TIMER_CASCADE, 200, NULL);
            }
            selR = selC = -1;
        } else {
            selR = r; selC = c; // Просто поменяли выбор
        }
    }
    InvalidateRect(hWndMain, NULL, FALSE);
}

void NewGame() {
    score = 0; moves = 30; combo = 0; phase = IDLE; animating = false;
    selR = selC = -1; matched.clear();
    KillTimer(hWndMain, TIMER_CASCADE);
    InitBoard();
    InvalidateRect(hWndMain, NULL, FALSE);
}

void NextLevel() {
    level++;
    target = 2000 + (level - 1) * 1000;
    moves = max(15, 30 - (level - 1) * 2);
    score = 0; combo = 0; phase = IDLE; animating = false;
    selR = selC = -1; matched.clear();
    KillTimer(hWndMain, TIMER_CASCADE);
    InitBoard();
    InvalidateRect(hWndMain, NULL, FALSE);
}

// === ГЛАВНАЯ ТОЧКА ВХОДА В ПРОГРАММУ ===
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
    wcex.hCursor = LoadCursor(nullptr, IDC_HAND);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT3);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;
    RECT rc = { 0, 0, WIN_W, WIN_H };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, TRUE);
    hWndMain = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, CW_USEDEFAULT, 0, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance, nullptr);
    if (!hWndMain) return FALSE;
    ShowWindow(hWndMain, nCmdShow);
    UpdateWindow(hWndMain);
    SetFocus(hWndMain);
    NewGame();
    return TRUE;
}

// === ОБРАБОТЧИК СОБЫТИЙ (Нажатия кнопок, таймер, рисование) ===
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_LBUTTONDOWN:
    {
        int mx = LOWORD(lParam);
        int my = HIWORD(lParam);
        HandleClick(mx, my);
        break;
    }
    case WM_TIMER:
    {
        if (wParam == TIMER_CASCADE) {
            switch (phase) {
            case SWAPPED:
                if (FindMatches()) {
                    combo++;
                    score += (int)matched.size() * 10 * combo;
                    phase = REMOVING;
                } else {
                    phase = IDLE;
                    animating = false;
                    KillTimer(hWnd, TIMER_CASCADE);
                }
                break;
            case SWAP_BACK:
                phase = IDLE;
                animating = false;
                KillTimer(hWnd, TIMER_CASCADE);
                break;
            case REMOVING:
                for (auto& m : matched) grid[m.first][m.second] = -1;
                matched.clear();
                phase = GRAVITY;
                break;
            case GRAVITY:
                DoGravity();
                phase = FILLING;
                break;
            case FILLING:
                DoFill();
                phase = CHECKING;
                break;
            case CHECKING:
                if (FindMatches()) {
                    combo++;
                    score += (int)matched.size() * 10 * combo;
                    phase = REMOVING;
                } else {
                    animating = false;
                    KillTimer(hWnd, TIMER_CASCADE);
                    if (score >= target) phase = WINSTATE;
                    else if (moves <= 0) phase = GAMEOVER;
                    else if (!HasValidMoves()) ShuffleBoard();
                    phase = (phase == WINSTATE || phase == GAMEOVER) ? phase : IDLE;
                }
                break;
            default:
                KillTimer(hWnd, TIMER_CASCADE);
                animating = false;
                break;
            }
            InvalidateRect(hWnd, NULL, FALSE);
        }
        break;
    }
    case WM_KEYDOWN:
    {
        if (wParam == 'R') { // Нажали R
            if (phase == WINSTATE) NextLevel();
            else NewGame();
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);
        Render(hWnd);
        EndPaint(hWnd, &ps);
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
        } else if (wmId == IDM_EXIT) {
            DestroyWindow(hWnd);
        }
        break;
    }
    case WM_DESTROY:
        KillTimer(hWnd, TIMER_CASCADE);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
```

---

## 🏆 Ты это сделал!

Поздравляю! 🎉 Ты только что создал настоящую компьютерную игру. 
Помни: **все великие игры** (Minecraft, Roblox, Among Us) начинались с того, что кто-то просто взял чужой код и решил поменять в нём цвета или правила. 

Экспериментируй, ломай код, чини его и придумывай свои безумные идеи. У тебя всё получится! 🚀

*Если захочешь добавить звуки, новые уровни или сохранение рекордов — это следующие крутые шаги, которые ты сможешь освоить!*
