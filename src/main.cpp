// Avatar Overlay
// VRChat(Spout) 아바타를 투명 배경의 항상-위 창으로 화면에 띄운다.
// Discord는 "모니터 공유"만 하면 되므로 OBS / 가상 오디오가 필요 없다.
//
// 성능: 크기 변환 / 투명 처리 / 상하 반전은 전부 GPU에서 하고,
// CPU로는 "창 크기만큼 줄어든" 작은 이미지만 읽어온다.
#ifndef UNICODE
#define UNICODE
#endif
#include <windows.h>
#include <mmsystem.h>
#include <shellapi.h>
#include <tlhelp32.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <dxgi1_2.h>
#include <dcomp.h>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include "SpoutDX.h"

#define WM_TRAY (WM_APP + 1)
enum { ID_EXITVRC = 300, ID_HOVER = 301, ID_SNAP_TL = 200, ID_SNAP_TR, ID_SNAP_BL, ID_SNAP_BR, ID_SNAP_CYCLE };
enum { ID_EDIT = 1, ID_VISIBLE, ID_KEY_SRC, ID_KEY_CORNER, ID_FLIP, ID_EXIT, ID_KEY_PREMUL, ID_KEY_ALPHAVIEW, ID_FLIP_H };
enum { HK_EDIT = 1, HK_VISIBLE = 2, HK_CYCLE = 3 };
enum { ID_LANG_AUTO = 400, ID_LANG_KO, ID_LANG_EN, ID_LANG_JA };

// ---------------------------------------------------------------- 다국어 (한국어 / English / 日本語)
enum Str {
    S_EDIT, S_SHOW, S_FLIPV, S_FLIPH, S_SNAP, S_TL, S_TR, S_BL, S_BR,
    S_TRANSP, S_T_STD, S_T_FIX, S_T_ALPHA, S_EXITVRC, S_LANG, S_LANG_AUTO, S_EXIT, S_WAIT, S_ERR_DX, S_CYCLE, S_HOVER, S_COUNT
};
static const wchar_t* kStr[3][S_COUNT] = {
    { // 한국어
        L"편집 모드\tCtrl+Alt+F9", L"아바타 표시\tCtrl+Alt+F10", L"상하 반전", L"좌우 반전", L"모서리에 붙이기",
        L"왼쪽 위", L"오른쪽 위", L"왼쪽 아래", L"오른쪽 아래",
        L"투명 처리", L"기본", L"이펙트·가장자리 보정", L"알파 확인",
        L"VRChat 종료 시 함께 종료", L"언어", L"자동", L"종료", L"Spout 대기 중", L"DirectX 11 초기화에 실패했습니다.",
        L"모서리 순환\tCtrl+Alt+F11", L"마우스를 올리면 반투명"
    },
    { // English
        L"Edit mode\tCtrl+Alt+F9", L"Show avatar\tCtrl+Alt+F10", L"Flip vertical", L"Flip horizontal", L"Snap to corner",
        L"Top left", L"Top right", L"Bottom left", L"Bottom right",
        L"Transparency", L"Standard", L"Effects / edge fix", L"Show alpha",
        L"Exit when VRChat closes", L"Language", L"Auto", L"Exit", L"Waiting for Spout", L"Failed to initialize DirectX 11.",
        L"Cycle corners\tCtrl+Alt+F11", L"Fade on mouse hover"
    },
    { // 日本語
        L"編集モード\tCtrl+Alt+F9", L"アバターを表示\tCtrl+Alt+F10", L"上下反転", L"左右反転", L"角に吸着",
        L"左上", L"右上", L"左下", L"右下",
        L"透過処理", L"標準", L"エフェクト・縁の補正", L"アルファ表示",
        L"VRChat終了時に一緒に終了", L"言語", L"自動", L"終了", L"Spout待機中", L"DirectX 11の初期化に失敗しました。",
        L"角を順に移動\tCtrl+Alt+F11", L"マウスを乗せると半透明"
    },
};
static int g_langSetting; // 0 = 자동(시스템 언어), 1 = 한국어, 2 = English, 3 = 日本語
static int CurLang() {
    if (g_langSetting >= 1 && g_langSetting <= 3) return g_langSetting - 1;
    switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
    case LANG_KOREAN: return 0;
    case LANG_JAPANESE: return 2;
    default: return 1;
    }
}
static const wchar_t* T(Str s) { return kStr[CurLang()][s]; }

static const wchar_t* kClass = L"AvatarOverlayWnd";
static HWND g_hwnd;
static spoutDX g_spout;

// GPU 리소스
static ID3D11VertexShader* g_vs;
static ID3D11PixelShader* g_ps;
static ID3D11SamplerState* g_smp;
static ID3D11Buffer* g_cb;
static ID3D11Texture2D* g_srcTex;      // 받은 프레임의 복사본 (셰이더 입력)
static ID3D11ShaderResourceView* g_srcSrv;
static UINT g_srcW, g_srcH;
static DXGI_FORMAT g_srcFmt;
static bool g_haveFrame;

