//
// Lab8Window.h
//
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "MainRadioButton.h"
#include "PlotWindow.h"
#include "Utils.h"
#include "Functions.h"
#include "IntegralSolver.h"
#include <memory>
#pragma once

#define L8_IDFIELD_LIM_MIN 8001
#define L8_IDFIELD_LIM_MAX 8002
#define L8_IDFIELD_RESULT 8003
#define L8_IDFIELD_EPSILON 8004
#define L8_IDFIELD_STEP 8005
#define L8_IDPLOT 8006
#define L8_IDBTTN_CALCULATE 8007

#define L8_IDRADI_GROUP 888
#define L8_IDRADI_PARTITIONING 8008
#define L8_IDRADI_AUTO 8009


class Lab8Window : public WndProps<Lab8Window, 4>, public MainWindow<Lab8Window>
{
    std::unique_ptr<MainEdit>   limMinField;
    std::unique_ptr<MainEdit>   limMaxField;
    std::unique_ptr<MainEdit>   resultField;
    std::unique_ptr<MainEdit>   partirionsField;
    std::unique_ptr<MainEdit>   epsilonField;
    std::unique_ptr<MainButton> bttnCalculate;
    std::unique_ptr<PlotWindow<decltype(&Functions::lab8Function)>> plotWindow;

    std::unique_ptr<MainRadioButton<L8_IDRADI_GROUP>> radiPartitions;
    std::unique_ptr<MainRadioButton<L8_IDRADI_GROUP>> radiAuto;

    IntegralSolver<decltype(&Functions::lab8Function)> integralSolver;

    double limMin_;
    double limMax_;
    double epsilon_;
    double partitions_;

    void shabang() { resultField->SetText(L"Функция x² + 5cos(x)\r\n"); };

    void setLimits();
    void setPartitions();
    void setEpsilon();
    void calculate();

public:
    Lab8Window() : WndProps(L"Интегрирование", 830, 530),
        limMin_(-1),
        limMax_(4),
        partitions_(20),
        epsilon_(0.001),
        integralSolver(&Functions::lab8Function, epsilon_)
    {}

    PCWSTR ClassName() const override { return L"Lab8WindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    CComPtr<IDWriteTextFormat>    pLabelTextFormat;
    CComPtr<ID2D1SolidColorBrush> pLabelColorBrush;

    void CreateDeviceDepRes(HRESULT& hr);
    void DrawContent();
};
