//
// main.cpp
//
//
#ifndef UNICODE
#define UNICODE
#endif 

#include "HubWindow.h"


int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    HubWindow win;
    if (!win.Create(
        win.Name(),
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, // Clip for not to clip the UI (children). Excludes area occupied by children windows from repaint
        0,
        CW_USEDEFAULT, CW_USEDEFAULT, win.Width(), win.Height()
    ))
    {
        return 0;
    }

    ShowWindow(win.Window(), nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}