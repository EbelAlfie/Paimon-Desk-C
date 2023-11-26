#include <wincodec.h>
#include <d2d1_2.h>
#include <tchar.h>
#pragma comment(lib, "d2d1")

class GifImage {
    private:
    IWICImagingFactory* pWICFactory;
    IWICBitmapDecoder* pDecoder;
    IWICBitmapFrameDecode* pFrame;
    
    public:
    GifImage(TCHAR* path) {
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_IWICImagingFactory,
            reinterpret_cast<void**>(&pWICFactory)
        );

    hr = pWICFactory->CreateDecoderFromFilename(
            L"your_gif_file.gif",
            nullptr,
            GENERIC_READ,
            WICDecodeMetadataCacheOnDemand,
            &pDecoder
        );

        ID2D1Bitmap* pBitmap;
        hr = pRenderTarget->CreateBitmapFromWicBitmap(
            pFrame,
            nullptr,
            &pBitmap
        );
    }
}; 