// 설정
static bool g_edit = true;     // 처음엔 편집 모드로 시작 (위치 잡기)
static bool g_visible;         // 실제로 보이는 상태
static bool g_hidden;          // 사용자가 직접 숨김
static bool g_exitWithVrc;     // VRChat이 종료되면 함께 종료
static bool g_sawVrc;          // 실행 중에 VRChat이 켜져 있는 것을 확인한 적이 있음
// Spout "프레임 카운트" 설정이 꺼져 있으면(기본값) 새 프레임인지 알 수 없어서 이 속도로만 그린다.
// VRChat Spout의 실제 송출 속도를 정확히 알 수 없으므로, 60fps 소스를 놓치지 않도록 여유를 둔 90fps로 고정한다.
static const int g_maxFps = 90;

// 마우스를 올리면 반투명 처리
static bool g_hoverFade = true;
static float g_opacity = 1.0f;   // 현재 불투명도 (부드럽게 변함)
static const float kHoverOpacity = 0.25f;
static int g_keyMode;          // 0 = 소스 알파, 1 = 왼쪽 위 모서리 색을 투명 처리
static bool g_flip;
static bool g_flipH;           // 좌우 반전
static std::wstring g_ini;


// ---------------------------------------------------------------- settings
static int GetI(const wchar_t* k, int d) { return GetPrivateProfileIntW(L"main", k, d, g_ini.c_str()); }
static void PutI(const wchar_t* k, int v) { wchar_t b[32]; wsprintfW(b, L"%d", v); WritePrivateProfileStringW(L"main", k, b, g_ini.c_str()); }

static void SaveSettings() {
    RECT r; GetWindowRect(g_hwnd, &r);
    PutI(L"x", r.left); PutI(L"y", r.top); PutI(L"w", r.right - r.left);
    PutI(L"key", g_keyMode); PutI(L"flip", g_flip); PutI(L"fliph", g_flipH);
}

static std::wstring Widen(const std::string& s) {
    if (s.empty()) return L"";
    int n = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, nullptr, 0);
    std::wstring w(n, 0); MultiByteToWideChar(CP_UTF8, 0, s.c_str(), -1, &w[0], n); w.resize(n - 1); return w;
}
static std::string Narrow(const std::wstring& w) {
    if (w.empty()) return "";
    int n = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string s(n, 0); WideCharToMultiByte(CP_UTF8, 0, w.c_str(), -1, &s[0], n, nullptr, nullptr); s.resize(n - 1); return s;
}

// ---------------------------------------------------------------- gpu
static const char* kShader = R"(
cbuffer CB : register(b0) {
    float2 destSize;
    int nx; int ny;
    int flip; int keyMode;
    int keyY; int flipH;
    int edit; int hasSrc; float opacity; int pad1;
};
Texture2D tex : register(t0);
SamplerState smp : register(s0);

struct VOut { float4 pos : SV_Position; };
VOut VS(uint id : SV_VertexID) {
    VOut o;
    float2 uv = float2((id << 1) & 2, id & 2);
    o.pos = float4(uv * float2(2, -2) + float2(-1, 1), 0, 1);
    return o;
}
float4 PS(VOut i) : SV_Target {
    float2 uv = i.pos.xy / destSize;
    float4 res = 0;
    if (hasSrc != 0) {
        float4 acc = 0;
        for (int j = 0; j < ny; j++) {
            for (int k = 0; k < nx; k++) {
                float2 o = (float2((k + 0.5) / nx, (j + 0.5) / ny) - 0.5) / destSize;
                float2 u = uv + o;
                if (flip != 0) u.y = 1.0 - u.y;
                if (flipH != 0) u.x = 1.0 - u.x;
                float4 c = tex.SampleLevel(smp, u, 0);
                if (keyMode == 2) acc += c;                       // 이미 알파가 곱해진(premultiplied) 소스
                else if (keyMode == 3) acc += float4(c.aaa, 1);   // 알파 채널만 흑백으로 표시
                else acc += float4(c.rgb * c.a, c.a);
            }
        }
        res = (acc / (nx * ny)) * opacity;   // premultiplied 이므로 전체에 곱하면 반투명
    }
    if (edit != 0) {
        // 편집 모드: 잡을 수 있게 옅은 배경 + 파란 테두리 + 오른쪽 아래 크기 조절 손잡이
        res = res + (1.0 - res.a) * float4(0.125, 0.125, 0.125, 0.196);
        float2 p = i.pos.xy;
        bool border = p.x < 2 || p.y < 2 || p.x >= destSize.x - 2 || p.y >= destSize.y - 2;
        float grip = min(28.0, min(destSize.x, destSize.y) / 2.0);
        float2 q = p - (destSize - grip);
        if (q.x >= 0 && q.y >= 0 && q.x + q.y >= grip) border = true;
        if (border) res = float4(0.184, 0.549, 1.0, 1.0);
    }
    return res;
}
)";

struct CBData { float w, h; int nx, ny, flip, keyMode, keyY, flipH, edit, hasSrc; float opacity; int pad1; };

static void SafeRelease(IUnknown*& p) { if (p) { p->Release(); p = nullptr; } }
template <class T> static void Rel(T*& p) { if (p) { p->Release(); p = nullptr; } }

