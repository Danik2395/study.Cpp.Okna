//
// Lab4Window.cpp
//
#include "Lab4Window.h"



void Lab4Window::translate()
{
    std::wstring expression = inField->GetText();
    if (expression.empty()) return;

    //outRpnField->RemoveText();
    outResultField->RemoveText();

    try
    {
        yard.shuntToRpn(expression);
        outRpnField->SetText(yard.getRpnWString());
    }
    catch (const shuntingYard::shuntingException &e)
    {
        outRpnField->SetText(e.wwhat());
    }
}



void Lab4Window::calculate()
{
    if (inField->GetText().empty()) return;

    //List<shuntingYard::Variables> syncedVars;
    ////int rowCount = lwvVariables->GetCellValue(0, 0).empty() ? 0 : lwvVariables->GetItemsCount();
    //int rowCount = vlvVariables->GetItemsCount();

    //auto varIter = variablesList.begin();
    //int  rowIndex = 0;
    //while (varIter != variablesList.end()) // Getting variables from the variableList
    //{
    //    std::wstring tableVarValue = vlvVariables->GetCellValue(rowIndex, 1);

    //    shuntingYard::Variables syncedVar;
    //    syncedVar.name = varIter->name;
    //    syncedVar.value = tableVarValue.empty() ? 1.0 : std::stod(tableVarValue);

    //    syncedVars.push_back(syncedVar);

    //    ++varIter;
    //    ++rowIndex;
    //}
    //yard.setVariables(syncedVars);         // To init the variables list in the shunting yard

    yard.setVariables(variablesList);
    outResultField->RemoveText();

    try
    {
        double result = yard.calculateRpn();
        outResultField->SetText(std::to_wstring(result));
    }
    catch (const shuntingYard::shuntingException &e)
    {
        outResultField->SetText(e.wwhat());
    }
}



// Rebuilds variablesList preserving values for already known variables,
// And refreshes the list view
void Lab4Window::createVariables()
{
    // The most terrifying method in this laboratory work. All the accumulating cases of not using hash table got out here.

    std::wstring expression = inField->GetText();
    if (expression.empty())
    {
        vlvVariables->ClearRows();
        variablesList.clear();
        return;
    }

    pcwList tokens;
    try
    {
        tokens = yard.unsortToTokens(expression);
    }
    catch (const shuntingYard::shuntingException &e)
    {
        outResultField->RemoveText();
        outRpnField->SetText(e.wwhat());
        return;
    }

    auto tokensIter = tokens.begin();
    while (tokensIter != tokens.end())                   // Clearing tokens
    {
        const wchar_t* token = *tokensIter;
        if (!iswalpha(token[0]))                         // Only variable tokens
        {
            tokensIter = tokens.erase(tokensIter);
            continue;
        }
        ++tokensIter;
    }
    if (tokens.empty())
    {
        vlvVariables->ClearRows();
        variablesList.clear();
        return;
    }

    // ~~~~~~~~~~~~~~~~~~~
    // #                 #
    // #  variablesList  # <----*----------------------*
    // #                 #      |                      |
    // ~~~~~~~~~~~~~~~~~~~      |                      |
    //                          |                      |
    //                          |                      |
    //            ~~~~~~~~~~~~~~~~~~              ~~~~~~~~~~~~
    //            #  vlvVariables  #              #  unique  #
    //            #       has      #              #  tokens  #
    //            #     tokens     #              #          #
    //            ~~~~~~~~~~~~~~~~~~              ~~~~~~~~~~~~


    // Setting already existing in VariablesListView values
    variablesList.clear();
    int vlvLastItemIndex = vlvVariables->GetItemsCount();
    int vlvIter{ 0 };
    while (vlvIter != vlvLastItemIndex)
    {
        std::wstring vlvCellName = vlvVariables->GetCellValue(vlvIter, 0);

        auto tIter = tokens.begin();
        while (tIter != tokens.end())
        {
            const wchar_t* token = *tIter;
            if (vlvCellName == token)                    // Pushing if value exists in the table (has token)
            {
                std::wstring vlvCellValue = vlvVariables->GetCellValue(vlvIter, 1);
                variablesList.push_back({ vlvCellName, std::stod(vlvCellValue) });

                auto tempTokensIter = tIter;
                while (tempTokensIter != tokens.end())   // Erasing all the duplicates
                {
                    const wchar_t* tempToken = *tempTokensIter;
                    if (vlvCellName == tempToken)
                    {
                        tempTokensIter = tokens.erase(tempTokensIter);
                        continue;
                    }
                    ++tempTokensIter;
                }
                break;
            }
            ++tIter;
        }
        ++vlvIter;
    }

    // Setting unique variables
    while (!tokens.empty())
    {
        std::wstring newVarName = *tokens.begin();
        variablesList.push_back({ newVarName, 0.0 });

        auto tempTokensIter = tokens.begin();
        while (tempTokensIter != tokens.end())           // Erasing all the duplicates of the new variable
        {
            const wchar_t* tempToken = *tempTokensIter;
            if (newVarName == tempToken)                 // ITS NEW VARIABLES. They could have duplicates
            {
                tempTokensIter = tokens.erase(tempTokensIter);
                continue;
            }
            ++tempTokensIter;                 // ITS NEW VARIABLES. They could have duplicates
        }
    }

    vlvVariables->ClearRows();
    for (const auto &var : variablesList)                // Refreshing the list view
    {
        vlvVariables->AddRow(var.name, std::to_wstring(var.value));
    }
}
//void Lab4Window::createVariables()
//{
//    std::wstring expression = inField->GetText();
//    if (expression.empty()) return;
//
//    pcwList tokens;
//    try
//    {
//		tokens = yard.unsortToTokens(expression);
//    }
//    catch (const shuntingYard::shuntingException &e)
//    {
//        outResultField->RemoveText();
//        outRpnField->SetText(e.wwhat());
//    }
//
//    List<shuntingYard::Variables> newVarsList;
//
//    for (const auto &token : tokens)                  // The most terrifying method in this laboratory work. All the accumulating cases of not using hash table got out here.
//    {
//        if (!iswalpha(token[0])) continue;            // Only variable tokens
//
//		bool alreadyAdded = false;
//        for (const auto &var : newVarsList)           // Checking for duplicates in new list
//        {
//            if (var.name == token)
//            {
//                alreadyAdded = true;
//                break;
//            }
//        }
//        if (alreadyAdded) continue;
//
//        shuntingYard::Variables newVar;               // If new variable
//        newVar.name = token;
//
//        bool found = false;
//        for (const auto &existingVar : variablesList) // Checking previous variable list for the already written value
//        {
//            if (existingVar.name == token)
//            {
//                newVar.value = existingVar.value;
//                found = true;
//                break;
//            }
//        }
//        if (!found) newVar.value = 0.0;
//
//        newVarsList.push_back(newVar);
//    }
//
//    variablesList = newVarsList;
//
//    vlvVariables->ClearRows();
//    for (const auto &var : variablesList) // Refreshing the list view
//    {
//        vlvVariables->AddRow(var.name, std::to_wstring(var.value));
//    }
//}


