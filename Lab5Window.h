//
// Lab5Window.h
//
#pragma once
#include "WndProps.h"
#include "MainWindow.h"
#include "MainEdit.h"
#include "MainButton.h"
#include "MapleTree.h"
#include "TreeDrawer.h"
#include "Utils.h"
#include <memory>

#define L5_IDTREEDRAWER      5001
#define L5_IDKEY_FIELD       5002
#define L5_IDVALUE_FIELD     5003
#define L5_IDBTTN_DELETE     5004
#define L5_IDBTTN_CLEAR_ALL  5005
#define L5_IDBTTN_MIDDLE_KEY 5006

using LAB5_KEYTYPE = int; // Hardcoded. Just for clean typing (or to fast change by name)
using LAB5_VALUETYPE = std::wstring;

class Lab5Window : public WndProps<Lab5Window, 4>, public MainWindow<Lab5Window>
{
    std::unique_ptr<TreeDrawer<LAB5_KEYTYPE, LAB5_VALUETYPE>> treeDrawer;
    std::unique_ptr<MainEdit>                                 keyField;
    std::unique_ptr<MainEdit>                                 valueField;
    std::unique_ptr<MainButton>                               bttnDelete;
    std::unique_ptr<MainButton>                               bttnClearAll;
    std::unique_ptr<MainButton>                               bttnMiddleKey;

    MapleTree<LAB5_KEYTYPE, LAB5_VALUETYPE> labTree;

    LAB5_KEYTYPE keyFromField();

    void infoOnKeyInsert();
    void writeInfoInKey();
    void deleteKey();
    void clearAllFields();
    void middleKeyInfo();

public:
    Lab5Window() : WndProps(L"Дерево", 720, 410)
    {}

    PCWSTR ClassName() const override { return L"Lab5WindowClass"; }
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    CComPtr<IDWriteTextFormat>    pLabelTextFormat;
    CComPtr<ID2D1SolidColorBrush> pLabelColorBrush;

    void CreateDeviceDepRes(HRESULT& hr);
    void DrawContent();
};

