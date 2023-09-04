#include <windows.h>

class Entity: public WNDCLASS {
    public:
        typedef WNDCLASS super;
        Entity(const wchar_t className[], HINSTANCE hInstance): WNDCLASS() {
            this->lpszClassName = className ;
            this->hInstance = hInstance ;   
        }
        
        HWND materializeEntity() {
            return CreateWindowEx(
                0,
                this->lpszClassName,
                this->lpszClassName,
                WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                NULL,
                NULL,
                this->hInstance, 
                NULL
            );
        }

        void disMaterializeEntity(){

        }
        void animateEntity() { 

        }
        void moveEntity() { 

        }
};

