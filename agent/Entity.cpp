#include <Windows.h>
#include <winuser.h>
#include "./Brain.cpp"
#include <dxgi1_3.h>
#include <d3d11_2.h>
#include <d2d1_2.h>
#include <d2d1_2helper.h>
#include <dcomp.h>
#include <wrl.h>
using namespace Microsoft::WRL;
#pragma comment(lib, "dxgi")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d2d1")
#pragma comment(lib, "dcomp")
#pragma comment(lib, "ole32")
#pragma comment(lib, "user32.lib")

class Entity: public WNDCLASSEX {
    public:
        typedef WNDCLASSEX super;
        ComPtr<ID2D1DeviceContext> dc;
        ComPtr<IDXGISwapChain1> swapChain;
        ComPtr<ID2D1Bitmap1> bitmap;
        ComPtr<IDXGIDevice> dxgiDevice ;
        ComPtr<IDCompositionDevice> dcompDevice;

        Brain* entityBrain;
        HWND hWindow;

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

            this->frame = frame; 
            // this->rightBody = new Image(rightIdleBody) ;
            // this->leftBody = new Image(leftIdleBody) ;
        }

        bool materializeEntity() {
            this->entityBrain = new Brain() ;
            
            HRESULT hr = CoInitializeEx(
                NULL, 
                COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE
            );
            if (hr != S_OK) return false;
            
            if (initializeCanvas() != true) return false;

            return true;   
        }

        bool initializeCanvas() {
            UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
            D3D_FEATURE_LEVEL featureLevels[] =
            {
                D3D_FEATURE_LEVEL_11_1,
                D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1
            };


            ComPtr<ID3D11Device> d3dDevice;
            ComPtr<ID3D11DeviceContext> context;
            HRESULT hr = D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_HARDWARE,
                nullptr,
                creationFlags,
                featureLevels,
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,
                &d3dDevice,
                nullptr,
                &context
            );
            if (hr != S_OK) return false ;

            
            hr = d3dDevice.As(&this->dxgiDevice) ;
            if (hr != S_OK) return false ;

            ComPtr<IDXGIFactory2> dxFactory;
            hr = CreateDXGIFactory2(
                DXGI_CREATE_FACTORY_DEBUG,
                __uuidof(dxFactory),
                reinterpret_cast<void **>(dxFactory.GetAddressOf())
            );
            if (hr != S_OK) return false ;

            //debuging
            DXGI_SWAP_CHAIN_DESC1 description = {};
            description.Format           = DXGI_FORMAT_B8G8R8A8_UNORM;     
            description.BufferUsage      = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            description.SwapEffect       = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
            description.BufferCount      = 2;                              
            description.SampleDesc.Count = 1;                              
            description.AlphaMode        = DXGI_ALPHA_MODE_PREMULTIPLIED;

            RECT rect = {};
            GetClientRect(this->hWindow, &rect);
            description.Width  = rect.right - rect.left;  
            description.Height = rect.bottom - rect.top;

            hr = dxFactory->CreateSwapChainForComposition(
                    dxgiDevice.Get(),
                    &description,
                    nullptr, 
                    this->swapChain.GetAddressOf()
                );
            if (hr != S_OK) return false ;

            // Create a single-threaded Direct2D factory with debugging information
            ComPtr<ID2D1Factory2> d2Factory;
            D2D1_FACTORY_OPTIONS const options = { D2D1_DEBUG_LEVEL_INFORMATION };
            hr = D2D1CreateFactory(
                D2D1_FACTORY_TYPE_SINGLE_THREADED,
                options,
                d2Factory.GetAddressOf()
            );
            if (hr != S_OK) return false ;

            // Create the Direct2D device that links back to the Direct3D device
            ComPtr<ID2D1Device1> d2Device;
            hr = d2Factory->CreateDevice(
                dxgiDevice.Get(),
                d2Device.GetAddressOf()
            );
            if (hr != S_OK) return false ;

            // Create the Direct2D device context that is the actual render target
            // and exposes drawing commands
            hr = d2Device->CreateDeviceContext(
                D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                this->dc.GetAddressOf()
            );
            if (hr != S_OK) return false ;

            // Retrieve the swap chain's back buffer
            ComPtr<IDXGISurface2> surface;
            hr = this->swapChain->GetBuffer(
                0,
                __uuidof(surface),
                reinterpret_cast<void **>(surface.GetAddressOf())
            );
            if (hr != S_OK) return false ;

            // Create a Direct2D bitmap that points to the swap chain surface
            D2D1_BITMAP_PROPERTIES1 properties = {};
            properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
            properties.pixelFormat.format    = DXGI_FORMAT_B8G8R8A8_UNORM;
            properties.bitmapOptions         = D2D1_BITMAP_OPTIONS_TARGET |
                                            D2D1_BITMAP_OPTIONS_CANNOT_DRAW;

            hr = this->dc->CreateBitmapFromDxgiSurface(
                surface.Get(),
                properties,
                this->bitmap.GetAddressOf()
            );
            if (hr != S_OK) return false ;

            // Point the device context to the bitmap for rendering
            this->dc->SetTarget(this->bitmap.Get());

            hr = DCompositionCreateDevice(
                this->dxgiDevice.Get(),
                __uuidof(dcompDevice),
                reinterpret_cast<void **>(dcompDevice.GetAddressOf())
            );
            if (hr != S_OK) return false;

            ComPtr<IDCompositionTarget> target;
            hr = dcompDevice->CreateTargetForHwnd(
                this->hWindow,
                true, 
                target.GetAddressOf()
            );
            if (hr != S_OK) return false;

            ComPtr<IDCompositionVisual> visual;
            hr = dcompDevice->CreateVisual(visual.GetAddressOf());
            if (hr != S_OK) return false;

            hr = visual->SetContent(swapChain.Get());
            if (hr != S_OK) return false;

            hr = target->SetRoot(visual.Get()) ;
            if (hr != S_OK) return false;

            hr = dcompDevice->Commit() ;

            this->animateEntity(this->hWindow) ;
            return (hr == S_OK)? true : false ;
        }

        void setHandle() {
            this->hWindow = createHandle();
            if (this->hWindow == NULL) return ;
            SetWindowLongPtr(this->hWindow, GWLP_USERDATA, (LONG_PTR)this);
        }

        HWND createHandle() {
            return CreateWindowEx(
                WS_EX_NOREDIRECTIONBITMAP, //expmn 0x00200000L
                this->lpszClassName,
                this->lpszClassName,
                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,//150, 150,
                NULL,
                NULL,
                this->hInstance, 
                NULL
            );
        }

        RECT getSelfBody(HWND hwnd) {
            RECT body ;
            GetWindowRect(hwnd, &body) ;
            return body ;
        }

        bool animateEntity(HWND hwnd) {
            static int i = -1;
            i++;
            if (i >= 3) i = 0;
            float alpha[] = { 0.25f, 0.5f, 1.0f };
            // Draw something
            this->dc->BeginDraw();
            this->dc->Clear();
            ComPtr<ID2D1SolidColorBrush> brush;
            D2D1_COLOR_F const brushColor = D2D1::ColorF(0.18f,  // red
                                                        0.55f,  // green
                                                        0.34f,  // blue
                                                        alpha[i]); // alpha
            HRESULT hr = this->dc->CreateSolidColorBrush(
                brushColor,
                brush.GetAddressOf()
            );
            if (hr != S_OK) return false;

            D2D1_POINT_2F const ellipseCenter = D2D1::Point2F(150.0f,  // x
                                                            150.0f); // y
            D2D1_ELLIPSE const ellipse = D2D1::Ellipse(ellipseCenter,
                                                    100.0f,  // x radius
                                                    100.0f); // y radius
            this->dc->FillEllipse(ellipse, brush.Get());
            hr = this->dc->EndDraw();
            // Make the swap chain available to the composition engine
            hr = this->swapChain->Present(1, 0); 

            return (hr != S_OK)? true : false;
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
                case WM_MOUSEMOVE:
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    SendMessage(GetParent(hwnd), message, wParam, lParam);
                case WM_NCHITTEST: {
                    LRESULT hit = DefWindowProc(hwnd, message, wParam, lParam);
                    if (hit == HTCLIENT) hit = HTCAPTION;
                    return hit;
                }
                case WM_DESTROY:
                    PostQuitMessage(0);   
                    disMaterializeEntity();   
                    return 0;
                case WM_TIMER:
                    
                    return 0; 
                case WM_PAINT:
                    //PAINTSTRUCT ps;
                    //HDC hdc = BeginPaint(hwnd, &ps);
                    animateEntity(hwnd) ;
                    //moveEntity(hwnd) ;
                    //EndPaint(hwnd, &ps);
                    return 0;            
            }
            return DefWindowProc (hwnd, message, wParam, lParam);
        }
};