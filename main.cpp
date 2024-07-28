#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include "./agent/Entity.cpp"
#pragma comment(lib, "user32.lib")

struct Component{
    const TCHAR* name = _T("Paimon") ;
    const wchar_t* rightIdleBody = L"./entity/paimonRight.gif" ;
    const wchar_t* leftIdleBody = L"./entity/paimonLeft.gif" ;
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
        hThisInstance) ;

    creature.lpfnWndProc = WindowsProc ;

    if (!RegisterClassEx (&creature))
        return 0;
    
    creature.setHandle() ;

    creature.materializeEntity() ;

    SetWindowPos (creature.hWindow, 
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
    if (pEntity)
        return pEntity->onAlive(hwnd, message, wParam, lParam);
    return DefWindowProc(hwnd, message, wParam, lParam);
}
