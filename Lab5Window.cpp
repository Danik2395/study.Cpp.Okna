//
// Lab5Window.cpp
//
#include "Lab5Window.h"



LAB5_KEYTYPE Lab5Window::keyFromField()
{
    std::wstring keyFieldText = keyField->GetText();
    LAB5_KEYTYPE insertedKey = UTL::GetNumber<int>(keyFieldText);
    keyField->SetText(std::to_wstring(insertedKey));
    return insertedKey;
}



void Lab5Window::infoOnKeyInsert()
{
    LAB5_KEYTYPE insertedKey = keyFromField();
    auto iter = labTree.find(insertedKey);
    if (iter == labTree.end())
    {
        valueField->RemoveText();
        return;
    }
    valueField->SetText((*iter).second);
}



void Lab5Window::writeInfoInKey()
{
    LAB5_KEYTYPE insertedKey = keyFromField();
    std::wstring valueFieldText = valueField->GetText();
    if (valueFieldText.empty()) labTree.erase(insertedKey);
    else                        labTree[insertedKey] = valueFieldText;
    treeDrawer->UpdateRenderData();
}



void Lab5Window::deleteKey()
{
    LAB5_KEYTYPE insertedKey = keyFromField();
    labTree.erase(insertedKey);
    valueField->RemoveText();
    treeDrawer->UpdateRenderData();
}



void Lab5Window::clearAllFields()
{
    labTree.erase(labTree.begin(), labTree.end());
    treeDrawer->UpdateRenderData();
    keyField->RemoveText();
    valueField->RemoveText();
}



void Lab5Window::middleKeyInfo()
{
    LAB5_KEYTYPE middleKey{};
    auto iter = labTree.begin();
    while (iter != labTree.end())
    {
        middleKey += (*iter++).first;
    }
    middleKey = middleKey / labTree.size();
    auto writeMiddleKey = [&middleKey, this](auto iter, bool &isKeyFound)
        {
            if (iter != labTree.end())
            {
                middleKey = (*iter).first;
                isKeyFound = true;
            }
        };
    LAB5_KEYTYPE minKey{};
    LAB5_KEYTYPE maxKey{};
    minKey = maxKey = middleKey;
    while (true)
    {
        auto iterCheckMiddle = labTree.find(minKey);
        bool isKeyFound = false;
        writeMiddleKey(iterCheckMiddle, isKeyFound);
        if (isKeyFound) break;
		iterCheckMiddle = labTree.find(maxKey);
        writeMiddleKey(iterCheckMiddle, isKeyFound);
        if (isKeyFound) break;
        ++maxKey;
        --minKey;
    }
    keyField->SetText(std::to_wstring(middleKey));
    infoOnKeyInsert();
}



//void Lab5Window::middleKeyInfo()
//{
//    LAB5_KEYTYPE minKey{};
//    LAB5_KEYTYPE maxKey{};
//    auto iter = labTree.begin();
//    minKey = maxKey = (*iter).first;
//    while (iter != labTree.end())
//    {
//        auto currentKey = (*iter).first;
//        minKey = currentKey < minKey ? currentKey : minKey;
//        maxKey = currentKey > maxKey ? currentKey : maxKey;
//        ++iter;
//    }
//    minKey = maxKey = (minKey + maxKey) / 2;
//    LAB5_KEYTYPE middleKey{};
//    auto writeMiddleKey = [&middleKey, this](auto iter, bool &isFindKey)
//        {
//            if (iter != labTree.end())
//            {
//                middleKey = (*iter).first;
//                isFindKey = true;
//            }
//        };
//    while (true)
//    {
//        auto iterCheckMiddle = labTree.find(minKey);
//        bool isFindKey = false;
//        writeMiddleKey(iterCheckMiddle, isFindKey);
//        if (isFindKey) break;
//		iterCheckMiddle = labTree.find(maxKey);
//        writeMiddleKey(iterCheckMiddle, isFindKey);
//        if (isFindKey) break;
//        ++maxKey;
//        --minKey;
//    }
//    keyField->SetText(std::to_wstring(middleKey));
//    infoOnKeyInsert();
//}



void Lab5Window::CreateDeviceDepRes(HRESULT& hr)
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



void Lab5Window::DrawContent()
{
    MainWindow::DrawContent();

    if (pLabelTextFormat && pLabelColorBrush)
    {
        D2D1_RECT_F textRect = D2D1::RectF(335.0f, 30.0f, 447.0f, 60.0f);
        pRenderTarget->DrawText(
            L"Число-ключ",
            12,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

        textRect = D2D1::RectF(330.0f, 80.0f, 447.0f, 110.0f);
        pRenderTarget->DrawText(
            L"Информация",
            12,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );
    }
}



LRESULT Lab5Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        dpiS.Init(m_hwnd);

        treeDrawer = std::make_unique<TreeDrawer<LAB5_KEYTYPE, LAB5_VALUETYPE>>(
            L5_IDTREEDRAWER,
            S(300), S(345),
            S(20), S(20),
            S(80),  S(60),
            labTree,
            m_hwnd
        );

        keyField = std::make_unique<MainEdit>(
            L5_IDKEY_FIELD,
            S(240), S(30),
            S(450), S(30),
            15,
            m_hwnd,
            ES_CENTER | ES_NUMBER
        );

        valueField = std::make_unique<MainEdit>(
            L5_IDVALUE_FIELD,
            S(240), S(30),
            S(450), S(80),
            15,
            m_hwnd,
            ES_CENTER
        );

        bttnDelete = std::make_unique<MainButton>(
            L5_IDBTTN_DELETE,
            L"Удалить",
            S(180), S(40),
            S(450), S(130),
            m_hwnd
        );

        bttnClearAll = std::make_unique<MainButton>(
            L5_IDBTTN_CLEAR_ALL,
            L"Очистить всё",
            S(180), S(40),
            S(450), S(180),
            m_hwnd
        );

        bttnMiddleKey = std::make_unique<MainButton>(
            L5_IDBTTN_MIDDLE_KEY,
            L"Средний ключ",
            S(180), S(40),
            S(450), S(230),
            m_hwnd
        );

        treeDrawer->Create();
        keyField->Create();
        valueField->Create();
        bttnDelete->Create();
        bttnClearAll->Create();
        bttnMiddleKey->Create();

        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        WORD hWord = HIWORD(wParam);
        switch (id)
        {
        case L5_IDKEY_FIELD:
            if (hWord == EN_KILLFOCUS)
            {
                infoOnKeyInsert();
                return 0;
            }
            break;

        case L5_IDVALUE_FIELD:
            if (hWord == EN_KILLFOCUS)
            {
                writeInfoInKey();
                return 0;
            }
            break;

        case L5_IDBTTN_DELETE:
            deleteKey();
            return 0;

        case L5_IDBTTN_CLEAR_ALL:
            clearAllFields();
            return 0;

        case L5_IDBTTN_MIDDLE_KEY:
            middleKeyInfo();
            return 0;
        }
        return 0;
    }
    }
    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}
