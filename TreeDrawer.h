#include "ScrollWindowBase.h"
#include "List.h"
#include "Pair.h"
#include "MapleTree.h"
#pragma once

template<typename KeyType, typename ValueType>
class TreeDrawer : public ScrollWindowBase<TreeDrawer<KeyType, ValueType>>
{
	struct Edge
	{
		int pointX1, pointY1;
		int pointX2, pointY2;
	};

	struct NodeVisual
	{
		std::wstring text;
		int pointX, pointY;
	};

	using NodesVisual = MapleTree<KeyType, NodeVisual>;
	NodesVisual nodesVisual_;

	using Edges = List<Edge>;
	Edges edges_;

	MapleTree<KeyType, ValueType>* tree_;

	int nodeDia_;
	int margin_;
	int hStep_;
	int vStep_;

	void SetNodeProperties()
	{
		nodeDia_ = 40;
		margin_  = 50;
		hStep_   = nodeDia_ + 20; // Horizontal gap
		vStep_   = nodeDia_ + 20; // Vertical gap
	}

public:
	TreeDrawer(
		int id,
		MapleTree<KeyType, ValueType> &tree
	) :
		ScrollWindowBase<TreeDrawer<KeyType, ValueType>>(id),
		tree_(&tree)
	{
	}

	TreeDrawer(
		int id,
		int width,
		int height,
		int x,
		int y,
		int horizontalLineSize,
		int verticalLineSize,
		MapleTree<KeyType, ValueType> &tree,
		HWND parent
	) :
		ScrollWindowBase<TreeDrawer<KeyType, ValueType>>(
			id,
			width,
			height,
			x,
			y,
			horizontalLineSize,
			verticalLineSize,
			parent
		),
		tree_(&tree)
	{
	}

	void UpdateRenderData()
	{
		nodesVisual_.clear();
		edges_.clear();
		auto lambdaAddNode = [](KeyType key, ValueType value, NodesVisual &nodesVisual, int &xOffset) -> void
			{
				//            5          
				//           / \         
				//         3     7       
				//        / \   / \      
				//      -1   4 6   8     
				//      0 1 2 3 4 5 6    
				NodeVisual node{};
				node.text = std::to_wstring(key);
				node.pointX = xOffset++;
				node.pointY = 0;
				nodesVisual[key] = std::move(node);
			};

		struct LinksGather
		{
			NodesVisual &nodesVisual;
			Edges &edges;
			int &maxDepth;
		};
		auto lambdaMakeLookUp = [](KeyType key, ValueType value, const KeyType* parentKey, LinksGather &linksGather)
			{
				//            5        0 
				//           / \         
				//         3     7     1 
				//        / \   / \      
				//      -1   4 6   8   2 
				NodeVisual &nodeToSetY = linksGather.nodesVisual[key];
				int depth = parentKey ? linksGather.nodesVisual[*parentKey].pointY + 1 : 0; // 0 for root; else increment depth

				nodeToSetY.pointY = depth;

				int &maxDepth = linksGather.maxDepth;
				maxDepth = depth > maxDepth ? depth : maxDepth;                // Changing maxDepth

				if (parentKey != nullptr)
				{
					auto &nodesV = linksGather.nodesVisual;

					auto &node = nodesV[key];              // Node to draw line from
					auto &parentNode = nodesV[*parentKey]; // Node to draw line to

					Edge edge{};
					edge.pointX1 = parentNode.pointX;
					edge.pointY1 = parentNode.pointY;
					edge.pointX2 = node.pointX;
					edge.pointY2 = node.pointY;

					auto &edges = linksGather.edges;
					edges.push_back(edge);
				}
			};

		int xOffset = 0;
		tree_->ForEach(lambdaAddNode, nodesVisual_, xOffset);

		int maxDepth = 0;
		LinksGather linksGather{ nodesVisual_, edges_, maxDepth };
		tree_->ForEachStructural(lambdaMakeLookUp, linksGather);

		int virtualW = margin_ * 2 + (xOffset > 0 ? xOffset - 1 : 0) * hStep_ + nodeDia_;
		//int virtualW = margin_ * 2 + xOffset * hStep_ + nodeDia_;
		int virtualH = margin_ * 2 + maxDepth * vStep_ + nodeDia_;

		this->SetVirtualSize(virtualW, virtualH);
	}

