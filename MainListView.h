//
// MainListView.h
//
#include "IControl.h"
#include "DpiScale.h"
#include <Windows.h>
#include <string>
#include <commctrl.h>
#include <vector>
#pragma comment(lib, "comctl32.lib")
#pragma once

#define ID_TEMP_EDIT 1001

class MainListView : public IControl
{
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
	DWORD exStyles_;

	DpiScale dpi;

	HWND hEdit_;
	int editRow_;
	int editCol_;

	// Subclass windowProc for the ListView
	static LRESULT CALLBACK ListViewSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		MainListView* pThis = reinterpret_cast<MainListView*>(dwRefData);

		switch (uMsg)
		{
		case WM_LBUTTONDBLCLK: // Double click creates temporary edit
		{
			LVHITTESTINFO hti = { 0 };
			hti.pt.x = (short)LOWORD(lParam);
			hti.pt.y = (short)HIWORD(lParam);
			ListView_SubItemHitTest(hWnd, &hti); // Determining what subItem (column) and iItem (raw) is under the cursor
			                                     // Sets additional flags on what element click was
			                                     // Empty click is -1

			if (hti.iItem != -1 && hti.iSubItem == 1)
			{
				pThis->editRow_ = hti.iItem;
				pThis->editCol_ = hti.iSubItem;

				RECT rc;
				ListView_GetSubItemRect(hWnd, hti.iItem, hti.iSubItem, LVIR_BOUNDS, &rc);

				wchar_t buffer[256] = { 0 };
				ListView_GetItemText(hWnd, hti.iItem, hti.iSubItem, buffer, 256);

				// Creating edit as a child element of the list view
				pThis->hEdit_ = CreateWindowEx(
					0, L"EDIT", buffer,
					WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
					rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
					hWnd,
					(HMENU)ID_TEMP_EDIT,
					GetModuleHandle(NULL),
					NULL
				);

				// Setting font as in the listview
				SendMessage(pThis->hEdit_, WM_SETFONT, SendMessage(hWnd, WM_GETFONT, 0, 0), TRUE);
				SetFocus(pThis->hEdit_);
				SendMessage(pThis->hEdit_, EM_SETSEL, 0, -1); // Selecting all text in the edit

				// Setting edit subclass
				SetWindowSubclass(pThis->hEdit_, EditSubclassProc, 1, dwRefData); // It takes MainListView class in the dwRefData so it uppears in the EditSubaClassProc
			}
			return 0;
		}

		case WM_COMMAND:
		{
			// Moment when user pressed in another place (unfocus)
			if (HIWORD(wParam) == EN_KILLFOCUS && pThis->hEdit_ && (HWND)lParam == pThis->hEdit_)
			{
				wchar_t buffer[256] = { 0 };
				GetWindowText(pThis->hEdit_, buffer, 256);

				pThis->SetCellValue(pThis->editRow_, pThis->editCol_, buffer);

				DestroyWindow(pThis->hEdit_);
				pThis->hEdit_ = NULL;
			}
			break;
		}

		case WM_MOUSEWHEEL: // On the scroll setting saving value and setting unfocus
		case WM_VSCROLL:
		case WM_HSCROLL:
			if (pThis->hEdit_)
			{
				SetFocus(hWnd); // This will handle saving
			}
			break;

		case WM_NCDESTROY:
			RemoveWindowSubclass(hWnd, ListViewSubclassProc, uIdSubclass);
			break;
		}

		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

