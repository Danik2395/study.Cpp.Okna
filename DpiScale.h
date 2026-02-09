//
// DpiScale.h
//
#include <windows.h>
#pragma once

class DpiScale
{
    float dpi_;
    float dpi_scale_;

public:
    DpiScale() = default;

    // Window init or WM_CREATE
    void Init(HWND hwnd)
    {
        UINT dpi = GetDpiForWindow(hwnd);
        if (dpi < 1) dpi = USER_DEFAULT_SCREEN_DPI;

        SetDpi(dpi);
    }

    // WM_DPICHANGED
    void SetDpi(UINT dpi)
    {
        dpi_ = static_cast<float>(dpi);
        dpi_scale_ = dpi / static_cast<float>(USER_DEFAULT_SCREEN_DPI);
    }

    float GetDpi() const { return dpi_; }
    float Scale() const { return dpi_scale_; }

    // DIPs to physical pixels
    int Scale(int DIPs) const
    {
        return static_cast<int>(DIPs * dpi_scale_);
    }

    // Physical pixels to DIPs
    int Unscale(int px) const
    {
        return static_cast<int>(px / dpi_scale_);
    }

    // DIPs rectangle to physical
    void ScaleRect(const RECT &rc) const
    {
        Scale(rc.left);
        Scale(rc.top);
        Scale(rc.right);
        Scale(rc.bottom);
    }

    // DIPs to physical pixels from system scale
    static int ScaleForSystem(int DIPs)
    {
        UINT dpi = GetDpiForSystem();
        if (dpi < 1) dpi = USER_DEFAULT_SCREEN_DPI;

        return MulDiv(DIPs, dpi, 96); // For safe scale
    }
};