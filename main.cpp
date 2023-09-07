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

    creature.materializeEntity() ;

    if (!RegisterClassEx (&creature))
        return 0;
    
    creature.setHandle() ;

    ShowWindow (creature.hWindow, nCmdShow);

    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
