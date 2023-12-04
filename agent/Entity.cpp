#include <Windows.h>
#include <winuser.h>
#include "./Brain.cpp"
#include "./GifImage.cpp"
#include "./Canvas.cpp"

#pragma comment(lib, "user32.lib")

class Entity: public WNDCLASSEX {
    public:
        typedef WNDCLASSEX super;
        HWND hWindow;

        Brain* entityBrain;
        Canvas* windowCanvas; 

        GifImage* rightBody;
        GifImage* leftBody;

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
            this->hIcon = LoadIcon (NULL, _T("../res/icon.ico"));
            this->hCursor = LoadCursor (NULL, IDC_ARROW);                  
            this->style = CS_HREDRAW | CS_VREDRAW;                
            this->cbSize = sizeof (WNDCLASSEX);

            this->frame = frame; 
            this->rightBody = new GifImage(rightIdleBody) ;
            this->leftBody = new GifImage(leftIdleBody) ;
        }

        bool materializeEntity() {
            this->entityBrain = new Brain() ;
            
            HRESULT hr = CoInitializeEx(
                NULL, 
                COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
            );
            if (hr != S_OK) return false;

            windowCanvas = new Canvas(&this->hWindow) ;
            windowCanvas->initializeCanvas() ;
            this->rightBody->initilizeGif(); 
            this->leftBody->initilizeGif();

            return true;   
        }

        void setHandle() {
            this->hWindow = createHandle();
            if (this->hWindow == NULL) return ;
            SetWindowLongPtr(this->hWindow, GWLP_USERDATA, (LONG_PTR)this);
        }

        HWND createHandle() {
            return CreateWindowEx (
                WS_EX_NOREDIRECTIONBITMAP, //expmn 0x00200000L
                this->lpszClassName,
                this->lpszClassName,
                WS_VISIBLE,
                300, 200, 200, 200,//150, 150,
                nullptr,
                nullptr,
                this->hInstance, 
                nullptr
            );
        }

        RECT getSelfBody(HWND hwnd) {
            RECT body ;
            GetWindowRect(hwnd, &body) ;
            return body ;
        }

        bool animateEntity(HWND hwnd) { 
            windowCanvas->drawEntity() ;
            //return (hr != S_OK)? true : false;
            return true ;
        }
        
        void moveEntity(HWND hwnd) { 
            POINT targ = entityBrain->getTargetPosition(hwnd, 1);
            RECT bodyPos = entityBrain->getBodyPosition(hwnd) ;
            POINT moveTo = entityBrain->calculatePosition(targ, bodyPos); 
            MoveWindow(hwnd, moveTo.x, moveTo.y, 150, 150, TRUE);
        }

        void disMaterializeEntity(){ 
            
        }

        void setTimer(HWND hwnd, UINT timerId, UINT value) {
            SetTimer(hwnd, timerId, value, NULL);
        }

        LRESULT onAlive (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)                  
            {
                case WM_NCHITTEST: {
                    LRESULT hit = DefWindowProc(hwnd, message, wParam, lParam);
                    if (hit == HTCLIENT) hit = HTCAPTION;
                    return hit;
                }
                case WM_DESTROY:
                    PostQuitMessage(0);   
                    disMaterializeEntity();   
                    return 0;
                case WM_PAINT:
                    animateEntity(hwnd) ;
                    //moveEntity(hwnd) ;
                    break ; //return 0; is bad     
            }
            return DefWindowProc (hwnd, message, wParam, lParam);
        }
};