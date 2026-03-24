//
// Lab2Window.h
//
#pragma once
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "MainRadioButton.h"
#include "m_random.h"
#include "Stack.h"
#include <memory>

#define L2_BOTTOM_LINE L"-----------------"
#define L2_BOTTOM_LNLEN 17

#define L2_IDIN_FIELD 2001
#define L2_IDOUT_FIELD 2002

#define L2_IDBTTN_RAND_CR 2003
#define L2_IDBTTN_ADD_EL 2004
#define L2_IDBTTN_CLEAR 2005
#define L2_IDBTTN_POP 2006
#define L2_IDBTTN_SORT 2007

#define L2_IDRADI_GROUP 222
#define L2_IDRADI_GENERAL 2008
#define L2_IDRADI_EVEN 2009
#define L2_IDRADI_ODD 2010

class Lab2Window : public WndProps<Lab2Window, 4>, public MainWindow<Lab2Window>
{
    std::unique_ptr<MainEdit> inField;
    std::unique_ptr<MainEdit> outField;

    std::unique_ptr<MainButton> bttnRandCreate;
    std::unique_ptr<MainButton> bttnAddElem;
    std::unique_ptr<MainButton> bttnClear;
    std::unique_ptr<MainButton> bttnPop;
    std::unique_ptr<MainButton> bttnSort;

    std::unique_ptr<MainRadioButton<L2_IDRADI_GROUP>> radiGeneral;
    std::unique_ptr<MainRadioButton<L2_IDRADI_GROUP>> radiEven;
    std::unique_ptr<MainRadioButton<L2_IDRADI_GROUP>> radiOdd;
    int selectedRadi;
    bool isSplitted;

    Stack<int> stackGeneral;
    Stack<int> stackEven;
    Stack<int> stackOdd;
    Stack<int>* manipStack;
    int oddStackStart; // For text logic

    void setBottomLine(int carPos = -1)
    {
        if (carPos == -1)
        {
            outField->AppendText(L2_BOTTOM_LINE);
            return;
        }
        outField->ReplaceSelText(L2_BOTTOM_LINE, carPos, carPos);
    }

    void randCreateStack();
    void clearAll();
    void addToStack();
    void popFromStack();
    void sortStack();
    void splitHandler();

public:
    Lab2Window() : WndProps(L"Стек", 450, 450),
        selectedRadi(L2_IDRADI_GENERAL),
        isSplitted(false),
        oddStackStart(0),
        manipStack(&stackGeneral)
    {}

    PCWSTR ClassName() const override { return L"Lab2WindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    CComPtr<IDWriteTextFormat> pLabelTextFormat;
    CComPtr<ID2D1SolidColorBrush> pLabelColorBrush;

    void CreateDeviceDepRes(HRESULT& hr);
    void DrawContent();
};

#undef BUTTOM_LINE