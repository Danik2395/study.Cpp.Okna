//
// PlotWindow.h
//
#include "D2DWindow.h"
#include "IControl.h"
#include <vector>
#include <string>
#include <cstdio>
#include <concepts>
#pragma once

template<typename FunctionType>
concept PlotFunc = requires(FunctionType function, double x)
{
	{ function(x) } -> std::same_as<double>;
};

template<typename FunctionType>
class PlotWindow : public D2DWindow<PlotWindow<FunctionType>>, public IControl
{
	int posX_;
	int posY_;
	int width_;
	int height_;

	HWND hWndParent_;
	int  id_;

	double xMin_;
	double xMax_;
	double yMin_;
	double yMax_;
	double xStep_;
	double yStep_;
	int    pointCount_;

	FunctionType func_;
	bool hasFunction_;

	static constexpr float margin_ = 40.0f;

	float PlotLeftStart()   const { return margin_; }
	float PlotRightStart()  const { return this->pRenderTarget->GetSize().width  - margin_; }
	float PlotTopStart()    const { return margin_; }
	float PlotBottomStart() const { return this->pRenderTarget->GetSize().height - margin_; }

	float ToScreenX(double x) const
	{
		return static_cast<float>(PlotLeftStart() + (x - xMin_) / (xMax_ - xMin_) * (PlotRightStart() - PlotLeftStart()));
	}

	float ToScreenY(double y) const
	{
		// Inversion of coordinates, conversion to DIPs.
		// (y - yMin_) / (yMax_ - yMin_) = range from minimal Y to wanted Y
		// () / () * (PlotBottomStart() - PlotTopStart()) = range on DIPs field
		// Take range on DIPs with -
		// PlotBottomStart() - range in DIPs = Y from top of the screen (regular winapi client coordinates)
		return static_cast<float>(PlotBottomStart() - (y - yMin_) / (yMax_ - yMin_) * (PlotBottomStart() - PlotTopStart()));
	}

	static std::wstring FormatLabel(double val)
	{
		wchar_t buf[32];
		swprintf_s(buf, L"%.4g", val);
		return std::wstring(buf);
	}

	BOOL pltInvalidate() { return InvalidateRect(this->m_hwnd, NULL, FALSE); }

public:
	PlotWindow(
		int id,
		int width,
		int height,
		int x,
		int y,
		HWND parent,
		int xMin = -10,
		int xMax = 10,
		int yMin = 10, 
		int yMax = 10,
		int xStep = 1,
		int yStep = 1
	) :
		id_(id),
        width_(width), height_(height), posX_(x), posY_(y),
        xMin_(xMin), xMax_(xMax),
        yMin_(yMin), yMax_(yMax),
        xStep_(xStep), yStep_(yStep),
        pointCount_(80),
		hasFunction_(false),
        hWndParent_(parent)
	{}

	void SetXRange(double xMin, double xMax)
	{
		if (xMin > xMax)
		{
			double xTemp = xMin;
			xMin = xMax;
			xMax = xTemp;
		}
		xMin_ = xMin;
		xMax_ = xMax;
		pltInvalidate();
	}

	void SetYRange(double yMin, double yMax)
	{
		if (yMin > yMax)
		{
			double yTemp = yMin;
			yMin = yMax;
			yMax = yTemp;
		}
		yMin_ = yMin;
		yMax_ = yMax;
		pltInvalidate();
	}

	void SetSteps(double xStep, double yStep)
	{
		xStep_ = xStep;
		yStep_ = yStep;
		pltInvalidate();
	}

	void SetPointCount(int pointCount)
	{
		pointCount_ = pointCount;
		pltInvalidate();
	}

	// plotFunction(double x) -> double
	template<typename FunctionT = FunctionType>
	requires PlotFunc<FunctionT>
	void SetFunction(FunctionT &plotFunction)
	{
		func_ = plotFunction;
		hasFunction_ = true;
		pltInvalidate();
	}

	void Create()
	{
		BaseWindow<PlotWindow<FunctionType>>::Create(
			L"PlotWindow",
			WS_VISIBLE | WS_CHILD,
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
		case WM_ERASEBKGND:
			return 1;
		}
		return D2DWindow<PlotWindow<FunctionType>>::HandleMessage(uMsg, wParam, lParam);
	}

