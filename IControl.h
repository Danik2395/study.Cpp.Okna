//
// IControl.h
//
#include <windows.h>
#pragma once

// Interface for controls-windows
class IControl
{
public:
	virtual ~IControl() = default; // To not to destruct object from base class. To avoid memory leak because base class won't destruct derived class
	virtual BOOL Move(int x, int y) = 0;
	virtual int GetWidth() const = 0;
	virtual int GetHeight() const = 0;
};