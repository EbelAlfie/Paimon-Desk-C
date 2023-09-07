#include <Windows.h>
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

class Entity: public WNDCLASSEX {
    public:
        typedef WNDCLASSEX super;

        HWND hWindow;
        GdiplusStartupInput gdiplusStartupInput;
        ULONG_PTR gdiplusToken;  
        const wchar_t* rightBodyFile;
        const wchar_t* leftBodyFile;
        int frame;

        Entity(
            const TCHAR className[], 
            const wchar_t* rightIdleBody,
            const wchar_t* leftIdleBody,
            int frame,
            HINSTANCE hInstance): WNDCLASSEX()
        {
            this->lpszClassName = className ;
            this->hInstance = hInstance ;
            this->lpfnWndProc = WindowsProc ;
            /* Use default icon and mouse-pointer */
            this->hIcon = LoadIcon (NULL, IDI_APPLICATION);
            this->hIconSm = LoadIcon (NULL, IDI_APPLICATION);
            this->hCursor = LoadCursor (NULL, IDC_ARROW);
            this->lpszMenuName = NULL;                 /* No menu */
            this->cbClsExtra = 0;                      /* No extra bytes after the window class */
            this->cbWndExtra = 0;                      /* structure or the window instance */
            /* Use Windows's default colour as the background of the window */
            this->style = CS_DBLCLKS;                 /* Catch double-clicks */
            this->cbSize = sizeof (WNDCLASSEX);
            this->hbrBackground = (HBRUSH) TRANSPARENT;

            this->frame = frame; 
            this->rightBodyFile = rightIdleBody ;
            this->leftBodyFile = leftIdleBody ;
        }

        void setHandle() {
            this->hWindow = createHandle();
            if (this->hWindow != NULL) 
                SetWindowLongPtr(this->hWindow, GWLP_USERDATA, (LONG_PTR)this);
        }

        HWND createHandle() {
            return CreateWindowEx(
                0,
                this->lpszClassName,
                this->lpszClassName,
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, 180, 180,
                NULL,
                NULL,
                this->hInstance, 
                NULL);
        }
        
        bool materializeEntity() {
            GdiplusStartup(
                &this->gdiplusToken, 
                &this->gdiplusStartupInput, 
                NULL);
            return true;   
        }

        void animateEntity(HDC hdc) { 
            Image current = loadImage(this->rightBodyFile);
        
            Graphics graphic(hdc);  
            //current.SelectActiveFrame(*dimensionID, 90) ;
            graphic.DrawImage(&current, 0, 0); 
        }

        Image loadImage(const wchar_t* pImageFile) {
            return Image(pImageFile) ;
        }
        
        void moveEntity() { 

        }
        void disMaterializeEntity(){ 
            GdiplusShutdown(this->gdiplusToken);
        }

        LRESULT onAlive (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            switch (message)                  
            {
                case WM_DESTROY:
                    PostQuitMessage(0);     
                    break;
                case WM_PAINT:
                    PAINTSTRUCT ps;
                    HDC hdc = BeginPaint(hwnd, &ps);
                    animateEntity(hdc) ;
                    EndPaint(hwnd, &ps);
                    break;            
            }
            return DefWindowProc (hwnd, message, wParam, lParam);
        }

        static LRESULT CALLBACK WindowsProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
            Entity* pEntity = reinterpret_cast<Entity*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            if (pEntity) {
                return pEntity->onAlive(hwnd, message, wParam, lParam);
            }
            return DefWindowProc(hwnd, message, wParam, lParam);
        }
};