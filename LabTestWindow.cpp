//
// LabTestWindow.cpp
//
#include "LabTestWindow.h"


void LabTestWindow::calc(const std::wstring &wstr)
{
    std::wstring answer;

    answer.assign(wstr);
    if (testCheckBox->State())
    {
        answer.append(L" out wstring");
        if (radio1->IsSelected())
        {
            answer.append(L"\r\nping radio1");
        }
    }
    else
    {
        answer.assign(L"no checkbox");
    }



    outField->SetText(answer);
}



//void LabWindow::CreateDeviceDepRes(HRESULT &hr)
//{
//    MainWindow::CreateDeviceDepRes(hr);
//
//}



//void LabWindow::DrawContent()
//{
//    MainWindow::DrawContent();
//
//}



LRESULT LabTestWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        dpiS.Init(m_hwnd);

        inField = std::make_unique<MainEdit>(LTEST_IDIN_FIELD, S(200), S(100), S(10), S(10), 12, m_hwnd, ES_CENTER | ES_MULTILINE);
        outField = std::make_unique<MainEdit>(LTEST_IDOUT_FIELD, S(200), S(100), S(10), S(210), 12, m_hwnd, ES_CENTER | ES_MULTILINE | ES_READONLY);
        calcButton = std::make_unique<MainButton>(LTEST_IDBTN_CALC, L"Посчитать", S(100), S(30), S(20), S(150), m_hwnd);
        testCheckBox = std::make_unique<MainCheckBox>(LTEST_IDCHECK_SMTH, S(40), S(40), S(300), S(10), m_hwnd);

        radio1 = std::make_unique<MainRadioButton<LTEST_IDRADI_GROUP>>(LTEST_IDRADI_1, S(30), S(30), S(340), S(40), m_hwnd);
        radio2 = std::make_unique<MainRadioButton<LTEST_IDRADI_GROUP>>(LTEST_IDRADI_2, S(30), S(50), S(390), S(40), m_hwnd);

        listView = std::make_unique<MainListView>(ID_LIST_VIEW, S(300), S(300), S(340), S(60), 12, m_hwnd, LVS_SINGLESEL | LVS_SHOWSELALWAYS, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

        inField->Create();
        outField->Create();
        calcButton->Create();
        testCheckBox->Create();
        radio1->Create();
        radio2->Create();
        listView->Create();

        listView->AddColumn(0, L"some text", 130);
        listView->AddColumn(0, L"subitem", 110);
        listView->AddRow(L"a", L"2");
        listView->AddRow(L"b", L"i");
        listView->AddRow(L"A", L"8");

        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        switch (id)
        {
        case LTEST_IDBTN_CALC:
            calc(inField->GetText());
            return 0;

        case LTEST_IDRADI_1:
        case LTEST_IDRADI_2:
            selectedRadi = radio1->WhichSel();
            outField->SetText(L"radio: " + std::to_wstring(selectedRadi));
        }
        return 0;
    }
    }
    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}