static bool InitGpu() {
    ID3D11Device* dev = g_spout.GetDX11Device();
    ID3DBlob *vb = nullptr, *pb = nullptr, *err = nullptr;
    if (FAILED(D3DCompile(kShader, strlen(kShader), nullptr, nullptr, nullptr, "VS", "vs_5_0", 0, 0, &vb, &err))) { Rel(err); return false; }
    if (FAILED(D3DCompile(kShader, strlen(kShader), nullptr, nullptr, nullptr, "PS", "ps_5_0", 0, 0, &pb, &err))) { Rel(err); return false; }
    dev->CreateVertexShader(vb->GetBufferPointer(), vb->GetBufferSize(), nullptr, &g_vs);
    dev->CreatePixelShader(pb->GetBufferPointer(), pb->GetBufferSize(), nullptr, &g_ps);
    vb->Release(); pb->Release();
    D3D11_SAMPLER_DESC sd = {};
    sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sd.MaxLOD = D3D11_FLOAT32_MAX;
    dev->CreateSamplerState(&sd, &g_smp);
    D3D11_BUFFER_DESC bd = {};
    bd.ByteWidth = sizeof(CBData); bd.Usage = D3D11_USAGE_DEFAULT; bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    dev->CreateBuffer(&bd, nullptr, &g_cb);
    return g_vs && g_ps && g_smp && g_cb;
}

static DXGI_FORMAT NormalizeFormat(DXGI_FORMAT f) {
    switch (f) {
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8A8_UNORM;
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM;
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB: return DXGI_FORMAT_B8G8R8X8_UNORM;
    default: return f;
    }
}

static void ReleaseSource() {
    Rel(g_srcSrv); Rel(g_srcTex);
    g_srcW = g_srcH = 0; g_haveFrame = false;
}

// ---------------------------------------------------------------- 스왑체인 (DirectComposition)
// GPU에서 그린 결과를 CPU로 가져오지 않고 그대로 창에 합성한다. (읽어오는 동기화가 없어서 프레임 간격이 일정하다)
static IDXGISwapChain1* g_sc;
static IDCompositionDevice* g_dcDev;
static IDCompositionTarget* g_dcTarget;
static IDCompositionVisual* g_dcVisual;
static ID3D11RenderTargetView* g_bbRtv;
static int g_scW, g_scH;

static bool CreateBackBufferView() {
    ID3D11Texture2D* bb = nullptr;
    if (FAILED(g_sc->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&bb))) return false;
    HRESULT hr = g_spout.GetDX11Device()->CreateRenderTargetView(bb, nullptr, &g_bbRtv);
    bb->Release();
    return SUCCEEDED(hr);
}

static bool InitSwapChain(int w, int h) {
    ID3D11Device* dev = g_spout.GetDX11Device();
    IDXGIDevice1* dxgiDev = nullptr;
    if (FAILED(dev->QueryInterface(__uuidof(IDXGIDevice1), (void**)&dxgiDev))) return false;
    dxgiDev->SetMaximumFrameLatency(1); // 쌓인 프레임 때문에 지연이 늘어나지 않게
    IDXGIAdapter* ad = nullptr; IDXGIFactory2* fac = nullptr;
    bool ok = SUCCEEDED(dxgiDev->GetAdapter(&ad)) && SUCCEEDED(ad->GetParent(__uuidof(IDXGIFactory2), (void**)&fac));
    if (ok) {
        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = w; sd.Height = h; sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM; sd.SampleDesc = { 1, 0 };
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; sd.BufferCount = 2;
        sd.Scaling = DXGI_SCALING_STRETCH; sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        sd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
        ok = SUCCEEDED(fac->CreateSwapChainForComposition(dev, &sd, nullptr, &g_sc));
    }
    if (ok) ok = SUCCEEDED(DCompositionCreateDevice(dxgiDev, __uuidof(IDCompositionDevice), (void**)&g_dcDev));
    if (ok) ok = SUCCEEDED(g_dcDev->CreateTargetForHwnd(g_hwnd, TRUE, &g_dcTarget));
    if (ok) ok = SUCCEEDED(g_dcDev->CreateVisual(&g_dcVisual));
    if (ok) ok = SUCCEEDED(g_dcVisual->SetContent(g_sc)) && SUCCEEDED(g_dcTarget->SetRoot(g_dcVisual)) && SUCCEEDED(g_dcDev->Commit());
    Rel(fac); Rel(ad); Rel(dxgiDev);
    if (!ok) return false;
    g_scW = w; g_scH = h;
    return CreateBackBufferView();
}

static void ResizeSwapChain(int w, int h) {
    if (!g_sc || (w == g_scW && h == g_scH)) return;
    ID3D11DeviceContext* ctx = g_spout.GetDX11Context();
    ctx->OMSetRenderTargets(0, nullptr, nullptr);
    Rel(g_bbRtv);
    if (SUCCEEDED(g_sc->ResizeBuffers(0, w, h, DXGI_FORMAT_UNKNOWN, 0))) { g_scW = w; g_scH = h; CreateBackBufferView(); }
}

static void ReleaseSwapChain() {
    Rel(g_bbRtv); Rel(g_dcVisual); Rel(g_dcTarget); Rel(g_dcDev); Rel(g_sc);
}

