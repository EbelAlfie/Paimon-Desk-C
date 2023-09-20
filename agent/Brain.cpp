#include <Windows.h>
#include <math.h>

class Brain {
    public:
    Brain() { }

    void chooseAction() {

    }

    POINT getTargetPosition(int mode) {
        POINT lpPoint ;
        switch (mode) {
            case 1: 
                GetCursorPos(&lpPoint) ;
                break;
            case 2:
                break;
            default:
                break;
        }
        return lpPoint ;
    }

    RECT getBodyPosition(HWND hwnd) {
        RECT body; 
        GetWindowRect(hwnd, &body) ;
        MapWindowPoints(hwnd, GetParent(hwnd), (LPPOINT) &body, 2) ;
        return body ;
    }

    bool determineOrientation(HWND hwnd, int mode) {
        POINT target = getTargetPosition(mode) ;
        RECT self = getBodyPosition(hwnd);
        return true ; //sementara 
    }

    POINT calculatePosition(POINT target, RECT self) {
        //pivotnya top left (0,0)
        int movToX = 30 + target.x;
        int movToY = 30 + target.y;
        return { 
            (movToX >= 1980)? 1980 : movToX , 
            (movToY >= 1080)? 1080 : movToY  
            };
    }
};