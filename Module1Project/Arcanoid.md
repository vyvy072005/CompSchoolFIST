Вот готовый, интерактивный `README.md` файл, созданный специально для этого кода. Он разбит на крошечные шаги, объясняет *почему* код работает именно так, и содержит задания, которые заставят ребёнка экспериментировать, а не просто копировать.

Скопируй этот текст и сохрани его как `README.md` в папке с проектом.

---

# 🧱 Твой первый Арканоид на C++ (Без сложных массивов!)

Привет! 👋 Добро пожаловать в мир создания игр. 
Обычно в Арканоиде используют "массивы" (таблицы данных), чтобы создать 100 кирпичей. Но это сложно для начала. 

Мы пойдём хитрым путём: мы создадим всего **3 кирпича** как отдельные переменные. Так ты точно поймёшь, как работает каждое столкновение, прежде чем усложнять игру.

> ⚠️ **Как пользоваться этим гайдом:** 
> Создай пустой проект C++ в Visual Studio, добавь файл `Arkanoid.cpp` и добавляй код **по одному шагу за раз**. Не копируй всё сразу!

---

## 🎒 Шаг 1: Инвентарь (Переменные)

Сначала мы должны рассказать компьютеру, какие объекты есть в игре и где они находятся.

```cpp
#include <windows.h>
#include <cstdlib>

// --- НАСТРОЙКИ ОКНА ---
const int WIN_W = 600; // Ширина
const int WIN_H = 400; // Высота

// --- РАКЕТКА ---
int padX = 250; // Позиция по горизонтали (можно менять)
const int PAD_W = 100; // Ширина ракетки
const int PAD_H = 15;  // Высота ракетки
const int PAD_Y = WIN_H - 30; // Ракетка всегда прижата к низу

// --- МЯЧ ---
int ballX = 300;
int ballY = 350;
int ballDX = 4;  // Скорость по X (вправо/влево)
int ballDY = -4; // Скорость по Y (вверх/вниз). Минус = летит вверх!
const int BALL_SIZE = 16;

// --- КИРПИЧИ (Наши 3 героя без массивов!) ---
// true = кирпич стоит, false = кирпич разбит
bool b1 = true; 
bool b2 = true; 
bool b3 = true;

const int BRICK_W = 100;
const int BRICK_H = 20;
const int BRICK_Y = 50; // Высота, на которой висят все кирпичи
```

> 🛠️ **Твой эксперимент!**
> 1. Поменяй `PAD_W` (ширину ракетки) на `300`. Запусти игру. Легко ли теперь выиграть?
> 2. Поменяй `ballDY` на `-15`. Мяч теперь летит как ракета! Верни `-4`, иначе играть будет невозможно.

---

## ❤️ Шаг 2: Сердцебиение игры (Движение)

Игра должна обновляться 60 раз в секунду. Функция `UpdateGame` меняет координаты мяча. Если `ballX` был 300, а `ballDX` равен 4, в следующем кадре `ballX` станет 304. Мяч сдвинулся!

Добавь этот код ниже переменных:

```cpp
void UpdateGame(HWND hWnd) {
    // 1. Двигаем мяч
    ballX += ballDX;
    ballY += ballDY;

    // 2. Отскок от стен (лево, право, верх)
    // Если мяч коснулся края, мы просто меняем знак скорости на противоположный!
    if (ballX <= 0 || ballX >= WIN_W - BALL_SIZE) ballDX = -ballDX;
    if (ballY <= 0) ballDY = -ballDY;

    // 3. Проигрыш (мяч улетел вниз)
    if (ballY > WIN_H) {
        ballX = 300; ballY = 350; // Возвращаем мяч на старт
        ballDX = 4; ballDY = -4;  // Возвращаем скорость
        b1 = true; b2 = true; b3 = true; // Чиним кирпичи
    }

    // Эта команда говорит Windows: "Эй, что-то изменилось, перерисуй экран!"
    InvalidateRect(hWnd, NULL, FALSE); 
}
```

> 🛠️ **Твой эксперимент!**
> Закомментируй (поставь `//` перед) строку `b1 = true; b2 = true; b3 = true;` внутри блока проигрыша. Урони мяч вниз. Что произошло? Кирпичи не восстановились! Убери `//`, чтобы вернуть игру к жизни.

---

## 🏓 Шаг 3: Ракетка и Кирпичи (Магия столкновений)

Как компьютер понимает, что мяч ударился? Он проверяет, пересекаются ли прямоугольник мяча и прямоугольник объекта.

Добавь этот код в конец функции `UpdateGame` (перед `InvalidateRect`):