// ---------------------------------------------------------------- render
static void Render(bool = true) {
    if (!g_sc) return;
    RECT wr; GetWindowRect(g_hwnd, &wr);
    int W = wr.right - wr.left, H = wr.bottom - wr.top;
    if (W < 1 || H < 1) return;
    ResizeSwapChain(W, H);
    ID3D11DeviceContext* ctx = g_spout.GetDX11Context();
    if (!g_bbRtv || !ctx) return;

    bool hasSrc = g_haveFrame && g_srcSrv;
    CBData cb = {};
    cb.w = (float)W; cb.h = (float)H;
    cb.nx = hasSrc ? std::clamp((int)std::ceil((float)g_srcW / W), 1, 4) : 1;
    cb.ny = hasSrc ? std::clamp((int)std::ceil((float)g_srcH / H), 1, 4) : 1;
    cb.flip = g_flip; cb.keyMode = g_keyMode; cb.flipH = g_flipH;
    cb.edit = g_edit; cb.hasSrc = hasSrc; cb.opacity = g_opacity;
    ctx->UpdateSubresource(g_cb, 0, nullptr, &cb, 0, 0);

    D3D11_VIEWPORT vp = { 0, 0, (float)W, (float)H, 0, 1 };
    ctx->RSSetViewports(1, &vp);
    ctx->OMSetRenderTargets(1, &g_bbRtv, nullptr);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ctx->IASetInputLayout(nullptr);
    ctx->VSSetShader(g_vs, nullptr, 0);
    ctx->PSSetShader(g_ps, nullptr, 0);
    ctx->PSSetConstantBuffers(0, 1, &g_cb);
    ID3D11ShaderResourceView* srv = hasSrc ? g_srcSrv : nullptr;
    ctx->PSSetShaderResources(0, 1, &srv);
    ctx->PSSetSamplers(0, 1, &g_smp);
    ctx->Draw(3, 0);
    ID3D11ShaderResourceView* nullSrv = nullptr;
    ctx->PSSetShaderResources(0, 1, &nullSrv);
    g_sc->Present(0, 0);
}

// ---------------------------------------------------------------- spout
static void UpdateTip() {
    NOTIFYICONDATAW nid = { sizeof(nid) };
    nid.hWnd = g_hwnd; nid.uID = 1; nid.uFlags = NIF_TIP;
    std::wstring t = L"Avatar Overlay - ";
    if (g_spout.IsConnected()) t += Widen(g_spout.GetSenderName()) + L" (" + std::to_wstring(g_spout.GetSenderWidth()) + L"x" + std::to_wstring(g_spout.GetSenderHeight()) + L")";
    else t += T(S_WAIT);
    wcsncpy_s(nid.szTip, t.c_str(), _TRUNCATE);
    Shell_NotifyIconW(NIM_MODIFY, &nid);
}

static void ApplyAspect() {
    if (!g_srcW || !g_srcH) return;
    RECT r; GetWindowRect(g_hwnd, &r);
    int w = r.right - r.left, h = (int)((double)w * g_srcH / g_srcW);
    SetWindowPos(g_hwnd, HWND_TOPMOST, r.left, r.top, w, std::max(h, 1), SWP_NOACTIVATE);
}

// 새 프레임이 있으면 GPU 텍스처로 복사하고 그렸으면 true
static bool Tick() {
    if (!g_visible) return false;
    bool ok = g_spout.ReceiveTexture();
    if (g_spout.IsUpdated()) {
        // 보내는 쪽이 새로 연결되었거나 크기가 바뀜
        ReleaseSource();
        UINT w = g_spout.GetSenderWidth(), h = g_spout.GetSenderHeight();
        ID3D11Texture2D* tex = g_spout.GetSenderTexture();
        if (tex && w && h) {
            D3D11_TEXTURE2D_DESC d; tex->GetDesc(&d);
            D3D11_TEXTURE2D_DESC nd = {};
            nd.Width = d.Width; nd.Height = d.Height; nd.MipLevels = 1; nd.ArraySize = 1;
            nd.Format = NormalizeFormat(d.Format); nd.SampleDesc = { 1, 0 };
            nd.Usage = D3D11_USAGE_DEFAULT; nd.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            ID3D11Device* dev = g_spout.GetDX11Device();
            if (SUCCEEDED(dev->CreateTexture2D(&nd, nullptr, &g_srcTex)) && SUCCEEDED(dev->CreateShaderResourceView(g_srcTex, nullptr, &g_srcSrv))) {
                g_srcW = d.Width; g_srcH = d.Height; g_srcFmt = nd.Format;
            } else ReleaseSource();
        }
        ApplyAspect();
        UpdateTip();
        return false;
    }
    if (!ok) {
        if (g_haveFrame) { ReleaseSource(); Render(); UpdateTip(); }
        return false;
    }
    if (!g_spout.IsFrameNew() || !g_srcTex) return false;

    ID3D11Texture2D* tex = g_spout.GetSenderTexture();
    ID3D11DeviceContext* ctx = g_spout.GetDX11Context();
    if (!tex || !ctx) return false;
    ctx->CopyResource(g_srcTex, tex);
    g_haveFrame = true;
    Render(false);
    return true;
}

// ---------------------------------------------------------------- ui
// 창이 있는 모니터의 작업 영역(작업표시줄 제외)
static RECT WorkAreaOf(const RECT& r) {
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(MonitorFromRect(&r, MONITOR_DEFAULTTONEAREST), &mi);
    return mi.rcWork;
}

