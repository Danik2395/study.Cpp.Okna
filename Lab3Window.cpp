//
// Lab3Window.cpp
//
#include "Lab3Window.h"


#define CLEAR_LIST(lst)\
do\
{\
(lst)->~List();\
new (lst) List<int>();\
} while (0);\



int numStrLen(int val)
{
    int len = 2; // \r + \n
    if (val < 0)
    {
        ++len;
        val = -val;
    }
    do
    {
        val /= 10;
        ++len;
    } while (val != 0);
    return len;
}



void Lab3Window::clearAll()
{
    CLEAR_LIST(&list)

	outField->RemoveText();
    inField->RemoveText();
    setBottomLine();

    radiBottom->SetSelected(L3_IDRADI_BOTTOM);
    selectedRadi = L3_IDRADI_BOTTOM;
    bottomLinePos = 0;
}



void Lab3Window::randCreateList()
{
    m_random<int> dist(5, 15);
    int listSize = UTL::GetNumber<int>(inField->GetText());
    if (listSize < 1) listSize = dist;
    else if (listSize > 100) listSize = 100;

    inField->SetText(std::to_wstring(listSize));

    if (!list.empty())
    {
        CLEAR_LIST(&list)
    }

    outField->RemoveText();
    bottomLinePos = 0;
    setBottomLine();

    dist.setLimits(-1000000, 1000000);
    if (radiBottom->IsSelected())
    {
		while (listSize != 0)
		{
			int val = dist();

			list.push_back(val);

			outField->ReplaceSelText(std::to_wstring(val).append(L"\r\n"), bottomLinePos, bottomLinePos);
			bottomLinePos += numStrLen(val);

			--listSize;
		}
    }
    else
    {
		while (listSize != 0)
		{
			int val = dist();

			list.push_front(val);

			outField->ReplaceSelText(std::to_wstring(val).append(L"\r\n"), 0, 0);
			bottomLinePos += numStrLen(val);

			--listSize;
		}
    }
}



void Lab3Window::addToList()
{
    std::wstring setOfNumber = inField->GetText();
    if (setOfNumber.length() == 0) return;

    std::wstring buffer;
    std::wstring cleanString;
    for (auto iter = setOfNumber.begin(); iter != setOfNumber.end();)
    {
        bool isNumber = false;
        while (
            iter != setOfNumber.end() &&
            (iswdigit(*iter) || *iter == L'-' && iswdigit(*(iter + 1)))
            )
        {
            isNumber = true;
            buffer.push_back(*iter);
            ++iter;
        }

        if (isNumber)
        {
            //int val = std::stoi(buffer);
            int val = UTL::GetNumber<int>(buffer);
            buffer.assign(std::to_wstring(val));

            if (radiBottom->IsSelected())
            {
                list.push_back(val);
                outField->ReplaceSelText(buffer.append(L"\r\n"), bottomLinePos, bottomLinePos);
            }
            else
            {
				list.push_front(val);
                outField->ReplaceSelText(buffer.append(L"\r\n"), 0, 0);
			}
            bottomLinePos += numStrLen(val);

            cleanString.append(buffer + L" ");
            buffer.clear();
            continue;
        }
        ++iter;
    }

    inField->SetText(cleanString);
}



void Lab3Window::popFromList()
{
    if (list.empty()) return;

    bool isBottomSelected = radiBottom->IsSelected();
    int numToPop = isBottomSelected ? list.back() : list.front();
    int strLen = numStrLen(numToPop);

    if (isBottomSelected)
    {
        list.pop_back();
		outField->ReplaceSelText(L"", bottomLinePos, bottomLinePos - strLen);
    }
    else
    {
        list.pop_front();
		outField->ReplaceSelText(L"", 0, strLen);
    }
    bottomLinePos -= strLen;
}



void Lab3Window::redrawOutField()
{
    outField->RemoveText();

	for (const auto &val : list)
	{
		outField->AppendText(std::to_wstring(val).append(L"\r\n"));
	}
	setBottomLine();
}



void Lab3Window::sortList()
{
    list.sort();
    redrawOutField();
}



void Lab3Window::invertList()
{
    list.reverse();
    redrawOutField();
}



void Lab3Window::radiHandler()
{
    int newSel = radiTop->WhichSel();
    if (newSel == selectedRadi) return;

    selectedRadi = newSel;
}



