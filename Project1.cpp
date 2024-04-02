// Project1.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "pch.h"
#include "framework.h"
#include "Project1.h"
#include <d3d11.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "Sprite.h"
#include "BoxModel.h"

#pragma comment( lib, "d3d11.lib")
#pragma comment( lib, "d3dCompiler.lib")


#define SAFE_RELEASE(x) if(x){ x->Release(); x=nullptr; }

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HWND gWnd = nullptr;
INT  gWindowWidth = 0;
INT  gWindowHeight = 0;

D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;    //드라이버 타입
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;//드라이버 버전

IDXGISwapChain* pSwapChain = nullptr;             // the pointer to the swap chain interface
ID3D11Device* pd3dDevice = nullptr;                     // the pointer to our Direct3D device interface
ID3D11DeviceContext* pd3dContext = nullptr;
ID3D11RenderTargetView* g_pRenderTargetView = nullptr;

Sprite* g_Sprite1;
BoxModel* g_BoxModel;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
HRESULT InitDevice();
void ReleaseDeviceObjects();
HRESULT Init_RenderTarget_ViewPort();
void Render();
void Update();

#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_PROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    InitDevice();
    Init_RenderTarget_ViewPort();

    g_Sprite1 = new Sprite;
    g_Sprite1->Init();

    g_BoxModel = new BoxModel;
    g_BoxModel->Init();

    MSG msg = {};

    // 기본 메시지 루프입니다:
    while (WM_QUIT != msg.message) 
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            Update();
            Render();
        }
    }

    delete g_Sprite1;
    delete g_BoxModel;

    ReleaseDeviceObjects();

    return (int) msg.wParam;
}

void ReleaseDeviceObjects()
{
    SAFE_RELEASE(g_pRenderTargetView);
    SAFE_RELEASE(pSwapChain);
    SAFE_RELEASE(pd3dContext);
    SAFE_RELEASE(pd3dDevice);
}

void Update()
{
    XMMATRIX world = XMMatrixIdentity();
    g_BoxModel->SetWorldTM(world);

	XMVECTOR Eye = XMVectorSet(2.5f, 2.5f, -2.5f, 0.0f);
	XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(Eye, At, Up);
	g_BoxModel->SetViewTM(view);

    XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, gWindowWidth / (FLOAT)gWindowHeight, 0.01f, 100.0f);
    g_BoxModel->SetProjTM(proj);
}

void Render()
{
    pd3dContext->ClearRenderTargetView(g_pRenderTargetView, 
        DirectX::Colors::Thistle);

    //g_Sprite1->Render();
    g_BoxModel->Render();

    pSwapChain->Present(1, 0);
}


//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+2);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_PROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    RECT rc = { 0,0, 800,600 };
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, true);


   HWND hWnd = CreateWindowW(szWindowClass, szTitle, 
       WS_OVERLAPPEDWINDOW,
       CW_USEDEFAULT, 0, 
       rc.right-rc.left, rc.bottom-rc.top, 
       nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   gWnd = hWnd;

   RECT rc1 = {};
   GetClientRect(hWnd, &rc1);
   gWindowWidth = rc1.right - rc1.left;
   gWindowHeight = rc1.bottom - rc1.top;


   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void ChangeRectLT()
{
    static int rectLeft = 30;
    static int rectTop = 30;
    //참일경우 +, 거짓일경우 -
    static bool rectLeftCheck = true;
    static bool rectTopCheck = true;

    if (rectLeftCheck)
    {
        rectLeft += 10;
        if (rectLeft >= gWindowWidth - 100)
        {
            rectLeftCheck = false;
        }
    }
    else
    {
        rectLeft -= 10;
        if (rectLeft <= 0)
        {
            rectLeftCheck = true;
        }
    }
    if (rectTopCheck)
    {
        rectTop += 10;
        if (rectTop >= gWindowHeight - 100)
        {
            rectTopCheck = false;
        }
    }
    else
    {
        rectTop -= 10;
        if (rectTop <= 0)
        {
            rectTopCheck = true;
        }
    }

    g_Sprite1->Move(rectLeft, rectTop, 100, 100);

}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        if (wParam == VK_SPACE) {
            SetTimer(hWnd, 101, 50, nullptr);
        }
        break;
    case WM_TIMER:
        ChangeRectLT();
        break;

    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


HRESULT InitDevice()
{
    HRESULT hr = S_OK;
    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,//디버깅용
        D3D_DRIVER_TYPE_REFERENCE,//디버깅용
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = gWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &sd, &pSwapChain, &pd3dDevice, &g_featureLevel, &pd3dContext);
        if (SUCCEEDED(hr))
            break;
    }//D3D11디바이스 생성후 스왑체인()을 만든다
    
    return hr;
}

HRESULT Init_RenderTarget_ViewPort()
{
    HRESULT hr = S_OK;

    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
        reinterpret_cast<void**>(&pBackBuffer));
    if (FAILED(hr))
        return hr;

    hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr,
        &g_pRenderTargetView);
    pBackBuffer->Release();
    if (FAILED(hr))
        return hr;

    pd3dContext->OMSetRenderTargets(1, &g_pRenderTargetView, nullptr);


    // ViewPort
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    D3D11_VIEWPORT vp;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    vp.Width = (float)gWindowWidth;
    vp.Height = (float)gWindowHeight;
    vp.MinDepth = 0.f;
    vp.MaxDepth = 1.f;

    pd3dContext->RSSetViewports(1, &vp);

    return hr;
}


