//
// Lab3Window.h
//
#pragma once
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "MainRadioButton.h"
#include "m_random.h"
#include "Utils.h"
#include "List.h"
#include <memory>

#define L3_BOTTOM_LINE L"-----------------"
#define L3_BOTTOM_LNLEN 17

#define L3_IDIN_FIELD 3001
#define L3_IDOUT_FIELD 3002

#define L3_IDBTTN_RAND_CR 3003
#define L3_IDBTTN_ADD_EL 3004
#define L3_IDBTTN_CLEAR 3005
#define L3_IDBTTN_POP 3006
#define L3_IDBTTN_SORT 3007
#define L3_IDBTTN_INVERT 3008
#define L3_IDBTTN_TASK 3009

#define L3_IDRADI_GROUP 333
#define L3_IDRADI_TOP 3010
#define L3_IDRADI_BOTTOM 3011

class Lab3Window : public WndProps<Lab3Window, 4>, public MainWindow<Lab3Window>
{
    std::unique_ptr<MainEdit> inField;
    std::unique_ptr<MainEdit> outField;

    std::unique_ptr<MainButton> bttnRandCreate;
    std::unique_ptr<MainButton> bttnAddElem;
    std::unique_ptr<MainButton> bttnPop;
    std::unique_ptr<MainButton> bttnClear;
    std::unique_ptr<MainButton> bttnSort;
    std::unique_ptr<MainButton> bttnInvert;
    std::unique_ptr<MainButton> bttnTask;

    std::unique_ptr<MainRadioButton<L3_IDRADI_GROUP>> radiTop;
    std::unique_ptr<MainRadioButton<L3_IDRADI_GROUP>> radiBottom;

    int selectedRadi;
    int bottomLinePos;

    List<int> list;

    void setBottomLine(int carPos = -1)
    {
        if (carPos == -1)
        {
            outField->AppendText(L3_BOTTOM_LINE);
            return;
        }
        outField->ReplaceSelText(L3_BOTTOM_LINE, carPos, carPos);
    }

    void randCreateList();
    void clearAll();
    void addToList();
    void popFromList();
    void sortList();
    void invertList();
    void radiHandler();
    void redrawOutField();

    void labTask();

public:
    Lab3Window() : WndProps(L"Список", 450, 450),
        selectedRadi(L3_IDRADI_BOTTOM),
        bottomLinePos(0)
    {}

    PCWSTR ClassName() const override { return L"Lab3WindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    CComPtr<IDWriteTextFormat> pLabelTextFormat;
    CComPtr<ID2D1SolidColorBrush> pLabelColorBrush;

    void CreateDeviceDepRes(HRESULT& hr);
    void DrawContent();
};

#undef BUTTOM_LINE