void Lab4Window::CreateDeviceDepRes(HRESULT& hr)
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



void Lab4Window::DrawContent()
{
    MainWindow::DrawContent();

    if (pLabelTextFormat && pLabelColorBrush)
    {
        D2D1_RECT_F textRect = D2D1::RectF(185.0f, 15.0f, 545.0f, 33.0f);
        pRenderTarget->DrawText(
            L"Введите выражение",
            17,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

        textRect = D2D1::RectF(185.0f, 80.0f, 545.0f, 98.0f);
        pRenderTarget->DrawText(
            L"Полученная ОПЗ",
            14,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );

        textRect = D2D1::RectF(185.0f, 145.0f, 545.0f, 163.0f);
        pRenderTarget->DrawText(
            L"Результат",
            9,
            pLabelTextFormat,
            textRect,
            pLabelColorBrush
        );
    }
}



LRESULT Lab4Window::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        dpiS.Init(m_hwnd);

        inField = std::make_unique<MainEdit>(
            L4_IDIN_FIELD,
            S(345), S(28),
            S(185), S(36),
            15,
            m_hwnd,
            ES_LEFT
        );

        outRpnField = std::make_unique<MainEdit>(
            L4_IDOUT_RPN_FIELD,
            S(345), S(28),
            S(185), S(101),
            15,
            m_hwnd,
            ES_LEFT | ES_READONLY
        );

        outResultField = std::make_unique<MainEdit>(
            L4_IDOUT_RESULT_FIELD,
            S(345), S(28),
            S(185), S(166),
            15,
            m_hwnd,
            ES_LEFT | ES_READONLY
        );

        bttnTranslate = std::make_unique<MainButton>(
            L4_IDBTTN_TRANSL,
            L"Перевести",
            S(110), S(40),
            S(290), S(220),
            m_hwnd
        );

        bttnCalculate = std::make_unique<MainButton>(
            L4_IDBTTN_CALC,
            L"Посчитать",
            S(110), S(40),
            S(420), S(220),
            m_hwnd
        );

        vlvVariables = std::make_unique<VariablesListView>(
            L4_IDLWV_VARS,
            S(155), S(250),
            S(10), S(10),
            12,
            m_hwnd
        );
        inField->Create();
        outRpnField->Create();
        outResultField->Create();
        bttnCalculate->Create();
        bttnTranslate->Create();
        vlvVariables->Create();

        vlvVariables->AddColumn(0, L"Имя", S(50));
        vlvVariables->AddColumn(1, L"Знач.", S(105));

        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        WORD hWord = HIWORD(wParam);
        switch (id)
        {
        case L4_IDBTTN_CALC:
			createVariables();
            translate();
            calculate();
            return 0;

        case L4_IDBTTN_TRANSL:
			createVariables();
            translate();
            return 0;

        case L4_IDIN_FIELD:
            if (hWord == EN_KILLFOCUS)
            {
                createVariables();
                return 0;
            }
        }
        return 0;
    }
    }
    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}
