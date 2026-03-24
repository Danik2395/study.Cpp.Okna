//
// Lab4Window.h
//
#pragma once
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "VariablesListView.h"
#include "List.h"
#include "shuntingYard.h"
#include <memory>

#define L4_IDIN_FIELD 4001
#define L4_IDOUT_RPN_FIELD 4002
#define L4_IDOUT_RESULT_FIELD 4003
#define L4_IDOUT_CALCULATE_FIELD 4004

#define L4_IDBTTN_TRANSL 4005
#define L4_IDBTTN_CALC 4006

#define L4_IDLWV_VARS 4007

class Lab4Window : public WndProps<Lab4Window, 4>, public MainWindow<Lab4Window>
{
    std::unique_ptr<MainEdit> inField;
    std::unique_ptr<MainEdit> outRpnField;
    std::unique_ptr<MainEdit> outResultField;

    std::unique_ptr<MainButton> bttnTranslate;
    std::unique_ptr<MainButton> bttnCalculate;

    std::unique_ptr<VariablesListView> vlvVariables;

    List<shuntingYard::Variables> variablesList;

    shuntingYard yard;

    void translate();
    void calculate();
    void createVariables();

public:
    Lab4Window() : WndProps(L"Сортировочная станция", 580, 340)
    {}

    PCWSTR ClassName() const override { return L"Lab4WindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    CComPtr<IDWriteTextFormat> pLabelTextFormat;
    CComPtr<ID2D1SolidColorBrush> pLabelColorBrush;

    void CreateDeviceDepRes(HRESULT& hr);
    void DrawContent();
};
