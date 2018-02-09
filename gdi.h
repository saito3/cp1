//////////////////////////////////////////////////////////////////////
//  GDI Graphic Function Library Header File : gdi.h                //
//  Copyright (C) 2012 Akira Yamada                                 //
//                                                                  //
//  Release Notes:                                                  //
//  Released on January 04 2012                                     //
//  Revised on December 17 2012, December 19 2012                   //
//                                                                  //
// Reference:                                                       //
// [1]Hiroshi Serizawa:                                             //
//  "Introductory Windows Graphics using C"                         //
//   (CによるWindowsグラフィックス入門)in Japanese,                 //
//   Morikita Publishing (Octber 1977).                             //
//                                                                  //
//////////////////////////////////////////////////////////////////////
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winnls.h>

#ifndef TIMER
#define TIMER FALSE
#endif

#ifndef TIMEINT
#define TIMEINT 20
#endif

#ifndef ERASE
#define ERASE TRUE
#endif

#ifndef PROGRAM
#define PROGRAM "CP1-Programming"
#endif

#define PI 3.1415927
#define EXP 2.7182818
#define ID_TIMER 32767   //タイマID．何かと重複しなさそうな値を適当に

#define White 0
#define DarkGray 1
#define DarkBlue 2
#define Blue 3
#define DarkRed 4
#define Red 5
#define DarkMagenta 6
#define Magenta 7
#define DarkGreen 8
#define Green 9
#define DarkCyan 10
#define Cyan 11
#define DarkYellow 12
#define Yellow 13
#define Gray 14
#define Black 15

HDC hdc;                            // ＤＣハンドル
HWND hwnd;                          // ウィンドウハンドル

int WX,WY;                          // ウィンドウサイズ
int SX,SY;                          // スクリーンサイズ
int CSX,CSY;                        // スクリーン中央のｘ座標とｙ座標
const int SXL=640L,SYL=480L;        // スクリーンサイズ
int X0,Y0;                          // 原点のｘ座標とｙ座標
int YAX;                            // ｙ軸方向
int Count;                          // カウンタ

COLORREF FRCOLOR[256];              // フォアグラウンドカラー
COLORREF BKCOLOR;                   // バックグラウンドカラー

char STR[256];                      // 文字列格納用配列


LRESULT CALLBACK WindowProc(HWND,UINT,WPARAM,LPARAM);


#ifdef TIMER
    void wm_create(void);
#endif

void wm_paint(void);
//void wm_char(void);

//////////////////////////////////////////////////////////////////////

void setwindowsize(int x,int y)     // ウィンドウサイズの設定
{
	WX=x;WY=y;
}

int getwindowsizex(void)            // ｘ軸方向ウィンドウサイズの取得
{   RECT r;
    GetWindowRect(hwnd,&r);
    return r.right-r.left;
}

int getwindowsizey(void)            // ｙ軸方向ウィンドウサイズの取得
{   RECT r;
    GetWindowRect(hwnd,&r);
    return r.bottom-r.top;
}

int getscreensizex(void)            // ｘ軸方向スクリーンサイズの取得
{   RECT r;
    GetClientRect(hwnd,&r);
    return r.right;
}

int getscreensizey(void)            // ｙ軸方向スクリーンサイズの取得// ???????X?N???[???T?C?Y??擾
{   RECT r;
    GetClientRect(hwnd,&r);
    return r.bottom;
}

void setcolormode16(void)            //16色カラーモードの設定
{
    const int index[]={19,12, 4,16, 1,13, 5,17, 2,14, 6,18, 3,15,7,0};
//白,暗灰色,暗青,青,暗赤,赤,暗紫,紫,暗緑,緑,暗水色,水色,暗黄色,黄色,明灰色,黒
    int i;
    for(i=0;i<16;i++)
        FRCOLOR[i]=PALETTEINDEX(index[i]);
}

void setbkcolor(int bkc)           // バックグラウンドカラーの設定
{
	BKCOLOR=FRCOLOR[bkc];
}

int fint(double x)                  // double型データのint型データへの変換
{
	return (int)floor(x+0.5);
}

void set0(int x0,int y0,int yax)    // 原点とｙ軸方向の設定
{
	X0=x0;Y0=y0;YAX=yax;
}

int Utf8ToSJis(char *out, char *inp)
{
    char *temp;
    int  size;

    // UTF8 -> UNICODE
    size = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)inp, -1, 0, 0);
    temp = (char *)malloc(size * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)inp, -1, (LPWSTR)temp, size);

    //UNICODE -> SJIS
    size = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)temp, -1, 0, 0, 0, 0);
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)temp, -1, (LPSTR)out, size, 0, 0);

    free(temp);
    return(size);
}

