//
// WndProps.h
//
#include "DpiScale.h"
#include <windows.h>
#include <type_traits>
#include <atomic>
#include <string>
#pragma once

class BaseWndProps
{
protected:
	std::wstring wndName;

	int wndWidth;
	int wndHeight;
    int wndPosX;
    int wndPosY;

public:
    BaseWndProps(
        const std::wstring &name,
        int width,
        int height,
        int posX = CW_USEDEFAULT,
        int posY = CW_USEDEFAULT
    ) : wndName(name)
    {
        wndWidth = DpiScale::ScaleForSystem(width);
        wndHeight = DpiScale::ScaleForSystem(height);
        wndPosX = posX == CW_USEDEFAULT ? posX : DpiScale::ScaleForSystem(posX);
        wndPosY = posY == CW_USEDEFAULT ? posY : DpiScale::ScaleForSystem(posY);
    }
	BaseWndProps() : wndName(L"Window"), wndWidth(800), wndHeight(800), wndPosX(CW_USEDEFAULT), wndPosY(CW_USEDEFAULT) {}

	PCWSTR Name() const { return wndName.c_str(); }
	int Width() const { return wndWidth; }
	int Height() const { return wndHeight; }
	int PosX() const { return wndPosX; }
	int PosY() const { return wndPosY; }
};

// Basic template
template <class DERIVED_TYPE, int WND_COUNT = 0> // Class for creating parent windows
class WndProps : public BaseWndProps
{
    inline static std::atomic<int> sInstanceCount{ 0 };

public:
    // WndProps(name, width, height, x, y)
    template <typename... Args>
    WndProps(Args&&... args) : BaseWndProps(std::forward<Args>(args)...)
    {
        ++sInstanceCount;
    }

    ~WndProps()
    {
        --sInstanceCount;
    }

    int CheckInstanceCount() const
    {
        if (sInstanceCount > WND_COUNT)
        {
            std::wstring msg = L"Максимум окон для лабораторной: " + std::to_wstring(WND_COUNT);

            MessageBox(NULL, msg.c_str(), L"Ошибка", MB_ICONERROR);
            return 1;
        }
        return 0;
    }
};

// And if default argument no isInstanceCount
template <class DERIVED_TYPE>                    // Class for creating parent windows
class WndProps<DERIVED_TYPE, 0> : public BaseWndProps
{
public:
    using BaseWndProps::BaseWndProps; // Inheriting constructors

    int CheckInstanceCount() const { return 0; }
};