	// Subclass windowProc for temporary edit to handle the Enter and Esc button clicks
	static LRESULT CALLBACK EditSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
	{
		                                // Here we have MainListView class with hWnd_ of it
		MainListView* pThis = reinterpret_cast<MainListView*>(dwRefData);

		if (uMsg == WM_KEYDOWN)
		{
			if (wParam == VK_RETURN)
			{
				SetFocus(pThis->hWnd_); // Using hWnd_ of the MainListView
				return 0;
			}
			else if (wParam == VK_ESCAPE)
			{
				pThis->hEdit_ = NULL;   // Setting to NULL so the MainListView won't fell into WM_COMMAND -> EN_KILLFOCUS and saved value
				DestroyWindow(hWnd);
				SetFocus(pThis->hWnd_);
				return 0;
			}
		}
		else if (uMsg == WM_NCDESTROY)
		{
			RemoveWindowSubclass(hWnd, EditSubclassProc, uIdSubclass);
		}

		return DefSubclassProc(hWnd, uMsg, wParam, lParam);
	}

public:
	MainListView(
		int id,
		int width,
		int height,
		int x,
		int y,
		int fontSize,
		HWND parent,
		DWORD styles = 0,
		DWORD exStyles = 0
	) :
		id_(id),
		width_(width), height_(height), posX_(x), posY_(y),
		fontSize_(fontSize),
		hWndParent_(parent),
		styles_(styles),
		exStyles_(exStyles),
		hWnd_(NULL),
		hFont_(NULL),
		editRow_(0),
		editCol_(0),
		hEdit_(0)
	{
		INITCOMMONCONTROLSEX icex; // Initializing required common control class
		icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icex.dwICC = ICC_LISTVIEW_CLASSES;
		InitCommonControlsEx(&icex);

	}
	~MainListView()
	{
		if (hWnd_) DestroyWindow(hWnd_);
		if (hFont_) DeleteObject(hFont_);
	}

	// Set subItemIndex not to -1 to get set not 0 column
	void AddColumn(int colIndex, const std::wstring &text, int width, int subItemIndex = -1)
	{
		LVCOLUMN lvc = { 0 };
		lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;            // Mask so the structure doesn't pull all the fields
		lvc.cx = width;                                              // Width in pixels
		lvc.pszText = const_cast<LPWSTR>(text.c_str());
		lvc.iSubItem = subItemIndex == -1 ? colIndex : subItemIndex; // Linking subitem with the column index

		ListView_InsertColumn(hWnd_, colIndex, &lvc);
	}

	// Inserts two element row into the table as |Name|Value|
	void AddRow(const std::wstring &varName, const std::wstring &varValue)
	{
		int rowCount = ListView_GetItemCount(hWnd_);

		LVITEM lvi = { 0 };
		lvi.mask = LVIF_TEXT;
		lvi.iItem = rowCount; // Item index
		lvi.iSubItem = 0;     // On the new raw it need to be 0 in the ListView_InsertItem()
		lvi.pszText = const_cast<LPWSTR>(varName.c_str());

		int newRowIndex = ListView_InsertItem(hWnd_, &lvi);

		if (newRowIndex != -1)
		{
			SetCellValue(newRowIndex, 1, varValue);
		}
	}

	void SetCellValue(int rowIndex, int colIndex, const std::wstring &text)
	{
		ListView_SetItemText(hWnd_, rowIndex, colIndex, const_cast<LPWSTR>(text.c_str()));
	}

	std::wstring GetCellValue(int rowIndex, int colIndex) const
	{
		wchar_t buffer[256] = { 0 };
		ListView_GetItemText(hWnd_, rowIndex, colIndex, buffer, 256);
		return std::wstring(buffer);
	}

	bool DeleteRow(int rowIndex)
	{
		return ListView_DeleteItem(hWnd_, rowIndex) == TRUE; // All the delete macros return wether TRUE or FALSE
	}

	bool ClearRows()
	{
		return ListView_DeleteAllItems(hWnd_) == TRUE;
	}

	bool DeleteColumn(int colIndex)
	{
		return ListView_DeleteColumn(hWnd_, colIndex) == TRUE;
	}

	void ClearColumns()
	{
		while (ListView_DeleteColumn(hWnd_, 0));
	}

	void Create()
	{
		DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT; // Report stands for the table with main column in 0 position
		style |= styles_;

		hWnd_ = CreateWindowEx(
			0,
			WC_LISTVIEW,
			NULL,
			style,
			posX_, posY_, width_, height_,
			hWndParent_,
			(HMENU)id_,
			GetModuleHandle(NULL),
			NULL
		);

		ListView_SetExtendedListViewStyle(hWnd_, exStyles_);

		dpi.Init(hWnd_);

		int fontHeight = -MulDiv(fontSize_, dpi.GetDpi(), 72); // Minus for only glyph size without paddings

		// Setting subclass to handle specified messages and setting this in dwRefData
		SetWindowSubclass(hWnd_, ListViewSubclassProc, 1, reinterpret_cast<DWORD_PTR>(this));

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
