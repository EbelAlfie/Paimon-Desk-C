#include <windows.h>
#include <wincodec.h>
#include <commdlg.h>
#include <d2d1.h>
#include <tchar.h>

template <typename T>
inline void SafeRelease(T *&pI)
{
    if (NULL != pI)
    {
        pI->Release();
        pI = NULL;
    }
}

class Gif2 {
private:
    wchar_t* imagePath = nullptr ;
    
public:
    Gif2 (wchar_t* path) {
        imagePath = path ;
    }

    void composeNextFrame() {
        HRESULT hr = S_OK;

        // Check to see if the render targets are initialized
        // if (m_pHwndRT && m_pFrameComposeRT)
        // {
        //     // First, kill the timer since the delay is no longer valid
        //     KillTimer(m_hWnd, DELAY_TIMER_ID);

        //     // Compose one frame
        //     hr = DisposeCurrentFrame();
        //     if (SUCCEEDED(hr))
        //     {
        //         hr = OverlayNextFrame();
        //     }

        //     // Keep composing frames until we see a frame with delay greater than
        //     // 0 (0 delay frames are the invisible intermediate frames), or until
        //     // we have reached the very last frame.
        //     while (SUCCEEDED(hr) && m_uFrameDelay == 0 && !IsLastFrame())
        //     {
        //         hr = DisposeCurrentFrame();
        //         if (SUCCEEDED(hr))
        //         {
        //             hr = OverlayNextFrame();
        //         }
        //     }

        //     // If we have more frames to play, set the timer according to the delay.
        //     // Set the timer regardless of whether we succeeded in composing a frame
        //     // to try our best to continue displaying the animation.
        //     if (!EndOfAnimation() && m_cFrames > 1)
        //     {
        //         // Set the timer according to the delay
        //         SetTimer(m_hWnd, DELAY_TIMER_ID, m_uFrameDelay, NULL);
        //     }
        // }
    }

};