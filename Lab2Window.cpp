//
// Lab2Window.cpp
//
#include "Lab2Window.h"


void Lab2Window::clearStack()
{
                                // For not to pop all elements on clear button
    lStack.~Stack();            // Dstructing but the name is still avaliable to interact with
    new (&lStack) Stack<int>(); // Constructing new stack to this name

}



void Lab2Window::clearAll()
{
    clearStack();

    outField->RemoveText();
    inField->RemoveText();
    setBottomLine();
}



void Lab2Window::randCreateStack()
{
    m_random<int> dist(5, 15);
    int stackSize = MainEdit::GetNumber<int>(inField->GetText());
    if (stackSize < 1) stackSize = dist;

    inField->SetText(std::to_wstring(stackSize));

    if (!lStack.empty()) clearStack();
    outField->RemoveText();
    setBottomLine();

    dist.setLimits(-1000000, 1000000);
    while(stackSize != 0)
    {
        lStack.push(dist);
        outField->PrependText(std::to_wstring(lStack.top()).append(L"\r\n"));
        --stackSize;
    }
}



void Lab2Window::addToStack()
{
    std::wstring setOfNumber = inField->GetText();
    if (setOfNumber.length() == 0) return;
    
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
            lStack.push(std::stoi(buffer));
            outField->PrependText(buffer.append(L"\r\n"));
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
    if (lStack.empty()) return;

    int strLen{ 2 };         // \0 + num
    int numToPop = lStack.top();

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

    lStack.pop();

    outField->ReplaceSelText(L"", 0, strLen);
}



void Lab2Window::sortStack()
{
    lStack.sort();
    outField->RemoveText();

    for (const auto &val : lStack)
    {
        outField->AppendText(std::to_wstring(val).append(L"\r\n"));
    }

    setBottomLine();
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
        D2D1_RECT_F inLabelRect = D2D1::RectF(20.0f, 20.0f, 150.0f, 45.0f);
        pRenderTarget->DrawText(
            L"Свободное значение",
            18,
            pLabelTextFormat,
            inLabelRect,
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
            S(220), S(240),
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

        inField->Create();
        outField->Create();
        bttnAddElem->Create();
        bttnRandCreate->Create();
        bttnClear->Create();
        bttnPop->Create();
        bttnSort->Create();

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
        }
        return 0;
    }
    }
    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}