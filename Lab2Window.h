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

#define BOTTOM_LINE L"-----------------"
#define BOTTOM_LNLEN 17

#define ID_IN_FIELD 2001
#define ID_OUT_FIELD 2002

#define ID_BTTN_RAND_CR 2003
#define ID_BTTN_ADD_EL 2004
#define ID_BTTN_CLEAR 2005
#define ID_BTTN_POP 2006
#define ID_BTTN_SORT 2007

#define ID_RADI_GROUP 222
#define ID_RADI_GENERAL 2008
#define ID_RADI_EVEN 2009
#define ID_RADI_ODD 2010

class Lab2Window : public WndProps<Lab2Window, 4>, public MainWindow<Lab2Window>
{
    std::unique_ptr<MainEdit> inField;
    std::unique_ptr<MainEdit> outField;

    std::unique_ptr<MainButton> bttnRandCreate;
    std::unique_ptr<MainButton> bttnAddElem;
    std::unique_ptr<MainButton> bttnClear;
    std::unique_ptr<MainButton> bttnPop;
    std::unique_ptr<MainButton> bttnSort;

    std::unique_ptr<MainRadioButton<ID_RADI_GROUP>> radiGeneral;
    std::unique_ptr<MainRadioButton<ID_RADI_GROUP>> radiEven;
    std::unique_ptr<MainRadioButton<ID_RADI_GROUP>> radiOdd;
    int selectedRadi;
    bool isSplitted;

    Stack<int> stackGeneral;
    Stack<int> stackEven;
    Stack<int> stackOdd;
    int oddStackStart; // For text logic

    void setBottomLine() { outField->AppendText(BOTTOM_LINE); }

    void randCreateStack();
    //void clearStack();
    void clearAll();
    void addToStack();
    void popFromStack();
    void sortStack();
    void splitHandler();

public:
    Lab2Window() : WndProps(L"Стек", 450, 450), // 350 height
        selectedRadi(ID_RADI_GENERAL),
        isSplitted(false),
        oddStackStart(0)
    {}

    PCWSTR ClassName() const override { return L"Lab2WindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    CComPtr<IDWriteTextFormat> pLabelTextFormat;
    CComPtr<ID2D1SolidColorBrush> pLabelColorBrush;

    void CreateDeviceDepRes(HRESULT& hr);
    void DrawContent();
};