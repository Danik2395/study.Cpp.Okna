//
// Lab2Window.cpp
//
#include "Lab2Window.h"


// For not to pop all elements on clear button
// Dstructing but the name is still avaliable to interact with
// Constructing new stack to this name
#define CLEAR_STACK(st)\
do\
{\
(st)->~Stack();\
new (st) Stack<int>();\
} while (0);\


// Text carriage position
#define CAR_POS(carPos) if (radiOdd->IsSelected()) carPos = oddStackStart;



void Lab2Window::clearAll()
{
    CLEAR_STACK(&stackGeneral)
    CLEAR_STACK(&stackEven)
    CLEAR_STACK(&stackOdd)

    outField->RemoveText();
    inField->RemoveText();
    setBottomLine();

    radiGeneral->SetSelected(ID_RADI_GENERAL);
    selectedRadi = ID_RADI_GENERAL;
    isSplitted = false;
}



void Lab2Window::randCreateStack()
{
    m_random<int> dist(5, 15);
    int stackSize = MainEdit::GetNumber<int>(inField->GetText());
    if (stackSize < 1) stackSize = dist;

    inField->SetText(std::to_wstring(stackSize));

    if (!manipStack->empty())
    {
        CLEAR_STACK(manipStack)
    }

    int carPos{ 0 };
    if (isSplitted)
    {
        CAR_POS(carPos)
    }

    if (radiEven->IsSelected())
    {
        outField->ReplaceSelText(L"", 0, oddStackStart);
        outField->PrependText(L"\r\n");
        setBottomLine(0);

        oddStackStart = 0;
        oddStackStart += BOTTOM_LNLEN + 2; // line + \r + \n

    }
    else
    {
        outField->ReplaceSelText(L"", carPos, 10000000);
        setBottomLine();
    }

    dist.setLimits(-1000000, 1000000);
    while(stackSize != 0)
    {
        manipStack->push(dist);

        int tempNum{ manipStack->top() };
        if (radiEven->IsSelected())
        {
            if (tempNum < 0)
            {
                ++oddStackStart;
                tempNum = -tempNum;
            }

            do                   // Calculating where odd stack will start
            {
                tempNum /= 10;
                ++oddStackStart; // num digit
            } while (tempNum != 0);
            oddStackStart += 2;  // \r + \n of num

        }

        outField->ReplaceSelText(std::to_wstring(manipStack->top()).append(L"\r\n"), carPos, carPos);
        --stackSize;
    }
}



void Lab2Window::addToStack()
{
    std::wstring setOfNumber = inField->GetText();
    if (setOfNumber.length() == 0) return;

    int carPos{ 0 };
    if (isSplitted)
    {
        CAR_POS(carPos)
    }
    
    std::wstring buffer;
    std::wstring cleanString;
    for (auto iter = setOfNumber.begin(); iter != setOfNumber.end();)       // Picking the numbers from field
    {
        bool isNumber = false;
        while (
            iter != setOfNumber.end() &&
            ( iswdigit(*iter) || *iter == L'-' && iswdigit(*(iter + 1)) )
           )                                                                // While number or some value is under iterator
        {
            isNumber = true;                                                // Setting number flag
            buffer.push_back(*iter);                                        // Assembling the number (it is easier then messing with pointers)
            ++iter;
        }

        if (isNumber)                                                       // Sending the number to the outField
        {
            manipStack->push(std::stoi(buffer));

            int tempNum{ manipStack->top() };
            if (radiEven->IsSelected())
            {
                if (tempNum < 0)                                            // Mind the minus
                {
                    ++oddStackStart;
                    tempNum = -tempNum;
                }

                do
                {
                    tempNum /= 10;
                    ++oddStackStart;
                } while (tempNum != 0);
                oddStackStart += 2;
            }

            outField->ReplaceSelText(buffer.append(L"\r\n"), carPos, carPos);
            cleanString.append(buffer + L" ");
            buffer.clear();
            continue;                                                       // If it was number we already on the non number so continuing without increment (out of range or past end error also)
        }
        ++iter;
    }

    inField->SetText(cleanString);
}



