//
// HubWindow.h
//
#include "WndProps.h"
#include "MainWindow.h"
#include "MainButton.h"
#include "ThreadLauncher.h"
#include <memory>

#include "LabTestWindow.h"
#include "Lab1Window.h"
#include "Lab2Window.h"
#include "Lab3Window.h"
#include "Lab4Window.h"
#include "Lab5Window.h"
#include "Lab6Window.h"
#include "Lab8Window.h"
#pragma once

#define HUB_ID_BTN_LAB_TEST 1111
#define HUB_ID_BTN_LAB1 1101
#define HUB_ID_BTN_LAB2 1102
#define HUB_ID_BTN_LAB3 1103
#define HUB_ID_BTN_LAB4 1104
#define HUB_ID_BTN_LAB5 1105
#define HUB_ID_BTN_LAB6 1106
#define HUB_ID_BTN_LAB8 1108

class HubWindow : public WndProps<HubWindow>, public MainWindow<HubWindow>
{
    struct LabDef { int id; LPCWSTR name; };

    std::vector <std::unique_ptr< MainButton >> buttons;

    int btnWidth;
    int btnHeight;
    int btnPadding;
 
    void RecalculateLayout();

public:
    HubWindow() : WndProps(L"Learn to Program Windows", 500, 500),
        btnWidth(120),
        btnHeight(50),
        btnPadding(10)
    {}

    PCWSTR  ClassName() const override { return L"HubWindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    //void DrawContent() override;
};

