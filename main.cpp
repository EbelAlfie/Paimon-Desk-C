#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include "./agent/Entity.cpp"

struct Component{
    const TCHAR* name = _T("Paimon") ;
    const wchar_t* rightIdleBody = L"./entity/paimonRight.gif" ;
    const wchar_t* leftIdleBody = L"./entity/paimonLeft.gif" ;
    int frameIdle = 90 ;
};

LRESULT CALLBACK WindowsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    MSG msg;
    Component paimon;
    Entity creature = Entity(
        paimon.name, 
        paimon.rightIdleBody, 
        paimon.leftIdleBody, 
        paimon.frameIdle,
        hThisInstance) ;

    creature.lpfnWndProc = WindowsProc ;

    if (!RegisterClassEx (&creature))
        return 0;
    
    creature.setHandle() ;
    
    SetLayeredWindowAttributes(
        creature.hWindow,
        RGB(0, 255, 0),
        0,
        LWA_COLORKEY
    ) ;

    SetWindowPos(creature.hWindow, 
                    HWND_TOPMOST, 
                    0, 0, 0, 0, 
                    SWP_NOMOVE | SWP_NOSIZE);

    ShowWindow (creature.hWindow, nCmdShow);

    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

LRESULT CALLBACK WindowsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    Entity* pEntity = reinterpret_cast<Entity*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (pEntity) {
        return pEntity->onAlive(hwnd, message, wParam, lParam);
    }
    return DefWindowProc(hwnd, message, wParam, lParam);
}
