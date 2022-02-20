#include <windows.h>
#include <vector>
#include <ctime>

#define UP 0;     // зачем мне это - непонятно
#define RIGHT 1;  //
#define DOWN 2;   //
#define LEFT 3;   //

using namespace std;

LONG WINAPI WndProc(HWND, UINT, WPARAM, LPARAM);

HWND hwnd;
MSG msg;
WNDCLASS win = { 0 };
HDC dc;
HBRUSH hBrush, hBrush_t;
HPEN hPen;
HANDLE Nit = NULL;
DWORD IdNiti = 0;
HFONT hfont;
RECT rc_for_text = {0, 0, 600, 600};

bool Shake_thread = false;
int standart_size = 13;
int standart_step = 2;
int standart_time_turn_sigments = standart_size / standart_step - 1;
int standart_size_apple = 10;
vector <int> Apple = { 100, 100 };

int ppp = 3; // для тестов

class Snake {
private:
    vector <int> Ways = { 0, 0, 0, 0, 0 };
    vector <vector <int>> Snake_coords = { {120, 150}, {120, 160}, {120, 170}, {120, 180}, {120, 190} };
    vector <vector <int>> Critical_points = {};
public:
    int Sigment_coord_x(int number) { return Snake_coords[number][0]; }
    int Sigment_coord_y(int number) { return Snake_coords[number][1]; }
    int Snake_lenght() { return Ways.size(); }
    void Add_sigment() {
        srand(time(NULL));
        int k = Ways[Ways.size() - 1];
        int x = Snake_coords[Snake_coords.size() - 1][0];
        int y = Snake_coords[Snake_coords.size() - 1][1];
        Ways.push_back(k);
        switch (k) {
        case 0:
            Snake_coords.push_back({ x, y + 10 });
            break;
        case 1:
            Snake_coords.push_back({ x - 10, y });
            break;
        case 2:
            Snake_coords.push_back({ x, y - 10 });
            break;
        case 3:
            Snake_coords.push_back({ x + 10, y });
            break;
        }
        Apple.clear();
        while (true) {
            int rx = rand() % 580 + 20;
            int ry = rand() % 580 + 20;
            bool z = true;
            for (int i = 0; i < Ways.size(); i++) {
                if (pow(pow(Snake_coords[i][0] - rx, 2) + pow(Snake_coords[i][1] - ry, 2), 0.5) <= standart_size + standart_size_apple) {
                    z = false;
                    break;
                }
            }
            if (z) {
                Apple.push_back(rx);
                Apple.push_back(ry);
                break;
            }
        }
    }
    void Add_critical_point(int x, int y, int way) {
        if (way != Ways[0] && way != (Ways[0] + 2)%4) {
            Critical_points.push_back({ x, y, way, standart_time_turn_sigments, 1 });
            Ways[0] = way;
        }
    }
    void Snake_death() {
        Ways = { 0, 0, 0, 0, 0 };
        Snake_coords = { {120, 150}, {120, 160}, {120, 170}, {120, 180}, {120, 190} };
        Critical_points = {};
        Apple = { 100, 100 };
    }
    void Snake_step() {
        for (int i = 0; i < Ways.size(); i++) {
            switch (Ways[i])
            {
            case 0:
                Snake_coords[i][1] -= standart_step;
                break;
            case 1:
                Snake_coords[i][0] += standart_step;
                break;
            case 2:
                Snake_coords[i][1] += standart_step;
                break;
            case 3:
                Snake_coords[i][0] -= standart_step;
                break;
            }
        }                    

        for (int i = 0; i < Critical_points.size(); i++) {               
            Critical_points[i][3] -= 1;
            if (Critical_points[i][3] == 0 && Critical_points[i][4] < Ways.size()) {  //Critical_points[i][4] < Ways.size()
                Ways[Critical_points[i][4]] = Critical_points[i][2];
                Critical_points[i][3] = standart_time_turn_sigments;
                Critical_points[i][4] += 1;
                if (Critical_points[i][4] < Ways.size()) {}                      // придумать удаление
            }
        }
    }
};

Snake Vladimir;

void Out_text(int size, int color, int x, int y, LPCTSTR text) {
    hfont = CreateFont(size, 0, 0, 0, 0, 0u, 0U, 0U, ANSI_CHARSET, 0U, 0U, 0U, 0U, TEXT("Arial"));
    SelectObject(dc, hfont);
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, color);
    rc_for_text.left = x;
    rc_for_text.top = y;
    DrawText(dc, text, lstrlen(text), &rc_for_text, DT_TOP | DT_LEFT);
    DeleteObject(hfont);
}

