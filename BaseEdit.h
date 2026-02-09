//
// BaseEdit.h
//
#include "IControl.h"
#include <Windows.h>
#include <string>
#pragma once

class BaseEdit : public IControl
{
	std::wstring text_;
	HFONT hFont_;

	int posX_;
	int posY_;
	int width_;
	int height_;

	HWND hWnd_;
	HWND hWndParent_;
	int id_;

public:
	//BaseEdit(int id) :
	//	id_(id),
	//	height_(0), width_(0), posX_(0), posY_(0),
	//	hWndParent_(NULL), hWnd_(NULL)
	//{}
	BaseEdit(
		int id,
		int width,
		int height,
		int x,
		int y,
		HWND parent
	) :
		id_(id),
		width_(width), height_(height), posX_(x), posY_(y),
		hWndParent_(parent), hWnd_(NULL)
	{}
	~BaseEdit() { if (hFont_) DeleteObject(hFont_); }

	std::wstring GetText()
	{
		if (!hWnd_) return L"";
		int len = GetWindowTextLength(hWnd_);
		if (len == 0) return L"";

		text_.clear();
		text_.resize(len); // Without \0
		GetWindowText(hWnd_, &text_[0], len + 1);
		return text_;
	}

	void SetText(const std::wstring & text) const
	{
		if (hWnd_) SetWindowText(hWnd_, text.c_str());
	}

	void Create(bool readOnly = false)
	{
		DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_CENTER | ES_MULTILINE;
		if (readOnly) style |= ES_READONLY;

		hWnd_ = CreateWindowEx(
			0,
			L"EDIT",
			NULL,
			style,
			posX_, posY_, width_, height_,
			hWndParent_,
			(HMENU)id_,
			GetModuleHandle(NULL),
			NULL
		);

		UINT dpi = GetDpiForWindow(hWndParent_);
		if (dpi < 1) dpi = GetDpiForSystem();

		int fontSizePoints = 12;
		int fontHeight = -MulDiv(fontSizePoints, dpi, 72); // Minus for only gliph size without paddings

		if (hFont_) DeleteObject(hFont_);

		hFont_ = CreateFont(
			fontHeight,
			0,
			0, 0,
			FW_MEDIUM,
			FALSE, FALSE, FALSE,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			CLEARTYPE_QUALITY,
			DEFAULT_PITCH | FF_SWISS,
			L"Segoe UI"
		);

		SendMessage(hWnd_, WM_SETFONT, (WPARAM)hFont_, TRUE);
	}

	BOOL Move(int x, int y) override
	{
		BOOL res = TRUE;
		posX_ = x;
		posY_ = y;

		res = SetWindowPos(
			hWnd_,
			NULL,
			posX_,
			posY_,
			0,
			0,
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE // No child reorder, no resize, no parent window activation
		);

		return res;
	}

	int GetWidth() const override { return width_; }
	int GetHeight() const override { return height_; }
};