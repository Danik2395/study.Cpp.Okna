//
// MainEdit.h
//
#include "IControl.h"
#include "DpiScale.h"
#include <Windows.h>
#include <string>
#pragma once

class MainEdit : public IControl
{
	std::wstring text_;
	HFONT hFont_;
	int fontSize_;
	

	int posX_;
	int posY_;
	int width_;
	int height_;

	HWND hWnd_;
	HWND hWndParent_;
	int id_;

	DWORD styles_;

	DpiScale dpi;

public:
	//MainEdit(int id) :
	//	id_(id),
	//	height_(0), width_(0), posX_(0), posY_(0),
	//	hWndParent_(NULL), hWnd_(NULL)
	//{}
	MainEdit(
		int id,
		int width,
		int height,
		int x,
		int y,
		int fontSize,
		HWND parent,
		DWORD styles = 0
	) :
		id_(id),
		width_(width), height_(height), posX_(x), posY_(y),
		fontSize_(fontSize),
		hWndParent_(parent),
		styles_(styles),
		hWnd_(NULL),
		hFont_(NULL)
	{}
	~MainEdit() { if (hFont_) DeleteObject(hFont_); }

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

	void SetText(const std::wstring &text) const
	{
		if (hWnd_) SetWindowText(hWnd_, text.c_str());
	}
	
	void RemoveText() const
	{
		if (hWnd_) SetWindowText(hWnd_, L""); // To not to write L"" and call c_srt() every time
	}

	void AppendText(const std::wstring &newText) const
	{
		int length = GetWindowTextLength(hWnd_);

		SendMessage(hWnd_, EM_SETSEL, (WPARAM)length, (LPARAM)length);    // Empty select at the end of the text

		SendMessage(hWnd_, EM_REPLACESEL, NULL, (LPARAM)newText.c_str()); // Replacing this select with new text
	}

	void PrependText(const std::wstring &newText) const
	{
		SendMessage(hWnd_, EM_SETSEL, (WPARAM)0, (LPARAM)0);
		SendMessage(hWnd_, EM_REPLACESEL, NULL, (LPARAM)newText.c_str());
	}

	void ReplaceSelText(const std::wstring &newText, int stPos, int endPos) const
	{
		int length = GetWindowTextLength(hWnd_);

		if (endPos > length) endPos = length;

		SendMessage(hWnd_, EM_SETSEL, (WPARAM)stPos, (LPARAM)endPos);     // Empty select at the end of the text

		SendMessage(hWnd_, EM_REPLACESEL, NULL, (LPARAM)newText.c_str()); // Replacing this select with new text
	}

	template<typename T>
	static T GetNumber(std::wstring buffer)
	{
		static constexpr bool isIntegral = std::is_integral<T>::value;
		static constexpr bool isUnsigned = std::is_unsigned<T>::value;
		bool hasDot = false;
		bool hasSign = false;

		size_t len = buffer.length();

		if constexpr (!isUnsigned)
		{
			if (buffer[0] == L'-')
			{
				buffer.erase(0, 1);
				--len;
				hasSign = true;
			}
		}

		for (int i = 0; i < len; ++i)
		{
			if (buffer[i] >= L'0' && buffer[i] <= L'9') continue;

			if constexpr (!isIntegral)
			{
				if (buffer[i] == L'.')
				{
					if (hasDot)
					{
						buffer.erase(i--, 1);
						--len;
					}
					hasDot = true;
				}
				else if (buffer[i] == L',')
				{                                     
					buffer[i] = L'.';
					hasDot = true;
				}

				continue;
			}

			buffer.erase(i--, 1);
			--len;
		}

		if (len == 0) return 0;

		if (len > 10 && buffer[0] > L'2') buffer.erase(11);

		if constexpr (isIntegral)
		{
			return hasSign ? -std::stoi(buffer) : std::stoi(buffer);
		}
		else
		{
			return hasSign ? -std::stof(buffer) : std::stof(buffer);
		}
	}

	void Create(/*bool readOnly = false*/)
	{
		DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL;
		//if (readOnly) style |= ES_READONLY;
		style |= styles_;

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

		//UINT dpi = GetDpiForWindow(hWndParent_);
		//if (dpi < 1) dpi = GetDpiForSystem();
		dpi.Init(hWnd_);

		int fontHeight = -MulDiv(fontSize_, dpi.GetDpi(), 72); // Minus for only glyph size without paddings

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