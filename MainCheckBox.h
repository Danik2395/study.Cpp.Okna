//
// MainCheckBox.h
//
#include "D2DWindow.h"
#include "IControl.h"
#include "m_random.h"
#include <vector>
#include <string>
#pragma once

class MainCheckBox : public D2DWindow<MainCheckBox>, public IControl
{
	PCWSTR name_;

	int posX_;
	int posY_;
	int width_;
	int height_;

	HWND hWndParent_;
	int id_;

	bool isHovered;
	bool isPressed;
	bool isSelected;

	inline BOOL chbxInvalidate() { return InvalidateRect(this->m_hwnd, NULL, FALSE); }

public:
	MainCheckBox(int id) :
		id_(id),
		name_(NULL),
		height_(0), width_(0), posX_(0), posY_(0),
		hWndParent_(NULL),
		isHovered(false),
		isPressed(false),
		isSelected(false)
	{}
	MainCheckBox(
		int id,
		int width,
		int height,
		int x,
		int y,
		HWND parent
	) :
		id_(id),
		name_(NULL),
		width_(width), height_(height), posX_(x), posY_(y),
		hWndParent_(parent),
		isHovered(false),
		isSelected(false),
		isPressed(false)
	{}

	inline bool State() const { return isSelected; } // Because idk how to overload operator though the pointer

	void Create()
	{
		BaseWindow::Create(
			name_,
			WS_VISIBLE | WS_CHILD, // Child window without border and title bar
			0,
			posX_, posY_, width_, height_,
			hWndParent_,
			(HMENU)id_
		);
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override
	{
		switch (uMsg)
		{
		case WM_SETCURSOR:
		{
			if (isHovered)
			{
				LPCTSTR cursor = IDC_ARROW;
				HCURSOR hCursor = LoadCursor(NULL, cursor);
				SetCursor(hCursor);
				return 0;
			}
			else
			{
				return DefWindowProc(this->m_hwnd, uMsg, wParam, lParam);
			}
		}

		case WM_MOUSEMOVE:
		{
			if (!isHovered)
			{
				isHovered = true;

				TRACKMOUSEEVENT tme = { sizeof(TRACKMOUSEEVENT), TME_LEAVE, this->m_hwnd, 0 };
				TrackMouseEvent(&tme);

				chbxInvalidate();
			}
			else // Because when SetCapture() "set" it makes system not to send WM_MOUSELEAVE (you captured, you processing)
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				RECT rc; GetClientRect(m_hwnd, &rc);

				if (!PtInRect(&rc, pt))
				{
					isHovered = false;
				}
			}
			return 0;
		}

		case WM_MOUSELEAVE:
		{
			isHovered = false;
			chbxInvalidate();
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			isPressed = true;
			SetCapture(this->m_hwnd);
			chbxInvalidate();
			return 0;
		}

		case WM_LBUTTONUP:
		{
			if (isPressed)
			{
				isPressed = false;
				ReleaseCapture();
				chbxInvalidate();

				if (isHovered)
				{
					//SendMessage(GetParent(m_hwnd), WM_COMMAND, MAKEWPARAM(id_, BN_CLICKED), (LPARAM)this->m_hwnd);

					isSelected = !isSelected;
				}
			}
			return 0;
		}
		}
		return D2DWindow::HandleMessage(uMsg, wParam, lParam);
	}

	PCWSTR  ClassName() const override { return L"MainCheckBoxClass"; }

	BOOL Move(int x, int y) override
	{
		BOOL res = TRUE;
		posX_ = x;
		posY_ = y;

		res = SetWindowPos(
			this->m_hwnd,
			NULL,
			posX_,
			posY_,
			0,
			0,
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE // No child reorder, no resize, no parent window activation
		);

		res = chbxInvalidate();

		return res;
	}

	int GetWidth() const override { return width_; }
	int GetHeight() const override { return height_; }

protected:
	CComPtr<ID2D1BitmapBrush> pBaseNoiseBrush;
	CComPtr<ID2D1BitmapBrush> pHoverNoiseBrush;
	CComPtr<ID2D1SolidColorBrush> pBorderBrush;
	CComPtr<ID2D1SolidColorBrush> pSelBrush;

	void CreateDeviceDepRes(HRESULT& hr)
	{
		const UINT width = 32;
		const UINT height = 32;
		std::vector<UINT32> pixelData(width * height);
		m_random<int> dist(12345, 0, 10);

		for (int i = 0; i < 2; ++i) // 0 - base, 1 - hover
		{
			if (i == 0)
			{
				for (UINT32 &px : pixelData)
				{
					px = dist() > 8 ? thm::chbxBaseColorLight : thm::chbxBaseColorDark;
				}
			}
			else
			{
				for (UINT32 &px : pixelData)
				{
					px = dist() > 8 ? thm::chbxHoverColorLight : thm::chbxHoverColorDark;
				}
			}

			CComPtr<ID2D1Bitmap> pBitmap;
			D2D1_BITMAP_PROPERTIES props;
			props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
			props.dpiX = 96.0f;
			props.dpiY = 96.0f;


			if (this->pRenderTarget)
			{
				this->pRenderTarget->CreateBitmap(D2D1::SizeU(width, height), pixelData.data(), width * 4, &props, &pBitmap);

				if (pBitmap)
				{
					D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
						D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
					this->pRenderTarget->CreateBitmapBrush(pBitmap, brushProps, &(i == 0 ? pBaseNoiseBrush : pHoverNoiseBrush));
				}
			}
		}

		if (this->pRenderTarget)
		{
			this->pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(thm::chbxBorderColor), &pBorderBrush);
			this->pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(thm::chbxSelColor), &pSelBrush);
		}
	}

	void DrawContent()
	{
		D2D1_SIZE_F size = this->pRenderTarget->GetSize();
		D2D1_RECT_F rect = D2D1::RectF(0, 0, size.width, size.height);

		if (pBaseNoiseBrush && pHoverNoiseBrush)
		{
			this->pRenderTarget->FillRectangle(rect, isHovered ? pHoverNoiseBrush : pBaseNoiseBrush);
		}

		if (pBorderBrush)
		{
			this->pRenderTarget->DrawRectangle(rect, pBorderBrush, thm::chbxBorderWidth);
		}
		
		if (isSelected && pSelBrush)
		{
			float paddingX = size.width * 0.3;
			float paddingY = size.height * 0.3;
			D2D1_RECT_F rectSel = D2D1::RectF(paddingX, paddingY, size.width - paddingX, size.height - paddingY);

			this->pRenderTarget->FillRectangle(rectSel, pSelBrush);
		}
	}
};