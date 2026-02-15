//
// LabTestWindow.h
//
#pragma once
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "MainCheckBox.h"
#include "MainRadioButton.h"
#include <memory>

#define ID_BTN_CALC 9999
#define ID_IN_FIELD 9991
#define ID_OUT_FIELD 9990
#define ID_CHECK_SMTH 9994

#define ID_RADI_GROUP 128
#define ID_RADI_1 9995
#define ID_RADI_2 9996

class LabTestWindow : public WndProps<LabTestWindow, 4>, public MainWindow<LabTestWindow>
{
    std::unique_ptr<MainEdit> inField;
    std::unique_ptr<MainEdit> outField;
    std::unique_ptr<MainButton> calcButton;
    std::unique_ptr<MainCheckBox> testCheckBox;

    std::unique_ptr<MainRadioButton<ID_RADI_GROUP>> radio1;
    std::unique_ptr<MainRadioButton<ID_RADI_GROUP>> radio2;
    int selectedRadi;

    void calc(const std::wstring &wstr);

public:
    LabTestWindow() : WndProps(L"Лабораторная тест", 700, 500, 600, 400), selectedRadi(0) {}

    PCWSTR ClassName() const override { return L"LabTestWindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

//protected:
//    void CreateDeviceDepRes(HRESULT& hr);
//    void DrawContent();
};