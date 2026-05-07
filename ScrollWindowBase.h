//
// ScrollWindowBase.h
//
// Name conflict with ScrollWindow
#include "D2DWindow.h"
#include "IControl.h"
#include "m_random.h"
#include "List.h"
#include <string>
#include <vector>
#pragma once

template<class DERIVED_TYPE>
class ScrollWindowBase : public D2DWindow<DERIVED_TYPE>, public IControl
{
	int posX_;
	int posY_;
	int width_;
	int height_;

	int scrollX_;
	int scrollY_;
	int virtualW_;
	int virtualH_;

	int lineXSize_;
	int lineYSize_;

	HWND hWndParent_;
	int id_;

	void UpdateScrollBars()
	{
		RECT rc;
		GetClientRect(this->m_hwnd, &rc);
		int clientW = this->dpiS.Unscale(rc.right - rc.left); // DIPs
		int clientH = this->dpiS.Unscale(rc.bottom - rc.top); // DIPs

		SCROLLINFO si = {};
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_DISABLENOSCROLL; // 1-3 enables structure fields. 4th enables gray inactive mode

		// Horizontal
		si.nMin = 0;
		si.nMax = virtualW_;
		si.nPage = clientW; // Translating sizes for proportional slider
		si.nPos = scrollX_;
		SetScrollInfo(this->m_hwnd, SB_HORZ, &si, TRUE);

		// Vertical
		si.nMax = virtualH_;
		si.nPage = clientH;
		si.nPos = scrollY_;
		SetScrollInfo(this->m_hwnd, SB_VERT, &si, TRUE);
	}

	int CalcNewScrollPos(HWND hwnd, int barDirection, int currentPos, WPARAM wParam)
	{
		SCROLLINFO si = {};
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		GetScrollInfo(hwnd, barDirection, &si);

		int newPos = currentPos;
		int maxPos = si.nMax - (int)si.nPage;

		int lineDelta = barDirection == SB_HORZ ? lineXSize_ : lineYSize_;

		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			newPos -= lineDelta;
			break;

		case SB_LINEDOWN:
			newPos += lineDelta;
			break;

		case SB_PAGEUP:
			newPos -= si.nPage;
			break;
		
		case SB_PAGEDOWN:
			newPos += si.nPage;
			break;

		case SB_THUMBTRACK:
			newPos = si.nTrackPos;
			break;

		case SB_THUMBPOSITION:
			newPos = si.nTrackPos;
			break;

		case SB_TOP:
			newPos = 0;
			break;

		case SB_BOTTOM:
			newPos = maxPos;
			break;
		}

		if (newPos < 0)      newPos = 0;
		if (newPos > maxPos) newPos = maxPos;

		return newPos;
	}