```cpp
    // 4. Отскок от ракетки
    if (ballY + BALL_SIZE >= PAD_Y && ballY <= PAD_Y + PAD_H &&
        ballX + BALL_SIZE >= padX && ballX <= padX + PAD_W) {
        
        ballDY = -abs(ballDY); // Мяч летит строго вверх
        
        // Хитрый трюк: чем дальше от центра ракетки удар, тем сильнее мяч летит вбок
        ballDX = (ballX + BALL_SIZE / 2 - (padX + PAD_W / 2)) / 10; 
    }

    // 5. Разбивание кирпичей (Проверяем каждый ВРУЧНУЮ, без циклов)
    
    // Кирпич 1 (Слева: X от 100 до 200)
    if (b1 && ballX + BALL_SIZE >= 100 && ballX <= 200 && ballY <= BRICK_Y + BRICK_H && ballY + BALL_SIZE >= BRICK_Y) {
        b1 = false; // Ломаем кирпич!
        ballDY = -ballDY; // Отскакиваем
    }
    
    // Кирпич 2 (Центр: X от 250 до 350)
    if (b2 && ballX + BALL_SIZE >= 250 && ballX <= 350 && ballY <= BRICK_Y + BRICK_H && ballY + BALL_SIZE >= BRICK_Y) {
        b2 = false; 
        ballDY = -ballDY; 
    }
    
    // Кирпич 3 (Справа: X от 400 до 500)
    if (b3 && ballX + BALL_SIZE >= 400 && ballX <= 500 && ballY <= BRICK_Y + BRICK_H && ballY + BALL_SIZE >= BRICK_Y) {
        b3 = false; 
        ballDY = -ballDY; 
    }
```

> 🛠️ **Твой эксперимент!**
> 1. Попробуй добавить **4-й кирпич**! 
>    - В Шаге 1 добавь: `bool b4 = true;`
>    - Здесь добавь блок `if` для `b4` с координатами X от `50` до `150`.
> 2. Удали `-abs` из строки `ballDY = -abs(ballDY);`. Ударь мячом о движущуюся ракетку. Видишь, как мяч иногда пролетает *сквозь* ракетку вниз? `abs` (модуль числа) гарантирует, что скорость вверх всегда будет отрицательной. Верни `abs` на место!

---

## 🎨 Шаг 4: Художник (Рисование)

Windows даёт нам "холст" (`HDC`). Мы берём "кисть" (`HBRUSH`), рисуем и **обязательно выбрасываем** её (`DeleteObject`), иначе память компьютера засорится и игра начнёт тормозить.

Добавь эту функцию выше `UpdateGame`:

```cpp
void DrawGame(HDC hdc) {
    // 1. Рисуем тёмно-синий фон
    HBRUSH hBg = CreateSolidBrush(RGB(20, 20, 40));
    RECT rect = { 0, 0, WIN_W, WIN_H };
    FillRect(hdc, &rect, hBg);
    DeleteObject(hBg);

    // 2. Ракетка (Зелёная)
    HBRUSH hPad = CreateSolidBrush(RGB(0, 255, 100));
    SelectObject(hdc, hPad); // Берём кисть в руку
    Rectangle(hdc, padX, PAD_Y, padX + PAD_W, PAD_Y + PAD_H);

    // 3. Мяч (Белый)
    HBRUSH hBall = CreateSolidBrush(RGB(255, 255, 255));
    SelectObject(hdc, hBall);
    Ellipse(hdc, ballX, ballY, ballX + BALL_SIZE, ballY + BALL_SIZE);

    // 4. Кирпичи (Красные). Рисуем только если переменная == true!
    HBRUSH hBrick = CreateSolidBrush(RGB(255, 100, 100)); 
    SelectObject(hdc, hBrick);
    
    if (b1) Rectangle(hdc, 100, BRICK_Y, 200, BRICK_Y + BRICK_H);
    if (b2) Rectangle(hdc, 250, BRICK_Y, 350, BRICK_Y + BRICK_H);
    if (b3) Rectangle(hdc, 400, BRICK_Y, 500, BRICK_Y + BRICK_H);

    // 5. Сообщение о победе
    if (!b1 && !b2 && !b3) { // Если все три == false
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(0, 255, 0)); // Ярко-зелёный текст
        TextOutW(hdc, 200, 150, L"ПОБЕДА! НАЖМИ R", 17);
    }

    // УБОРКА: Уничтожаем кисти, чтобы не было утечки памяти
    DeleteObject(hPad);
    DeleteObject(hBall);
    DeleteObject(hBrick);
}
```

