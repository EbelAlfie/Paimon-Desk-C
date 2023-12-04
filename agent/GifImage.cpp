#include <wincodec.h>
#include <d2d1_2.h>
#include <tchar.h>
#include <wrl.h>
using namespace Microsoft::WRL;
#pragma comment(lib, "d2d1")

class GifImage {
    private:
    ComPtr<IWICImagingFactory> wicFactory;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICFormatConverter> formatConverter;

    const wchar_t* imgPath ;

    public:
    GifImage(const wchar_t* path) {

        imgPath = path ;
    }

    bool initilizeGif() {
        HRESULT hr = CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&wicFactory)
        );
        if (hr != S_OK) return nullptr ;

        hr = wicFactory->CreateDecoderFromFilename(
                this->imgPath,
                nullptr,
                GENERIC_READ,
                WICDecodeMetadataCacheOnLoad,
                &decoder
            );
        if (hr != S_OK) return nullptr ;

        hr = wicFactory->CreateFormatConverter(&formatConverter);
        if (hr != S_OK) return nullptr ;

        return hr == S_OK ;
    }

    ComPtr<ID2D1Bitmap> getBitmapFrameAt(
        int index,
        ComPtr<ID2D1DeviceContext> d2dContext
        ) {

        ComPtr<ID2D1Bitmap> frameBitmap ;

        HRESULT hr = decoder->GetFrame(index, &frame);
        if (hr != S_OK) return nullptr ;
        
        hr = formatConverter->Initialize(
            frame.Get(),
            GUID_WICPixelFormat32bppPBGRA,  
            WICBitmapDitherTypeNone,
            nullptr,
            0.0f,
            WICBitmapPaletteTypeCustom  
        );
        if (hr != S_OK) return nullptr ;

        hr = d2dContext->CreateBitmapFromWicBitmap(
            formatConverter.Get(),
            nullptr,
            &frameBitmap
        );

        return (hr == S_OK) ? frameBitmap : nullptr ;
    }
}; 