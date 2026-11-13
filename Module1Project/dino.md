Отличный выбор! "Динозаврик из Chrome" — это идеальная игра, чтобы понять, как работает **гравитация** и **бесконечная генерация** в играх. 

И самое главное: нам **не нужны массивы**! Мы сделаем всего один кактус, который будет "телепортироваться" обратно за правый край экрана, как только улетит за левый. Это создаст иллюзию бесконечного бега.

Создай пустой проект, добавь файл `Dino.cpp` и следуй шагам ниже.

---

# 🦖 Твой собственный DINO RUN на C++ (Без массивов!)

Привет! 👋 Сегодня мы напишем клон знаменитого динозаврика. Ты узнаешь, как сделать реалистичный прыжок с гравитацией и как обмануть игрока, заставляя думать, что кактусов бесконечно много.

> ⚠️ **Правило:** Добавляй код по одному шагу за раз. Читай блоки "Твой эксперимент!", чтобы понять, как работает физика!

---

## 🎒 Шаг 1: Инвентарь и Физика

Здесь мы задаём правила мира. Обрати внимание на `GRAVITY` и `JUMP_FORCE`. Именно они делают прыжок плавным, а не мгновенным телепортом.

```cpp
#include <windows.h>
#include <cstdlib>
#include <ctime>
#include <stdio.h>

#define _CRT_SECURE_NO_WARNINGS

// --- НАСТРОЙКИ ОКНА ---
const int WIN_W = 800; // Широкое и низкое окно, как в оригинале
const int WIN_H = 300;
const int GROUND_Y = 250; // Уровень земли (Y-координата)

// --- ДИНОЗАВРИК ---
int dinoX = 50;
int dinoY = GROUND_Y - 40; // Стоит на земле
const int DINO_W = 30;
const int DINO_H = 40;
int dinoDY = 0;            // Вертикальная скорость (изменяется!)

const int GRAVITY = 1;     // Сила притяжения (тянет вниз каждый кадр)
const int JUMP_FORCE = -14; // Сила прыжка (резко толкает вверх)

// --- КАКТУС ---
int cactusX = WIN_W;       // Начинает за правым краем экрана
int cactusY = GROUND_Y - 30;
const int CACTUS_W = 20;
int cactusH = 30;          // Высота может меняться
int cactusSpeed = 6;       // Скорость движения влево

// --- СОСТОЯНИЕ ИГРЫ ---
int score = 0;
bool isGameOver = false;
```

> 🛠️ **Твой эксперимент!**
> 1. Поменяй `GRAVITY` на `3`. Запусти игру. Динозаврик стал тяжёлым, как камень, и падает очень быстро!
> 2. Поменяй `JUMP_FORCE` на `-25`. Теперь это не прыжок, а запуск ракеты! Верни `-14` для комфортной игры.

---

## ❤️ Шаг 2: Сердцебиение (Физика и Движение)

В отличие от Арканоида, где скорость была постоянной, здесь скорость динозавра (`dinoDY`) **постоянно меняется** из-за гравитации.

```cpp
void ResetGame() {
    dinoY = GROUND_Y - DINO_H;
    dinoDY = 0;
    cactusX = WIN_W;
    cactusH = 30;
    cactusSpeed = 6;
    score = 0;
    isGameOver = false;
}

void UpdateGame(HWND hWnd) {
    if (isGameOver) return; // Если проиграли, ничего не обновляем

    score++; // Очки капают каждую долю секунды

    // 1. ФИЗИКА ДИНОЗАВРИКА
    dinoDY += GRAVITY;       // Гравитация тянет вниз (увеличивает Y)
    dinoY += dinoDY;         // Применяем скорость к позиции

    // Если динозаврик коснулся земли, останавливаем его
    if (dinoY >= GROUND_Y - DINO_H) {
        dinoY = GROUND_Y - DINO_H;
        dinoDY = 0;          // Скорость падает до нуля
    }

    // 2. ДВИЖЕНИЕ КАКТУСА
    cactusX -= cactusSpeed;  // Двигаем влево

    // Если кактус ушёл за левый край, "телепортируем" его направо!
    if (cactusX < -CACTUS_W) {
        cactusX = WIN_W + (rand() % 200); // Случайная задержка перед появлением
        cactusH = 30 + (rand() % 40);     // Случайная высота (от 30 до 70)
        cactusY = GROUND_Y - cactusH;     // Прижимаем к земле
        cactusSpeed += 0.5;               // Игра постепенно ускоряется!
    }

    // 3. ПРОВЕРКА СТОЛКНОВЕНИЯ (Классический AABB)
    if (dinoX < cactusX + CACTUS_W && dinoX + DINO_W > cactusX &&
        dinoY < cactusY + cactusH && dinoY + DINO_H > cactusY) {
        isGameOver = true; // БУМ! Игра окончена
    }

    InvalidateRect(hWnd, NULL, FALSE);
}
```

> 🛠️ **Твой эксперимент!**
> Найди строку `if (dinoY >= GROUND_Y - DINO_H)`. Закомментируй её (поставь `//`). Запусти игру. Динозаврик провалится сквозь землю и будет падать в бесконечность! Это доказывает, что именно эта строка "ставит" его на землю. Убери `//`.

---

## 🎮 Шаг 3: Управление

