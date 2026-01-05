
#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

struct lorenz_state{
    double x;
    double y;
    double z;
};

internal void update_lorenz(lorenz_state *state, float dt){
    const float sigma = 10.0f;
    const float rho = 28.0f;
    const float beta = 8.0f / 3.0f;

    float dx = sigma * (state->y - state->x);
    float dy = state->x * (rho - state->z) - state->y;
    float dz = state->x * state->y - beta * state->z;

    state->x += dx * dt;
    state->y += dy * dt;
    state->z += dz * dt;
}

struct win32_offscreen_buffer{
    BITMAPINFO info;
    void *memory;
    HBITMAP handle;
    HDC device_context;
    int width;
    int height;
    int pitch;
    int bytes_per_pixel;

};

global_variable bool running;

global_variable win32_offscreen_buffer global_back_buffer;

struct win32_window_dimension{
    int width;
    int height;
};

internal void draw_pixel(win32_offscreen_buffer *buffer, int x, int y, uint8 r, uint8 g, uint8 b){
    if(x < 0 || x >= buffer->width || y < 0 || y > buffer->height){
        return;
    }

    uint8 *row = (uint8 *) buffer->memory + y * buffer->pitch;
    uint32 *pixel = (uint32 *)(row + x * buffer->bytes_per_pixel);
    *pixel = (r << 16) | (g << 8) | b;
}

internal void FadeBuffer(win32_offscreen_buffer *buffer){
    uint8 *row = (uint8 *)buffer->memory;

    for (int y = 0; y < buffer->height; ++y)
    {
        uint32 *pixel = (uint32 *)row;
        for (int x = 0; x < buffer->width; ++x)
        {
            uint32 c = *pixel;

            uint8 r = (c >> 16) & 0xFF;
            uint8 g = (c >> 8)  & 0xFF;
            uint8 b =  c        & 0xFF;

            r = (uint8)(r * 0.95f);
            g = (uint8)(g * 0.95f);
            b = (uint8)(b * 0.95f);

            *pixel++ = (r << 16) | (g << 8) | b;
        }
        row += buffer->pitch;
    }
}

internal void render_lorenz(win32_offscreen_buffer *buffer){
    local_persist lorenz_state lorenz = {0.1, 0.0, 0.0};
    int center_x = buffer->width / 2;
    int center_y = buffer->height / 2;

    float scale = 10.0f;
    for(int i = 0; i < 1000; ++i){
        update_lorenz(&lorenz, 0.01f);
        int x = center_x + (int)(lorenz.x * scale);
        int y = center_y + (int)(lorenz.y * scale);

        uint8 r = (uint8)(lorenz.z * 5.0f);
        uint8 g = 128;
        uint8 b = 255 - r;
        //uint8 color = (uint8)(128 + lorenz.z * 2.0f);
        draw_pixel(buffer, x, y, r, g, b);
    }
}

internal void RenderWeirdGradient(win32_offscreen_buffer buffer, int x_offset, int y_offset){
    // int width = buffer.width;
    // int height = buffer.height;

    int bytes_per_pixel = 4;
    // int pitch = width * buffer.bytes_per_pixel;
    uint8 *row = (uint8 *)buffer.memory;

    for(int y = 0; y< buffer.height; ++y){
        uint32 *pixel  = (uint32 *)row;
        for(int x = 0; x < buffer.width; ++x){
            uint8 blue = (uint8)(x + x_offset);
            uint8 green = (uint8)(y + y_offset);
            // *pixel++ = ((green << 8) | blue);
            draw_pixel(&buffer, x, y, 0, green, blue);

        } 
        row += buffer.pitch;
    }
}

internal void Win32ResizeDIBSection(win32_offscreen_buffer *buffer, int width, int height){
    // TODO (ayo): Resize the DIB section to match the window size

    if(buffer->memory){
        VirtualFree(buffer->memory, 0, MEM_RELEASE);
    }

    buffer->width = width; 
    buffer->height = height;

    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = buffer->width;
    buffer->info.bmiHeader.biHeight = -buffer->height;
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;

    buffer->bytes_per_pixel = 4;
    int bitmap_memory_size = (buffer->width * buffer->height) * buffer->bytes_per_pixel;
    buffer->memory = VirtualAlloc(0, bitmap_memory_size, MEM_COMMIT, PAGE_READWRITE);
    buffer->pitch = width * buffer->bytes_per_pixel;
    uint8 *row = (uint8 *)buffer->memory;
}

internal void Win32DispalyBufferInWindow(HDC device_context, RECT *window_rect, win32_offscreen_buffer buffer , int x, int y, int width, int height){
    int window_width = window_rect->right - window_rect->left;
    int window_height = window_rect->bottom - window_rect->top;

    StretchDIBits(
        device_context,
        // x,y,width,height,
        // x,y,width,height,
        0,0,buffer.width, buffer.height,
        0,0, buffer.width, buffer.height,
        buffer.memory,
        &buffer.info,
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
            Win32ResizeDIBSection(&global_back_buffer, width, height);
            
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
            Win32DispalyBufferInWindow(device_context, &client_rect, global_back_buffer, X, Y, width, height);
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
                //RenderWeirdGradient(global_back_buffer, x_offset, y_offset);
                FadeBuffer(&global_back_buffer);
                render_lorenz(&global_back_buffer);
                HDC device_context = GetDC(window);
                RECT client_rect;
                GetClientRect(window, &client_rect);
                int window_wiidth = client_rect.right - client_rect.left;
                int window_height = client_rect.bottom - client_rect.top;
                Win32DispalyBufferInWindow(device_context, &client_rect, global_back_buffer, 0, 0, window_wiidth, window_height);
                ReleaseDC(window, device_context);
                ++x_offset;
                
            }
        }
    }
    return (0);
}