	void Create()
	{
		ScrollWindowBase<TreeDrawer<KeyType, ValueType>>::Create();

		SetNodeProperties();
	}

private:
	CComPtr<ID2D1BitmapBrush> pNodeNoiseBrush;

	CComPtr<ID2D1SolidColorBrush> pLinkColorBrush;
	CComPtr<ID2D1SolidColorBrush> pRingColorBrush;

	CComPtr<IDWriteTextFormat> pSignTextFormat;
	CComPtr<ID2D1SolidColorBrush> pSignColorBrush;

	void CreateDeviceDepRes(HRESULT &hr)
	{
		ScrollWindowBase<TreeDrawer<KeyType, ValueType>>::CreateDeviceDepRes(hr);
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
				px = dist() > 8 ? thm::trdwNodeColorDark : thm::trdwNodeColorLight;
			}
			
		if (this->pRenderTarget)
		{
			this->pRenderTarget->CreateBitmap(D2D1::SizeU(width, height), pixelData.data(), width * 4, &props, &pBitmap);

			if (pBitmap)
			{
				D2D1_BITMAP_BRUSH_PROPERTIES brushProps = D2D1::BitmapBrushProperties(
					D2D1_EXTEND_MODE_WRAP, D2D1_EXTEND_MODE_WRAP, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
				this->pRenderTarget->CreateBitmapBrush(pBitmap, brushProps, &(pNodeNoiseBrush));
			}
		}



		if (SUCCEEDED(hr) && !pSignTextFormat && this->pDWriteFactory)
		{
			hr = this->pDWriteFactory->CreateTextFormat(
				L"Segoe UI", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
				15.0f, L"ru-ru", &pSignTextFormat
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
				D2D1::ColorF(thm::trdwTextColor),
				&pSignColorBrush
			);
		}
		if (SUCCEEDED(hr) && !pLinkColorBrush)
		{
			hr = this->pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(thm::trdwLinkColor),
				&pLinkColorBrush
			);
		}
		if (SUCCEEDED(hr) && !pRingColorBrush)
		{
			hr = this->pRenderTarget->CreateSolidColorBrush(
				D2D1::ColorF(thm::trdwRingColor),
				&pRingColorBrush
			);
		}
	}

	void DrawOnScroll()
	{
		float nodeRadi = static_cast<float>(nodeDia_) / 2;
		for (auto &edge : edges_)
		{
			float x1 = static_cast<float>(margin_ + edge.pointX1 * hStep_);
			float y1 = static_cast<float>(margin_ + edge.pointY1 * vStep_);
			float x2 = static_cast<float>(margin_ + edge.pointX2 * hStep_);
			float y2 = static_cast<float>(margin_ + edge.pointY2 * vStep_);

			this->pRenderTarget->DrawLine(
				D2D1::Point2F(x1, y1),
				D2D1::Point2F(x2, y2),
				pLinkColorBrush,
				thm::trdwLinkWidth
				);
		}

		for (auto& [key, value] : nodesVisual_)
		{
			float x = static_cast<float>(margin_ + value.pointX * hStep_);
			float y = static_cast<float>(margin_ + value.pointY * vStep_);

			D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(x, y), nodeRadi, nodeRadi);

			this->pRenderTarget->FillEllipse(ellipse, pNodeNoiseBrush);
			this->pRenderTarget->DrawEllipse(ellipse, pRingColorBrush, thm::trdwRingWidth);

			float squareEdge = 0.707107 * nodeDia_; // 1 / sqrt(2)
			float rectX1 = x - squareEdge / 2;
			float rectY1 = y - squareEdge / 2;
			float rectX2 = x + squareEdge / 2;
			float rectY2 = y + squareEdge / 2;
			D2D1_RECT_F textRect = D2D1::RectF(rectX1, rectY1, rectX2, rectY2);

			std::wstring nodeText = std::to_wstring(key);
			this->pRenderTarget->DrawText(
				nodeText.c_str(),
				nodeText.length(),
				pSignTextFormat,
				textRect,
				pSignColorBrush
			);
		}
	}
};