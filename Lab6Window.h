//
// Lab6Window.h
//
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "PlotWindow.h"
#include "Utils.h"
#include "NoLinearSolver.h"
#include <memory>
#include <cmath>
#pragma once

#define L6_IDFIELD_LIM_MIN 5001
#define L6_IDFIELD_LIM_MAX 5002
#define L6_IDFIELD_RESULT 5003
#define L6_IDFIELD_EPSILON 5004
#define L6_IDFIELD_STEP 5005
#define L6_IDPLOT 5006
#define L6_IDBTTN_CALCULATE 5007

namespace Functions
{
	inline double linearFunction(double x)
	{
		return x * x + 5 * cos(x) - 3;
	}

	inline double linearDFunction(double x)
	{
		return 2 * x - 5 * sin(x);
	}
}

class Lab6Window : public WndProps<Lab6Window, 4>, public MainWindow<Lab6Window>
{
    std::unique_ptr<MainEdit>   limMinField;
    std::unique_ptr<MainEdit>   limMaxField;
    std::unique_ptr<MainEdit>   resultField;
    std::unique_ptr<MainEdit>   stepField;
    std::unique_ptr<MainEdit>   epsilonField;
    std::unique_ptr<MainButton> bttnCalculate;
    std::unique_ptr<PlotWindow<decltype(&Functions::linearFunction)>> plotWindow;

    NoLinearSolver<decltype(&Functions::linearFunction)> noLinearSolver;

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
    Lab6Window() : WndProps(L"Корни уравнений", 830, 530), noLinearSolver(Functions::linearFunction, Functions::linearDFunction)
    {}

    PCWSTR ClassName() const override { return L"Lab6WindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    CComPtr<IDWriteTextFormat>    pLabelTextFormat;
    CComPtr<ID2D1SolidColorBrush> pLabelColorBrush;

    void CreateDeviceDepRes(HRESULT& hr);
    void DrawContent();
};