//////////////////////////////////////////////////////////////////////

//#pragma argsused

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine,int nCmdShow)
{
    WNDCLASS wc;                    // ウィンドウクラス構造体
// int MSX=GetSystemMetrics(SM_CXSCREEN);// ｘ軸方向最大スクリーンサイズ
// int MSY=GetSystemMetrics(SM_CYSCREEN);// ｙ軸方向最大スクリーンサイズ

    char* string;
    string=(char *)malloc(strlen(PROGRAM)+1);
    Utf8ToSJis(string, PROGRAM);

    setwindowsize(SXL,SYL);          // ウィンドウサイズの設定
    setcolormode16();                // 16色カラーモードの設定
    setbkcolor(0);                   // バックグラウンドカラーの設定
    set0(0,0,0);                     // 原点とｙ軸方向の設定

    HBRUSH hbrush=CreateSolidBrush(BKCOLOR);

    wc.lpszClassName=string;
    wc.lpszMenuName =NULL;
    wc.hInstance    =hInstance;
    wc.lpfnWndProc  =WindowProc;
    wc.hCursor      =LoadCursor(NULL,IDC_ARROW);
    wc.hIcon        =LoadIcon(NULL,IDI_APPLICATION);
    wc.hbrBackground=hbrush;
    wc.style        =0;
    wc.cbClsExtra   =0;
    wc.cbWndExtra   =0;
    if(!RegisterClass(&wc))
        return 0;

    hwnd=CreateWindow(wc.lpszClassName,wc.lpszClassName,
                      WS_OVERLAPPEDWINDOW,0,0,WX,WY,
                      HWND_DESKTOP,NULL,wc.hInstance,NULL);
    SetWindowPos(hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
    ShowWindow(hwnd,nCmdShow);

    MSG msg;                        // メッセージ構造体
    while(GetMessage(&msg,NULL,0,0))
    {
    	TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    free(string);
    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd,UINT message,WPARAM wParam,
                            LPARAM lParam)    // ウィンドウ関数
{
    PAINTSTRUCT paintstruct;

	switch(message){
       case WM_CREATE:

           if(TIMER){
        	  wm_create();
              SetTimer(hwnd, ID_TIMER, TIMEINT, NULL );//タイマ設定
           }
            return 0;
       case WM_PAINT:
            hdc=BeginPaint(hwnd,&paintstruct);
            wm_paint();
            EndPaint(hwnd,&paintstruct);
            return 0;
        case WM_CHAR:
            MessageBox(hwnd,STR,"",MB_OK);
            return 0;
        case WM_TIMER: //----タイマ----
        	Count = (Count+1)%1000; //カウンタを増やす
            //長方形領域を再描画、ERASEで前の画像を消去または上書きを選択
             InvalidateRect( hwnd,  NULL, ERASE);
            UpdateWindow( hwnd );
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            PostQuitMessage(0);
            return 0;
        default:
            return DefWindowProc(hwnd,message,wParam,lParam);
    }
}

//////////////////////////////////////////////////////////////////////

void G_COLOR(int c)
{
    HPEN hpen=CreatePen(PS_SOLID,1,FRCOLOR[c]);
    SelectObject(hdc,hpen);
}

void G_PALLET(int ic, int red, int green,int blue)
{
    FRCOLOR[ic]=PALETTERGB(red,green,blue);
}


void G_PSET(int x,int y,int c) // 点の描画
{
   x=x+X0;y=y*(1-2*YAX)+Y0;
   SetPixel(hdc,x,y,FRCOLOR[c]);
}

void G_LINE(int x0,int y0,int x1,int y1,int c) // 直線の描画
{
    x0=x0+X0;y0=y0*(1-2*YAX)+Y0;
    x1=x1+X0;y1=y1*(1-2*YAX)+Y0;
    POINT p[2];
    p[0].x=x0;p[0].y=y0;p[1].x=x1;p[1].y=y1;
    HPEN hpen=CreatePen(PS_SOLID,1,FRCOLOR[c]);
    HPEN hpen_old=SelectObject(hdc,hpen);
    Polyline(hdc,p,2);
    SelectObject(hdc,hpen_old);
    DeleteObject(hpen);
}

void G_CIRCLE(int x,int y,int r,int c)  //塗りつぶし無しの円の描画
{
    x=x+X0;y=y*(1-2*YAX)+Y0;
    HPEN hpen=CreatePen(PS_SOLID,1,FRCOLOR[c]);
    HPEN hpen_old=SelectObject(hdc,hpen);
    HBRUSH hbrush=GetStockObject(NULL_BRUSH);
    HBRUSH hbrush_old=SelectObject(hdc,hbrush);
    Ellipse(hdc,x-r,y-r,x+r+1,y+r+1);
    SelectObject(hdc,hbrush_old);
    DeleteObject(hbrush);
    SelectObject(hdc,hpen_old);
    DeleteObject(hpen);
}

void G_FCIRCLE(int x,int y,int r,int c) // 塗りつぶした円の描画
{
    x=x+X0;y=y*(1-2*YAX)+Y0;
    HPEN hpen=CreatePen(PS_SOLID,1,FRCOLOR[c]);
    HPEN hpen_old=SelectObject(hdc,hpen);
    HBRUSH hbrush=CreateSolidBrush(FRCOLOR[c]);
    HBRUSH hbrush_old=SelectObject(hdc,hbrush);
    Ellipse(hdc,x-r,y-r,x+r+1,y+r+1);
    SelectObject(hdc,hbrush_old);
    DeleteObject(hbrush);
    SelectObject(hdc,hpen_old);
    DeleteObject(hpen);
}

void G_RECTANGLE(int x0,int y0,int x1,int y1,int c)// 塗りつぶし無しの長方形の描画
{
    if(x0>x1){
       int x=x0;x0=x1;x1=x;
    }
    if(y0>y1){
       int y=y0;y0=y1;y1=y;
    }
    x0=x0+X0;y0=y0*(1-2*YAX)+Y0;
    x1=x1+X0;y1=y1*(1-2*YAX)+Y0;
    HPEN hpen=CreatePen(PS_SOLID,1,FRCOLOR[c]);
    HPEN hpen_old=SelectObject(hdc,hpen);
    HBRUSH hbrush=GetStockObject(NULL_BRUSH);
    HBRUSH hbrush_old=SelectObject(hdc,hbrush);
    if(YAX==0)
        Rectangle(hdc,x0,y0,x1+1,y1+1);
    else
        Rectangle(hdc,x0,y0+1,x1+1,y1);
    SelectObject(hdc,hbrush_old);
    DeleteObject(hbrush);
    SelectObject(hdc,hpen_old);
    DeleteObject(hpen);
}

void G_FRECTANGLE(int x0,int y0,int x1,int y1,int c)// 塗りつぶした長方形の描画
{
    if(x0>x1){
       int x=x0;x0=x1;x1=x;
    }
    if(y0>y1){
       int y=y0;y0=y1;y1=y;
    }
    x0=x0+X0;y0=y0*(1-2*YAX)+Y0;
    x1=x1+X0;y1=y1*(1-2*YAX)+Y0;
    HPEN hpen=CreatePen(PS_SOLID,1,FRCOLOR[c]);
    HPEN hpen_old=SelectObject(hdc,hpen);
    HBRUSH hbrush=CreateSolidBrush(FRCOLOR[c]);
    HBRUSH hbrush_old=SelectObject(hdc,hbrush);
    if(YAX==0)
        Rectangle(hdc,x0,y0,x1+1,y1+1);
    else
        Rectangle(hdc,x0,y0+1,x1+1,y1);
    SelectObject(hdc,hbrush_old);
    DeleteObject(hbrush);
    SelectObject(hdc,hpen_old);
    DeleteObject(hpen);
}

void G_TEXT(int x,int y, char *s,int c)  // 文字列の出力
{
	int size;
	char *temp,*out;

// UTF8 -> UNICODE
    size = MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, -1, 0, 0);
    temp = (char *)malloc(size * 2 + 2);
    MultiByteToWideChar(CP_UTF8, 0, (LPCSTR)s, -1, (LPWSTR)temp, size);

// UNICODE -> SJIS
     size = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)temp, -1, 0, 0, 0, 0);
     out = (char *)malloc(size+1);
     WideCharToMultiByte(CP_ACP, 0, (LPWSTR)temp, -1, (LPSTR)out, size, 0, 0);

    SetBkMode(hdc,TRANSPARENT);
    SetTextColor(hdc,FRCOLOR[c]);
    TextOut(hdc,x,y,out,strlen(out));

    free(temp);
    free(out);
}

void G_CLS(void)                // 画面の消去
{
    HBRUSH hbrush=CreateSolidBrush(BKCOLOR);
    HBRUSH hbrush_old=SelectObject(hdc,hbrush);
    PatBlt(hdc,0,0,SX,SY,PATCOPY);
    SelectObject(hdc,hbrush_old);
    DeleteObject(hbrush);
}