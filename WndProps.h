//
// WndProps.h
//
#include "DpiScale.h"
#include <windows.h>
#include <atomic>
#include <string>
#pragma once

class BaseWndProps
{
	std::wstring wndName;

	int wndWidth;
	int wndHeight;

public:
	//BaseWndProps(
	//	const std::wstring &name,
	//	int width,
	//	int height
	//) : 
	//	wndName(name),
	//	wndWidth(width),
	//	wndHeight(height)
	//{}
    BaseWndProps(
        const std::wstring &name,
        int width,
        int height
    ) : wndName(name)
    {
        wndWidth = DpiScale::ScaleForSystem(width);
        wndHeight = DpiScale::ScaleForSystem(height);
    }
	BaseWndProps() : wndName(L"Window"), wndWidth(800), wndHeight(800) {}

	PCWSTR Name() const { return wndName.c_str(); }
	int Width() const { return wndWidth; }
	int Height() const { return wndHeight; }
};

// Basic template
template <class DERIVED_TYPE, int WND_COUNT = 0> // Class for creating parent windows
class WndProps : public BaseWndProps
{
    static std::atomic<int> sInstanceCount;

public:
    WndProps(const std::wstring& name, int width, int height)
        : BaseWndProps(name, width, height)
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
    WndProps(const std::wstring& name, int width, int height)
        : BaseWndProps(name, width, height)
    {}

    int CheckInstanceCount() const { return 0; }
};

template <class DERIVED_TYPE, int WND_COUNT>
std::atomic<int> WndProps<DERIVED_TYPE, WND_COUNT>::sInstanceCount{ 0 };



class ControlWndProps
{

};