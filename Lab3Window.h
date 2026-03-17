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
#include "List.h"
#include <memory>

#define BOTTOM_LINE L"-----------------"
#define BOTTOM_LNLEN 17

#define ID_IN_FIELD 3001
#define ID_OUT_FIELD 3002

#define ID_BTTN_RAND_CR 3003
#define ID_BTTN_ADD_EL 3004
#define ID_BTTN_CLEAR 3005
#define ID_BTTN_POP 3006
#define ID_BTTN_SORT 3007
#define ID_BTTN_INVERT 3008
#define ID_BTTN_TASK 3009

#define ID_RADI_GROUP 333
#define ID_RADI_TOP 3010
#define ID_RADI_BOTTOM 3011

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

    std::unique_ptr<MainRadioButton<ID_RADI_GROUP>> radiTop;
    std::unique_ptr<MainRadioButton<ID_RADI_GROUP>> radiBottom;

    int selectedRadi;
    int bottomLinePos;

    List<int> list;

    void setBottomLine(int carPos = -1)
    {
        if (carPos == -1)
        {
            outField->AppendText(BOTTOM_LINE);
            return;
        }
        outField->ReplaceSelText(BOTTOM_LINE, carPos, carPos);
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
        selectedRadi(ID_RADI_BOTTOM),
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