public:
	ScrollWindowBase(int id) :
		id_(id),
		height_(0), width_(0), posX_(0), posY_(0),
		hWndParent_(NULL),
		scrollX_(0), scrollY_(0),
		virtualW_(0), virtualH_(0),
		lineXSize_(20), lineYSize_(20)
	{}
	ScrollWindowBase(
		int id,
		int width,
		int height,
		int x,
		int y,
		int horizontalLineSize,
		int verticalLineSize,
		HWND parent
	) :
		id_(id),
		width_(width), height_(height), posX_(x), posY_(y),
		hWndParent_(parent),
		scrollX_(0), scrollY_(0),
		virtualW_(0), virtualH_(0),
		lineXSize_(horizontalLineSize), lineYSize_(verticalLineSize)
	{}

	void SetProps(HWND hWndParent, std::wstring sign, int width, int height, int x, int y)
	{
		posX_ = x;
		posY_ = y;
		width_ = width;
		height_ = height;
		hWndParent_ = hWndParent;
	}

	void Create()
	{
		BaseWindow<DERIVED_TYPE>::Create(
			L"ScrollWindow",
			WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL,
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
		case WM_SIZE:
		{
			RECT rc;
			GetClientRect(this->m_hwnd, &rc);
			int clientW = this->dpiS.Unscale(rc.right - rc.left); // DIPs
			int clientH = this->dpiS.Unscale(rc.bottom - rc.top); // DIPs
			int maxX = max(0, virtualW_ - clientW);
			int maxY = max(0, virtualH_ - clientH);
			if (scrollX_ > maxX) scrollX_ = maxX;
			if (scrollY_ > maxY) scrollY_ = maxY;
			UpdateScrollBars();
			break;
		}

		case WM_HSCROLL:
		{
			scrollX_ = CalcNewScrollPos(this->m_hwnd, SB_HORZ, scrollX_, wParam);
			SCROLLINFO si = {};
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS;
			si.nPos = scrollX_;
			SetScrollInfo(this->m_hwnd, SB_HORZ, &si, TRUE);
			InvalidateRect(this->m_hwnd, NULL, FALSE);
			return 0;
		}

		case WM_VSCROLL:
		{
			scrollY_ = CalcNewScrollPos(this->m_hwnd, SB_VERT, scrollY_, wParam);
			SCROLLINFO si = {};
			si.cbSize = sizeof(SCROLLINFO);
			si.fMask = SIF_POS;
			si.nPos = scrollY_;
			SetScrollInfo(this->m_hwnd, SB_VERT, &si, TRUE);
			InvalidateRect(this->m_hwnd, NULL, FALSE);
			return 0;
		}
		}
		return D2DWindow<DERIVED_TYPE>::HandleMessage(uMsg, wParam, lParam);
	}

	PCWSTR  ClassName() const override { return L"ScrollWindowClass"; }

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

		res = InvalidateRect(this->m_hwnd, NULL, FALSE);

		return res;
	}

	int GetWidth() const override { return width_; }
	int GetHeight() const override { return height_; }

protected:
	virtual void DrawOnScroll() = 0;

	CComPtr<ID2D1BitmapBrush> pBaseNoiseBrush;

	// Values in pixels
	void SetVirtualSize(int virtualW, int virtualH)
	{
		virtualW_ = virtualW;
		virtualH_ = virtualH;

		RECT rc;
		GetClientRect(this->m_hwnd, &rc);
		int clientW = this->dpiS.Unscale(rc.right - rc.left); // DIPs
		int clientH = this->dpiS.Unscale(rc.bottom - rc.top); // DIPs
		int maxX = max(0, virtualW_ - clientW);
		int maxY = max(0, virtualH_ - clientH);
		if (scrollX_ > maxX) scrollX_ = maxX;
		if (scrollY_ > maxY) scrollY_ = maxY;

		UpdateScrollBars();
		InvalidateRect(this->m_hwnd, NULL, FALSE);
	}

	void CreateDeviceDepRes(HRESULT &hr)
	{
		const UINT width = 32;
		const UINT height = 32;
		std::vector<UINT32> pixelData(width * height);
		m_random<int> dist(12345, 0, 10);

		CComPtr<ID2D1Bitmap> pBitmap;
		D2D1_BITMAP_PROPERTIES props;
		props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
		props.dpiX = 96.0f;
		props.dpiY = 96.0f;

		for (auto &px : pixelData)
		{
			px = dist() > 8 ? thm::trdwBaseColorDark : thm::trdwBaseColorLight;
		}

		if (this->pRenderTarget)
		{
			this->pRenderTarget->CreateBitmap(D2D1::SizeU(width, height), pixelData.data(), width * 4, &props, &pBitmap);

			if (pBitmap)
			{
				D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
					D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
				this->pRenderTarget->CreateBitmapBrush(pBitmap, brushProps, &(pBaseNoiseBrush));
			}
		}
	}

	void DrawContent() final
	{
		D2D1_SIZE_F size = this->pRenderTarget->GetSize();
		this->pRenderTarget->FillRectangle(
			D2D1::RectF(0, 0, size.width, size.height),
			this->pBaseNoiseBrush
		);

		this->pRenderTarget->SetTransform(
			D2D1::Matrix3x2F::Translation(
				static_cast<float>(-scrollX_),
				static_cast<float>(-scrollY_)
			)
		);

		DrawOnScroll();

		this->pRenderTarget->SetTransform(D2D1::Matrix3x2F::Identity());
	}
};
