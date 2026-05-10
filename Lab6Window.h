//
// Lab6Window.h
//
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "PlotWindow.h"
#include "Utils.h"
#include "Functions.h"
#include "NoLinearSolver.h"
#include <memory>
#pragma once

#define L6_IDFIELD_LIM_MIN 6001
#define L6_IDFIELD_LIM_MAX 6002
#define L6_IDFIELD_RESULT 6003
#define L6_IDFIELD_EPSILON 6004
#define L6_IDFIELD_STEP 6005
#define L6_IDPLOT 6006
#define L6_IDBTTN_CALCULATE 6007

class Lab6Window : public WndProps<Lab6Window, 4>, public MainWindow<Lab6Window>
{
    std::unique_ptr<MainEdit>   limMinField;
    std::unique_ptr<MainEdit>   limMaxField;
    std::unique_ptr<MainEdit>   resultField;
    std::unique_ptr<MainEdit>   stepField;
    std::unique_ptr<MainEdit>   epsilonField;
    std::unique_ptr<MainButton> bttnCalculate;
    std::unique_ptr<PlotWindow<decltype(&Functions::lab6Function)>> plotWindow;

    NoLinearSolver<decltype(&Functions::lab6Function)> noLinearSolver;

    double limMin_;
    double limMax_;
    double step_;
    double epsilon_;

    void shabang() { resultField->SetText(L"Функция x² + 5cos(x) - 3\r\n"); };

    void setLimits();
    void setStep();
    void setEpsilon();
    void calculate();

public:
    Lab6Window() : WndProps(L"Корни уравнений", 830, 530),
        limMin_(-4),
        limMax_(2),
        epsilon_(0.001),
        step_(0.01),
        noLinearSolver(Functions::lab6Function, Functions::lab6DFunction, epsilon_)
    {}

    PCWSTR ClassName() const override { return L"Lab6WindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    CComPtr<IDWriteTextFormat>    pLabelTextFormat;
    CComPtr<ID2D1SolidColorBrush> pLabelColorBrush;

    void CreateDeviceDepRes(HRESULT& hr);
    void DrawContent();
};
