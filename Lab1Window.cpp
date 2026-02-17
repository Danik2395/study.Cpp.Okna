//
// Lab1Window.cpp
//
#include "Lab1Window.h"


double Lab1Window::cycle(int n)
{
    double result{ 1 };

    for (n *= 2; n != 0; n -= 2) // Using steps as series member and multiplying it by two
    {
        result *= static_cast<double>(n) * n / (n * n - 1);
    }
    return result;
}

double Lab1Window::recursion(int n)
{                                // The same but in other form
    return n == 1 ? static_cast<double>(n) * n * 4 / (n * n * 4 - 1) :
                    static_cast<double>(n) * n * 4 / (n * n * 4 - 1) * recursion(n - 1);
}


void Lab1Window::calc()
{
    int steps = MainEdit::GetNumber<int>(std::move(inField->GetText()));

    if (steps > 1000)  steps = 1000;

    else if (steps < 2) steps = 2;

    inField->SetText(std::to_wstring(steps));

    outCycleField->SetText(std::to_wstring(cycle(steps)));
    outRecursionField->SetText(std::to_wstring(recursion(steps)));
}



void Lab1Window::CreateDeviceDepRes(HRESULT &hr)
{
    MainWindow::CreateDeviceDepRes(hr);

    if (!pLabelTextFormat && pDWriteFactory)
    {
        hr = pDWriteFactory->CreateTextFormat(
            L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
            15.0f, L"ru-ru", &pLabelTextFormat
        );


        if (SUCCEEDED(hr))
        {
            hr = pLabelTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
            hr = pLabelTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
        }
    }

    if (SUCCEEDED(hr) && !pLabelColorBrush)
    {
        hr = this->pRenderTarget->CreateSolidColorBrush(
            D2D1::ColorF(thm::textColor),
            &pLabelColorBrush
        );
    }
}



void Lab1Window::DrawContent()
{
    MainWindow::DrawContent();

    if (pLabelTextFormat && pLabelColorBrush)
    {
        D2D1_RECT_F textRect = D2D1::RectF(20.0f, 20.0f, 20.0f + 100.0f, 30.0f);
        pRenderTarget->DrawText(
            L"Количество итераций",
            19,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

        textRect = D2D1::RectF(200.0f, 5.0f, 200.0f + 280.0f, 20.0f);
        pRenderTarget->DrawText(
            L"Вариант 12",
            10,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

        textRect = D2D1::RectF(20.0f, 150.0f, 20.0f + 150.0f, 180.0f);
        pRenderTarget->DrawText(
            L"Результат цикла",
            15,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

        textRect = D2D1::RectF(200.0f, 150.0f, 200.0f + 150.0f, 180.0f);
        pRenderTarget->DrawText(
            L"Результат рекурсии",
            18,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

    }
}



LRESULT Lab1Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        dpiS.Init(m_hwnd);

        staticField = std::make_unique<MainEdit>(
            ID_VAR_12_FIELD,
            S(280), S(50),
            S(200), S(30),
            15,
            m_hwnd,
            ES_CENTER | ES_READONLY
        );

        inField = std::make_unique<MainEdit>(
            ID_VAR_12_FIELD,
            S(100), S(30),
            S(20), S(50),
            16,
            m_hwnd,
            ES_CENTER | ES_NUMBER
        );

        outCycleField = std::make_unique<MainEdit>(
            ID_VAR_12_FIELD,
            S(150), S(30),
            S(20), S(180),
            16,
            m_hwnd,
            ES_CENTER | ES_READONLY
        );

        outRecursionField = std::make_unique<MainEdit>(
            ID_VAR_12_FIELD,
            S(150), S(30),
            S(200), S(180),
            16,
            m_hwnd,
            ES_CENTER | ES_READONLY
        );

        calcButton = std::make_unique<MainButton>(
            ID_BTTN_CALC,
            L"Посчитать",
            S(120), S(35),
            S(20), S(100),
            m_hwnd
        );



        staticField->Create();
        inField->Create();
        outCycleField->Create();
        outRecursionField->Create();
        calcButton->Create();

        staticField->SetText(L"Y = (²⁄₁ · ²⁄₃) · (⁴⁄₃ · ⁴⁄₅) · (⁶⁄₅ · ⁶⁄₇)");

        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        switch (id)
        {
        case ID_BTTN_CALC:
            calc();
            return 0;
        }
        return 0;
    }
    }
    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}