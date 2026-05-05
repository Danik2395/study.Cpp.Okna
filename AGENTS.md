# Окна

## Build

Single Visual Studio 2022 solution (`Окна.sln`), one project (`testWinApp.vcxproj`).
Build from VS or MSBuild: any `Debug|x64` / `Release|x64`. Platform toolset v143.
C++ standard: ISO C++17 (`/std:c++17`). Subsystem: Windows (not Console).

No external dependencies — only Windows SDK + Direct2D/DirectWrite.
No tests, no linter, no formatter, no CI.

## Architecture

- All source `.h` / `.cpp` files live in the repo root (no subdirectories).
- CRTP window hierarchy: `BaseWindow<T>` → `D2DWindow<T>` → `MainWindow<T>` → concrete windows.
- BaseWindow registers the window class and stores `this` via `GWLP_USERDATA` on `WM_NCCREATE`.
- D2DWindow adds Direct2D render target, DirectWrite factory, and `DpiScale` (DIP-to-physical-pixel scaling). Use macro `S(value)` to scale DIPs at runtime.
- MainWindow adds `WM_SETCURSOR` handling and noisy background drawing.
- Each lab window inherits `WndProps<T, maxInstances>` (position, size, instance count cap) + `MainWindow<T>`.
- `HubWindow` is the launcher: shows buttons that call `LaunchLabInThread<T>()` (from `ThreadLauncher.h`), which creates the lab window **in a detached thread with its own message loop** (non-resizable, no maximize box).
- Controls (`MainButton`, `MainEdit`, `MainCheckBox`, `MainRadioButton`, `VariablesListView`) are child windows, each implementing `IControl`. `MainEdit` and `VariablesListView` use Win32 subclassing.
- Random: `my_random.h` wraps XORshift64 (not std::mt19937).
- Containers: `SingleNodeList` (singly-linked, bubble sort by pointer swap), `List` (doubly-linked with C-string support), `Stack` (adapter over SingleNodeList).
- `shuntingYard.h` implements Dijkstra's shunting-yard algorithm; throws `shuntingExeption` (custom `wchar_t`-compatible exception).
- Color palette in `Theme.h` namespace `thm` — dark theme (dark grays, purple accent).
- `Utils.h` / `UTL::GetNumber<T>()` — string-to-number parser that filters non-numeric characters and checks type limits.

## DPI

- Manifest enables DPI awareness (`<EnableDpiAwareness>true</EnableDpiAwareness>`).
- `WndProps` constructor scales width/height/position via `DpiScale::ScaleForSystem()` (system DPI before window creation).
- Per-window scaling via `DpiScale::Init(hwnd)` / `S()` macro in `WM_CREATE`.
- `MainEdit` uses `CreateWindowEx` with font size pre-scaled by DPI.
- `MainRadioButton` uses GDI region clipping for circular shape.

## Conventions

- `#define UNICODE` before any includes in `main.cpp` (file starts with it).
- `#pragma once` in headers.
- `CComPtr` from `<atlbase.h>` for COM resource management.
- Windows-style `\r\n` for multi-line `MainEdit` text.
- Button IDs are `#define`d per window file (e.g. `HUB_ID_BTN_LAB1`, `L1_ID_BTTN_CALC`).

## Code Style

### Naming
- **Classes**: PascalCase (`BaseWindow`, `D2DWindow`, `MainButton`, `HubWindow`).
- **Template params**: UPPER_SNAKE (`DERIVED_TYPE`) or `T`.
- **Member variables**: `m_` prefix only for HWND (`m_hwnd`); `_` suffix for others (`sign_`, `posX_`, `wndWidth_`).
- **Pointers**: `Type* pVar` — `*` adjacent to type, `p` prefix (`pFactory`, `pRenderTarget`, `pThis`).
- **Methods**: PascalCase (`HandleMessage`, `CreateGraphicsResources`, `DrawContent`).
- **Locals**: camelCase (`len`, `dotCount`, `buffer`).
- **Macros / constants**: UPPER_SNAKE (`HUB_ID_BTN_LAB1`, `S(value)`).

### Formatting
- **Indentation**: tabs (inconsistent in older files, but tabs are the common pattern).
- **Braces**: K&R (opening brace on same line).
- **Switch**: `case` labels at same indent level as `switch` (not inside the `{` block).
- **Spacing**: `if (cond)`, `for (;;)`, `switch (uMsg)` — space after keyword, no space inside parens.

### Comments
- Standard `//` only (never `///` Doxygen).
- File header: `// FileName.h` / `// FileName.cpp` at top.
- Functional/logic comments in in English.

### Idioms
- `#pragma once` in headers. `#pragma comment(lib, "d2d1.lib")` in headers that need it (no .vcxproj lib refs for Direct2D/DirectWrite).
- `CComPtr` from `<atlbase.h>` for COM. `std::make_unique` for ownership. `new` in custom containers.
- `NULL` (not `nullptr`). `L"..."` wide string literals. `std::wstring` throughout.
- `const &` for non-trivial parameters (`const std::wstring &text`).
- Constructor init lists: comma-first per-member alignment.
- `inline static` for class-level constants/statics in headers.
- `if constexpr`, `std::is_same_v`, structured bindings (C++17).
- Use `UTL::GetNumber<T>()` (from `Utils.h`) for string-to-number, not `std::stoi`/`std::stod` directly.
- Use `m_random<T>` (XORshift64 wrapper) for random, not `<random>`.
- Use custom containers (`SingleNodeList`, `List`, `Stack`) over STL equivalents.

## Output format
- Think only in English in the`<Think>` block.
- Answer the user in Russian.