void Lab3Window::labTask()
{
    bool hasFifth = false;
    auto iter = list.begin();
    while (iter != list.end())
    {
        if (*iter % 5 == 0 && (*iter & 1)) // In reverse code negative number lowest bit is 1. Also 5 is 0000 00101 so it would be true
        {
            hasFifth = true;
            bottomLinePos -= numStrLen(*iter);
            iter = list.erase(iter);
            continue;
        }
        ++iter;
    }
    if (hasFifth) redrawOutField();
}



void Lab3Window::CreateDeviceDepRes(HRESULT& hr)
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



void Lab3Window::DrawContent()
{
    MainWindow::DrawContent();

    if (pLabelTextFormat && pLabelColorBrush)
    {
        D2D1_RECT_F textRect = D2D1::RectF(20.0f, 20.0f, 150.0f, 45.0f);
        pRenderTarget->DrawText(
            L"Свободное значение",
            18,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

        textRect = D2D1::RectF(286.0f, 355.0f, 450.0f, 375.0f);
        pRenderTarget->DrawText(
            L"Начало",
            6,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

        textRect = D2D1::RectF(286.0f, 381.0f, 450.0f, 401.0f);
        pRenderTarget->DrawText(
            L"Конец",
            5,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );
    }
}



LRESULT Lab3Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        dpiS.Init(m_hwnd);

        inField = std::make_unique<MainEdit>(
            L3_IDIN_FIELD,
            S(120), S(30),
            S(120), S(20),
            16,
            m_hwnd,
            ES_CENTER
        );

        outField = std::make_unique<MainEdit>(
            L3_IDOUT_FIELD,
            S(220), S(340),
            S(20), S(60),
            16,
            m_hwnd,
            ES_MULTILINE | WS_VSCROLL | ES_READONLY | ES_CENTER
        );

        bttnRandCreate = std::make_unique<MainButton>(
            L3_IDBTTN_RAND_CR,
            L"Создать случайно",
            S(140), S(35),
            S(260), S(20),
            m_hwnd
        );

        bttnAddElem = std::make_unique<MainButton>(
            L3_IDBTTN_ADD_EL,
            L"Добавить",
            S(140), S(35),
            S(260), S(65),
            m_hwnd
        );

        bttnPop = std::make_unique<MainButton>(
            L3_IDBTTN_POP,
            L"Очистить крайний",
            S(140), S(35),
            S(260), S(110),
            m_hwnd
        );

        bttnClear = std::make_unique<MainButton>(
            L3_IDBTTN_CLEAR,
            L"Очистить всё",
            S(140), S(35),
            S(260), S(155),
            m_hwnd
        );

        bttnSort = std::make_unique<MainButton>(
            L3_IDBTTN_SORT,
            L"Сортировать",
            S(140), S(35),
            S(260), S(200),
            m_hwnd
        );

        bttnInvert = std::make_unique<MainButton>(
            L3_IDBTTN_INVERT,
            L"Перевернуть",
            S(140), S(35),
            S(260), S(245),
            m_hwnd
        );

        bttnTask = std::make_unique<MainButton>(
            L3_IDBTTN_TASK,
            L"Удалить *5",
            S(140), S(35),
            S(260), S(290),
            m_hwnd
        );

        radiTop = std::make_unique<MainRadioButton<L3_IDRADI_GROUP>>(
            L3_IDRADI_TOP,
            S(16), S(16),
            S(260), S(355),
            m_hwnd
        );

        radiBottom = std::make_unique<MainRadioButton<L3_IDRADI_GROUP>>(
            L3_IDRADI_BOTTOM,
            S(16), S(16),
            S(260), S(381),
            m_hwnd,
            1
        );

        inField->Create();
        outField->Create();

        bttnAddElem->Create();
        bttnRandCreate->Create();
        bttnClear->Create();
        bttnPop->Create();
        bttnSort->Create();
        bttnInvert->Create();
        bttnTask->Create();

        radiTop->Create();
        radiBottom->Create();

        setBottomLine();

        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        switch (id)
        {
        case L3_IDBTTN_RAND_CR:
            randCreateList();
            return 0;

        case L3_IDBTTN_ADD_EL:
            addToList();
            return 0;

        case L3_IDBTTN_CLEAR:
            clearAll();
            return 0;

        case L3_IDBTTN_POP:
            popFromList();
            return 0;

        case L3_IDBTTN_SORT:
            sortList();
            return 0;

        case L3_IDBTTN_INVERT:
            invertList();
            return 0;

        case L3_IDBTTN_TASK:
            labTask();
            return 0;

        case L3_IDRADI_TOP:
        case L3_IDRADI_BOTTOM:
            radiHandler();
            return 0;
        }
        return 0;
    }
    }
    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}



#undef CLEAR_LIST