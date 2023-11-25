#include <Windows.h>
#include <gdiplus.h>
#include "./Brain.cpp"
using namespace Gdiplus;
#pragma comment (lib, "Gdiplus.lib")
#pragma comment(lib, "user32.lib")

class Entity: public WNDCLASSEX {
    public:
        typedef WNDCLASSEX super;

        Brain* entityBrain;
        HWND hWindow;

        GdiplusStartupInput gdiplusStartupInput = NULL;
        ULONG_PTR gdiplusToken;
        Image* currentBody; 
        Image* rightBody; 
        Image* leftBody ;
        PropertyItem* frameDelay ;
        int frame;
        UINT move = 0; 
        
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
            this->hIcon = LoadIcon (NULL, IDI_APPLICATION);
            this->hIconSm = LoadIcon (NULL, IDI_APPLICATION);
            this->hCursor = LoadCursor (NULL, IDC_ARROW);
            this->lpszMenuName = NULL;                 
            this->cbClsExtra = 0;                      
            this->cbWndExtra = 0;                      
            this->style = CS_DBLCLKS;                
            this->cbSize = sizeof (WNDCLASSEX);
            this->hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);

            materializeEntity();

            this->frame = frame; 
            this->rightBody = new Image(rightIdleBody) ;
            this->leftBody = new Image(leftIdleBody) ;
        }

        bool materializeEntity() {
            this->entityBrain = new Brain() ;
            CoInitialize(nullptr) ;
            GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

            return true;   
        }

        void setHandle() {
            this->hWindow = createHandle();
            if (this->hWindow == NULL) return ;
            SetWindowLongPtr(this->hWindow, GWLP_USERDATA, (LONG_PTR)this);
        }

        HWND createHandle() {
            return CreateWindowEx(
                WS_EX_LAYERED | WS_EX_TRANSPARENT, //expmn 0x00200000L
                this->lpszClassName,
                this->lpszClassName,
                WS_POPUP,
                CW_USEDEFAULT, CW_USEDEFAULT, 150, 150,
                NULL,
                NULL,
                this->hInstance, 
                NULL);
        }

        RECT getSelfBody(HWND hwnd) {
            RECT body ;
            GetWindowRect(hwnd, &body) ;
            return body ;
        }

        void getFrameDelay(Image* image) {
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
            Graphics graphics(hdc); 

            getFrameDelay(this->currentBody) ;
            
            graphics.DrawImage(
                this->currentBody, 
                0, 0, 
                150, 150
            ) ;
            setTimer(hwnd, 1, ((UINT*)this->frameDelay[0].value)[this->move] * 10);
            ReleaseDC(hwnd, hdc);
        }
        
        void moveEntity(HWND hwnd) { 
            POINT targ = entityBrain->getTargetPosition(1);
            RECT bodyPos = entityBrain->getBodyPosition(hwnd) ;
            POINT moveTo = entityBrain->calculatePosition(targ, bodyPos); 
            MoveWindow(hwnd, moveTo.x, moveTo.y, 150, 150, TRUE);
        }

        void disMaterializeEntity(){ 
            GdiplusShutdown(this->gdiplusToken);
        }

        void setTimer(HWND hwnd, UINT timerId, UINT value) {
            SetTimer(hwnd, timerId, value, NULL);
        }

        LRESULT onAlive (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)                  
            {
                case WM_MOUSEMOVE:
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    SendMessage(GetParent(hwnd), message, wParam, lParam);
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

                    InvalidateRect(hwnd, NULL, TRUE)  ;
                    setTimer(hwnd, 1, ((UINT*)this->frameDelay[0].value)[this->move] * 10);
                    return 0; 
                case WM_PAINT:
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);
                    animateEntity(hwnd, hdc) ;
                    moveEntity(hwnd) ;
                    //updateWindow(hwnd, hdc) ;
                    EndPaint(hwnd, &ps);
                    break ; //do not use return 0            
            }
            return DefWindowProc (hwnd, message, wParam, lParam);
        }
};