void Lab2Window::popFromStack()
{
    if (manipStack->empty()) return;

    int carPos{ 0 };
    if (isSplitted)
    {
        CAR_POS(carPos)
    }

    int strLen{ 2 };         // \r + \n
    int numToPop = manipStack->top();

    if (numToPop < 0)        // Minus adds length
    {
        numToPop = -numToPop;
        ++strLen;
    }

    do
    {
        numToPop /= 10;
        ++strLen;
    } while (numToPop != 0); // Works for int

    manipStack->pop();

    if (radiEven->IsSelected()) oddStackStart -= strLen ;

    outField->ReplaceSelText(L"", carPos, carPos + strLen);
}



void Lab2Window::sortStack()
{
    manipStack->sort();

    int carPos{ 0 };
    if (radiEven->IsSelected())
    {
        outField->ReplaceSelText(L"", 0, oddStackStart);

        for (const auto &val : *manipStack)
        {
            outField->ReplaceSelText(std::to_wstring(val).append(L"\r\n"), carPos, carPos);

            carPos += 2;         
            int numToAdd = val;

            if (numToAdd < 0)        // Minus adds length
            {
                numToAdd = -numToAdd;
                ++carPos;
            }

            do
            {
                numToAdd /= 10;
                ++carPos;
            } while (numToAdd != 0); // Works for int
        }
        setBottomLine(carPos);
        carPos += BOTTOM_LNLEN;
        outField->ReplaceSelText(L"\r\n", carPos, carPos);
    }
    else
    {
        if (isSplitted)
        {
            CAR_POS(carPos)

        }

        outField->ReplaceSelText(L"", carPos, 10000000);

        for (const auto &val : *manipStack)
        {
            outField->AppendText(std::to_wstring(val).append(L"\r\n"));
        }
        setBottomLine();
    }
}



void Lab2Window::splitHandler()
{
    int prevSelRadi = selectedRadi;
    selectedRadi = radiGeneral->WhichSel();
    if (prevSelRadi == selectedRadi) return; // Nothing to recalculate

    manipStack = radiGeneral->IsSelected() ? &stackGeneral : radiEven->IsSelected() ? &stackEven : &stackOdd; // Setting the manipulating stack

    if (
        (prevSelRadi == ID_RADI_EVEN || prevSelRadi == ID_RADI_ODD) &&
        (selectedRadi == ID_RADI_EVEN || selectedRadi == ID_RADI_ODD)
        ) return; // Nothing to recalculate because already splitted stack

    switch (selectedRadi)
    {
    case ID_RADI_GENERAL:                    // Need to assemble two parts
    {
        isSplitted = false;

        for (const auto &num : stackOdd)     // Adding odd part
        {
            stackGeneral.push(num);
        }
        CLEAR_STACK(&stackOdd)

        for (const auto &num : stackEven)    // Adding even part
        {
            stackGeneral.push(num);
        }
        CLEAR_STACK(&stackEven)

        outField->RemoveText();

        for (const auto &num : stackGeneral) // Printing assembled stack
        {
            outField->AppendText(std::to_wstring(num).append(L"\r\n"));
        }
        setBottomLine();

        return;
    }

    case ID_RADI_EVEN:
        [[fallthrough]];                                                     // Idk. C++17 thing. For readability
    case ID_RADI_ODD:                                                        // Need to split stack
    {
        isSplitted = true;

        outField->RemoveText();
        oddStackStart = 0;
        oddStackStart += BOTTOM_LNLEN + 2;                                   // line + \r + \n

        for (const auto &num : stackGeneral)
        {
            if (num % 2 == 0)                                                // Even stack is on top
            {
                stackEven.push(num);

                int tempNum{ num };
                if (tempNum < 0)
                {
                    ++oddStackStart;
                    tempNum = -tempNum;
                }

                do                                                           // Calculating where odd stack will start
                {
                    tempNum /= 10;
                    ++oddStackStart;                                      // num digit
                } while (tempNum != 0);
                oddStackStart += 2;                                       // \r + \n of num

                outField->PrependText(std::to_wstring(num).append(L"\r\n")); // Prepending text because pushing even numbers TO the top

                continue;
            }
            stackOdd.push(num);
        }
        CLEAR_STACK(&stackGeneral)
        
        setBottomLine();
        outField->AppendText(L"\r\n");

        for (const auto &num : stackOdd)
        {
            outField->AppendText(std::to_wstring(num).append(L"\r\n"));      // And odd stack reading numbers FROM the top
        }
        setBottomLine();

        return;
    }
    }

    return;
}



