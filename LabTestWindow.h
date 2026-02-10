//
// LabTestWindow.h
//
#pragma once
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "MainCheckBox.h"
#include <memory>

#define ID_BTN_CALC 99999
#define ID_IN_FIELD 99991
#define ID_OUT_FIELD 99990
#define ID_CHECK_SMTH 99994

class LabTestWindow : public WndProps<LabTestWindow, 4>, public MainWindow<LabTestWindow>
{
    std::unique_ptr<MainEdit> inField;
    std::unique_ptr<MainEdit> outField;
    std::unique_ptr<MainButton> calcButton;
    std::unique_ptr<MainCheckBox> testCheckBox;

    void calc(const std::wstring &wstr);

public:
    LabTestWindow() : WndProps(L"Лабораторная тест", 700, 500) {}

    PCWSTR ClassName() const override { return L"LabTestWindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

//protected:
//    void CreateDeviceDepRes(HRESULT& hr);
//    void DrawContent();
};