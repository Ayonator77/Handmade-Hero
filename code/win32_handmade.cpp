
#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct win32_offscreen_buffer{
    BITMAPINFO info;
    void *memory;
    HBITMAP handle;
    HDC device_context;
    int width;
    int height;
    int bytes_per_pixel;

};

global_variable bool running;
global_variable BITMAPINFO bitmap_info;
global_variable void *bitmap_memory;
global_variable HBITMAP bitmap_handle;
global_variable HDC bitmap_device_context;
global_variable int bitmap_width;
global_variable int bitmap_height;

internal void RenderWeirdGradient(win32_offscreen_buffer *buffer, int x_offset, int y_offset){
    int width = buffer->width;
    int height = buffer->height;

    int bytes_per_pixel = 4;
    int pitch = bitmap_width * bytes_per_pixel;
    uint8 *row = (uint8 *)bitmap_memory;

    for(int y = 0; y< bitmap_height; ++y){
        uint32 *pixel  = (uint32 *)row;
        for(int x = 0; x < bitmap_width; ++x){
            uint8 blue = (uint8)(x + x_offset);
            uint8 green = (uint8)(y + y_offset);
            *pixel++ = ((green << 8) | blue);

        } 
        row += pitch;
    }
}

internal void Win32ResizeDIBSection(int width, int height){
    // TODO (ayo): Resize the DIB section to match the window size

    if(bitmap_memory){
        VirtualFree(bitmap_memory, 0, MEM_RELEASE);
    }

    bitmap_width = width; 
    bitmap_height = height;

    bitmap_info.bmiHeader.biSize = sizeof(bitmap_info.bmiHeader);
    bitmap_info.bmiHeader.biWidth = bitmap_width;
    bitmap_info.bmiHeader.biHeight = -bitmap_height;
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = 32;
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    int bytes_per_pixel = 4;
    int bitmap_memory_size = (bitmap_width * bitmap_height) * bytes_per_pixel;
    bitmap_memory = VirtualAlloc(0, bitmap_memory_size, MEM_COMMIT, PAGE_READWRITE);
    int pitch = bitmap_width * bytes_per_pixel;
    uint8 *row = (uint8 *)bitmap_memory;

    // for(int y = 0; y< bitmap_height; ++y){
    //     uint8 *pixel  = (uint8 *)row;
    //     for(int x = 0; x < bitmap_width; ++x){

    //         *pixel = (uint8)x;
    //         ++pixel;

    //         *pixel = (uint8)y;
    //         ++pixel;

    //         *pixel = 0;
    //         ++pixel;

    //         *pixel = 0;
    //         ++pixel;
    //     } 
    //     row += pitch;
    // }
   // RenderWeirdGradient(128,0);

    
   //bitmap_handle =  CreateDIBSection(bitmap_device_context, &bitmap_info, DIB_RGB_COLORS, &bitmap_memory, 0, 0); 
}

internal void Win32UpdateWindow(HDC device_context, RECT *window_rect, int x, int y, int width, int height){
    int window_width = window_rect->right - window_rect->left;
    int window_height = window_rect->bottom - window_rect->top;

    StretchDIBits(
        device_context,
        // x,y,width,height,
        // x,y,width,height,
        0,0,bitmap_width, bitmap_height,
        0,0, bitmap_width, bitmap_height,
        bitmap_memory,
        &bitmap_info,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}


LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam){
   
    LRESULT Result = 0;

    switch(Message){
        case WM_SIZE:
        {
            RECT client_rect;
            GetClientRect(Window, &client_rect);
            int width = client_rect.right - client_rect.left;
            int height = client_rect.bottom - client_rect.top; 
            Win32ResizeDIBSection(width, height);
            
            OutputDebugStringA("WM_SIZE\n");
        }break;

        case WM_CLOSE:
        {
    
            running = false; // TODO (ayo): handle as  a message to the user
            OutputDebugStringA("WM_CLOSE\n");
        }break;

        case WM_DESTROY:
        {
            running = false; // TODO (ayo):  handle as an error
            OutputDebugStringA("WM_DESTROY\n");
        }break; 

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        }break;

        case WM_PAINT: 
        {
            PAINTSTRUCT paint;
            HDC device_context = BeginPaint(Window, &paint);
            int X = paint.rcPaint.left;
            int Y = paint.rcPaint.top;
            int width = paint.rcPaint.right - paint.rcPaint.left;
            int height = paint.rcPaint.bottom - paint.rcPaint.top;

            RECT client_rect;
            GetClientRect(Window, &client_rect);
            Win32UpdateWindow(device_context, &client_rect, X, Y, width, height);
            //PatBlt(device_context, X, Y, width, height, WHITENESS);
            EndPaint(Window, &paint);
        }break;

        default:
        {
            //OutputDebugStringA("Default Message\n");
            Result = DefWindowProcW(Window, Message, WParam, LParam);
        }break;
    }
    return(Result);
}                                    

int CALLBACK WinMain( HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CmdLine, int CmdShow){


    //MessageBoxExW(NULL, L"This is a test", L"Handmade Hero", MB_OK | MB_ICONINFORMATION, 0);
    WNDCLASSW window_class =  {};
    window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = Win32MainWindowCallback;
    //window_class.lpfnWndProc = DefWindowProcW;
    window_class.hInstance = Instance;
    window_class.lpszClassName = L"HandmadeWindowClass";
    if(RegisterClassW(&window_class)){
        running = true;
        HWND window = CreateWindowExW(0, window_class.lpszClassName, L"Handmade Hero", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0, Instance, 0);
        if(window){
            int x_offset = 0;
            int y_offset = 0;
            while(running){
                MSG message;
                while (PeekMessage(&message, 0, 0, 0 , PM_REMOVE)){
                    if(message.message == WM_QUIT){
                        running = false;
                    }
                    TranslateMessage(&message);
                    DispatchMessage(&message); 
                }
                RenderWeirdGradient(x_offset, y_offset);
                HDC device_context = GetDC(window);
                RECT client_rect;
                GetClientRect(window, &client_rect);
                int window_wiidth = client_rect.right - client_rect.left;
                int window_height = client_rect.bottom - client_rect.top;
                Win32UpdateWindow(device_context, &client_rect, 0, 0, window_wiidth, window_height);
                ReleaseDC(window, device_context);
                ++x_offset;
                
            }
        }
    }
    return (0);
}