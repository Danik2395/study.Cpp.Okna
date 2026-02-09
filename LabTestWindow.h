//
// LabTestWindow.h
//
#pragma once
#include "WndProps.h"
#include "MainWindow.h"
#include "BaseEdit.h"
#include "MainButton.h"
#include "MainCheckBox.h"
#include <memory>

#define ID_BTN_CALC 2001
#define ID_IN_FIELD 2002
#define ID_OUT_FIELD 2003
#define ID_CHECK_SMTH 2004

class LabTestWindow : public WndProps<LabTestWindow, 4>, public MainWindow<LabTestWindow>
{
    std::unique_ptr<BaseEdit> inField;
    std::unique_ptr<BaseEdit> outField;
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