// 0 = 왼쪽 위, 1 = 오른쪽 위, 2 = 왼쪽 아래, 3 = 오른쪽 아래
static void SnapToCorner(int corner) {
    RECT r; GetWindowRect(g_hwnd, &r);
    RECT a = WorkAreaOf(r);
    int w = r.right - r.left, h = r.bottom - r.top;
    int x = (corner & 1) ? a.right - w : a.left;
    int y = (corner & 2) ? a.bottom - h : a.top;
    SetWindowPos(g_hwnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
    Render();
    SaveSettings();
}

// 현재 가장 가까운 모서리에서 시계 방향으로 다음 모서리로 이동 (왼쪽 위 → 오른쪽 위 → 오른쪽 아래 → 왼쪽 아래)
static void CycleCorner() {
    RECT r; GetWindowRect(g_hwnd, &r);
    RECT a = WorkAreaOf(r);
    int cx = (r.left + r.right) / 2, cy = (r.top + r.bottom) / 2;
    int cur = (cx > (a.left + a.right) / 2 ? 1 : 0) | (cy > (a.top + a.bottom) / 2 ? 2 : 0);
    static const int order[4] = { 0, 1, 3, 2 };
    int idx = 0;
    for (int i = 0; i < 4; i++) if (order[i] == cur) idx = i;
    SnapToCorner(order[(idx + 1) % 4]);
}

static void SetEdit(bool on) {
    g_edit = on;
    LONG_PTR ex = GetWindowLongPtrW(g_hwnd, GWL_EXSTYLE);
    if (on) ex &= ~WS_EX_TRANSPARENT; else ex |= WS_EX_TRANSPARENT;
    SetWindowLongPtrW(g_hwnd, GWL_EXSTYLE, ex);
    Render();
    if (!on) SaveSettings();
}

// 실제 표시 여부 = 사용자가 숨기지 않았을 때
static void RefreshVisibility() {
    bool v = !g_hidden;
    if (v == g_visible) return;
    g_visible = v;
    ShowWindow(g_hwnd, v ? SW_SHOWNOACTIVATE : SW_HIDE);
    if (v) { SetTimer(g_hwnd, 3, 40, nullptr); Render(); }   // 마우스 올림 감지 타이머는 보일 때만
    else {
        KillTimer(g_hwnd, 3);
        // 숨겨진 상태에서는 메모리를 최소한으로 줄인다
        SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T)-1, (SIZE_T)-1);
    }
}

static void SetVisible(bool on) {
    g_hidden = !on;
    RefreshVisibility();
}

static bool IsProcessRunning(const wchar_t* exe) {
    bool found = false;
    HANDLE s = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (s == INVALID_HANDLE_VALUE) return false;
    PROCESSENTRY32W pe = { sizeof(pe) };
    for (BOOL ok = Process32FirstW(s, &pe); ok; ok = Process32NextW(s, &pe))
        if (_wcsicmp(pe.szExeFile, exe) == 0) { found = true; break; }
    CloseHandle(s);
    return found;
}

