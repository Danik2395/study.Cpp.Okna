//
// MainButton.h
//
#include "D2DWindow.h"
#include "IControl.h"
#include "m_random.h"
#include <vector>
#include <string>
#pragma once

class MainButton : public D2DWindow<MainButton>, public IControl
{
	std::wstring sign_;
	size_t signLength;

	int posX_;
	int posY_;
	int width_;
	int height_;

	HWND hWndParent_;
	int id_;

	bool isHovered;
	bool isPressed;

	inline BOOL bttnInvalidate() { return InvalidateRect(this->m_hwnd, NULL, FALSE); }

	void SetSign(const std::wstring &newSign)
	{
		sign_.assign(newSign);
		signLength = sign_.length();
	}

public:
	MainButton(int id) :
		id_(id),
		height_(0), width_(0), posX_(0), posY_(0),
		hWndParent_(NULL),
		isHovered(false),
		isPressed(false)
	{
		SetSign(L"");
	}
	MainButton(
		int id,
		const std::wstring sign,
		int width,
		int height,
		int x,
		int y,
		HWND parent
	) :
		id_(id),
		width_(width), height_(height), posX_(x), posY_(y),
		hWndParent_(parent),
		isHovered(false),
		isPressed(false)
	{
		SetSign(sign);
	}

	void SetProps(HWND hWndParent, std::wstring sign, int width, int height, int x, int y)
	{
		SetSign(sign);
		posX_ = x;
		posY_ = y;
		width_ = width;
		height_ = height;
		hWndParent_ = hWndParent;
	}

	void Create()
	{
		BaseWindow::Create(
			sign_.c_str(),
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
				LPCTSTR cursor = IDC_HAND;
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

				bttnInvalidate();
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
			bttnInvalidate();
			return 0;
		}

		case WM_LBUTTONDOWN:
		{
			isPressed = true;
			SetCapture(this->m_hwnd);
			bttnInvalidate();
			return 0;
		}

		case WM_LBUTTONUP:
		{
			if (isPressed)
			{
				isPressed = false;
				ReleaseCapture();
				bttnInvalidate();

				if (isHovered)
				{
					SendMessage(GetParent(m_hwnd), WM_COMMAND, MAKEWPARAM(id_, BN_CLICKED), (LPARAM)this->m_hwnd);
				}
			}
			return 0;
		}	
		}
		return D2DWindow::HandleMessage(uMsg, wParam, lParam);
	}

	PCWSTR  ClassName() const override { return L"MainButtonClass"; }

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

		res = bttnInvalidate();

		return res;
	}

	int GetWidth() const override { return width_; }
	int GetHeight() const override { return height_; }

protected:
	CComPtr<ID2D1BitmapBrush> pBaseNoiseBrush;
	CComPtr<ID2D1BitmapBrush> pHoverNoiseBrush;

	CComPtr<IDWriteTextFormat> pSignTextFormat;
	CComPtr<ID2D1SolidColorBrush> pSignColorBrush;

	void CreateDeviceDepRes(HRESULT &hr)
	{
		const UINT width = 32;
		const UINT height = 32;
		std::vector<UINT32> pixelData(width * height);
		m_random<int> dist(12345, 0, 10);

		for (int i = 0; i < 2; ++i) // 0 - base, 1 - hover
		{
			if (i == 0)
			{
				for (auto &px : pixelData)
				{
					px = dist() > 8 ? thm::bttnBaseColorLight : thm::bttnBaseColorDark;
				}
			}
			else
			{
				for (auto &px : pixelData)
				{
					px = dist() > 8 ? thm::bttnHoverColorLight : thm::bttnHoverColorDark;
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

		if (SUCCEEDED(hr) && !pSignTextFormat && pDWriteFactory)
		{
			hr = pDWriteFactory->CreateTextFormat(
				L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
				14.0f, L"ru-ru", &pSignTextFormat
			);


			if (SUCCEEDED(hr))
			{
				hr = pSignTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				hr = pSignTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
		}
		if (SUCCEEDED(hr) && !pSignColorBrush)
		{
			hr = this->pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(thm::bttnTextColor),
				&pSignColorBrush
			);
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

		if (pSignTextFormat && pSignColorBrush && !sign_.empty())
		{
			this->pRenderTarget->DrawText(
				sign_.c_str(),
				static_cast<UINT32>(signLength),
				pSignTextFormat,
				rect,
				pSignColorBrush
			);
		}
	}
};