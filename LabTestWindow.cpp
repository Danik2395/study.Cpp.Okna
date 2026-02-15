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

        inField = std::make_unique<MainEdit>(ID_IN_FIELD, S(200), S(100), S(10), S(10), 12, m_hwnd, ES_CENTER | ES_MULTILINE);
        outField = std::make_unique<MainEdit>(ID_OUT_FIELD, S(200), S(100), S(10), S(210), 12, m_hwnd, ES_CENTER | ES_MULTILINE | ES_READONLY);
        calcButton = std::make_unique<MainButton>(ID_BTN_CALC, L"Посчитать", S(100), S(30), S(20), S(150), m_hwnd);
        testCheckBox = std::make_unique<MainCheckBox>(ID_CHECK_SMTH, S(40), S(40), S(300), S(10), m_hwnd);

        radio1 = std::make_unique<MainRadioButton<ID_RADI_GROUP>>(ID_RADI_1, S(30), S(30), S(340), S(40), m_hwnd);
        radio2 = std::make_unique<MainRadioButton<ID_RADI_GROUP>>(ID_RADI_2, S(30), S(50), S(390), S(40), m_hwnd);

        inField->Create();
        outField->Create();
        calcButton->Create();
        testCheckBox->Create();
        radio1->Create();
        radio2->Create();

        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        switch (id)
        {
        case ID_BTN_CALC:
            calc(inField->GetText());
            return 0;

        case ID_RADI_1:
        case ID_RADI_2:
            selectedRadi = radio1->WhichSel();
            outField->SetText(L"radio: " + std::to_wstring(selectedRadi));
        }
        return 0;
    }
    }
    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}