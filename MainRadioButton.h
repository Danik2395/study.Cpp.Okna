//
// MainRadioButton.h
//
#include "D2DWindow.h"
#include "IControl.h"
#include "m_random.h"
#include <vector>
#include <string>
#pragma once

template<int GROUP_ID>
class MainRadioButton : public D2DWindow<MainRadioButton<GROUP_ID>>, public IControl
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

	inline static int selected_{ 0 };

	BOOL radiInvalidate() { return InvalidateRect(this->m_hwnd, NULL, FALSE); }

	void InvalidatePrevSel(int prevSel)
	{
		if (prevSel != 0 && prevSel != id_)
		{
			HWND hPrev = GetDlgItem(GetParent(this->m_hwnd), prevSel); // We can use it because every id was put into hMenu
			if (hPrev) InvalidateRect(hPrev, NULL, FALSE);
		}
		radiInvalidate();
	}

public:
	MainRadioButton(int id) :
		id_(id),
		name_(NULL),
		height_(0), width_(0), posX_(0), posY_(0),
		hWndParent_(NULL),
		isHovered(false),
		isPressed(false)
	{
	}
	MainRadioButton(
		int id,
		int width,
		int height,
		int x,
		int y,
		HWND parent,
		bool selDefault = false
	) :
		id_(id),
		name_(NULL),
		width_(width), height_(height), posX_(x), posY_(y),
		hWndParent_(parent),
		isHovered(false),
		isPressed(false)
	{
		if (selDefault)
		{
			selected_ = id_;
		}
	}

	int WhichSel() const { return selected_; }
	bool IsSelected() const { return selected_ == id_; }

	void SetSelected(int id)
	{
		int prevSelected = selected_;
		selected_ = id;
		InvalidatePrevSel(prevSelected);
	}

	void Create()
	{
		BaseWindow<MainRadioButton<GROUP_ID>>::Create(
			name_,
			WS_CHILD | WS_VISIBLE,
			0,
			posX_, posY_, width_, height_,
			hWndParent_,
			(HMENU)id_
		);

		if (this->m_hwnd)
		{
			HRGN hRgn = CreateEllipticRgn(-1, -1, width_ + 2, height_ + 2);
			SetWindowRgn(this->m_hwnd, hRgn, TRUE);
		}
	}

	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override
	{
		switch (uMsg)
		{
		case WM_ERASEBKGND:
			return 1;

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

				radiInvalidate();
			}
			else // Because when SetCapture() "set" it makes system not to send WM_MOUSELEAVE (you captured, you processing)
			{
				POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
				RECT rc; GetClientRect(this->m_hwnd, &rc);

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
			radiInvalidate();
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			isPressed = true;
			SetCapture(this->m_hwnd);
			radiInvalidate();
			return 0;
		}

		case WM_LBUTTONUP:
		{
			if (isPressed)
			{
				isPressed = false;
				ReleaseCapture();
				radiInvalidate();

				if (isHovered)
				{
					int prevSel = selected_;
					selected_ = id_;
					SendMessage(GetParent(this->m_hwnd), WM_COMMAND, MAKEWPARAM(id_, BN_CLICKED), (LPARAM)this->m_hwnd);

					InvalidatePrevSel(prevSel);
				}
			}
			return 0;
		}
		}
		return D2DWindow<MainRadioButton<GROUP_ID>>::HandleMessage(uMsg, wParam, lParam);
	}

	PCWSTR  ClassName() const override { return L"MainRadioButtonClass"; }

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

		res = radiInvalidate();

		return res;
	}

	int GetWidth() const override { return width_; }
	int GetHeight() const override { return height_; }

protected:
	CComPtr<ID2D1BitmapBrush> pBaseNoiseBrush;
	CComPtr<ID2D1BitmapBrush> pHoverNoiseBrush;
	CComPtr<ID2D1SolidColorBrush> pBorderBrush;
	CComPtr<ID2D1SolidColorBrush> pSelBrush;

	bool EnableAlpha() const override { return true; }

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
					px = dist() > 8 ? thm::selBaseColorLight : thm::selBaseColorDark;
				}
			}
			else
			{
				for (UINT32 &px : pixelData)
				{
					px = dist() > 8 ? thm::selHoverColorLight : thm::selHoverColorDark;
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
			this->pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(thm::selBorderColor), &pBorderBrush);
			this->pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(thm::selSelColor), &pSelBrush);
		}
	}

	void DrawContent()
	{
		D2D1_SIZE_F size = this->pRenderTarget->GetSize();

		const float x = size.width / 2;
		const float y = size.height / 2;
		const float radiX = x - thm::selBorderWidth / 2;
		const float radiY = y - thm::selBorderWidth / 2;

		D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), radiX, radiY);



		if (pBaseNoiseBrush && pHoverNoiseBrush)
		{
			this->pRenderTarget->FillEllipse(ellipse, isHovered ? pHoverNoiseBrush : pBaseNoiseBrush);
		}

		if (pBorderBrush)
		{
			this->pRenderTarget->DrawEllipse(ellipse, pBorderBrush, thm::selBorderWidth);
		}

		if (selected_ == id_ && pSelBrush)
		{
			ellipse.radiusX = radiX * 0.45;
			ellipse.radiusY = radiY * 0.45;

			this->pRenderTarget->FillEllipse(ellipse, pSelBrush);
		}
	}
};