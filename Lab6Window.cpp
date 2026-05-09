//
// Lab6Window.cpp
//
#include "Lab6Window.h"

void Lab6Window::setLimits()
{
    if (limMinField->Empty()) limMinField->SetText(L"-4");
    if (limMaxField->Empty()) limMaxField->SetText(L"2");

    std::wstring minLimFieldText = limMinField->GetText();
    std::wstring maxLimFieldText = limMaxField->GetText();

    limMin_ = UTL::GetNumber<double>(minLimFieldText);
    limMax_ = UTL::GetNumber<double>(maxLimFieldText);

    limMinField->SetText(std::to_wstring(limMin_));
    limMaxField->SetText(std::to_wstring(limMax_));

    plotWindow->SetXRange(limMin_, limMax_);
}

void Lab6Window::setStep()
{
    if (stepField->Empty()) stepField->SetText(L"0.01");
    std::wstring stepFieldText = stepField->GetText();
    step_ = UTL::GetNumber<double>(stepFieldText);
    stepField->SetText(std::to_wstring(step_));
}

void Lab6Window::setEpsilon()
{
    if (stepField->Empty()) stepField->SetText(L"0.001");
    std::wstring epsilonFieldText = epsilonField->GetText();
    epsilon_ = UTL::GetNumber<double>(epsilonFieldText);
    epsilonField->SetText(std::to_wstring(epsilon_));
}

void Lab6Window::calculate()
{
    shabang();
    auto intervals = noLinearSolver.FindIntervals(limMin_, limMax_, step_);
    if (intervals.empty())
    {
        resultField->AppendText(L"Корни не найдены.");
        return;
    }
    std::wstring intervalsFoundText{L"Интервалов найдено: "};
    intervalsFoundText.append(std::to_wstring(intervals.size()));
    resultField->AppendText(std::move(intervalsFoundText));
    resultField->AppendText(L"\r\n\r\n");
    int rootCount = 1;
    for (auto &interval : intervals)
    {
        std::wstring rootText{ L"Корень " + std::to_wstring(rootCount++) + L":\r\n\t"};
        double initialGuess = (interval.first + interval.second) / 2.0;
        try
        {
            double root = noLinearSolver.SolveNewton(initialGuess);
            rootText.append(std::to_wstring(root).append(L"\r\n"));
        }
        catch (const NoLinearSolver<decltype(&Functions::linearFunction)>::noLinearException &e)
        {
            rootText.append(std::wstring(e.wwhat()).append(L"\r\n"));
        }
        resultField->AppendText(rootText);
    }
}

void Lab6Window::CreateDeviceDepRes(HRESULT& hr)
{
    MainWindow::CreateDeviceDepRes(hr);

    if (!pLabelTextFormat && pDWriteFactory)
    {
        hr = pDWriteFactory->CreateTextFormat(
            L"Segoe UI", NULL,
            DWRITE_FONT_WEIGHT_BOLD, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
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

void Lab6Window::DrawContent()
{
    MainWindow::DrawContent();

    if (pLabelTextFormat && pLabelColorBrush)
    {
        pRenderTarget->DrawText(
            L"A",
            1,
            pLabelTextFormat,
            D2D1::RectF(20.0f, 18.0f, 40.0f, 46.0f),
            pLabelColorBrush
        );

        pRenderTarget->DrawText(
            L"B",
            1,
            pLabelTextFormat,
            D2D1::RectF(20.0f, 63.0f, 40.0f, 91.0f),
            pLabelColorBrush
        );

        pRenderTarget->DrawText(
            L"Шаг",
            3,
            pLabelTextFormat,
            D2D1::RectF(240.0f, 18.0f, 280.0f, 46.0f),
            pLabelColorBrush
        );

        pRenderTarget->DrawText(
            L"eps",
            3,
            pLabelTextFormat,
            D2D1::RectF(240.0f, 63.0f, 280.0f, 91.0f),
            pLabelColorBrush
        );
    }
}

LRESULT Lab6Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
	{
	case WM_CREATE:
	{
		dpiS.Init(m_hwnd);

		plotWindow = std::make_unique<PlotWindow<decltype(&Functions::linearFunction)>>(
			L6_IDPLOT,
			S(585), S(360),
			S(215), S(110),
			m_hwnd,
            -5, 5,
            -3, 12
		);
		plotWindow->SetFunction(Functions::linearFunction);

		limMinField = std::make_unique<MainEdit>(
			L6_IDFIELD_LIM_MIN,
			S(120), S(28),
			S(43), S(18),
			14,
			m_hwnd,
			ES_CENTER
		);

		limMaxField = std::make_unique<MainEdit>(
			L6_IDFIELD_LIM_MAX,
			S(120), S(28),
			S(43), S(63),
			14,
			m_hwnd,
			ES_CENTER
		);

		stepField = std::make_unique<MainEdit>(
			L6_IDFIELD_STEP,
			S(120), S(28),
			S(282), S(18),
			14,
			m_hwnd,
			ES_CENTER
		);

		epsilonField = std::make_unique<MainEdit>(
			L6_IDFIELD_EPSILON,
			S(120), S(28),
			S(282), S(63),
			14,
			m_hwnd,
			ES_CENTER
		);

		resultField = std::make_unique<MainEdit>(
			L6_IDFIELD_RESULT,
			S(185), S(360),
			S(20), S(110),
			12,
			m_hwnd,
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY
		);

		bttnCalculate = std::make_unique<MainButton>(
			L6_IDBTTN_CALCULATE,
			L"Посчитать",
			S(140), S(53),
			S(462), S(28),
			m_hwnd
		);

		plotWindow->Create();
		limMinField->Create();
		limMaxField->Create();
		stepField->Create();
		epsilonField->Create();
		resultField->Create();
		bttnCalculate->Create();

        shabang();
        limMaxField->SetText(L"2");
        limMinField->SetText(L"-4");
        epsilonField->SetText(L"0.001");
        stepField->SetText(L"0.01");

		return 0;
	}

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        WORD hWord = HIWORD(wParam);
        switch (id)
        {
        case L6_IDFIELD_LIM_MAX:
            [[fallthrough]];
        case L6_IDFIELD_LIM_MIN:
            if (hWord == EN_KILLFOCUS)
            {
                setLimits();
                return 0;
            }
            break;

        case L6_IDFIELD_STEP:
            if (hWord == EN_KILLFOCUS)
            {
                setStep();
                return 0;
            }
            break;

        case L6_IDFIELD_EPSILON:
            if (hWord == EN_KILLFOCUS)
            {
                setEpsilon();
                return 0;
            }
            break;

        case L6_IDBTTN_CALCULATE:
            setLimits();
            setStep();
            setEpsilon();
            calculate();
        }
        return 0;
    }
    }
    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}
