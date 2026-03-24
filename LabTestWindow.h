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
#include "MainListView.h"
#include <memory>

#define LTEST_IDBTN_CALC 9999
#define LTEST_IDIN_FIELD 9991
#define LTEST_IDOUT_FIELD 9990
#define LTEST_IDCHECK_SMTH 9994

#define LTEST_IDRADI_GROUP 128
#define LTEST_IDRADI_1 9995
#define LTEST_IDRADI_2 9996

#define ID_LIST_VIEW 9997

class LabTestWindow : public WndProps<LabTestWindow, 4>, public MainWindow<LabTestWindow>
{
    std::unique_ptr<MainEdit> inField;
    std::unique_ptr<MainEdit> outField;
    std::unique_ptr<MainButton> calcButton;
    std::unique_ptr<MainCheckBox> testCheckBox;

    std::unique_ptr<MainRadioButton<LTEST_IDRADI_GROUP>> radio1;
    std::unique_ptr<MainRadioButton<LTEST_IDRADI_GROUP>> radio2;
    int selectedRadi;

    std::unique_ptr<MainListView> listView;

    void calc(const std::wstring &wstr);

public:
    LabTestWindow() : WndProps(L"Лабораторная тест", 700, 500, 600, 400), selectedRadi(0) {}

    PCWSTR ClassName() const override { return L"LabTestWindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

//protected:
//    void CreateDeviceDepRes(HRESULT& hr);
//    void DrawContent();
};