Мы разрешаем прыгать **только если динозаврик стоит на земле**. Иначе он мог бы прыгать в воздухе бесконечно (как в Flappy Bird).

```cpp
// Эту функцию мы вызовем внутри WndProc
void HandleInput(WPARAM wParam) {
    if (wParam == VK_SPACE || wParam == VK_UP) {
        if (isGameOver) {
            ResetGame(); // Перезапуск по пробелу
        } 
        else {
            // Прыгаем ТОЛЬКО если мы на земле (или очень близко к ней)
            if (dinoY >= GROUND_Y - DINO_H - 5) {
                dinoDY = JUMP_FORCE;
            }
        }
    }
}
```

> 🛠️ **Твой эксперимент!**
> Удали условие `if (dinoY >= GROUND_Y - DINO_H - 5)`. Оставь только `dinoDY = JUMP_FORCE;`. Теперь ты можешь нажимать пробел в воздухе и лететь вверх, как супермен! Это называется "Double Jump" или "Flight mode". Попробуй, а потом верни ограничение.

---

## 🎨 Шаг 4: Рисование

Сделаем стиль, похожий на оригинал: светлый фон, чёрный динозаврик и зелёный кактус.

```cpp
void DrawGame(HDC hdc) {
    // 1. Светлый фон (как в Chrome)
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

    // 3. Рисуем Динозаврика (Чёрный прямоугольник с глазом для милоты)
    HBRUSH hDino = CreateSolidBrush(RGB(50, 50, 50));
    SelectObject(hdc, hDino);
    Rectangle(hdc, dinoX, dinoY, dinoX + DINO_W, dinoY + DINO_H);
    
    // Глаз (белый пиксель)
    HBRUSH hEye = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, hEye);
    Rectangle(hdc, dinoX + 18, dinoY + 8, dinoX + 24, dinoY + 14);
    DeleteObject(hEye);
    DeleteObject(hDino);

    // 4. Рисуем Кактус (Зелёный)
    HBRUSH hCactus = CreateSolidBrush(RGB(34, 139, 34)); // Лесной зелёный
    SelectObject(hdc, hCactus);
    Rectangle(hdc, cactusX, cactusY, cactusX + CACTUS_W, cactusY + cactusH);
    DeleteObject(hCactus);

    // 5. Текст (Счёт и Game Over)
    SetBkMode(hdc, TRANSPARENT);
    wchar_t scoreText[50];
    swprintf_s(scoreText, L"Счёт: %d", score / 10); // Делим на 10, чтобы не рос слишком быстро
    SetTextColor(hdc, RGB(50, 50, 50));
    TextOutW(hdc, 20, 20, scoreText, (int)wcslen(scoreText));

    if (isGameOver) {
        SetTextColor(hdc, RGB(255, 0, 0));
        TextOutW(hdc, WIN_W / 2 - 80, WIN_H / 2 - 20, L"GAME OVER! ЖМИ R", 18);
    }
}
```

> 🛠️ **Твой эксперимент!**
> Динозаврик выглядит просто как квадрат. Попробуй нарисовать ему "ноги" или "хвост", добавив ещё один маленький `Rectangle` с координатами, зависящими от `dinoX` и `dinoY`. Например: `Rectangle(hdc, dinoX - 10, dinoY + 20, dinoX, dinoY + 30);` (это будет хвост).

---

## 🏗️ Шаг 5: Собираем всё вместе (Код Windows)

Стандартная, надёжная обвязка, которую мы уже отточили в Арканоиде.

```cpp
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow) {
    srand((unsigned)time(NULL)); // Важно для случайных кактусов!

    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = L"DinoRun";
    RegisterClassExW(&wcex);

    RECT wr = { 0, 0, WIN_W, WIN_H };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    HWND hWnd = CreateWindowW(L"DinoRun", L"Dino Run C++",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0, wr.right - wr.left, wr.bottom - wr.top,
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
```

---

## 🏆 Босс-челленджи (Сделай игру легендарной!)

1. **🐦 Птичка (Летающее препятствие):** 
   Добавь переменные `birdX`, `birdY` (например, `birdY = GROUND_Y - 70`). Заставь птичку лететь слева направо (или быстрее, чем кактус). Добавь проверку столкновения с ней. Теперь игроку нужно иногда *пригибаться* (сделать клавишу `VK_DOWN`, которая временно уменьшает `DINO_H`).
2. **🌙 Ночной режим:** 
   Сделай так, чтобы каждые 500 очков (`if (score % 5000 == 0)`) фон менялся на тёмно-синий `RGB(20, 20, 40)`, а динозаврик и текст становились белыми.
3. **⚡ Замедление времени:** 
   Добавь клавишу 'F', которая на 3 секунды делает `cactusSpeed = 2` (используй `SetTimer` с другим ID или простую переменную-таймер `slowMoTimer`), чтобы успеть проскочить сложное место.

---

### 💡 Почему этот код хорош для обучения:
* Ты видишь, как **гравитация** (`dinoDY += GRAVITY`) отличается от простого движения.
* Ты понимаешь трюк с **переиспользованием одного объекта** (кактуса) для создания бесконечного уровня.
* Здесь нет сложных циклов или массивов, только чистая логика координат.

Собирай код (Ctrl+Shift+B), запускай и попробуй набрать хотя бы 1000 очков! Если захочешь добавить птичку или ночной режим — пиши, я подскажу, какие строчки добавить! 🚀
