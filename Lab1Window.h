//
// Lab1Window.h
//
#pragma once
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "Utils.h"
#include <memory>

#define L1_ID_VAR_12_FIELD 1001
#define L1_ID_OUT_CYLCLE_FIELD 1002
#define L1_ID_OUT_RECURSION_FIELD 1003
#define L1_ID_BTTN_CALC 1004

class Lab1Window : public WndProps<Lab1Window, 4>, public MainWindow<Lab1Window>
{
    std::unique_ptr<MainEdit> staticField;
    std::unique_ptr<MainEdit> inField;
    std::unique_ptr<MainEdit> outCycleField;
    std::unique_ptr<MainEdit> outRecursionField;

    std::unique_ptr<MainButton> calcButton;

    void calc();
    double cycle(int steps);
    double recursion(int steps);

public:
    Lab1Window() : WndProps(L"Рекурсия", 500, 300) {}

    PCWSTR ClassName() const override { return L"Lab1WindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

    protected:
        CComPtr<IDWriteTextFormat> pLabelTextFormat;
        CComPtr<ID2D1SolidColorBrush> pLabelColorBrush;

        void CreateDeviceDepRes(HRESULT& hr);
        void DrawContent();
};