//
// D2DWindow.h
//
#include "BaseWindow.h"
#include "DPIScale.h"
#include "Theme.h"
#include <atlbase.h> // For CComPtr
#include <WindowsX.h> // For GET_X_LPARAM macro
#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")
#include <dwrite.h>
#pragma comment(lib, "dwrite.lib")
#pragma once

#define S(DIPs) dpiS.Scale(DIPs)

template <class DERIVED_TYPE>
class D2DWindow : public BaseWindow<DERIVED_TYPE>
{
public:
    void ResizeRenderTarget()
    {
        if (pRenderTarget)
        {
            RECT rc;
            GetClientRect(this->m_hwnd, &rc);
            D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
            pRenderTarget->Resize(size);
        }
    }

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override
    {
        switch (uMsg)
        {
        case WM_CREATE:
            dpiS.Init(this->m_hwnd);
            break;

        case WM_PAINT:
            OnPaint();
            return 0;

        case WM_SIZE:
            ResizeRenderTarget();
            return 0;
        }
		return BaseWindow<DERIVED_TYPE>::HandleMessage(uMsg, wParam, lParam);
    }

protected:
    DpiScale dpiS;

	CComPtr<ID2D1Factory> pFactory;
	CComPtr<ID2D1HwndRenderTarget> pRenderTarget;

    CComPtr<IDWriteFactory> pDWriteFactory;
    CComPtr<IDWriteTextFormat> pTextFormat;
    CComPtr<ID2D1SolidColorBrush> pTextColorBrush;

    virtual void CreateDeviceDepRes(HRESULT& hr) = 0;
    virtual void DrawContent() = 0;

    HRESULT CreateGraphicsResources()
    {
        HRESULT hr = S_OK;

        if (!pFactory)
        {
            hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
            if (FAILED(hr)) return hr;
		}

        if (!pRenderTarget)
        {
            RECT rc;
            GetClientRect(this->m_hwnd, &rc);
            D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);

            hr = pFactory->CreateHwndRenderTarget(
                D2D1::RenderTargetProperties(),
                D2D1::HwndRenderTargetProperties(this->m_hwnd, size),
                &pRenderTarget
            );

            if (SUCCEEDED(hr) && !pTextColorBrush)
            {
                hr = this->pRenderTarget->CreateSolidColorBrush(
                    D2D1::ColorF(thm::bttnTextColor),
                    &pTextColorBrush
                );
            }
            if (SUCCEEDED(hr) && !pDWriteFactory)
            {
                hr = DWriteCreateFactory(
                    DWRITE_FACTORY_TYPE_SHARED,
                    __uuidof(IDWriteFactory),
                    reinterpret_cast<IUnknown**>(&pDWriteFactory) // Cannot use IID_PPV_ARGS cause of IUnknown**. Macro takes void**
                );
            }
            if (SUCCEEDED(hr) && !pTextFormat && pDWriteFactory)
            {
                hr = pDWriteFactory->CreateTextFormat(
                    L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                    14.0f, L"ru-ru", &pTextFormat
                );


                if (SUCCEEDED(hr))
                {
                    hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
                    hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
                }
            }

            CreateDeviceDepRes(hr);
        }
        return hr;
    }

    void OnPaint()
    {
        HRESULT hr = CreateGraphicsResources();
        if (SUCCEEDED(hr))
        {
            PAINTSTRUCT ps;
            BeginPaint(this->m_hwnd, &ps);

            if (pRenderTarget)
            {
                pRenderTarget->BeginDraw();

                DrawContent();

                hr = pRenderTarget->EndDraw();
            }
            EndPaint(this->m_hwnd, &ps);
        }
    }
};