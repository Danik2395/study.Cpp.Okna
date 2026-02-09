//
// ThreadLauncher.h
//
#pragma once
#include <thread>
//#include "LabWindow.h"

template <typename WindowClass>
void LaunchLabInThread()
{
    std::thread t([]() {
        WindowClass win;
        if (win.CheckInstanceCount()) return;

        if (!win.Create(
            win.Name(),
            (WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN) & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
            0,
			CW_USEDEFAULT, CW_USEDEFAULT, win.Width(), win.Height()
        ))
        {
            return;
        }

        ShowWindow(win.Window(), SW_SHOWNORMAL);

        MSG msg = { };
        while (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    });

    t.detach();
}