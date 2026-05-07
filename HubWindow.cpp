//
// HubWindow.cpp
//
#include "HubWindow.h"


void HubWindow::RecalculateLayout()
{
    if (buttons.empty()) return;

    RECT rc;
    GetClientRect(m_hwnd, &rc);
    int winWidth = rc.right - rc.left;

    if (winWidth < btnWidth) return;

    int x = btnPadding; // Default left top corner placement
    int y = btnPadding;

    for (auto &btn : buttons)
    {
        if (x + btnWidth + btnPadding > winWidth) // Window shrinks, button no longer fits in row
        {
            x = btnPadding; // Column placement
            y += btnHeight + btnPadding;
        }

		btn->Move(x, y);

		x += btnWidth + btnPadding; // Else row placement. And if button don't fits next iteration will set "column placement"
    }
}



//void HubWindow::DrawContent()
//{
//	MainWindow::DrawContent();
//
//}



LRESULT HubWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
		CreateGraphicsResources();
        this->dpiS.Init(this->m_hwnd);
        btnHeight = S(btnHeight);
        btnWidth = S(btnWidth);
        btnPadding = S(btnPadding);

        LabDef labs[] = {
            { HUB_ID_BTN_LAB1, L"Рекурсия" },
            { HUB_ID_BTN_LAB2, L"Стек" },
            { HUB_ID_BTN_LAB3, L"Список" },
            { HUB_ID_BTN_LAB4, L"Сортировочная станция" },
            { HUB_ID_BTN_LAB5, L"Дерево" },
            { HUB_ID_BTN_LAB6, L"Лаборатоная" },
            { HUB_ID_BTN_LAB_TEST, L"Лабораторная тест" },
        };

		for (const auto &lab : labs) // Range-based for. labs __bound is known at compile time
        {
            auto btn = std::make_unique<MainButton>(
                lab.id,
                lab.name,
                btnWidth, btnHeight,
                0, 0,
                m_hwnd
			);

            btn->Create();
			buttons.push_back(std::move(btn)); // No longer need object in btn. And unique_ptr cannot be copied
        }
        return 0;
    }

    case WM_SIZE:
    {
        RecalculateLayout();
        break; // Cascading lower stream (D2DWindow handles resizing)
    }

    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        mmi->ptMinTrackSize.x = S(300);
        mmi->ptMinTrackSize.y = S(250);
        return 0;
    }

    case WM_COMMAND:
    {
        int id = LOWORD(wParam);
        switch (id)
        {
        case HUB_ID_BTN_LAB_TEST:
            LaunchLabInThread<LabTestWindow>();
            break;

        case HUB_ID_BTN_LAB1:
            LaunchLabInThread<Lab1Window>();
            break;

        case HUB_ID_BTN_LAB2:
            LaunchLabInThread<Lab2Window>();
            break;

        case HUB_ID_BTN_LAB3:
            LaunchLabInThread<Lab3Window>();
            break;

        case HUB_ID_BTN_LAB4:
            LaunchLabInThread<Lab4Window>();
            break;

        case HUB_ID_BTN_LAB5:
            LaunchLabInThread<Lab5Window>();
            break;
        }
        return 0;
    }
    }

    return MainWindow::HandleMessage(uMsg, wParam, lParam);
}