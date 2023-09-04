#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include "./agent/Entity.cpp"

struct Component{
    const wchar_t* name = L"Paimon" ;
    const char* rightIdleBody = "./entity/paimonRight.gif" ;
    const char* leftIdleBody = "./entity/paimonLeft.gif" ; 
    int frameIdle = 90 ;
};

//proto func
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow){
    Component paimon;
    Entity creature = Entity(paimon.name, hInstance);

    creature.lpfnWndProc = WindowProc ; 

    RegisterClass(&creature);

    ShowWindow(creature.materializeEntity(), nCmdShow);

    MSG msg = { };
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0 ;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch(uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
        return 0; 
        case WM_PAINT:
            PAINTSTRUCT ps ;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            EndPaint(hwnd, &ps);
        return 0 ; 
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}