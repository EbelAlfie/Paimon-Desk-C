#include <Windows.h>
#include <wincodec.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")

class Entity: public WNDCLASSEX {
    public:
        typedef WNDCLASSEX super;

        HWND hWindow;
        GdiplusStartupInput gdiplusStartupInput = NULL;
        ULONG_PTR gdiplusToken;
        const wchar_t* rightBodyFile;
        const wchar_t* leftBodyFile;
        int frame;
        UINT move = 0; 
        Image* currentBody = nullptr ;
        Image* rightBody = nullptr ; 
        Image* leftBody = nullptr ;
        PropertyItem* frameDelay = nullptr ;
        UINT TIMER_ANIM = 1 ;

        Entity(
            const TCHAR className[], 
            const wchar_t* rightIdleBody,
            const wchar_t* leftIdleBody,
            int frame,
            HINSTANCE hInstance): WNDCLASSEX()
        {
            this->lpszClassName = className ;
            this->hInstance = hInstance ;
            /* Use default icon and mouse-pointer */
            this->hIcon = LoadIcon (NULL, IDI_APPLICATION);
            this->hIconSm = LoadIcon (NULL, IDI_APPLICATION);
            this->hCursor = LoadCursor (NULL, IDC_ARROW);
            this->lpszMenuName = NULL;                 /* No menu */
            this->cbClsExtra = 0;                      /* No extra bytes after the window class */
            this->cbWndExtra = 0;                      /* structure or the window instance */
            this->style = CS_DBLCLKS;                 /* Catch double-clicks */
            this->cbSize = sizeof (WNDCLASSEX);
            this->hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);

            materializeEntity();

            this->frame = frame; 
            this->rightBody = new Image(rightIdleBody) ;
            this->leftBody = new Image(leftIdleBody) ;
        }

        void setHandle() {
            this->hWindow = createHandle();
            if (this->hWindow == NULL) return ;
            SetWindowLongPtr(this->hWindow, GWLP_USERDATA, (LONG_PTR)this);
        }

        HWND createHandle() {
            return CreateWindowEx(
                0,
                this->lpszClassName,
                this->lpszClassName,
                WS_POPUPWINDOW | WS_EX_LAYERED ,
                CW_USEDEFAULT, CW_USEDEFAULT, 150, 150,
                NULL,
                NULL,
                this->hInstance, 
                NULL);
        }
        
        bool materializeEntity() {
            HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
            if (FAILED(hr)) return hr;
            GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
            return SUCCEEDED(hr);   
        }

        void GetFrameDelay(Image* image) {
            UINT count = image->GetFrameDimensionsCount();

            GUID* dimensionId =new GUID[count];
            image->GetFrameDimensionsList(dimensionId, count);
            
            WCHAR strGuid[39];
            StringFromGUID2(dimensionId[0], strGuid, 39);
            this->frame = image->GetFrameCount(&dimensionId[0]);

            UINT propSize = image->GetPropertyItemSize(PropertyTagFrameDelay);
            this->frameDelay = (PropertyItem*)malloc(propSize);
            image->GetPropertyItem(
                PropertyTagFrameDelay,
                propSize,
                this->frameDelay
            );
        }

        void animateEntity(HWND hwnd, HDC hdc) {
            this->currentBody = this->rightBody;
            POINT object = determineObjectPosition(1) ;
            Graphics graphics(hdc); 

            GetFrameDelay(this->currentBody) ;

            graphics.DrawImage(
                this->currentBody, 
                0, 0, 
                150,//this->currentBody->GetHeight(), 
                150//this->currentBody->GetWidth()
            ) ;
            setTimer(hwnd, 1, ((UINT*)this->frameDelay[0].value)[this->move] * 10);
        }

        POINT determineObjectPosition(int mode) {
            POINT lpPoint ;
            if (mode == 1) GetCursorPos(&lpPoint) ;
            return lpPoint ;
        }

        // POINT currentBodyPosition(HWND hwnd) {
        //     RECT body; 
        //     //GetWindowRect(hwnd, &body) ;
        // }
        
        void moveEntity(HWND hwnd) { 
            POINT obj = determineObjectPosition(1);
            //currentBodyPosition(hwnd) ;
            MoveWindow(hwnd, obj.x, obj.y, 150, 150, TRUE); 
        }

        void disMaterializeEntity(){ 
            GdiplusShutdown(this->gdiplusToken);
        }

        bool updateWindow(HWND hwnd, HDC hdc) {
            RECT currentWindow ;
            if (!GetWindowRect(hwnd, &currentWindow)) return false ;
            BLENDFUNCTION blend = { 0 };
            blend.BlendOp = AC_SRC_OVER;
            blend.SourceConstantAlpha = 255;
            blend.AlphaFormat = AC_SRC_ALPHA;

            SIZE sizeSplash = { this->currentBody->GetHeight(), 
                this->currentBody->GetWidth() };
            POINT ptSrc = { 0 };  
	        POINT ptWinSize = { currentWindow.left, currentWindow.top };
            HDC hdcScreen = GetDC(NULL);
            HDC hdcMem = CreateCompatibleDC(hdcScreen);

            UpdateLayeredWindow(hwnd, hdcScreen, &ptWinSize, &sizeSplash, 
            hdcMem, &ptSrc, RGB(0, 255, 0), &blend, ULW_COLORKEY);
            return true ;
        }

        void setTimer(HWND hwnd, UINT timerId, UINT value) {
            SetTimer(hwnd, timerId, value, NULL);
        }

        LRESULT onAlive (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)                  
            {
                case WM_DESTROY:
                    PostQuitMessage(0);   
                    disMaterializeEntity();   
                    return 0;
                case WM_TIMER:
                    KillTimer(hwnd, 1); 

                    this->currentBody->SelectActiveFrame(&FrameDimensionTime, this->move);
                    this->move = (this->move + 1) % 
                        this->currentBody->GetFrameCount(&FrameDimensionTime);
                    
                    if (!((UINT*)this->frameDelay[0].value)[this->move]) return 0 ;

                    setTimer(hwnd, 1, ((UINT*)this->frameDelay[0].value)[this->move] * 10);
                    //InvalidateRect(hwnd, NULL, FALSE);
                    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE); 
                    return 0; 
                case WM_PAINT:
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);
                    animateEntity(hwnd, hdc) ;
                    moveEntity(hwnd) ;
                    //updateWindow(hwnd, hdc) ;
                    EndPaint(hwnd, &ps);
                    return 0;            
            }
            return DefWindowProc (hwnd, message, wParam, lParam);
        }
};