void Lab2Window::CreateDeviceDepRes(HRESULT& hr)
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



void Lab2Window::DrawContent()
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

        textRect = D2D1::RectF(286.0f, 265.0f, 450.0f, 285.0f);
        pRenderTarget->DrawText(
            L"Общий",
            5,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

        textRect = D2D1::RectF(286.0f, 291.0f, 450.0f, 311.0f);
        pRenderTarget->DrawText(
            L"Чётный",
            6,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

        textRect = D2D1::RectF(286.0f, 317.0f, 450.0f, 337.0f);
        pRenderTarget->DrawText(
            L"Нечётный",
            8,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );
    }
}



LRESULT Lab2Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        dpiS.Init(m_hwnd);

        inField = std::make_unique<MainEdit>(
            ID_IN_FIELD,
            S(120), S(30),
            S(120), S(20),
            16,
            m_hwnd,
            ES_CENTER/* | ES_NUMBER*/
        );

        outField = std::make_unique<MainEdit>(
            ID_OUT_FIELD,
            S(220), S(340),
            S(20), S(60),
            16,
            m_hwnd,
            ES_MULTILINE/* | ES_WANTRETURN*/ | WS_VSCROLL | ES_READONLY | ES_CENTER
        );

        bttnRandCreate = std::make_unique<MainButton>(
            ID_BTTN_RAND_CR,
            L"Создать случайно",
            S(140), S(35),
            S(260), S(20),
            m_hwnd
        );

        bttnAddElem = std::make_unique<MainButton>(
            ID_BTTN_ADD_EL,
            L"Добавить",
            S(140), S(35),
            S(260), S(65),
            m_hwnd
        );

        bttnPop = std::make_unique<MainButton>(
            ID_BTTN_POP,
            L"Очистить верхний",
            S(140), S(35),
            S(260), S(110),
            m_hwnd
        );

        bttnClear = std::make_unique<MainButton>(
            ID_BTTN_CLEAR,
            L"Очистить всё",
            S(140), S(35),
            S(260), S(155),
            m_hwnd
        );

        bttnSort = std::make_unique<MainButton>(
            ID_BTTN_SORT,
            L"Сортировать",
            S(140), S(35),
            S(260), S(200),
            m_hwnd
        );

        radiGeneral = std::make_unique<MainRadioButton<ID_RADI_GROUP>>(
            ID_RADI_GENERAL,
            S(16), S(16),
            S(260), S(265),
            m_hwnd,
            1
        );

        radiEven = std::make_unique<MainRadioButton<ID_RADI_GROUP>>(
            ID_RADI_EVEN,
            S(16), S(16),
            S(260), S(291),
            m_hwnd
        );

        radiOdd = std::make_unique<MainRadioButton<ID_RADI_GROUP>>(
            ID_RADI_ODD,
            S(16), S(16),
            S(260), S(317),
            m_hwnd
        );

        inField->Create();
        outField->Create();

        bttnAddElem->Create();
        bttnRandCreate->Create();
        bttnClear->Create();
        bttnPop->Create();
        bttnSort->Create();

        radiGeneral->Create();
        radiEven->Create();
        radiOdd->Create();

        setBottomLine();

        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        switch (id)
        {
        case ID_BTTN_RAND_CR:
            randCreateStack();
            return 0;

        case ID_BTTN_ADD_EL:
            addToStack();
            return 0;

        case ID_BTTN_CLEAR:
            clearAll();
            return 0;

        case ID_BTTN_POP:
            popFromStack();
            return 0;

        case ID_BTTN_SORT:
            sortStack();
            return 0;

        case ID_RADI_GENERAL:
        case ID_RADI_EVEN:
        case ID_RADI_ODD:
            splitHandler();
            return 0;
        }
        return 0;
    }
    }
    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}



#undef CLEAR_STACK
#undef CAR_POS