DWORD WINAPI update(LPDWORD  Param) {
    while (true) {
        if (Shake_thread) {

            hBrush_t = CreateSolidBrush(RGB(0, 0, 255));
            SelectObject(dc, hBrush_t);
            Ellipse(dc, Vladimir.Sigment_coord_x(0), Vladimir.Sigment_coord_y(0), Vladimir.Sigment_coord_x(0) + standart_size, Vladimir.Sigment_coord_y(0) + standart_size);
            DeleteObject(hBrush_t);

            hBrush_t = CreateSolidBrush(RGB(0, 255, 0));
            SelectObject(dc, hBrush_t);
            for (int i = 1; i < Vladimir.Snake_lenght(); i++) {
                Ellipse(dc, Vladimir.Sigment_coord_x(i), Vladimir.Sigment_coord_y(i), Vladimir.Sigment_coord_x(i) + standart_size, Vladimir.Sigment_coord_y(i) + standart_size);
            }
            DeleteObject(hBrush_t);

            Sleep(50);

            hBrush_t = CreateSolidBrush(RGB(0, 0, 0));
            SelectObject(dc, hBrush_t);
            for (int i = 0; i < Vladimir.Snake_lenght(); i++) {
                Ellipse(dc, Vladimir.Sigment_coord_x(i), Vladimir.Sigment_coord_y(i), Vladimir.Sigment_coord_x(i) + standart_size, Vladimir.Sigment_coord_y(i) + standart_size);
            }
            DeleteObject(hBrush_t);

            hBrush_t = CreateSolidBrush(RGB(255, 255, 0));
            SelectObject(dc, hBrush_t);
            Ellipse(dc, Apple[0], Apple[1], Apple[0] + standart_size_apple, Apple[1] + standart_size_apple);
            DeleteObject(hBrush_t);

            Vladimir.Snake_step();

            if (pow(pow(Vladimir.Sigment_coord_x(0) - Apple[0], 2) + pow(Vladimir.Sigment_coord_y(0) - Apple[1], 2), 0.5) < standart_size_apple) {
                hBrush_t = CreateSolidBrush(RGB(0, 0, 0));
                SelectObject(dc, hBrush_t);
                Ellipse(dc, Apple[0], Apple[1], Apple[0] + standart_size_apple, Apple[1] + standart_size_apple);
                DeleteObject(hBrush_t);
                Vladimir.Add_sigment();
            }

            for (int i = 2; i < Vladimir.Snake_lenght(); i++) {
                if (pow(pow(Vladimir.Sigment_coord_x(0) - Vladimir.Sigment_coord_x(i), 2) + pow(Vladimir.Sigment_coord_y(0) - Vladimir.Sigment_coord_y(i), 2), 0.5) < standart_size) {
                    hBrush_t = CreateSolidBrush(RGB(0, 0, 0));
                    SelectObject(dc, hBrush_t);
                    Ellipse(dc, Apple[0], Apple[1], Apple[0] + standart_size_apple, Apple[1] + standart_size_apple);
                    DeleteObject(hBrush_t);
                    Vladimir.Snake_death();
                    break;
                }
            }

            if (Vladimir.Sigment_coord_x(0) < 0 || Vladimir.Sigment_coord_x(0) > 600 || Vladimir.Sigment_coord_y(0) < 0 || Vladimir.Sigment_coord_y(0) > 600) {
                hBrush_t = CreateSolidBrush(RGB(0, 0, 0));
                SelectObject(dc, hBrush_t);
                Ellipse(dc, Apple[0], Apple[1], Apple[0] + standart_size_apple, Apple[1] + standart_size_apple);
                DeleteObject(hBrush_t);
                Vladimir.Snake_death();
            }
        }
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    win.style = CS_HREDRAW | CS_VREDRAW;
    win.lpfnWndProc = WndProc;
    win.hInstance = hInstance;
    win.lpszClassName = L"My Class";
    RegisterClass(&win);

    hwnd = CreateWindow(win.lpszClassName, L"Snake", WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX, 500, 300, 600, 600, NULL, NULL, hInstance, NULL);
    SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(0, 0, 0)));

    dc = GetDC(hwnd);

    Nit = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&update, NULL, 0, &IdNiti);          // а зачем ему поток?

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    /*hPen = CreatePen(1, 1, RGB(255, 255, 255));
    SelectObject(dc, hPen);

    for (int i = 0; i < 60; i++) {
        MoveToEx(dc, i * 10, 0, NULL);                                  пережиток прошлого
        LineTo(dc, i * 10, 600);
    }

    for (int i = 0; i < 60; i++) {
        MoveToEx(dc, 0, i * 10, NULL);
        LineTo(dc, 600, i * 10);
    }

    DeleteObject(hPen);*/

    //Out_text(20, RGB(0, 255, 0), 100, 200, TEXT("out"));
    
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

LONG WINAPI WndProc(HWND hwnd, UINT Message, WPARAM wparam, LPARAM lparam)
{
    switch (Message)
    {
    case WM_CREATE:
        break;
    case WM_SHOWWINDOW:
        break;
    case WM_KEYDOWN:
        switch (wparam)
        {
        case VK_LEFT:
            Vladimir.Add_critical_point(Vladimir.Sigment_coord_x(0), Vladimir.Sigment_coord_y(0), 3);
            break;
        case VK_RIGHT:
            Vladimir.Add_critical_point(Vladimir.Sigment_coord_x(0), Vladimir.Sigment_coord_y(0), 1);
            break;
        case VK_UP:
            Vladimir.Add_critical_point(Vladimir.Sigment_coord_x(0), Vladimir.Sigment_coord_y(0), 0);
            break;
        case VK_DOWN:
            Vladimir.Add_critical_point(Vladimir.Sigment_coord_x(0), Vladimir.Sigment_coord_y(0), 2);
            break;
        default:
            break;
        }
    case WM_LBUTTONDOWN:
        Shake_thread = true;
        break;
    case WM_RBUTTONDOWN:
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, Message, wparam, lparam);
    }
    return 0;
}