> 🛠️ **Твой эксперимент!**
> 1. Сделай кирпичи разноцветными! Создай три разные кисти: `hBrick1` (синий), `hBrick2` (жёлтый), `hBrick3` (фиолетовый). Выбери нужную кисть через `SelectObject` перед рисованием каждого конкретного кирпича.
> 2. Замени функцию `Ellipse` (круг) на `Rectangle` (квадрат). Теперь у тебя майнкрафт-стайл мяч!

---

## 🏗️ Шаг 5: Собираем всё вместе (Код Windows)

Это "клей", который создаёт окно и слушает нажатия клавиш. 
Функция `WndProc` работает как почтовое отделение: она получает сообщения (`WM_KEYDOWN` = клавиша нажата) и решает, что с ними делать.

Вставь этот код в самый низ файла:

```cpp
// Предварительное объявление (чтобы компилятор не ругался)
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPWSTR, _In_ int nCmdShow) {
    // Настраиваем класс окна (ОЧЕНЬ ВАЖНО делать это по полям, чтобы не было ошибок!)
    WNDCLASSEXW wcex = {};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hIcon = nullptr;           
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);  // Курсор мыши
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); 
    wcex.lpszClassName = L"Arkanoid";
    
    RegisterClassExW(&wcex);

    // Рассчитываем размер окна так, чтобы игровое поле было ровно 600x400
    RECT wr = { 0, 0, WIN_W, WIN_H };
    AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, FALSE);

    HWND hWnd = CreateWindowW(L"Arkanoid", L"Простой Арканоид",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, 0, wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, hInstance, nullptr);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // Бесконечный цикл чтения сообщений
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}

// Обработка событий (нажатия клавиш, таймер, рисование)
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        SetTimer(hWnd, 1, 15, NULL); // Запуск таймера: 15 мс = ~60 кадров в секунду
        break;
    case WM_TIMER:
        UpdateGame(hWnd); // Каждый тик обновляем логику
        break;
    case WM_KEYDOWN:
        // Управление ракеткой
        if (wParam == VK_LEFT && padX > 0) padX -= 15;
        if (wParam == VK_RIGHT && padX < WIN_W - PAD_W) padX += 15;
        
        // Перезапуск игры кнопкой R
        if (wParam == 'R') { 
            b1 = true; b2 = true; b3 = true; 
            ballX = 300; ballY = 350; ballDX = 4; ballDY = -4; 
        }
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        DrawGame(hdc); // Рисуем кадр
        EndPaint(hWnd, &ps);
        break;
    }
    case WM_DESTROY:
        KillTimer(hWnd, 1); // Останавливаем таймер при закрытии
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
```

> 🛠️ **Твой эксперимент!**
> Поменяй управление! Замени `VK_LEFT` на `'A'`, а `VK_RIGHT` на `'D'`. Теперь ты управляешь как в современных играх, без стрелочек!

---

## 🏆 Босс-челленджи (Сделай игру уникальной!)

Выбери одно задание и попробуй реализовать его сам:

1. **👻 Кирпич-призрак:** Сделай так, чтобы один из кирпичей (например, `b2`) был невидимым, но мяч от него отскакивал! *(Подсказка: в `DrawGame` используй для него цвет фона `RGB(20, 20, 40)`).*
2. **⚡ Режим "Берсерк":** В блоке отскока от ракетки добавь строку `ballDX = ballDX * 1.2;` и `ballDY = ballDY * 1.2;`. С каждым ударом мяч будет становиться всё быстрее и быстрее!
3. **📏 Растущая ракетка:** В момент разбивания кирпича (`b1 = false;`) добавь строку `padX -= 10;` и измени `PAD_W` (но так как `PAD_W` это `const`, тебе придётся сделать его обычной переменной `int PAD_W = 100;` в начале файла).

---

## 🆘 Частые ошибки и их решение

| Ошибка | Почему возникла | Как исправить |
| :--- | :--- | :--- |
| `E0144 значение типа "HBRUSH" нельзя использовать...` | При создании `WNDCLASSEXW` пропущено поле (например, `hIcon`), и все значения сдвинулись. | Используй инициализацию по полям (`wcex.hCursor = ...`), как показано в Шаге 5. Не пиши всё через запятую в одной строке! |
| Мяч застревает в стене или ракетке | Мы забыли "вытолкнуть" мяч из объекта перед сменой направления. | Проверь, что в коде есть строки вроде `ballY = 0;` или `ballDY = -abs(ballDY);`. |
| Игра не реагирует на клавиши | Ты нажимаешь клавиши, когда окно не активно (не кликнул по окну мышкой). | Кликни по окну игры один раз, чтобы передать ему "фокус". |

> 💡 **Помни:** Программирование — это 90% экспериментов и 10% написания кода. Меняй цифры, ломай код и чини его снова. Удачи! 🚀
