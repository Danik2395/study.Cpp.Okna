//
// MainWindow.h
//
#include "D2DWindow.h"
#include "m_random.h"
#include <vector>
#pragma once


template <class DERIVED_TYPE>
class MainWindow : public D2DWindow<DERIVED_TYPE>
{
public:
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override
    {
        switch (uMsg)
        {
        case WM_SETCURSOR:
        {
            int id = LOWORD(lParam);
            LPCTSTR cursor{};
            switch (id)
            {
            case HTCLIENT:
                cursor = IDC_ARROW;
                break;

            case HTTOP:
            case HTBOTTOM:
				cursor = IDC_SIZENS;
                break;

			case HTLEFT:
            case HTRIGHT:
                cursor = IDC_SIZEWE;
				break;

			case HTTOPLEFT:
            case HTBOTTOMRIGHT:
				cursor = IDC_SIZENWSE;
                break;

            case HTTOPRIGHT:
            case HTBOTTOMLEFT:
				cursor = IDC_SIZENESW;
                break;

			default: return DefWindowProc(this->m_hwnd, uMsg, wParam, lParam);
            }
            HCURSOR hCursor = LoadCursor(NULL, cursor);
            SetCursor(hCursor);
            return 0;
        }
        }
		return D2DWindow<DERIVED_TYPE>::HandleMessage(uMsg, wParam, lParam);
    }
protected:
    CComPtr<ID2D1BitmapBrush> pNoiseBrush;
    
    void CreateDeviceDepRes(HRESULT &hr)
    {
        const UINT width = 32;
        const UINT height = 32;
        std::vector<UINT32> pixelData(width * height);
        m_random<int> dist(12345, 0, 10);

        for (auto &px : pixelData)
        {
            px = dist() > 8 ? thm::wndBaseColorLight : thm::wndBaseColorDark;
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
                this->pRenderTarget->CreateBitmapBrush(pBitmap, brushProps, &pNoiseBrush);
            }
        }
    }

    void DrawContent()
    {
        if (pNoiseBrush)
        {
            D2D1_SIZE_F size = this->pRenderTarget->GetSize();
            this->pRenderTarget->FillRectangle(D2D1::RectF(0, 0, size.width, size.height), pNoiseBrush);
        }
    }
};