#include <wincodec.h>
#include <d2d1_2.h>
#include <tchar.h>
#include <wrl.h>
using namespace Microsoft::WRL;
#pragma comment(lib, "d2d1")
#pragma once

class Gif {
    private:
    ComPtr<IWICImagingFactory> wicFactory;
    ComPtr<IWICBitmapDecoder> decoder;
    ComPtr<IWICBitmapFrameDecode> frame;
    ComPtr<IWICFormatConverter> formatConverter;
    ComPtr<ID2D1Bitmap> frameBitmap ;

    const wchar_t* imgPath ;
    UINT* frameCount ;

    public:
    Gif(const wchar_t* path) {
        this->imgPath = path ;
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

        // hr = decoder->GetFrameCount(frameCount);
        // if (hr != S_OK) return nullptr ;

        // if (frameCount == nullptr) {
        //     *frameCount = 1 ;
        // }

        return hr == S_OK ;
    }

    UINT getFrameCount() {
        return (frameCount != nullptr) ? *frameCount : 1 ; 
    }

    ComPtr<ID2D1Bitmap> getBitmapFrameAt(
        int index,
        ComPtr<ID2D1DeviceContext> d2dContext
        ) {

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

        formatConverter->Reset() ;

        return (hr == S_OK) ? frameBitmap : nullptr ;
    }


    bool erase() {
        wicFactory->Release();
        decoder->Release();
        frame->Release();
        formatConverter->Release();

        free(&imgPath) ;
        return true  ;
    }
}; 