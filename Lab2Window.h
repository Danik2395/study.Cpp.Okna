//
// Lab2Window.h
//
#pragma once
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "m_random.h"
#include "Stack.h"
#include <memory>

#define ID_IN_FIELD 1001
#define ID_OUT_FIELD 1002
#define ID_BTTN_RAND_CR 1003
#define ID_BTTN_ADD_EL 1004
#define ID_BTTN_CLEAR 1005
#define ID_BTTN_POP 1006
#define ID_BTTN_SORT 1007

class Lab2Window : public WndProps<Lab2Window, 4>, public MainWindow<Lab2Window>
{
    std::unique_ptr<MainEdit> inField;
    std::unique_ptr<MainEdit> outField;

    std::unique_ptr<MainButton> bttnRandCreate;
    std::unique_ptr<MainButton> bttnAddElem;
    std::unique_ptr<MainButton> bttnClear;
    std::unique_ptr<MainButton> bttnPop;
    std::unique_ptr<MainButton> bttnSort;

    Stack<int> lStack; // Lab Stack

    inline void setBottomLine() { outField->AppendText(L"-----------------"); }

    void randCreateStack();
    void clearStack();
    void clearAll();
    void addToStack();
    void popFromStack();
    void sortStack();
    //void splitEvenOdd();

public:
    Lab2Window() : WndProps(L"Стек", 450, 350) {}

    PCWSTR ClassName() const override { return L"Lab2WindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    CComPtr<IDWriteTextFormat> pLabelTextFormat;
    CComPtr<ID2D1SolidColorBrush> pLabelColorBrush;

    void CreateDeviceDepRes(HRESULT& hr);
    void DrawContent();
};