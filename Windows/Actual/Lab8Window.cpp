//
// Lab8Window.cpp
//
#include "Lab8Window.h"

void Lab8Window::setLimits()
{
    if (limMinField->Empty()) limMinField->SetText(L"-1");
    if (limMaxField->Empty()) limMaxField->SetText(L"4");

    std::wstring minLimFieldText = limMinField->GetText();
    std::wstring maxLimFieldText = limMaxField->GetText();

    limMin_ = UTL::GetNumber<double>(minLimFieldText);
    limMax_ = UTL::GetNumber<double>(maxLimFieldText);

    if (limMin_ > limMax_)
    {
        double limTemp = limMin_;
        limMin_ = limMax_;
        limMax_ = limTemp;
    }

    limMinField->SetText(std::to_wstring(limMin_));
    limMaxField->SetText(std::to_wstring(limMax_));

    plotWindow->SetXRange(limMin_, limMax_);
}

void Lab8Window::setPartitions()
{
    if (partirionsField->Empty()) partirionsField->SetText(L"0.01");
    std::wstring stepFieldText = partirionsField->GetText();
    partitions_ = UTL::GetNumber<double>(stepFieldText);
    partirionsField->SetText(std::to_wstring(partitions_));
}

void Lab8Window::setEpsilon()
{
    if (partirionsField->Empty()) partirionsField->SetText(L"0.001");
    std::wstring epsilonFieldText = epsilonField->GetText();
    epsilon_ = UTL::GetNumber<double>(epsilonFieldText);
    epsilonField->SetText(std::to_wstring(epsilon_));
    integralSolver.SetEpsilon(epsilon_);
}

void Lab8Window::calculate()
{
    shabang();
    try
    {
        std::wstring resultText{};
        if (radiAuto->IsSelected())
        {
            auto integralResult = integralSolver.SolveGauss3Auto(limMin_, limMax_);
            resultText = L"Расчёт по точности: " + std::to_wstring(epsilon_) + L"\r\n" +
                         L"Количество разбиений: " + std::to_wstring(integralResult.second) + L"\r\n" +
                         L"Значение интеграла: " + std::to_wstring(integralResult.first);
        }
        else
        {
            double integralResult = integralSolver.SolveGauss3(limMin_, limMax_, partitions_);
            resultText = L"Количество разбиений: " + std::to_wstring(partitions_) + L"\r\n" +
                         L"Значение интеграла: " + std::to_wstring(integralResult);
        }
        resultField->AppendText(resultText);
    }
    catch (const IntegralSolver<decltype(&Functions::lab8Function)>::integralException &e)
    {
        resultField->AppendText(e.wwhat());
    }
}

void Lab8Window::CreateDeviceDepRes(HRESULT& hr)
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

void Lab8Window::DrawContent()
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
            L"N",
            1,
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

		pRenderTarget->DrawText(
			L"По N",
			4,
			pLabelTextFormat,
			D2D1::RectF(434.0f, 18.0f, 500.0f, 46.0f),
			pLabelColorBrush
		);

		pRenderTarget->DrawText(
			L"Авто",
			4,
			pLabelTextFormat,
			D2D1::RectF(434.0f, 63.0f, 500.0f, 91.0f),
			pLabelColorBrush
		);
    }
}

LRESULT Lab8Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
	{
	case WM_CREATE:
	{
		dpiS.Init(m_hwnd);

		plotWindow = std::make_unique<PlotWindow<decltype(&Functions::lab6Function)>>(
			L8_IDPLOT,
			S(535), S(360),
			S(265), S(110),
			m_hwnd,
            -5, 5,
            -3, 12
		);
		plotWindow->SetFunction(Functions::lab6Function);

		limMinField = std::make_unique<MainEdit>(
			L8_IDFIELD_LIM_MIN,
			S(120), S(28),
			S(43), S(18),
			14,
			m_hwnd,
			ES_CENTER
		);

		limMaxField = std::make_unique<MainEdit>(
			L8_IDFIELD_LIM_MAX,
			S(120), S(28),
			S(43), S(63),
			14,
			m_hwnd,
			ES_CENTER
		);

		partirionsField = std::make_unique<MainEdit>(
			L8_IDFIELD_STEP,
			S(120), S(28),
			S(282), S(18),
			14,
			m_hwnd,
			ES_CENTER
		);

		epsilonField = std::make_unique<MainEdit>(
			L8_IDFIELD_EPSILON,
			S(120), S(28),
			S(282), S(63),
			14,
			m_hwnd,
			ES_CENTER
		);

		resultField = std::make_unique<MainEdit>(
			L8_IDFIELD_RESULT,
			S(230), S(360),
			S(20), S(110),
			11,
			m_hwnd,
			ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY
		);

        radiPartitions = std::make_unique<MainRadioButton<L8_IDRADI_GROUP>>(
            L8_IDRADI_PARTITIONING,
            S(20), S(20),
            S(410), S(22),
            m_hwnd
        );

        radiAuto = std::make_unique<MainRadioButton<L8_IDRADI_GROUP>>(
            L8_IDRADI_AUTO,
            S(20), S(20),
            S(410), S(67),
            m_hwnd,
            true
        );

		bttnCalculate = std::make_unique<MainButton>(
			L8_IDBTTN_CALCULATE,
			L"Посчитать",
			S(140), S(53),
			S(562), S(28),
			m_hwnd
		);

		plotWindow->Create();
		limMinField->Create();
		limMaxField->Create();
		partirionsField->Create();
		epsilonField->Create();
		resultField->Create();
		radiPartitions->Create();
		radiAuto->Create();
		bttnCalculate->Create();

        shabang();
        limMaxField->SetText(L"4");
        limMinField->SetText(L"-1");
        epsilonField->SetText(L"0.001");
        partirionsField->SetText(L"20");

		return 0;
	}

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        WORD hWord = HIWORD(wParam);
        switch (id)
        {
        case L8_IDFIELD_LIM_MAX:
            [[fallthrough]];
        case L8_IDFIELD_LIM_MIN:
            if (hWord == EN_KILLFOCUS)
            {
                setLimits();
                return 0;
            }
            break;

        case L8_IDFIELD_STEP:
            if (hWord == EN_KILLFOCUS)
            {
                setPartitions();
                return 0;
            }
            break;

        case L8_IDFIELD_EPSILON:
            if (hWord == EN_KILLFOCUS)
            {
                setEpsilon();
                return 0;
            }
            break;

        case L8_IDBTTN_CALCULATE:
            setLimits();
            setPartitions();
            setEpsilon();
            calculate();
        }
        return 0;
    }
    }
    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}
