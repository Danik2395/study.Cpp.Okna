# Окна

## Лабораторные
1. Рекурсия ([Код](Lab1WIndow.cpp))


---

## Классы
### [BaseWindow](BaseWindow.h)
- Первый в каскаде
- Создаёт окно и регистрирует класс
- "Кладёт" в окно указатель на его класс
- **pThis**
	- Легаси коду в `RegisterClass()` нужна конкретная функция с 4-мя параметрами
	- Метод создаёт 5-й параметр `this`
	- Обходим это `static` на `WindowProc`
- Обрабатывает 
	- `MW_DESTROY` 
	- И вызывает `DefWindowProc` на дефолт свича

### [D2DWindow](D2DWindow.h)
- Наследуется от [`BaseWindow`](#basewindow)
- Добавляет Direct2D и DirectWrite
- Создаёт
	- `ID2D1Factory`
	- `ID2D1HwndRenderTarget`
	- `IDWriteFactory`
	- Объект класса [`DpiScale`](#dpiscale)
		- Макрос `S()` для скейла
- `CreateGraphicsResources()`
	- Вызывает `virtual CreateDeviceDepRec()` для создания ресурсов для конкретных окон
- `OnPaint`
	- Инициализирует рисование
	- И вызывает `DrawContent` для отрисовки реализованной в конкретных окнах
- Обрабатывает
	- `WM_PAINT`
	- `WM_CREATE`
	- `WM_SIZE`
	- И отдаёт дефолт в `BaseWindow`

### [MainWindow](MainWindow.h)
- Наследуется от [`D2DWindow`](#d2dwindow)
- Обрабатывает 
	- `WM_SETCURSOR`
	- И отдаёт дефолт в `MainWindow`
- Рисует шумный фон

### [DpiScale](DpiScale.h)
- Создаётся для каждого окна
	- Композиция
- Переводит DIPs в физические пиксели и наоборот
- Имеет `static` метод для скейла по дефолтным настройкам системы
	- Там, где окно ещё не создано

### [ThreadLauncher](ThreadLauncher.h)
- Создаёт новое окно в отдельном потоке
	- В	случае,если сущностей окна не больше заданного числа

### [WndProps](WndProps.h)
- Наследуется Parent окнами
	- В конструкторе окна вызывается конструктор `WndProps`
- Содержит имя и начальные размеры каждого окна
- "Перегрузка" для дефолтного значения количества окон
	- Не добавялет логику проверки количества окон
	- Метод проверки возвращает 0

---

### [IControl](IControl.h)
- Чисто виртуальный класс
- Можно было и без него, но чтобы был `Move()` в каждом контроле

### [MainButton](Mainbutton.h)
- Наследуется от 
	- [`D2DWindow`](#d2dwindow)
- Реализует
	- [`IControl`](#icontrol)
- Окно-кнопка
- Создаёт себя через `BaseWindow::Create()` с заданными параметрами
- Рисует
	- Себя
	- Текст
- Обрабатывает
	- `WM_SETCURSOR`
	- Одиночные нажатия
		- Отправляет сообщение о нажатии
		- "Кладёт" id в lowWord `wParam`

### [MainCheckBox](MainCheckBox.h)
- Наследуется от 
	- [`D2DWindow`](#d2dwindow)
- Реализует
	- [`IControl`](#icontrol)
- Окно-чекбокс
- Работает как кнопка, но имеет состояние `isSelected`

### [MainEdit](MainEdit.h)
- Реализует
	- [`IControl`](#icontrol)
- Обёртка для `EDIT` (Не Subclass)
- Создаёт
	- Окно класса `EDIT`
	- C заданными флагами стиля
	- С заданными параметрами
	- C текстом, [скейленым](#dpiscale) под DPI
- Может
	- `SetText()`
	- `GetText()`
	- `static GetNumber()` для получения чистого числа

---

### [HubWindow](HubWindow.h)
- Наследуется от 
	- [`MainWindow`](#mainwindow)
- Реализует
	- [`WndProps`](#wndprops)
- Расставляет кнопки для создания окон лабораторных