// 이전 버전의 "VRChat 실행 시 자동 표시" 옵션이 등록해 둔 Windows 시작 프로그램 항목을 제거한다.
// (이 기능은 삭제되었고, 항목이 남아 있으면 오래된 경로를 계속 실행하려 하므로 정리한다. 이 프로그램이 만든 항목만 지운다.)
static void RemoveLegacyAutostart() {
    HKEY k;
    if (RegOpenKeyExW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Run", 0, KEY_SET_VALUE, &k) != ERROR_SUCCESS) return;
    RegDeleteValueW(k, L"AvatarOverlay");
    RegCloseKey(k);
}

static void SetExitWithVrc(bool on) {
    g_exitWithVrc = on;
    PutI(L"exitwithvrc", on);
    g_sawVrc = IsProcessRunning(L"VRChat.exe"); // 지금 VRChat이 켜져 있어야 "꺼진 뒤 종료"가 동작
}

static void ShowMenu() {
    HMENU m = CreatePopupMenu();
    HMENU cm = CreatePopupMenu();
    AppendMenuW(cm, MF_STRING, ID_SNAP_CYCLE, T(S_CYCLE));
    AppendMenuW(cm, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(cm, MF_STRING, ID_SNAP_TL, T(S_TL));
    AppendMenuW(cm, MF_STRING, ID_SNAP_TR, T(S_TR));
    AppendMenuW(cm, MF_STRING, ID_SNAP_BL, T(S_BL));
    AppendMenuW(cm, MF_STRING, ID_SNAP_BR, T(S_BR));
    HMENU am = CreatePopupMenu();
    AppendMenuW(am, MF_STRING | (g_keyMode == 0 ? MF_CHECKED : 0), ID_KEY_SRC, T(S_T_STD));
    AppendMenuW(am, MF_STRING | (g_keyMode == 2 ? MF_CHECKED : 0), ID_KEY_PREMUL, T(S_T_FIX));
    AppendMenuW(am, MF_STRING | (g_keyMode == 3 ? MF_CHECKED : 0), ID_KEY_ALPHAVIEW, T(S_T_ALPHA));
    HMENU lm = CreatePopupMenu();
    AppendMenuW(lm, MF_STRING | (g_langSetting == 0 ? MF_CHECKED : 0), ID_LANG_AUTO, T(S_LANG_AUTO));
    AppendMenuW(lm, MF_STRING | (g_langSetting == 2 ? MF_CHECKED : 0), ID_LANG_EN, L"English");
    AppendMenuW(lm, MF_STRING | (g_langSetting == 1 ? MF_CHECKED : 0), ID_LANG_KO, L"한국어");
    AppendMenuW(lm, MF_STRING | (g_langSetting == 3 ? MF_CHECKED : 0), ID_LANG_JA, L"日本語");

    AppendMenuW(m, MF_STRING | (g_edit ? MF_CHECKED : 0), ID_EDIT, T(S_EDIT));
    AppendMenuW(m, MF_STRING | (!g_hidden ? MF_CHECKED : 0), ID_VISIBLE, T(S_SHOW));
    AppendMenuW(m, MF_STRING | (g_flip ? MF_CHECKED : 0), ID_FLIP, T(S_FLIPV));
    AppendMenuW(m, MF_STRING | (g_flipH ? MF_CHECKED : 0), ID_FLIP_H, T(S_FLIPH));
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_POPUP, (UINT_PTR)cm, T(S_SNAP));
    AppendMenuW(m, MF_POPUP, (UINT_PTR)am, T(S_TRANSP));
    AppendMenuW(m, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(m, MF_STRING | (g_hoverFade ? MF_CHECKED : 0), ID_HOVER, T(S_HOVER));
    AppendMenuW(m, MF_STRING | (g_exitWithVrc ? MF_CHECKED : 0), ID_EXITVRC, T(S_EXITVRC));
    AppendMenuW(m, MF_POPUP, (UINT_PTR)lm, T(S_LANG));
    AppendMenuW(m, MF_STRING, ID_EXIT, T(S_EXIT));

    POINT p; GetCursorPos(&p);
    SetForegroundWindow(g_hwnd);
    int cmd = TrackPopupMenu(m, TPM_RETURNCMD | TPM_RIGHTBUTTON, p.x, p.y, 0, g_hwnd, nullptr);
    DestroyMenu(m);
    switch (cmd) {
    case ID_EDIT: SetEdit(!g_edit); break;
    case ID_VISIBLE: SetVisible(g_hidden); break;
    case ID_KEY_SRC: g_keyMode = 0; Render(); SaveSettings(); break;
    case ID_EXITVRC: SetExitWithVrc(!g_exitWithVrc); break;
    case ID_KEY_PREMUL: g_keyMode = 2; Render(); SaveSettings(); break;
    case ID_KEY_ALPHAVIEW: g_keyMode = 3; Render(); SaveSettings(); break;
    case ID_FLIP: g_flip = !g_flip; Render(); SaveSettings(); break;
    case ID_EXIT: DestroyWindow(g_hwnd); break;
    case ID_FLIP_H: g_flipH = !g_flipH; Render(); SaveSettings(); break;
    case ID_LANG_AUTO: case ID_LANG_KO: case ID_LANG_EN: case ID_LANG_JA:
        g_langSetting = cmd - ID_LANG_AUTO; PutI(L"lang", g_langSetting); UpdateTip(); break;
    case ID_SNAP_TL: SnapToCorner(0); break;
    case ID_SNAP_TR: SnapToCorner(1); break;
    case ID_SNAP_BL: SnapToCorner(2); break;
    case ID_SNAP_BR: SnapToCorner(3); break;
    case ID_SNAP_CYCLE: CycleCorner(); break;
    case ID_HOVER: g_hoverFade = !g_hoverFade; PutI(L"hoverfade", g_hoverFade); break;    }
}

static LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_TIMER:
        if (wp == 3) {
            // 마우스가 오버레이 위에 있으면 부드럽게 반투명, 벗어나면 원래대로 (편집 모드에서는 항상 불투명)
            RECT r; POINT p; GetWindowRect(h, &r); GetCursorPos(&p);
            bool over = g_hoverFade && !g_edit && PtInRect(&r, p);
            float target = over ? kHoverOpacity : 1.0f;
            if (g_opacity != target) {
                float step = 0.12f;
                g_opacity = g_opacity < target ? std::min(target, g_opacity + step) : std::max(target, g_opacity - step);
                Render();
            }
        } else if (wp == 2) {
            if (g_exitWithVrc) {
                bool run = IsProcessRunning(L"VRChat.exe");
                if (run) g_sawVrc = true;
                // VRChat이 실행 중인 것을 확인한 뒤 종료되면 함께 종료 (시작할 때 VRChat이 없다고 바로 꺼지지는 않음)
                if (g_sawVrc && !run) { DestroyWindow(h); return 0; }
            }
            if (g_visible) SetWindowPos(h, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }
        return 0;
    case WM_NCHITTEST: {
        if (!g_edit) return HTTRANSPARENT;
        POINT p = { (short)LOWORD(lp), (short)HIWORD(lp) };
        RECT r; GetWindowRect(h, &r);
        if (p.x > r.right - 28 && p.y > r.bottom - 28) return HTBOTTOMRIGHT;
        return HTCAPTION;
    }
    case WM_NCRBUTTONUP: ShowMenu(); return 0;
    case WM_MOUSEWHEEL: {
        if (!g_edit) break;
        RECT r; GetWindowRect(h, &r);
        int w = r.right - r.left, hh = r.bottom - r.top;
        double f = GET_WHEEL_DELTA_WPARAM(wp) > 0 ? 1.08 : 1.0 / 1.08;
        int nw = std::max(60, (int)(w * f)), nh = std::max(60, (int)(hh * f));
        SetWindowPos(h, HWND_TOPMOST, r.left, r.top, nw, nh, SWP_NOACTIVATE);
        return 0;
    }
    case WM_SIZING: {
        if (g_srcW && g_srcH) {
            RECT* r = (RECT*)lp;
            int w = std::max<LONG>(60, r->right - r->left);
            r->right = r->left + w;
            r->bottom = r->top + (int)((double)w * g_srcH / g_srcW);
        }
        return TRUE;
    }
    case WM_MOVING: {
        // 드래그 중 화면 가장자리(상하좌우)에 가까워지면 자석처럼 붙는다. Shift를 누르고 있으면 끈다.
        if (GetKeyState(VK_SHIFT) & 0x8000) break;
        RECT* r = (RECT*)lp;
        RECT a = WorkAreaOf(*r);
        const int T = 24;
        int w = r->right - r->left, hh = r->bottom - r->top;
        if (std::abs(r->left - a.left) < T) r->left = a.left;
        else if (std::abs(r->right - a.right) < T) r->left = a.right - w;
        if (std::abs(r->top - a.top) < T) r->top = a.top;
        else if (std::abs(r->bottom - a.bottom) < T) r->top = a.bottom - hh;
        r->right = r->left + w; r->bottom = r->top + hh;
        return TRUE;
    }
    case WM_SIZE: Render(); return 0;
    case WM_EXITSIZEMOVE: SaveSettings(); return 0;
    case WM_HOTKEY:
        if (wp == HK_EDIT) SetEdit(!g_edit);
        else if (wp == HK_VISIBLE) SetVisible(g_hidden);
        else if (wp == HK_CYCLE) CycleCorner();
        return 0;
    case WM_TRAY:
        if (lp == WM_RBUTTONUP || lp == WM_LBUTTONUP) ShowMenu();
        return 0;
    case WM_MOUSEACTIVATE: return MA_NOACTIVATE;
    case WM_DESTROY: {
        NOTIFYICONDATAW nid = { sizeof(nid) }; nid.hWnd = h; nid.uID = 1;
        Shell_NotifyIconW(NIM_DELETE, &nid);
        SaveSettings();
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProcW(h, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE hi, HINSTANCE, PWSTR, int) {
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(nullptr, path, MAX_PATH);
    // 같은 exe는 한 번만 실행 (서로 다른 폴더/버전은 동시에 실행 가능)
    uint32_t hash = 2166136261u;
    for (const wchar_t* c = path; *c; c++) hash = (hash ^ (uint32_t)towlower(*c)) * 16777619u;
    wchar_t mtxName[64]; wsprintfW(mtxName, L"AvatarOverlay_%08x", hash);
    HANDLE mtx = CreateMutexW(nullptr, TRUE, mtxName);
    if (GetLastError() == ERROR_ALREADY_EXISTS) return 0;
    g_ini = path; g_ini = g_ini.substr(0, g_ini.find_last_of(L'.')) + L".ini";
    {   // 설정 파일은 exe 옆에 저장(휴대용). 쓰기 권한이 없는 곳(Program Files 등)이면 %APPDATA%로 옮긴다.
        HANDLE t = CreateFileW(g_ini.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (t != INVALID_HANDLE_VALUE) CloseHandle(t);
        else {
            wchar_t ad[MAX_PATH];
            if (GetEnvironmentVariableW(L"APPDATA", ad, MAX_PATH)) {
                std::wstring dir = std::wstring(ad) + L"\\Avatar Overlay";
                CreateDirectoryW(dir.c_str(), nullptr);
                g_ini = dir + L"\\AvatarOverlay.ini";
            }
        }
    }

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc; wc.hInstance = hi; wc.lpszClassName = kClass; wc.hCursor = LoadCursor(nullptr, IDC_SIZEALL);
    wc.hIcon = LoadIconW(hi, MAKEINTRESOURCEW(1));
    RegisterClassW(&wc);

    RECT wa; SystemParametersInfoW(SPI_GETWORKAREA, 0, &wa, 0);
    // VRChat 얼굴/아바타 카메라 Spout 출력이 2048x2048(정사각형)이므로 연결 전 기본 모양도 정사각형으로 둔다.
    // 연결되면 실제 소스 비율에 맞춰진다.
    int w = GetI(L"w", 360), h = w;
    int x = GetI(L"x", wa.right - w - 40), y = GetI(L"y", wa.bottom - h - 40);
    bool firstRun = GetI(L"w", 0) == 0;
    g_keyMode = GetI(L"key", 0); if (g_keyMode == 1) g_keyMode = 0; // 모서리 색 제거 모드는 삭제됨
    g_flip = GetI(L"flip", 0) != 0; g_flipH = GetI(L"fliph", 0) != 0;
    g_exitWithVrc = GetI(L"exitwithvrc", 0) != 0;
    RemoveLegacyAutostart();
    g_langSetting = std::clamp(GetI(L"lang", 0), 0, 3);
    g_hoverFade = GetI(L"hoverfade", 1) != 0;
    // 팝업 메뉴를 시스템 다크/라이트 설정에 맞춘다 (비공개 API라 실패해도 무시)
    if (HMODULE ux = LoadLibraryW(L"uxtheme.dll")) {
        auto setMode = (int (WINAPI*)(int))GetProcAddress(ux, MAKEINTRESOURCEA(135));
        auto flush = (void (WINAPI*)())GetProcAddress(ux, MAKEINTRESOURCEA(136));
        if (setMode) setMode(1);
        if (flush) flush();
    }

    g_hwnd = CreateWindowExW(WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, kClass, L"Avatar Overlay",
        WS_POPUP, x, y, w, h, nullptr, nullptr, hi, nullptr);
    SetLayeredWindowAttributes(g_hwnd, 0, 255, LWA_ALPHA); // 클릭 통과(WS_EX_TRANSPARENT)를 쓰려면 layered 여야 함

    NOTIFYICONDATAW nid = { sizeof(nid) };
    nid.hWnd = g_hwnd; nid.uID = 1; nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAY; nid.hIcon = (HICON)LoadImageW(hi, MAKEINTRESOURCEW(1), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    wcscpy_s(nid.szTip, L"Avatar Overlay");
    Shell_NotifyIconW(NIM_ADD, &nid);

    RegisterHotKey(g_hwnd, HK_EDIT, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F9);
    RegisterHotKey(g_hwnd, HK_VISIBLE, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F10);
    RegisterHotKey(g_hwnd, HK_CYCLE, MOD_CONTROL | MOD_ALT | MOD_NOREPEAT, VK_F11);

    RECT cr; GetClientRect(g_hwnd, &cr);
    if (!g_spout.OpenDirectX11() || !InitGpu() || !InitSwapChain(std::max<LONG>(cr.right, 1), std::max<LONG>(cr.bottom, 1))) {
        MessageBoxW(nullptr, T(S_ERR_DX), L"Avatar Overlay", MB_ICONERROR);
        return 1;
    }

    g_sawVrc = IsProcessRunning(L"VRChat.exe");
    RefreshVisibility();
    SetEdit(firstRun); // 처음 실행할 때만 편집 모드로 시작
    SetTimer(g_hwnd, 2, 2000, nullptr);

    // 메시지 처리 + Spout 확인.  백그라운드 사용량을 줄이기 위해 "필요할 때만" 깨어난다.
    //  - 숨겨진 상태: 메시지가 올 때까지 완전히 대기 (CPU 0)
    //  - Sender 없음: 250ms마다 확인
    //  - 수신 중 + 프레임 카운트 켜짐: 실제 프레임 간격을 학습해 다음 프레임 직전까지 잠듦
    //  - 수신 중 + 프레임 카운트 꺼짐(기본 Spout 설정): 새 프레임 여부를 알 수 없으므로
    //    고정 90fps 이하로만 그린다. (이게 없으면 초당 수백 번 불필요하게 그림)
    LARGE_INTEGER freq; QueryPerformanceFrequency(&freq);
    auto nowMs = [&]() { LARGE_INTEGER c; QueryPerformanceCounter(&c); return (double)c.QuadPart * 1000.0 / (double)freq.QuadPart; };
    double lastFrame = 0, ema = 16.0, nextTick = 0;
    bool hiRes = false; // 1ms 타이머 해상도는 수신 중일 때만 요청 (시스템 전체 전력 소모에 영향)
    bool quit = false;
    while (!quit) {
        MSG m;
        while (PeekMessageW(&m, nullptr, 0, 0, PM_REMOVE)) {
            if (m.message == WM_QUIT) { quit = true; break; }
            TranslateMessage(&m); DispatchMessageW(&m);
        }
        if (quit) break;

        double now = nowMs();
        bool counted = g_spout.IsFrameCountEnabled();
        double interval = 1000.0 / g_maxFps;
        bool due = counted || !g_spout.IsConnected() || now >= nextTick;
        bool drew = false;
        if (due) {
            drew = Tick();
            if (!counted) { nextTick += interval; if (nextTick < now) nextTick = now + interval; }
        }
        now = nowMs();
        if (drew && counted) {
            double iv = now - lastFrame;
            if (lastFrame > 0 && iv > 2 && iv < 200) ema = ema * 0.8 + iv * 0.2;
        }
        if (drew) lastFrame = now;
        bool active = g_visible && g_spout.IsConnected();
        if (active != hiRes) { hiRes = active; if (hiRes) timeBeginPeriod(1); else timeEndPeriod(1); }

        DWORD wait;
        if (!g_visible) wait = INFINITE;
        else if (!active) wait = 250;
        else if (!counted) { double w = nextTick - now; wait = w > 1 ? (DWORD)w : 1; }
        else {
            double since = now - lastFrame;
            if (since > 3 * ema + 50) wait = 8;          // 프레임이 멈춘 상태
            else { double w = ema - 3 - since; wait = w > 1 ? (DWORD)w : 1; }
        }
        MsgWaitForMultipleObjects(0, nullptr, FALSE, wait, QS_ALLINPUT);
    }

    if (hiRes) timeEndPeriod(1);
    ReleaseSource();
    ReleaseSwapChain();
    g_spout.ReleaseReceiver();
    g_spout.CloseDirectX11();
    return 0;
}