	PCWSTR ClassName() const override { return L"PlotWindowClass"; }

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
			SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE
		);

		res = pltInvalidate();
		return res;
	}

	int GetWidth()  const override { return width_; }
	int GetHeight() const override { return height_; }

protected:
	CComPtr<ID2D1SolidColorBrush> pBgBrush;
	CComPtr<ID2D1SolidColorBrush> pGridBrush;
	CComPtr<ID2D1SolidColorBrush> pAxisBrush;
	CComPtr<ID2D1SolidColorBrush> pFuncBrush;
	CComPtr<ID2D1SolidColorBrush> pPointBrush;
	CComPtr<ID2D1SolidColorBrush> pLabelBrush;

	CComPtr<ID2D1StrokeStyle>     pDashStrokeStyle;

	CComPtr<IDWriteTextFormat>    pLabelTextFormat;

	void CreateDeviceDepRes(HRESULT &hr) override
	{
		if (this->pRenderTarget)
		{
			if (!pBgBrush)
				this->pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(thm::pltBgColor),    &pBgBrush);
			if (!pGridBrush)
				this->pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(thm::pltGridColor),  &pGridBrush);
			if (!pAxisBrush)
				this->pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(thm::pltAxisColor),  &pAxisBrush);
			if (!pFuncBrush)
				this->pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(thm::pltFuncColor),  &pFuncBrush);
			if (!pPointBrush)
				this->pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(thm::pltPointColor), &pPointBrush);
			if (!pLabelBrush)
				this->pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(thm::pltLabelColor), &pLabelBrush);
		}

		if (SUCCEEDED(hr) && !pDashStrokeStyle && this->pFactory)
		{
			D2D1_STROKE_STYLE_PROPERTIES strokeProps = {};
			strokeProps.dashStyle = D2D1_DASH_STYLE_DASH_DOT;
			hr = this->pFactory->CreateStrokeStyle(strokeProps, nullptr, 0, &pDashStrokeStyle);
		}

		if (SUCCEEDED(hr) && !pLabelTextFormat && this->pDWriteFactory)
		{
			hr = this->pDWriteFactory->CreateTextFormat(
				L"Segoe UI", NULL,
				DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
				10.0f, L"en-us", &pLabelTextFormat
			);

			if (SUCCEEDED(hr))
			{
				hr = pLabelTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
				hr = pLabelTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
			}
		}
	}

	void DrawContent() override
	{
		D2D1_SIZE_F size = this->pRenderTarget->GetSize();

		if (pBgBrush) this->pRenderTarget->FillRectangle(D2D1::RectF(0, 0, size.width, size.height), pBgBrush);

		DrawGrid();
		DrawAxes();
		DrawLabels();

		if (hasFunction_) DrawFunction();
	}

	void DrawGrid()
	{
		if (!pGridBrush || !pDashStrokeStyle) return;

		for (double x = xMin_; x <= xMax_ + xStep_ * 0.5; x += xStep_)
		{
			float currentX = ToScreenX(x);
			this->pRenderTarget->DrawLine(
				D2D1::Point2F(currentX, PlotTopStart()),
				D2D1::Point2F(currentX, PlotBottomStart()),
				pGridBrush, thm::pltGridLineWidth, pDashStrokeStyle
			);
		}

		for (double y = yMin_; y <= yMax_ + yStep_ * 0.5; y += yStep_)
		{
			float currentY = ToScreenY(y);
			this->pRenderTarget->DrawLine(
				D2D1::Point2F(PlotLeftStart(), currentY),
				D2D1::Point2F(PlotRightStart(), currentY),
				pGridBrush, thm::pltGridLineWidth, pDashStrokeStyle
			);
		}
	}

	void DrawAxes()
	{
		if (!pAxisBrush) return;

		// If ToScreen?(0.0) more than max field edge take max field edge as axis
		// If ToScreen?(0.0) less than min field edge take min field edge as axis
		float axisY = max(PlotTopStart(),  min(PlotBottomStart(), ToScreenY(0.0)));
		float axisX = max(PlotLeftStart(), min(PlotRightStart(),  ToScreenX(0.0)));

		this->pRenderTarget->DrawLine(
			D2D1::Point2F(PlotLeftStart(), axisY),
			D2D1::Point2F(PlotRightStart(), axisY),
			pAxisBrush, thm::pltAxisLineWidth
		);

		this->pRenderTarget->DrawLine(
			D2D1::Point2F(axisX, PlotTopStart()),
			D2D1::Point2F(axisX, PlotBottomStart()),
			pAxisBrush, thm::pltAxisLineWidth
		);

		// Serifs on X
		for (double x = xMin_; x <= xMax_ + xStep_ * 0.5; x += xStep_)
		{
			float currentX = ToScreenX(x);
			this->pRenderTarget->DrawLine(
				D2D1::Point2F(currentX, axisY - thm::pltTickSize),
				D2D1::Point2F(currentX, axisY + thm::pltTickSize),
				pAxisBrush, thm::pltAxisLineWidth
			);
		}

		// Serifs on Y
		for (double y = yMin_; y <= yMax_ + yStep_ * 0.5; y += yStep_)
		{
			float currentY = ToScreenY(y);
			this->pRenderTarget->DrawLine(
				D2D1::Point2F(axisX - thm::pltTickSize, currentY),
				D2D1::Point2F(axisX + thm::pltTickSize, currentY),
				pAxisBrush, thm::pltAxisLineWidth
			);
		}
	}

	void DrawLabels()
	{
		if (!pLabelTextFormat || !pLabelBrush) return;

		static constexpr float labelW = 44.0f;
		static constexpr float labelH = 14.0f;

		// Sings of Y
		for (double x = xMin_; x <= xMax_ + xStep_ * 0.5; x += xStep_)
		{
			float currentX = ToScreenX(x);
			std::wstring text = FormatLabel(x);

			this->pRenderTarget->DrawText(
				text.c_str(),
				static_cast<UINT32>(text.length()),
				pLabelTextFormat,
				D2D1::RectF(currentX - labelW / 2, PlotBottomStart() + 4.0f, currentX + labelW / 2, PlotBottomStart() + 4.0f + labelH),
				pLabelBrush
			);
		}

		// Sings of X
		for (double y = yMin_; y <= yMax_ + yStep_ * 0.5; y += yStep_)
		{
			float currentY = ToScreenY(y);
			std::wstring text = FormatLabel(y);

			this->pRenderTarget->DrawText(
				text.c_str(),
				static_cast<UINT32>(text.length()),
				pLabelTextFormat,
				D2D1::RectF(2.0f, currentY - labelH / 2, PlotLeftStart() - 4.0f, currentY + labelH / 2),
				pLabelBrush
			);
		}
	}

	void DrawFunction()
	{
		if (!pFuncBrush || !pPointBrush || !this->pFactory || pointCount_ < 2) return;

		double stepBetweenX = (xMax_ - xMin_) / (pointCount_ - 1);

		std::vector<D2D1_POINT_2F> points;
		points.reserve(pointCount_);

		int actualPoints{}; // To not to draw the line above or under plot field
		for (int i = 0; i < pointCount_; ++i)
		{
			double x = xMin_ + i * stepBetweenX;
			double y = func_(x);
			if (y > yMax_ || y < yMin_) continue;
			++actualPoints;
			points.push_back(D2D1::Point2F(ToScreenX(x), ToScreenY(y)));
		}

		CComPtr<ID2D1PathGeometry> pPath;
		if (FAILED(this->pFactory->CreatePathGeometry(&pPath))) return;

		CComPtr<ID2D1GeometrySink> pSink;
		if (FAILED(pPath->Open(&pSink))) return;

		pSink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_HOLLOW); // No fill inside

		for (int i = 1; i < actualPoints; ++i) pSink->AddLine(points[i]);

		pSink->EndFigure(D2D1_FIGURE_END_OPEN);                  // No ends linking -> just line
		pSink->Close();

		this->pRenderTarget->DrawGeometry(pPath, pFuncBrush, thm::pltFuncLineWidth);

		for (const auto &pt : points)
		{
			this->pRenderTarget->FillEllipse(
				D2D1::Ellipse(pt, thm::pltPointRadius, thm::pltPointRadius),
				pPointBrush
			);
		}
	}
};