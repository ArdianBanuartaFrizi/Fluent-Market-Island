// ==WindhawkMod==
// @id           market-island-dock
// @name         Fluent Market Island
// @description  Floating market island with smooth animation, market quote search, and independent shape background opacity.
// @version      2.9.2
// @author       Banuarta
// @include      windhawk.exe
// @compilerOptions -luser32 -lwinhttp -lgdiplus -lshell32 -lgdi32 -ldwmapi -lwinmm -lwindowsapp -lruntimeobject
// ==/WindhawkMod==

// ==WindhawkModReadme==
/*
# Fluent Market Island

A floating capsule-style market widget for Windows, distributed as a Windhawk mod.

## Features

- Compact capsule that expands into a market panel.
- Six screen anchors: top-left, top-center, top-right, bottom-left, bottom-center, and bottom-right.
- Crypto, US stock/ETF, and Indonesian stock watchlists.
- Instrument search through the Yahoo Finance website endpoints used by this source.
- Separate controls for Fluent Glass Tint and overall Shape Background Opacity.
- Fluent, Nusa, Purple, Ocean, Aurora, and Light color themes.
- Acrylic, Glass, and None material modes.

## Shape Background Opacity

`ShapeOpacity` controls the opacity of the complete capsule/panel surface from 0 to 100 percent. It is independent of `FluentGlassTint`.

Text and interactive controls are intentionally kept readable and are not globally faded by this setting.

## Installation

1. Install Windhawk from https://windhawk.net/.
2. Open the mod editor in Windhawk.
3. Paste the complete source file into the editor, save, and compile.
4. Enable the mod and configure its settings.

For an existing installation, use **Edit Code** and replace the source with this complete file. The mod ID is retained to preserve the existing mod identity and stored settings.

## Market-data notice

This source makes HTTPS requests to Yahoo Finance web endpoints. These endpoints are not a documented public market-data API and may change or stop working. Yahoo's Terms of Service restrict automated collection and certain reuse of service data. Review the applicable terms and obtain the necessary permission or replace the data provider before distributing or using this as a public/commercial market-data service.

Quotes are best-effort display information, may be delayed or unavailable, and are not investment advice.

## Maintainer and development note

Maintainer: **Banuarta**. This codebase was revised with AI assistance from source supplied by the project owner. See the repository README and references for implementation notes and source documentation.

*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- RefreshSeconds: 300
  $name: Market data refresh interval (seconds)
  $description: How often the market quotes are refreshed.
  #! $min: 60
  #! $max: 3600
  #! $format: slider
- OffsetX: 0
  $name: Horizontal position offset
  $description: Horizontal offset from the selected screen anchor, in pixels.
  #! $min: -1000
  #! $max: 1000
  #! $format: slider
- OffsetY: 30
  $name: Vertical position offset
  $description: Vertical offset from the selected screen anchor, in pixels.
  #! $min: 0
  #! $max: 1000
  #! $format: slider
- FontSize: 16
  $name: Price font size
  #! $min: 12
  #! $max: 28
  #! $format: slider
- FluentGlassTint: 116
  $name: Fluent glass tint
  $description: Controls the color tint strength of the glass material, independently from the overall shape opacity.
  #! $min: 0
  #! $max: 255
  #! $format: slider
- ShapeOpacity: 100
  $name: Shape background opacity
  $description: Controls the opacity of the entire Dynamic Island background, including the capsule and expanded panel. This is independent from Fluent Glass Tint. 0 is fully transparent and 100 is fully opaque.
  #! $min: 0
  #! $max: 100
  #! $format: slider
- ScalePercent: 100
  $name: UI scale
  #! $min: 70
  #! $max: 150
  #! $format: slider
- PillWidth: 380
  $name: Capsule width
  #! $min: 320
  #! $max: 560
  #! $format: slider
- PillHeight: 44
  $name: Capsule height
  #! $min: 44
  #! $max: 76
  #! $format: slider
- AnimationDuration: 420
  $name: Animation duration (milliseconds)
  #! $min: 100
  #! $max: 800
  #! $format: slider
- AlwaysOnTop: true
  $name: Always on top
- HideWhenFullscreen: true
  $name: Hide when an app is fullscreen
- Position: TopCenter
  $name: Island position
  $options:
    - TopLeft: Top left
    - TopCenter: Top center
    - TopRight: Top right
    - BottomLeft: Bottom left
    - BottomCenter: Bottom center
    - BottomRight: Bottom right
- GlassTheme: Fluent
  $name: Glass theme
  $options:
    - Fluent: Fluent
    - Nusa: Nusa
    - Purple: Purple
    - Ocean: Ocean
    - Aurora: Aurora
    - Light: Light
- BlurStyle: Acrylic
  $name: Glass material
  $options:
    - Acrylic: Acrylic
    - Glass: Glass
    - None: None
- FontFace: Segoe UI
  $name: Font family
  #! $format: fontFamily
- TextColor: "#F5F7FA"
  $name: Text color
  #! $format: colorRgb
- UpColor: "#31D98B"
  $name: Positive change color
  #! $format: colorRgb
- DownColor: "#FF5B67"
  $name: Negative change color
  #! $format: colorRgb
- FlatColor: "#A0AAB9"
  $name: Neutral change color
  #! $format: colorRgb
- MutedColor: "#A0AAB9"
  $name: Secondary text color
  #! $format: colorRgb
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#include <winhttp.h>
#include <gdiplus.h>
#include <windhawk_api.h>
#include <dwmapi.h>
#include <mmsystem.h>
#include <dispatcherqueue.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Foundation.Numerics.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.h>
#include <winrt/Windows.UI.Composition.h>
#include <winrt/Windows.UI.Composition.Desktop.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <cmath>
#include <cstdlib>
#include <cwchar>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {

constexpr wchar_t kWindowClassName[] = L"WindhawkMarketIslandDockWindow";
constexpr int kPillWidth = 380;
constexpr int kPillHeight = 44;
constexpr int kExpandedWidth = 480;
constexpr int kPanelHeight = 410;
constexpr int kExpandedHeight = 468;
constexpr int kTopDockHeight = 60;
constexpr int kTopDockDefaultWidth = 620;
constexpr int kSideDockWidth = 80;
constexpr int kSideDockDefaultHeight = 300;
constexpr int kSideExpandedWidth = kExpandedWidth + kSideDockWidth;
constexpr UINT kMessageMarketData = WM_APP + 61;
constexpr UINT kMessageRepaint = WM_APP + 62;
constexpr UINT kMessageSearchResults = WM_APP + 63;
constexpr UINT kMessageCustomQuote = WM_APP + 64;
constexpr UINT kMessageAnimationFrame = WM_APP + 65;
constexpr UINT_PTR kVisibilityTimer = 1;
constexpr UINT_PTR kAnimationTimer = 2;
constexpr UINT_PTR kCustomQuoteTimer = 3;

constexpr int kCryptoStart = 0;
constexpr int kUsStart = 5;
constexpr int kIdxStart = 10;
constexpr int kAssetsPerGroup = 5;
constexpr int kAssetCount = 15;

struct AssetInfo {
    const wchar_t* ticker;
    const wchar_t* symbol;
    const wchar_t* name;
    bool rupiah;
    int decimals;
};

constexpr AssetInfo kAssets[kAssetCount] = {
    {L"BTC",  L"BTC-USD", L"Bitcoin",                  false, 2},
    {L"ETH",  L"ETH-USD", L"Ethereum",                 false, 2},
    {L"SOL",  L"SOL-USD", L"Solana",                   false, 2},
    {L"XRP",  L"XRP-USD", L"XRP",                      false, 4},
    {L"BNB",  L"BNB-USD", L"BNB",                      false, 2},
    {L"SPY",  L"SPY",     L"SPDR S&P 500 ETF",         false, 2},
    {L"QQQ",  L"QQQ",     L"Invesco Nasdaq 100 ETF",   false, 2},
    {L"DIA",  L"DIA",     L"SPDR Dow Jones ETF",       false, 2},
    {L"NVDA", L"NVDA",    L"NVIDIA",                   false, 2},
    {L"AAPL", L"AAPL",    L"Apple",                    false, 2},
    {L"BBCA", L"BBCA.JK", L"Bank Central Asia",        true,  0},
    {L"BBRI", L"BBRI.JK", L"Bank Rakyat Indonesia",    true,  0},
    {L"BMRI", L"BMRI.JK", L"Bank Mandiri",             true,  0},
    {L"TLKM", L"TLKM.JK", L"Telkom Indonesia",         true,  0},
    {L"ASII", L"ASII.JK", L"Astra International",      true,  0},
};

struct Quote {
    double price = 0.0;
    double previousClose = 0.0;
    double changePercent = 0.0;
    bool valid = false;
};

struct MarketSnapshot {
    std::array<Quote, kAssetCount> quotes{};
};

struct SearchResult {
    std::wstring ticker;
    std::wstring symbol;
    std::wstring name;
    std::wstring exchange;
    std::wstring quoteType;
    bool rupiah = false;
    int decimals = 2;
};

struct SearchRequest {
    std::wstring query;
    unsigned int requestId = 0;
};

struct SearchResultsMessage {
    unsigned int requestId = 0;
    int count = 0;
    SearchResult items[5];
};

struct CustomQuoteMessage {
    std::wstring symbol;
    Quote quote;
};

enum class HitTarget : int {
    None = 0,
    Pill,
    PanelBackground,
    CategoryCrypto,
    CategoryUs,
    CategoryIdx,
    Search,
    Row0,
    Row1,
    Row2,
    Row3,
    Row4,
    Refresh
};

struct UiHitRect {
    int left;
    int top;
    int right;
    int bottom;
};

constexpr UiHitRect kCategoryHitRects[3] = {
    {20, 58, 166, 98},
    {166, 58, 313, 98},
    {313, 58, 460, 98},
};
constexpr UiHitRect kSearchHitRect{20, 102, 460, 145};
constexpr UiHitRect kRefreshHitRect{366, 377, 460, 406};
constexpr int kListRowTop = 172;
constexpr int kListRowStep = 41;
constexpr int kListRowHeight = 36;

struct Settings {
    int refreshSeconds = 300;
    int offsetX = 0;
    int offsetY = 30;
    int fontSize = 16;
    int glassTintOpacity = 116;
    int shapeOpacity = 100;
    int scalePercent = 100;
    int pillWidth = kPillWidth;
    int pillHeight = kPillHeight;
    int topDockWidth = kTopDockDefaultWidth;
    int sideDockHeight = kSideDockDefaultHeight;
    int animationDuration = 420;
    bool alwaysOnTop = true;
    bool hideWhenFullscreen = true;
    std::wstring position = L"TopCenter";
    std::wstring dockMode = L"Floating";
    std::wstring dockAlignment = L"Center";
    std::wstring dockSide = L"Left";
    std::wstring theme = L"Fluent";
    std::wstring blurStyle = L"Acrylic";
    std::wstring shape = L"Capsule";
    std::wstring fontFace = L"Segoe UI";
    std::wstring textColor = L"#F5F7FA";
    std::wstring upColor = L"#31D98B";
    std::wstring downColor = L"#FF5B67";
    std::wstring flatColor = L"#A0AAB9";
    std::wstring mutedColor = L"#A0AAB9";
};

void RenderWidget(HWND hwnd);
bool IsBottomPosition(const Settings& settings);

SRWLOCK g_settingsLock = SRWLOCK_INIT;
[[clang::no_destroy]] Settings g_settings;
MarketSnapshot g_snapshot;

HANDLE g_stopEvent = nullptr;
HANDLE g_refreshEvent = nullptr;
HANDLE g_windowReadyEvent = nullptr;
HANDLE g_uiThread = nullptr;
HANDLE g_dataThread = nullptr;
std::atomic<HWND> g_window{nullptr};
std::atomic<bool> g_uiFailed{false};
std::atomic<bool> g_isFetching{false};
std::atomic<ULONGLONG> g_lastUpdateTick{0};
bool g_windowRegionValid = false;  // UI-thread only; owner clipping status.
bool g_compositionGlassActive = false;  // UI-thread only; owner UI is transparent while glass is shown.

// These values are only read/written on the UI thread.
bool g_expanded = false;
bool g_hiddenForFullscreen = false;
int g_activeCategory = 0;  // 0 = Crypto, 1 = US, 2 = IDX
int g_selectedAsset = 0;
float g_animationProgress = 0.0f;
float g_animationFrom = 0.0f;
float g_animationTo = 0.0f;
double g_animationStartVelocity = 0.0;
double g_animationCurrentVelocity = 0.0;
ULONGLONG g_animationStartTick = 0;  // Fallback clock if QPC is unavailable.
LARGE_INTEGER g_animationStartQpc{};
LARGE_INTEGER g_animationQpcFrequency{};
int g_animationDuration = 420;
bool g_animationTimerResolutionActive = false;
HANDLE g_animationQueueTimer = nullptr;  // UI-thread owned timer-queue handle.
std::atomic<bool> g_animationFramePending{false};
bool g_animating = false;
bool g_searchFocused = false;
bool g_searchSubmitted = false;
bool g_searchLoading = false;
bool g_selectedCustom = false;
bool g_trackingMouse = false;
HitTarget g_hoverTarget = HitTarget::None;
HitTarget g_pressedTarget = HitTarget::None;
std::wstring g_searchText;
SearchResult g_searchResults[5];
int g_searchResultCount = 0;
unsigned int g_searchRequestId = 0;
SearchResult g_customAsset;
Quote g_customQuote;
ULONG_PTR g_gdiplusToken = 0;

// The animation surface is reused between frames. The previous renderer
// created/deleted a compatible DC and a full-size DIB on every tick, adding
// avoidable allocator and GDI handle churn to the animation hot path.
struct RenderSurface {
    HDC dc = nullptr;
    HBITMAP bitmap = nullptr;
    HGDIOBJ previousBitmap = nullptr;
    void* pixels = nullptr;
    int width = 0;
    int height = 0;

    bool Ensure(int requestedWidth, int requestedHeight) {
        if (dc && bitmap && pixels && width == requestedWidth &&
            height == requestedHeight) {
            return true;
        }
        Reset();

        HDC screenDc = GetDC(nullptr);
        if (!screenDc) return false;
        HDC newDc = CreateCompatibleDC(screenDc);
        if (!newDc) {
            ReleaseDC(nullptr, screenDc);
            return false;
        }

        BITMAPINFO info{};
        info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        info.bmiHeader.biWidth = requestedWidth;
        info.bmiHeader.biHeight = -requestedHeight;
        info.bmiHeader.biPlanes = 1;
        info.bmiHeader.biBitCount = 32;
        info.bmiHeader.biCompression = BI_RGB;

        void* newPixels = nullptr;
        HBITMAP newBitmap = CreateDIBSection(
            screenDc, &info, DIB_RGB_COLORS, &newPixels, nullptr, 0);
        ReleaseDC(nullptr, screenDc);
        if (!newBitmap || !newPixels) {
            if (newBitmap) DeleteObject(newBitmap);
            DeleteDC(newDc);
            return false;
        }

        HGDIOBJ oldBitmap = SelectObject(newDc, newBitmap);
        if (!oldBitmap || oldBitmap == HGDI_ERROR) {
            DeleteObject(newBitmap);
            DeleteDC(newDc);
            return false;
        }

        dc = newDc;
        bitmap = newBitmap;
        previousBitmap = oldBitmap;
        pixels = newPixels;
        width = requestedWidth;
        height = requestedHeight;
        return true;
    }

    void Reset() {
        if (dc && previousBitmap && previousBitmap != HGDI_ERROR) {
            SelectObject(dc, previousBitmap);
        }
        if (bitmap) DeleteObject(bitmap);
        if (dc) DeleteDC(dc);
        dc = nullptr;
        bitmap = nullptr;
        previousBitmap = nullptr;
        pixels = nullptr;
        width = 0;
        height = 0;
    }
};

RenderSurface g_renderSurface;

struct CachedRenderFonts {
    std::wstring face;
    int priceSize = -1;
    std::unique_ptr<Gdiplus::FontFamily> family;
    std::unique_ptr<Gdiplus::Font> pillSmall;
    std::unique_ptr<Gdiplus::Font> pillMain;
    std::unique_ptr<Gdiplus::Font> pillPrice;
    std::unique_ptr<Gdiplus::Font> pillChange;
    std::unique_ptr<Gdiplus::Font> title;
    std::unique_ptr<Gdiplus::Font> subtitle;
    std::unique_ptr<Gdiplus::Font> category;
    std::unique_ptr<Gdiplus::Font> ticker;
    std::unique_ptr<Gdiplus::Font> name;
    std::unique_ptr<Gdiplus::Font> rowPrice;
    std::unique_ptr<Gdiplus::Font> change;
    std::unique_ptr<Gdiplus::Font> footer;

    void Reset() {
        footer.reset(); change.reset(); rowPrice.reset(); name.reset();
        ticker.reset(); category.reset(); subtitle.reset(); title.reset();
        pillChange.reset(); pillPrice.reset(); pillMain.reset(); pillSmall.reset();
        family.reset();
        face.clear();
        priceSize = -1;
    }

    bool Ensure(const Settings& settings) {
        if (family && face == settings.fontFace &&
            priceSize == settings.fontSize) {
            return true;
        }
        Reset();

        family = std::make_unique<Gdiplus::FontFamily>(settings.fontFace.c_str());
        if (family->GetLastStatus() != Gdiplus::Ok) {
            family = std::make_unique<Gdiplus::FontFamily>(L"Segoe UI");
        }
        if (family->GetLastStatus() != Gdiplus::Ok) {
            Reset();
            return false;
        }

        auto makeFont = [&](Gdiplus::REAL size, INT style) {
            return std::make_unique<Gdiplus::Font>(
                family.get(), size, style, Gdiplus::UnitPixel);
        };
        pillSmall = makeFont(8.0f, Gdiplus::FontStyleRegular);
        pillMain = makeFont(11.0f, Gdiplus::FontStyleBold);
        pillPrice = makeFont(static_cast<Gdiplus::REAL>(settings.fontSize),
                             Gdiplus::FontStyleBold);
        pillChange = makeFont(9.0f, Gdiplus::FontStyleBold);
        title = makeFont(16.0f, Gdiplus::FontStyleBold);
        subtitle = makeFont(9.0f, Gdiplus::FontStyleRegular);
        category = makeFont(9.0f, Gdiplus::FontStyleBold);
        ticker = makeFont(10.0f, Gdiplus::FontStyleBold);
        name = makeFont(8.0f, Gdiplus::FontStyleRegular);
        rowPrice = makeFont(10.0f, Gdiplus::FontStyleBold);
        change = makeFont(9.0f, Gdiplus::FontStyleBold);
        footer = makeFont(8.0f, Gdiplus::FontStyleRegular);
        face = settings.fontFace;
        priceSize = settings.fontSize;
        return true;
    }
};

CachedRenderFonts g_renderFonts;

std::wstring ReadStringSetting(PCWSTR name, const wchar_t* fallback) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring result = (value && *value) ? value : fallback;
    if (value) {
        Wh_FreeStringSetting(value);
    }
    return result;
}

void LoadSettings() {
    Settings settings;
    settings.refreshSeconds = std::clamp(Wh_GetIntSetting(L"RefreshSeconds"), 60, 3600);
    settings.offsetX = std::clamp(Wh_GetIntSetting(L"OffsetX"), -1000, 1000);
    int configuredOffsetY = Wh_GetIntSetting(L"OffsetY");
    if (configuredOffsetY == 10) configuredOffsetY = 30;
    settings.offsetY = std::clamp(configuredOffsetY, 0, 1000);
    settings.fontSize = std::clamp(Wh_GetIntSetting(L"FontSize"), 12, 28);
    int configuredTint = Wh_GetIntSetting(L"FluentGlassTint");
    if (configuredTint == 84) configuredTint = 116;
    settings.glassTintOpacity = std::clamp(configuredTint, 0, 255);
    settings.shapeOpacity = std::clamp(Wh_GetIntSetting(L"ShapeOpacity"), 0, 100);
    settings.scalePercent = std::clamp(Wh_GetIntSetting(L"ScalePercent"), 70, 150);
    settings.pillWidth = std::clamp(Wh_GetIntSetting(L"PillWidth"), 320, 560);
    int configuredPillHeight = Wh_GetIntSetting(L"PillHeight");
    if (configuredPillHeight == 52) configuredPillHeight = 44;
    settings.pillHeight = std::clamp(configuredPillHeight, 44, 76);
    settings.animationDuration = std::clamp(Wh_GetIntSetting(L"AnimationDuration"), 100, 800);
    settings.alwaysOnTop = Wh_GetIntSetting(L"AlwaysOnTop") != 0;
    settings.hideWhenFullscreen = Wh_GetIntSetting(L"HideWhenFullscreen") != 0;

    // Lock the geometry engine to one supported layout. This also overrides
    // legacy values saved by earlier versions of the mod.
    settings.dockMode = L"Floating";
    settings.shape = L"Capsule";

    // Reject stale or manually edited values so every position has a matching
    // pill anchor, panel anchor, and animation direction.
    settings.position = ReadStringSetting(L"Position", L"TopCenter");
    if (settings.position != L"TopLeft" &&
        settings.position != L"TopCenter" &&
        settings.position != L"TopRight" &&
        settings.position != L"BottomLeft" &&
        settings.position != L"BottomCenter" &&
        settings.position != L"BottomRight") {
        settings.position = L"TopCenter";
    }

    settings.theme = ReadStringSetting(L"GlassTheme", L"Fluent");
    settings.blurStyle = ReadStringSetting(L"BlurStyle", L"Acrylic");
    settings.fontFace = ReadStringSetting(L"FontFace", L"Segoe UI");
    settings.textColor = ReadStringSetting(L"TextColor", L"#F5F7FA");
    settings.upColor = ReadStringSetting(L"UpColor", L"#31D98B");
    settings.downColor = ReadStringSetting(L"DownColor", L"#FF5B67");
    settings.flatColor = ReadStringSetting(L"FlatColor", L"#A0AAB9");
    settings.mutedColor = ReadStringSetting(L"MutedColor", L"#A0AAB9");
    if (settings.theme == L"Light") {
        if (settings.textColor == L"#F5F7FA") settings.textColor = L"#1B2430";
        if (settings.mutedColor == L"#A0AAB9") settings.mutedColor = L"#596579";
    }

    AcquireSRWLockExclusive(&g_settingsLock);
    g_settings = std::move(settings);
    ReleaseSRWLockExclusive(&g_settingsLock);
}

Settings GetSettings() {
    AcquireSRWLockShared(&g_settingsLock);
    Settings copy = g_settings;
    ReleaseSRWLockShared(&g_settingsLock);
    return copy;
}

struct ThemePalette {
    BYTE topR, topG, topB;
    BYTE bottomR, bottomG, bottomB;
    BYTE pillR, pillG, pillB;
    BYTE borderR, borderG, borderB;
};

ThemePalette GetThemePalette(const Settings& settings) {
    if (settings.theme == L"Fluent")
        return {36, 20, 28, 12, 8, 14, 22, 13, 20, 145, 125, 145};
    if (settings.theme == L"Nusa")
        return {26, 47, 83, 22, 24, 61, 8, 13, 31, 70, 196, 255};
    if (settings.theme == L"Purple")
        return {62, 43, 83, 24, 22, 42, 23, 16, 34, 185, 145, 235};
    if (settings.theme == L"Ocean")
        return {22, 65, 83, 12, 28, 43, 9, 27, 40, 90, 190, 220};
    if (settings.theme == L"Aurora")
        return {28, 68, 67, 17, 30, 44, 12, 31, 34, 95, 220, 180};
    if (settings.theme == L"Light")
        return {235, 240, 248, 205, 216, 232, 225, 232, 244, 115, 130, 155};
    return {38, 36, 61, 19, 24, 39, 8, 10, 17, 150, 170, 205};
}

float UiScale(const Settings& settings) {
    return static_cast<float>(settings.scalePercent) / 100.0f;
}

float ShapeOpacityFactor(const Settings& settings) {
    return static_cast<float>(std::clamp(settings.shapeOpacity, 0, 100)) / 100.0f;
}

BYTE ApplyShapeOpacity(BYTE alpha, const Settings& settings) {
    const unsigned int percent = static_cast<unsigned int>(
        std::clamp(settings.shapeOpacity, 0, 100));
    return static_cast<BYTE>(
        (static_cast<unsigned int>(alpha) * percent + 50u) / 100u);
}

float ShapeRadius(const Settings& settings, float height, bool panel = false) {
    if (settings.shape == L"Square") return panel ? 7.0f : 5.0f;
    if (settings.shape == L"Rounded") return panel ? 18.0f : 13.0f;
    return panel ? 27.0f : height / 2.0f;
}

double AnimationElapsedMs() {
    double elapsedMs =
        static_cast<double>(GetTickCount64() - g_animationStartTick);
    LARGE_INTEGER now{};
    if (g_animationQpcFrequency.QuadPart > 0 &&
        QueryPerformanceCounter(&now)) {
        elapsedMs = 1000.0 * static_cast<double>(
            now.QuadPart - g_animationStartQpc.QuadPart) /
            static_cast<double>(g_animationQpcFrequency.QuadPart);
    }
    return std::max(0.0, elapsedMs);
}

void SampleAnimation(double elapsedMs, float* progress, double* velocity) {
    const double duration = static_cast<double>(std::max(1, g_animationDuration));
    if (elapsedMs >= duration) {
        *progress = g_animationTo;
        *velocity = 0.0;
        return;
    }

    // A normalized critically-damped response gives the island its quick,
    // spring-like expansion. Two endpoint-preserving Hermite correction terms
    // carry velocity across hover reversals and force the final velocity to
    // zero. This avoids both a one-pixel snap at the end and a visible pause
    // when the pointer reverses direction mid-transition.
    constexpr double settleFactor = 4.0;
    const double u = std::clamp(elapsedMs / duration, 0.0, 1.0);
    const double decay = std::exp(-settleFactor * u);
    const double endValue =
        1.0 - (1.0 + settleFactor) * std::exp(-settleFactor);
    const double springValue =
        (1.0 - (1.0 + settleFactor * u) * decay) / endValue;
    const double springDerivative =
        (settleFactor * settleFactor * u * decay) / endValue;
    const double endDerivative =
        (settleFactor * settleFactor * std::exp(-settleFactor)) / endValue;

    const double u2 = u * u;
    const double h0 = u * (1.0 - u) * (1.0 - u);
    const double h0Derivative = 1.0 - 4.0 * u + 3.0 * u2;
    const double h1 = u2 * (u - 1.0);
    const double h1Derivative = 3.0 * u2 - 2.0 * u;
    const double distance = static_cast<double>(g_animationTo) - g_animationFrom;
    const double velocityTerm = g_animationStartVelocity * duration;

    double value = static_cast<double>(g_animationFrom) +
                   distance * springValue +
                   velocityTerm * h0 -
                   distance * endDerivative * h1;
    double speed = (distance * springDerivative +
                    velocityTerm * h0Derivative -
                    distance * endDerivative * h1Derivative) / duration;

    if (value < 0.0 || value > 1.0) {
        value = std::clamp(value, 0.0, 1.0);
        if ((value <= 0.0 && speed < 0.0) ||
            (value >= 1.0 && speed > 0.0)) {
            speed = 0.0;
        }
    }
    *progress = static_cast<float>(value);
    *velocity = speed;
}

bool IsHovered(HitTarget target) {
    return g_hoverTarget == target;
}

bool IsPressed(HitTarget target) {
    return g_pressedTarget == target;
}

int EffectiveTopDockWidth(const Settings& settings) {
    POINT origin{0, 0};
    HMONITOR monitor = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        return settings.topDockWidth;
    }

    const int monitorWidth = info.rcMonitor.right - info.rcMonitor.left;
    const int available = std::max(240, monitorWidth - 32);
    return std::clamp(settings.topDockWidth, 240, available);
}

float CurrentLogicalWidth(const Settings& settings, float progress) {
    progress = std::clamp(progress, 0.0f, 1.0f);
    if (settings.dockMode == L"TopAttached") {
        return static_cast<float>(EffectiveTopDockWidth(settings));
    }
    if (settings.dockMode == L"SideAttached") {
        return static_cast<float>(kSideDockWidth) +
               static_cast<float>(kSideExpandedWidth - kSideDockWidth) * progress;
    }
    // Keep the floating owner window at its final size. Only the visible
    // silhouette changes, so the HWND does not move/resize under the cursor.
    return static_cast<float>(std::max(kExpandedWidth, settings.pillWidth));
}

float CurrentLogicalHeight(const Settings& settings, float progress) {
    progress = std::clamp(progress, 0.0f, 1.0f);
    if (settings.dockMode == L"TopAttached") {
        return static_cast<float>(kTopDockHeight) +
               static_cast<float>(kPanelHeight) * progress;
    }
    if (settings.dockMode == L"SideAttached") {
        return static_cast<float>(settings.sideDockHeight) +
               static_cast<float>(std::max(kExpandedHeight, settings.sideDockHeight) -
                                  settings.sideDockHeight) * progress;
    }
    return static_cast<float>(settings.pillHeight + kPanelHeight);
}

float CurrentPanelVisibleHeight(const Settings& settings, float progress) {
    progress = std::clamp(progress, 0.0f, 1.0f);
    if (settings.dockMode == L"SideAttached") {
        return static_cast<float>(kPanelHeight);
    }
    return static_cast<float>(kPanelHeight) * progress;
}

float PanelLeft(const Settings& settings, float logicalWidth) {
    if (settings.dockMode == L"SideAttached") {
        return settings.dockSide == L"Left" ? static_cast<float>(kSideDockWidth) : 0.0f;
    }
    if (settings.dockMode == L"Floating") {
        if (settings.position == L"TopLeft" || settings.position == L"BottomLeft")
            return 0.0f;
        if (settings.position == L"TopRight" || settings.position == L"BottomRight")
            return logicalWidth - static_cast<float>(kExpandedWidth);
    }
    return (logicalWidth - static_cast<float>(kExpandedWidth)) / 2.0f;
}

float CurrentPanelTop(const Settings& settings, float progress,
                      float logicalHeight) {
    progress = std::clamp(progress, 0.0f, 1.0f);
    if (settings.dockMode == L"TopAttached") {
        return static_cast<float>(kTopDockHeight);
    }
    if (settings.dockMode == L"SideAttached") {
        return (logicalHeight - static_cast<float>(kPanelHeight)) / 2.0f;
    }
    // In the fixed-size floating canvas, bottom panels reveal upward while
    // top panels reveal downward. Their screen coordinates stay deterministic.
    return IsBottomPosition(settings)
        ? static_cast<float>(kPanelHeight) * (1.0f - progress)
        : static_cast<float>(settings.pillHeight);
}

float FloatingPillLeft(const Settings& settings, float logicalWidth) {
    if (settings.position == L"TopLeft" || settings.position == L"BottomLeft")
        return 0.0f;
    if (settings.position == L"TopRight" || settings.position == L"BottomRight")
        return logicalWidth - static_cast<float>(settings.pillWidth);
    return (logicalWidth - static_cast<float>(settings.pillWidth)) / 2.0f;
}

float FloatingPillTop(const Settings& settings, float logicalHeight) {
    return IsBottomPosition(settings)
        ? logicalHeight - static_cast<float>(settings.pillHeight) : 0.0f;
}

Gdiplus::RectF FloatingIslandRect(const Settings& settings,
                                  float logicalWidth, float logicalHeight,
                                  float progress) {
    // The selected anchor is invariant throughout the morph:
    // left/right edges stay fixed for side anchors, the center stays fixed for
    // center anchors, and the top/bottom edge stays fixed vertically.
    progress = std::clamp(progress, 0.0f, 1.0f);
    const float pillLeft = FloatingPillLeft(settings, logicalWidth);
    const float panelLeft = PanelLeft(settings, logicalWidth);
    const float left = pillLeft + (panelLeft - pillLeft) * progress;
    const float top = IsBottomPosition(settings)
        ? CurrentPanelTop(settings, progress, logicalHeight) : 0.0f;
    const float width = static_cast<float>(settings.pillWidth) +
        (static_cast<float>(kExpandedWidth) - settings.pillWidth) * progress;
    const float height = static_cast<float>(settings.pillHeight) +
        static_cast<float>(kPanelHeight) * progress;
    return Gdiplus::RectF(left, top, width, height);
}

float FloatingIslandRadius(const Settings& settings, float progress) {
    progress = std::clamp(progress, 0.0f, 1.0f);
    const float compactRadius = ShapeRadius(
        settings, static_cast<float>(settings.pillHeight));
    const float expandedRadius = ShapeRadius(
        settings, static_cast<float>(kPanelHeight), true);
    return compactRadius + (expandedRadius - compactRadius) * progress;
}

UINT AnimationFrameIntervalMs(HWND hwnd) {
    // DWM reports the compositor's refresh period in QPC ticks. Using that
    // period keeps animation work close to the active monitor's refresh rate.
    DWM_TIMING_INFO timing{};
    timing.cbSize = sizeof(timing);
    LARGE_INTEGER frequency{};
    if (SUCCEEDED(DwmGetCompositionTimingInfo(hwnd, &timing)) &&
        timing.qpcRefreshPeriod > 0 &&
        QueryPerformanceFrequency(&frequency) && frequency.QuadPart > 0) {
        const double periodMs =
            1000.0 * static_cast<double>(timing.qpcRefreshPeriod) /
            static_cast<double>(frequency.QuadPart);
        return static_cast<UINT>(
            std::clamp<long long>(static_cast<long long>(std::floor(periodMs + 0.25)), 1LL, 33LL));
    }

    // Fallback for systems where DWM timing information is unavailable.
    HDC dc = GetDC(hwnd);
    const int refreshHz = dc ? GetDeviceCaps(dc, VREFRESH) : 0;
    if (dc) ReleaseDC(hwnd, dc);
    if (refreshHz > 1) {
        return static_cast<UINT>(
            std::clamp<long long>(static_cast<long long>(std::floor(1000.0 / refreshHz + 0.25)), 1LL, 33LL));
    }
    return 16;
}

VOID CALLBACK AnimationTimerQueueProc(PVOID context, BOOLEAN) {
    HWND hwnd = static_cast<HWND>(context);
    if (!hwnd) return;
    bool expected = false;
    if (!g_animationFramePending.compare_exchange_strong(
            expected, true, std::memory_order_acq_rel)) {
        return;
    }
    if (!PostMessageW(hwnd, kMessageAnimationFrame, 0, 0)) {
        g_animationFramePending.store(false, std::memory_order_release);
    }
}

bool StartAnimationFrameTimer(HWND hwnd) {
    if (!hwnd) return false;
    // Reversing an in-flight animation changes its time origin, not its timer.
    // Reuse the existing periodic source instead of leaking another timer.
    if (g_animationQueueTimer) return true;

    const UINT interval = AnimationFrameIntervalMs(hwnd);
    if (CreateTimerQueueTimer(&g_animationQueueTimer, nullptr,
                              AnimationTimerQueueProc, hwnd,
                              interval, interval, WT_EXECUTEDEFAULT)) {
        KillTimer(hwnd, kAnimationTimer);
        return true;
    }
    g_animationQueueTimer = nullptr;
    // Fallback for systems where timer-queue creation is restricted.
    return SetTimer(hwnd, kAnimationTimer, interval, nullptr) != 0;
}

void StopAnimationTimer(HWND hwnd) {
    if (hwnd) KillTimer(hwnd, kAnimationTimer);
    if (g_animationQueueTimer) {
        HANDLE timer = g_animationQueueTimer;
        g_animationQueueTimer = nullptr;
        DeleteTimerQueueTimer(nullptr, timer, INVALID_HANDLE_VALUE);
    }
    g_animationFramePending.store(false, std::memory_order_release);
    g_animationCurrentVelocity = 0.0;
    g_animationStartVelocity = 0.0;
    if (g_animationTimerResolutionActive) {
        timeEndPeriod(1);
        g_animationTimerResolutionActive = false;
    }
    g_animating = false;
}

bool RestartAnimationFrameTimer(HWND hwnd) {
    if (!hwnd || !g_animating) return false;
    if (g_animationQueueTimer) {
        HANDLE timer = g_animationQueueTimer;
        g_animationQueueTimer = nullptr;
        DeleteTimerQueueTimer(nullptr, timer, INVALID_HANDLE_VALUE);
    }
    KillTimer(hwnd, kAnimationTimer);
    g_animationFramePending.store(false, std::memory_order_release);
    return StartAnimationFrameTimer(hwnd);
}

void AdvanceAnimationFrame(HWND hwnd) {
    if (!hwnd || !g_animating) return;

    const double elapsedMs = AnimationElapsedMs();
    SampleAnimation(elapsedMs, &g_animationProgress,
                    &g_animationCurrentVelocity);
    if (elapsedMs >= static_cast<double>(g_animationDuration)) {
        g_animationProgress = g_animationTo;
        g_animationCurrentVelocity = 0.0;
        StopAnimationTimer(hwnd);
    }
    RenderWidget(hwnd);
}

void StartAnimation(HWND hwnd, bool expand) {
    if (!hwnd) return;
    bool completedPreviousAnimation = false;

    // Sample the old trajectory at the exact retarget time. Using the last
    // painted progress here can rewind the island by one or two frames.
    if (g_animating) {
        const double elapsedMs = AnimationElapsedMs();
        SampleAnimation(elapsedMs, &g_animationProgress,
                        &g_animationCurrentVelocity);
        if (elapsedMs >= static_cast<double>(g_animationDuration)) {
            g_animationProgress = g_animationTo;
            g_animationCurrentVelocity = 0.0;
            StopAnimationTimer(hwnd);
            completedPreviousAnimation = true;
        }
    }

    const float target = expand ? 1.0f : 0.0f;
    if (g_animating && std::fabs(g_animationTo - target) < 0.001f) {
        return;
    }
    if (!g_animating && std::fabs(g_animationProgress - target) < 0.001f) {
        g_expanded = expand;
        g_animationCurrentVelocity = 0.0;
        if (completedPreviousAnimation) RenderWidget(hwnd);
        return;
    }

    Settings settings = GetSettings();
    g_animationFrom = g_animationProgress;
    g_animationTo = target;
    g_animationDuration = std::clamp(
        static_cast<int>(std::lround(
            settings.animationDuration * std::fabs(target - g_animationFrom))),
        90, settings.animationDuration);

    // Keep velocity continuous but cap it to a safe envelope. During a
    // reversal, allow only a tiny inertial continuation before changing
    // direction; this feels spring-like without overshooting the pill/panel.
    const double distance = static_cast<double>(target - g_animationFrom);
    const double omega = 4.0 / static_cast<double>(g_animationDuration);
    double velocity = g_animationCurrentVelocity;
    if (std::fabs(distance) > 0.0001) {
        const double maxToward = 2.5 * std::fabs(distance) /
                                 static_cast<double>(g_animationDuration);
        const double maxOpposing = 0.05 * std::fabs(distance) * omega;
        const double limit = velocity * distance < 0.0
            ? maxOpposing : maxToward;
        velocity = std::copysign(std::min(std::fabs(velocity), limit), velocity);
    } else {
        velocity = 0.0;
    }
    g_animationStartVelocity = velocity;
    g_animationCurrentVelocity = velocity;

    g_animationStartTick = GetTickCount64();
    g_animationQpcFrequency = LARGE_INTEGER{};
    g_animationStartQpc = LARGE_INTEGER{};
    LARGE_INTEGER qpcFrequency{};
    LARGE_INTEGER qpcStart{};
    if (QueryPerformanceFrequency(&qpcFrequency) &&
        QueryPerformanceCounter(&qpcStart)) {
        g_animationQpcFrequency = qpcFrequency;
        g_animationStartQpc = qpcStart;
    }

    g_expanded = expand;
    g_animating = true;

    if (!g_animationTimerResolutionActive &&
        timeBeginPeriod(1) == TIMERR_NOERROR) {
        g_animationTimerResolutionActive = true;
    }
    if (!StartAnimationFrameTimer(hwnd)) {
        g_animationProgress = target;
        g_animationCurrentVelocity = 0.0;
        StopAnimationTimer(hwnd);
    }
    RenderWidget(hwnd);
}

Gdiplus::Color ParseColor(const std::wstring& value, BYTE alpha = 255) {
    unsigned int rgb = 0xF5F7FA;
    const wchar_t* text = value.c_str();
    if (*text == L'#') {
        ++text;
    }
    if (wcslen(text) == 6) {
        wchar_t* end = nullptr;
        unsigned long parsed = wcstoul(text, &end, 16);
        if (end && *end == L'\0') {
            rgb = static_cast<unsigned int>(parsed);
        }
    }
    return Gdiplus::Color(alpha,
                          static_cast<BYTE>((rgb >> 16) & 0xFF),
                          static_cast<BYTE>((rgb >> 8) & 0xFF),
                          static_cast<BYTE>(rgb & 0xFF));
}

bool ExtractJsonNumber(const std::string& json, const char* key, double* value) {
    const std::string needle = std::string("\"") + key + "\"";
    size_t position = json.find(needle);
    if (position == std::string::npos) return false;
    position = json.find(':', position + needle.size());
    if (position == std::string::npos) return false;
    ++position;
    while (position < json.size() &&
           (json[position] == ' ' || json[position] == '\t' ||
            json[position] == '\r' || json[position] == '\n')) {
        ++position;
    }
    if (position >= json.size() || json.compare(position, 4, "null") == 0) return false;
    char* end = nullptr;
    double parsed = std::strtod(json.c_str() + position, &end);
    if (end == json.c_str() + position || !std::isfinite(parsed)) return false;
    *value = parsed;
    return true;
}

std::wstring UrlEncode(const std::wstring& value);

std::string HttpGetYahooPath(const std::wstring& path) {
    HINTERNET session = WinHttpOpen(
        L"Mozilla/5.0 (Windows NT 10.0; Win64; x64) FluentMarketIsland/2.9.2",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (!session) return {};
    WinHttpSetTimeouts(session, 2000, 2500, 2500, 2500);

    HINTERNET connection = WinHttpConnect(
        session, L"query1.finance.yahoo.com", INTERNET_DEFAULT_HTTPS_PORT, 0);
    if (!connection) {
        WinHttpCloseHandle(session);
        return {};
    }

    HINTERNET request = WinHttpOpenRequest(
        connection, L"GET", path.c_str(), nullptr, WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);

    std::string response;
    if (request) {
        WinHttpAddRequestHeaders(request, L"Accept: application/json\r\n",
                                 static_cast<DWORD>(-1),
                                 WINHTTP_ADDREQ_FLAG_ADD | WINHTTP_ADDREQ_FLAG_REPLACE);
        BOOL sent = WinHttpSendRequest(request, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                                       WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
        if (sent && WinHttpReceiveResponse(request, nullptr)) {
            DWORD statusCode = 0;
            DWORD statusSize = sizeof(statusCode);
            if (WinHttpQueryHeaders(request,
                                    WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
                                    WINHTTP_HEADER_NAME_BY_INDEX,
                                    &statusCode, &statusSize,
                                    WINHTTP_NO_HEADER_INDEX) && statusCode == 200) {
                char buffer[8192];
                DWORD bytesRead = 0;
                while (WinHttpReadData(request, buffer, sizeof(buffer), &bytesRead) && bytesRead > 0) {
                    if (response.size() + bytesRead > 1024 * 1024) {
                        response.clear();
                        break;
                    }
                    response.append(buffer, bytesRead);
                }
            }
        }
        WinHttpCloseHandle(request);
    }
    WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);
    return response;
}

std::string HttpGetYahoo(const std::wstring& symbol) {
    return HttpGetYahooPath(L"/v8/finance/chart/" + UrlEncode(symbol) + L"?range=1d&interval=1m");
}

Quote FetchQuote(const wchar_t* symbol) {
    Quote quote;
    std::string json = HttpGetYahoo(symbol);
    if (json.empty() || !ExtractJsonNumber(json, "regularMarketPrice", &quote.price)) return quote;
    if (!ExtractJsonNumber(json, "chartPreviousClose", &quote.previousClose)) {
        ExtractJsonNumber(json, "previousClose", &quote.previousClose);
    }
    if (quote.price <= 0.0) return Quote{};
    if (quote.previousClose > 0.0) {
        quote.changePercent = (quote.price / quote.previousClose - 1.0) * 100.0;
    }
    quote.valid = true;
    return quote;
}

std::wstring Utf8ToWide(const std::string& value) {
    if (value.empty()) return {};
    int count = MultiByteToWideChar(CP_UTF8, 0, value.c_str(),
                                    static_cast<int>(value.size()), nullptr, 0);
    if (count <= 0) return {};
    std::wstring result(static_cast<size_t>(count), L'\0');
    MultiByteToWideChar(CP_UTF8, 0, value.c_str(),
                        static_cast<int>(value.size()), result.data(), count);
    return result;
}

std::string WideToUtf8(const std::wstring& value) {
    if (value.empty()) return {};
    int count = WideCharToMultiByte(CP_UTF8, 0, value.c_str(),
                                    static_cast<int>(value.size()), nullptr, 0,
                                    nullptr, nullptr);
    if (count <= 0) return {};
    std::string result(static_cast<size_t>(count), '\0');
    WideCharToMultiByte(CP_UTF8, 0, value.c_str(),
                        static_cast<int>(value.size()), result.data(), count,
                        nullptr, nullptr);
    return result;
}

std::wstring UrlEncode(const std::wstring& value) {
    static const wchar_t hex[] = L"0123456789ABCDEF";
    std::string utf8 = WideToUtf8(value);
    std::wstring encoded;
    for (unsigned char ch : utf8) {
        if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z') ||
            (ch >= '0' && ch <= '9') || ch == '-' || ch == '_' ||
            ch == '.' || ch == '~') {
            encoded.push_back(static_cast<wchar_t>(ch));
        } else {
            encoded.push_back(L'%');
            encoded.push_back(hex[(ch >> 4) & 0xF]);
            encoded.push_back(hex[ch & 0xF]);
        }
    }
    return encoded;
}

bool ExtractJsonString(const std::string& object, const char* key, std::string* value) {
    const std::string needle = std::string("\"") + key + "\"";
    size_t pos = object.find(needle);
    if (pos == std::string::npos) return false;
    pos = object.find(':', pos + needle.size());
    if (pos == std::string::npos) return false;
    ++pos;
    while (pos < object.size() && (object[pos] == ' ' || object[pos] == '\t' ||
           object[pos] == '\r' || object[pos] == '\n')) ++pos;
    if (pos >= object.size() || object[pos] != '\"') return false;
    ++pos;
    std::string output;
    bool escaped = false;
    for (; pos < object.size(); ++pos) {
        char ch = object[pos];
        if (escaped) {
            switch (ch) {
                case 'n': output.push_back('\n'); break;
                case 'r': output.push_back('\r'); break;
                case 't': output.push_back('\t'); break;
                default: output.push_back(ch); break;
            }
            escaped = false;
        } else if (ch == '\\') {
            escaped = true;
        } else if (ch == '\"') {
            *value = std::move(output);
            return true;
        } else {
            output.push_back(ch);
        }
    }
    return false;
}

int ParseYahooSearchResults(const std::string& json, SearchResult* results, int maxResults) {
    size_t quotesPos = json.find("\"quotes\"");
    if (quotesPos == std::string::npos) return 0;
    size_t arrayStart = json.find('[', quotesPos);
    if (arrayStart == std::string::npos) return 0;
    int count = 0;
    bool inString = false;
    bool escaped = false;
    int depth = 0;
    size_t objectStart = std::string::npos;
    for (size_t i = arrayStart + 1; i < json.size() && count < maxResults; ++i) {
        char ch = json[i];
        if (inString) {
            if (escaped) escaped = false;
            else if (ch == '\\') escaped = true;
            else if (ch == '\"') inString = false;
            continue;
        }
        if (ch == '\"') { inString = true; continue; }
        if (ch == '{') {
            if (depth == 0) objectStart = i;
            ++depth;
        } else if (ch == ']' && depth == 0) {
            break;
        } else if (ch == '}') {
            --depth;
            if (depth == 0 && objectStart != std::string::npos) {
                std::string object = json.substr(objectStart, i - objectStart + 1);
                std::string symbol, shortName, longName, quoteType, exchange, currency;
                if (ExtractJsonString(object, "symbol", &symbol) &&
                    ExtractJsonString(object, "quoteType", &quoteType)) {
                    if (quoteType == "EQUITY" || quoteType == "ETF" ||
                        quoteType == "CRYPTO" || quoteType == "CRYPTOCURRENCY" ||
                        quoteType == "MUTUALFUND" ||
                        quoteType == "INDEX") {
                        ExtractJsonString(object, "shortname", &shortName);
                        if (shortName.empty()) ExtractJsonString(object, "shortName", &shortName);
                        ExtractJsonString(object, "longname", &longName);
                        if (longName.empty()) ExtractJsonString(object, "longName", &longName);
                        ExtractJsonString(object, "exchange", &exchange);
                        ExtractJsonString(object, "currency", &currency);
                        SearchResult& result = results[count++];
                        result.symbol = Utf8ToWide(symbol);
                        result.ticker = result.symbol;
                        size_t dot = result.ticker.find(L'.');
                        if (dot != std::wstring::npos) result.ticker = result.ticker.substr(0, dot);
                        result.name = Utf8ToWide(!shortName.empty() ? shortName : longName);
                        if (result.name.empty()) result.name = result.ticker;
                        result.exchange = Utf8ToWide(exchange);
                        result.quoteType = Utf8ToWide(quoteType);
                        result.rupiah = currency == "IDR" || symbol.size() >= 3 &&
                                        symbol.compare(symbol.size() - 3, 3, ".JK") == 0;
                        result.decimals = result.rupiah ? 0 : ((quoteType == "CRYPTO" || quoteType == "CRYPTOCURRENCY") ? 4 : 2);
                    }
                }
                objectStart = std::string::npos;
            }
        }
    }
    return count;
}

DWORD WINAPI SearchThreadProc(void* parameter) {
    SearchRequest request = *static_cast<SearchRequest*>(parameter);
    delete static_cast<SearchRequest*>(parameter);
    auto* message = new SearchResultsMessage;
    message->requestId = request.requestId;
    std::wstring path = L"/v1/finance/search?q=" + UrlEncode(request.query) +
                        L"&quotesCount=8&newsCount=0";
    std::string json = HttpGetYahooPath(path);
    message->count = ParseYahooSearchResults(json, message->items, 5);
    HWND hwnd = g_window.load();
    if (!hwnd || !PostMessageW(hwnd, kMessageSearchResults, 0,
                               reinterpret_cast<LPARAM>(message))) {
        delete message;
    }
    return 0;
}

DWORD WINAPI CustomQuoteThreadProc(void* parameter) {
    SearchResult asset = *static_cast<SearchResult*>(parameter);
    delete static_cast<SearchResult*>(parameter);
    auto* message = new CustomQuoteMessage;
    message->symbol = asset.symbol;
    message->quote = FetchQuote(asset.symbol.c_str());
    HWND hwnd = g_window.load();
    if (!hwnd || !PostMessageW(hwnd, kMessageCustomQuote, 0,
                               reinterpret_cast<LPARAM>(message))) {
        delete message;
    }
    return 0;
}

void StartSearch() {
    if (g_searchText.empty()) return;
    g_searchSubmitted = true;
    g_searchLoading = true;
    g_searchResultCount = 0;
    RenderWidget(g_window.load());
    auto* query = new SearchRequest;
    query->query = g_searchText;
    query->requestId = ++g_searchRequestId;
    HANDLE thread = CreateThread(nullptr, 0, SearchThreadProc, query, 0, nullptr);
    if (thread) CloseHandle(thread);
    else {
        delete query;
        g_searchLoading = false;
        RenderWidget(g_window.load());
    }
}

void StartCustomQuoteFetch(const SearchResult& asset) {
    auto* copy = new SearchResult(asset);
    HANDLE thread = CreateThread(nullptr, 0, CustomQuoteThreadProc, copy, 0, nullptr);
    if (thread) CloseHandle(thread);
    else delete copy;
}

DWORD WINAPI DataThreadProc(void*) {
    for (;;) {
        g_isFetching.store(true);
        auto* snapshot = new MarketSnapshot;
        for (int i = 0; i < kAssetCount; ++i) {
            if (WaitForSingleObject(g_stopEvent, 0) == WAIT_OBJECT_0) {
                delete snapshot;
                g_isFetching.store(false);
                return 0;
            }
            snapshot->quotes[i] = FetchQuote(kAssets[i].symbol);
        }
        g_isFetching.store(false);
        g_lastUpdateTick.store(GetTickCount64());

        HWND hwnd = g_window.load();
        if (hwnd) {
            if (!PostMessageW(hwnd, kMessageMarketData, 0, reinterpret_cast<LPARAM>(snapshot))) {
                delete snapshot;
            }
        } else {
            delete snapshot;
        }

        Settings settings = GetSettings();
        HANDLE events[] = {g_stopEvent, g_refreshEvent};
        DWORD result = WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE,
                                              static_cast<DWORD>(settings.refreshSeconds) * 1000);
        if (result == WAIT_OBJECT_0) break;
    }
    return 0;
}

std::wstring FormatGrouped(double value, int decimals,
                           wchar_t decimalSeparator, wchar_t thousandsSeparator) {
    wchar_t buffer[96]{};
    swprintf_s(buffer, ARRAYSIZE(buffer), L"%.*f", decimals, value);
    std::wstring raw(buffer);
    size_t decimalPosition = raw.find(L'.');
    std::wstring whole = raw.substr(0, decimalPosition);
    std::wstring fraction = decimalPosition == std::wstring::npos ? L"" : raw.substr(decimalPosition + 1);
    size_t firstDigit = (!whole.empty() && whole[0] == L'-') ? 1 : 0;
    for (size_t position = whole.size(); position > firstDigit + 3; position -= 3) {
        whole.insert(position - 3, 1, thousandsSeparator);
    }
    return decimals > 0 ? whole + decimalSeparator + fraction : whole;
}

std::wstring FormatPrice(const AssetInfo& asset, const Quote& quote) {
    if (!quote.valid) return L"--";
    if (asset.rupiah) return L"Rp " + FormatGrouped(quote.price, asset.decimals, L',', L'.');
    return L"$" + FormatGrouped(quote.price, asset.decimals, L'.', L',');
}

std::wstring FormatChange(const Quote& quote) {
    if (!quote.valid) return L"Waiting";
    wchar_t buffer[64]{};
    swprintf_s(buffer, ARRAYSIZE(buffer), L"%+.2f%%", quote.changePercent);
    return buffer;
}

Gdiplus::Color QuoteColor(const Quote& quote, const Settings& settings) {
    if (!quote.valid || std::fabs(quote.changePercent) < 0.000001) return ParseColor(settings.flatColor);
    return quote.changePercent > 0.0 ? ParseColor(settings.upColor) : ParseColor(settings.downColor);
}

void AddRoundedRectPath(Gdiplus::GraphicsPath& path, const Gdiplus::RectF& rect, float radius) {
    const float diameter = radius * 2.0f;
    path.AddArc(rect.X, rect.Y, diameter, diameter, 180.0f, 90.0f);
    path.AddArc(rect.GetRight() - diameter, rect.Y, diameter, diameter, 270.0f, 90.0f);
    path.AddArc(rect.GetRight() - diameter, rect.GetBottom() - diameter, diameter, diameter, 0.0f, 90.0f);
    path.AddArc(rect.X, rect.GetBottom() - diameter, diameter, diameter, 90.0f, 90.0f);
    path.CloseFigure();
}

void DrawSoftShadow(Gdiplus::Graphics& graphics,
                    const Gdiplus::RectF& rect, float radius,
                    BYTE strength);

void DrawGlassCard(Gdiplus::Graphics& graphics, const Gdiplus::RectF& rect,
                   float radius, const Settings& settings) {
    if (settings.dockMode != L"Floating") {
        DrawSoftShadow(
            graphics, rect, radius,
            ApplyShapeOpacity(24, settings));
    }
    if (!g_compositionGlassActive && settings.dockMode != L"Floating") {
        ThemePalette palette = GetThemePalette(settings);
        Gdiplus::GraphicsPath path;
        AddRoundedRectPath(path, rect, radius);
        const BYTE fallbackAlpha = ApplyShapeOpacity(
            static_cast<BYTE>(
                std::clamp(settings.glassTintOpacity + 105, 145, 225)),
            settings);
        Gdiplus::LinearGradientBrush fill(
            rect,
            Gdiplus::Color(fallbackAlpha, palette.topR, palette.topG, palette.topB),
            Gdiplus::Color(fallbackAlpha, palette.bottomR, palette.bottomG, palette.bottomB),
            Gdiplus::LinearGradientModeVertical);
        graphics.FillPath(&fill, &path);
        Gdiplus::Pen border(
            Gdiplus::Color(ApplyShapeOpacity(42, settings),
                           palette.borderR, palette.borderG, palette.borderB),
            0.8f);
        graphics.DrawPath(&border, &path);
    }
}

void DrawRoundedFill(Gdiplus::Graphics& graphics, const Gdiplus::RectF& rect,
                     float radius, const Gdiplus::Color& fillColor,
                     const Gdiplus::Color& borderColor) {
    Gdiplus::GraphicsPath path;
    AddRoundedRectPath(path, rect, radius);
    Gdiplus::SolidBrush fill(fillColor);
    graphics.FillPath(&fill, &path);
    Gdiplus::Pen border(borderColor, 0.8f);
    graphics.DrawPath(&border, &path);
}

void DrawText(Gdiplus::Graphics& graphics, const std::wstring& text,
              const Gdiplus::Font& font, float x, float y,
              const Gdiplus::Color& color) {
    if (text.empty()) return;
    Gdiplus::SolidBrush brush(color);
    graphics.DrawString(text.c_str(), static_cast<INT>(text.size()), &font,
                        Gdiplus::PointF(x, y), &brush);
}

void DrawTextRight(Gdiplus::Graphics& graphics, const std::wstring& text,
                   const Gdiplus::Font& font, float x, float y,
                   float width, float height, const Gdiplus::Color& color) {
    if (text.empty()) return;
    Gdiplus::SolidBrush brush(color);
    Gdiplus::RectF layout(x, y, width, height);
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentFar);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    graphics.DrawString(text.c_str(), static_cast<INT>(text.size()), &font,
                        layout, &format, &brush);
}

void DrawTextCentered(Gdiplus::Graphics& graphics, const std::wstring& text,
                      const Gdiplus::Font& font, float x, float y,
                      float width, float height, const Gdiplus::Color& color) {
    if (text.empty()) return;
    Gdiplus::SolidBrush brush(color);
    Gdiplus::RectF layout(x, y, width, height);
    Gdiplus::StringFormat format;
    format.SetAlignment(Gdiplus::StringAlignmentCenter);
    format.SetLineAlignment(Gdiplus::StringAlignmentCenter);
    graphics.DrawString(text.c_str(), static_cast<INT>(text.size()), &font,
                        layout, &format, &brush);
}

int CategoryStart(int category) {
    return category == 1 ? kUsStart : (category == 2 ? kIdxStart : kCryptoStart);
}

bool IsBottomPosition(const Settings& settings) {
    return settings.position == L"BottomLeft" ||
           settings.position == L"BottomCenter" ||
           settings.position == L"BottomRight";
}

void DrawSoftShadow(Gdiplus::Graphics& graphics,
                    const Gdiplus::RectF& rect, float radius,
                    BYTE strength) {
    // A zero surface-opacity setting must not leave a residual shadow.
    if (strength == 0) return;

    // Layered translucent silhouettes create a soft desktop shadow without
    // introducing a rectangular opaque backing surface.
    for (int layer = 5; layer >= 1; --layer) {
        const float offset = static_cast<float>(6 - layer) * 1.15f;
        const BYTE alpha = static_cast<BYTE>(
            std::max(1, static_cast<int>(strength) / (layer * 2)));
        Gdiplus::RectF shadow(rect.X, rect.Y + offset,
                              rect.Width, rect.Height);
        Gdiplus::GraphicsPath path;
        AddRoundedRectPath(path, shadow, radius + offset * 0.35f);
        Gdiplus::SolidBrush brush(Gdiplus::Color(alpha, 0, 0, 0));
        graphics.FillPath(&brush, &path);
    }
}

void DrawPill(Gdiplus::Graphics& graphics, const Settings& settings,
              const Gdiplus::Font& smallFont, const Gdiplus::Font& mainFont,
              const Gdiplus::Font& priceFont, const Gdiplus::Font& changeFont,
              float x, float y, float width, float height) {
    // The floating glass surface is drawn once by the shared morphing shape.
    // This routine only paints the live quote content inside the header.
    AssetInfo customInfo{};
    const AssetInfo* assetPtr = &kAssets[g_selectedAsset];
    const Quote* quotePtr = &g_snapshot.quotes[g_selectedAsset];
    if (g_selectedCustom) {
        customInfo = {g_customAsset.ticker.c_str(), g_customAsset.symbol.c_str(),
                      g_customAsset.name.c_str(), g_customAsset.rupiah,
                      g_customAsset.decimals};
        assetPtr = &customInfo;
        quotePtr = &g_customQuote;
    }
    const AssetInfo& asset = *assetPtr;
    const Quote& quote = *quotePtr;
    const Gdiplus::Color changeColor = QuoteColor(quote, settings);
    const float centerY = y + height * 0.5f;

    DrawText(graphics, asset.ticker, mainFont, x + 20.0f, y + 8.0f,
             ParseColor(settings.textColor));
    std::wstring groupLabel = g_selectedCustom
        ? (g_customAsset.quoteType.empty() ? L"MARKET" : g_customAsset.quoteType)
        : (g_activeCategory == 0 ? L"CRYPTO"
           : g_activeCategory == 1 ? L"US STOCKS / ETF" : L"INDONESIA / IDX");
    DrawText(graphics, groupLabel, smallFont, x + 20.0f, y + 27.0f,
             ParseColor(settings.mutedColor));

    const float badgeWidth = width < 350.0f ? 52.0f : 60.0f;
    const float badgeX = x + width - badgeWidth - 27.0f;
    const float priceX = x + 132.0f;
    const float priceWidth = std::max(64.0f, badgeX - priceX - 8.0f);
    DrawTextRight(graphics, FormatPrice(asset, quote), priceFont,
                  priceX, y + 5.0f, priceWidth, height - 10.0f,
                  ParseColor(settings.textColor));

    const Gdiplus::RectF badgeRect(badgeX, centerY - 12.0f,
                                   badgeWidth, 24.0f);
    DrawRoundedFill(graphics, badgeRect, 10.0f,
                    Gdiplus::Color(42, changeColor.GetR(),
                                   changeColor.GetG(), changeColor.GetB()),
                    Gdiplus::Color(42, changeColor.GetR(),
                                   changeColor.GetG(), changeColor.GetB()));
    DrawTextRight(graphics, quote.valid ? FormatChange(quote) : L"--",
                  changeFont, badgeX + 2.0f, badgeRect.Y + 1.0f,
                  badgeWidth - 4.0f, 21.0f, changeColor);

}

void AddTopDockPath(Gdiplus::GraphicsPath& path, const Gdiplus::RectF& rect,
                    float radius) {
    const float r = std::min(radius, rect.Height / 2.0f);
    path.StartFigure();
    path.AddLine(rect.X, rect.Y, rect.GetRight(), rect.Y);
    path.AddLine(rect.GetRight(), rect.Y, rect.GetRight(), rect.GetBottom() - r);
    path.AddArc(rect.GetRight() - 2.0f * r, rect.GetBottom() - 2.0f * r,
                2.0f * r, 2.0f * r, 0.0f, 90.0f);
    path.AddLine(rect.GetRight() - r, rect.GetBottom(), rect.X + r, rect.GetBottom());
    path.AddArc(rect.X, rect.GetBottom() - 2.0f * r,
                2.0f * r, 2.0f * r, 90.0f, 90.0f);
    path.AddLine(rect.X, rect.GetBottom() - r, rect.X, rect.Y);
    path.CloseFigure();
}

void AddAttachedSidePath(Gdiplus::GraphicsPath& path, const Gdiplus::RectF& rect,
                         float radius, bool attachedLeft) {
    const float r = std::min(radius, rect.Width / 2.0f);
    if (attachedLeft) {
        path.StartFigure();
        path.AddLine(rect.X, rect.Y, rect.GetRight() - r, rect.Y);
        path.AddArc(rect.GetRight() - 2.0f * r, rect.Y,
                    2.0f * r, 2.0f * r, 270.0f, 90.0f);
        path.AddLine(rect.GetRight(), rect.Y + r, rect.GetRight(), rect.GetBottom() - r);
        path.AddArc(rect.GetRight() - 2.0f * r, rect.GetBottom() - 2.0f * r,
                    2.0f * r, 2.0f * r, 0.0f, 90.0f);
        path.AddLine(rect.GetRight() - r, rect.GetBottom(), rect.X, rect.GetBottom());
        path.AddLine(rect.X, rect.GetBottom(), rect.X, rect.Y);
        path.CloseFigure();
    } else {
        path.StartFigure();
        path.AddLine(rect.GetRight(), rect.Y, rect.X + r, rect.Y);
        path.AddArc(rect.X, rect.Y, 2.0f * r, 2.0f * r, 180.0f, 90.0f);
        path.AddLine(rect.X, rect.Y + r, rect.X, rect.GetBottom() - r);
        path.AddArc(rect.X, rect.GetBottom() - 2.0f * r,
                    2.0f * r, 2.0f * r, 90.0f, 90.0f);
        path.AddLine(rect.X + r, rect.GetBottom(), rect.GetRight(), rect.GetBottom());
        path.AddLine(rect.GetRight(), rect.GetBottom(), rect.GetRight(), rect.Y);
        path.CloseFigure();
    }
}

void DrawTopDock(Gdiplus::Graphics& graphics, const Settings& settings,
                 const Gdiplus::Font& smallFont, const Gdiplus::Font& mainFont,
                 const Gdiplus::Font& priceFont, const Gdiplus::Font& changeFont,
                 float width) {
    Gdiplus::RectF rect(0.0f, 0.0f, width, static_cast<float>(kTopDockHeight));
    ThemePalette palette = GetThemePalette(settings);
    if (!g_compositionGlassActive) {
        Gdiplus::GraphicsPath path;
        AddTopDockPath(path, rect, 18.0f);
        const BYTE fallbackAlpha = ApplyShapeOpacity(
            static_cast<BYTE>(
                std::clamp(settings.glassTintOpacity + 105, 145, 225)),
            settings);
        Gdiplus::LinearGradientBrush fill(
            rect,
            Gdiplus::Color(fallbackAlpha, palette.pillR, palette.pillG, palette.pillB),
            Gdiplus::Color(fallbackAlpha, palette.bottomR, palette.bottomG, palette.bottomB),
            Gdiplus::LinearGradientModeVertical);
        graphics.FillPath(&fill, &path);
        Gdiplus::Pen border(
            Gdiplus::Color(ApplyShapeOpacity(95, settings),
                           palette.borderR, palette.borderG, palette.borderB),
            0.9f);
        graphics.DrawPath(&border, &path);
    }

    // A compact identity block followed by three equal quote cells. Widths
    // adapt to the actual monitor so labels, prices, and changes never collide.
    DrawText(graphics, L"MARKET", smallFont, 18.0f, 8.0f,
             ParseColor(settings.mutedColor));
    DrawText(graphics, g_isFetching.load() ? L"UPDATING" : L"LIVE WATCH",
             mainFont, 18.0f, 25.0f, ParseColor(settings.textColor));

    const int indices[] = {0, 5, 12};
    const float startX = 132.0f;
    const float rightInset = 36.0f;
    const float usableWidth = std::max(240.0f, width - startX - rightInset);
    const float segmentWidth = usableWidth / 3.0f;

    for (int i = 0; i < 3; ++i) {
        const int index = indices[i];
        const float x = startX + segmentWidth * i;
        const Quote& quote = g_snapshot.quotes[index];
        const Gdiplus::Color changeColor = QuoteColor(quote, settings);

        DrawText(graphics, kAssets[index].ticker, mainFont, x, 8.0f,
                 ParseColor(settings.textColor));
        DrawTextRight(graphics, FormatPrice(kAssets[index], quote), priceFont,
                      x, 25.0f, std::max(70.0f, segmentWidth - 58.0f), 22.0f,
                      ParseColor(settings.textColor));
        DrawTextRight(graphics, FormatChange(quote), changeFont,
                      x + segmentWidth - 55.0f, 26.0f, 48.0f, 20.0f,
                      changeColor);

        if (i < 2) {
            Gdiplus::Pen separator(Gdiplus::Color(60, 210, 220, 235), 1.0f);
            const float separatorX = x + segmentWidth - 8.0f;
            graphics.DrawLine(&separator, separatorX, 12.0f,
                              separatorX, 48.0f);
        }
    }

}

void DrawSideDock(Gdiplus::Graphics& graphics, const Settings& settings,
                  const Gdiplus::Font& smallFont, const Gdiplus::Font& mainFont,
                  float width, float height) {
    const float left = settings.dockSide == L"Right" ? width - kSideDockWidth : 0.0f;
    const float top = (height - settings.sideDockHeight) / 2.0f;
    Gdiplus::RectF rect(left, top, static_cast<float>(kSideDockWidth),
                        static_cast<float>(settings.sideDockHeight));
    ThemePalette palette = GetThemePalette(settings);
    if (!g_compositionGlassActive) {
        Gdiplus::GraphicsPath path;
        AddAttachedSidePath(path, rect, 18.0f, settings.dockSide == L"Left");
        const BYTE fallbackAlpha = ApplyShapeOpacity(
            static_cast<BYTE>(
                std::clamp(settings.glassTintOpacity + 105, 145, 225)),
            settings);
        Gdiplus::LinearGradientBrush fill(
            rect,
            Gdiplus::Color(fallbackAlpha, palette.pillR, palette.pillG, palette.pillB),
            Gdiplus::Color(fallbackAlpha, palette.bottomR, palette.bottomG, palette.bottomB),
            Gdiplus::LinearGradientModeVertical);
        graphics.FillPath(&fill, &path);
        Gdiplus::Pen border(
            Gdiplus::Color(ApplyShapeOpacity(95, settings),
                           palette.borderR, palette.borderG, palette.borderB),
            0.9f);
        graphics.DrawPath(&border, &path);
    }

    DrawTextCentered(graphics, L"MARKET", smallFont, left + 8.0f,
                     top + 18.0f, 64.0f, 24.0f,
                     ParseColor(settings.textColor));
    const int indices[] = {0, 1, 5, 12};
    for (int i = 0; i < 4; ++i) {
        const int index = indices[i];
        const float y = top + 70.0f + i * 38.0f;
        const Gdiplus::Color color = QuoteColor(g_snapshot.quotes[index], settings);
        DrawTextCentered(graphics, kAssets[index].ticker, mainFont,
                         left + 7.0f, y, 66.0f, 18.0f,
                         ParseColor(settings.textColor));
        DrawTextCentered(graphics, FormatChange(g_snapshot.quotes[index]),
                         smallFont, left + 7.0f, y + 17.0f, 66.0f, 18.0f,
                         color);
    }
}
void DrawCategoryButton(Gdiplus::Graphics& graphics, float x, float y,
                        float width, float height, const wchar_t* label,
                        bool active, HitTarget target,
                        const Gdiplus::Font& labelFont,
                        const Settings& settings) {
    const bool hovered = IsHovered(target);
    const bool pressed = IsPressed(target);
    Gdiplus::Color fill = active ? Gdiplus::Color(88, 150, 170, 196)
                                 : hovered ? Gdiplus::Color(68, 255, 255, 255)
                                           : Gdiplus::Color(25, 255, 255, 255);
    if (pressed) fill = Gdiplus::Color(95, 120, 150, 205);
    Gdiplus::Color border = active ? Gdiplus::Color(105, 155, 190, 245)
                                   : hovered ? Gdiplus::Color(75, 235, 245, 255)
                                             : Gdiplus::Color(28, 255, 255, 255);
    if (active && settings.theme == L"Nusa") {
        Gdiplus::RectF activeRect(x, y, width, height);
        Gdiplus::GraphicsPath activePath;
        AddRoundedRectPath(activePath, activeRect, 10.0f);
        Gdiplus::LinearGradientBrush activeFill(
            activeRect,
            Gdiplus::Color(150, 0, 166, 225),
            Gdiplus::Color(145, 112, 72, 220),
            Gdiplus::LinearGradientModeHorizontal);
        graphics.FillPath(&activeFill, &activePath);
        Gdiplus::Pen activeBorder(Gdiplus::Color(105, 123, 205, 255), 0.9f);
        graphics.DrawPath(&activeBorder, &activePath);
    } else {
        DrawRoundedFill(graphics, Gdiplus::RectF(x, y, width, height),
                        10.0f, fill, border);
    }
    DrawTextCentered(graphics, label, labelFont, x + 5.0f, y + 3.0f,
                     width - 10.0f, height - 6.0f,
                     active || hovered ? ParseColor(settings.textColor)
                                       : ParseColor(settings.mutedColor));
}

void DrawAssetRow(Gdiplus::Graphics& graphics, int assetIndex, int rowIndex,
                  float panelTop, const Gdiplus::Font& tickerFont,
                  const Gdiplus::Font& nameFont, const Gdiplus::Font& priceFont,
                  const Gdiplus::Font& changeFont, const Settings& settings) {
    const AssetInfo& asset = kAssets[assetIndex];
    const Quote& quote = g_snapshot.quotes[assetIndex];
    const float x = 20.0f;
    const float y = panelTop + static_cast<float>(kListRowTop +
                                                   rowIndex * kListRowStep);
    const HitTarget target = static_cast<HitTarget>(
        static_cast<int>(HitTarget::Row0) + rowIndex);
    const bool hovered = IsHovered(target);
    const bool pressed = IsPressed(target);
    const bool selected = assetIndex == g_selectedAsset && !g_selectedCustom;

    Gdiplus::Color fill = selected ? Gdiplus::Color(83, 94, 129, 184)
                                   : hovered ? Gdiplus::Color(62, 255, 255, 255)
                                             : Gdiplus::Color(25, 255, 255, 255);
    if (pressed) fill = Gdiplus::Color(100, 110, 145, 195);
    Gdiplus::Color border = selected ? Gdiplus::Color(80, 140, 180, 235)
                                     : hovered ? Gdiplus::Color(58, 230, 240, 255)
                                               : Gdiplus::Color(20, 255, 255, 255);
    DrawRoundedFill(graphics,
                    Gdiplus::RectF(x, y, 440.0f,
                                   static_cast<float>(kListRowHeight)),
                    10.0f, fill, border);

    const Gdiplus::Color changeColor = QuoteColor(quote, settings);
    DrawText(graphics, asset.ticker, tickerFont, x + 15.0f, y + 3.0f,
             ParseColor(settings.textColor));
    DrawText(graphics, asset.name, nameFont, x + 15.0f, y + 19.0f,
             ParseColor(settings.mutedColor));
    DrawTextRight(graphics, FormatPrice(asset, quote), priceFont,
                  x + 208.0f, y + 2.0f, 145.0f, 32.0f,
                  ParseColor(settings.textColor));
    DrawTextRight(graphics, FormatChange(quote), changeFont,
                  x + 354.0f, y + 2.0f, 76.0f, 32.0f,
                  quote.valid ? changeColor : ParseColor(settings.mutedColor));
}

void DrawSearchResultRow(Gdiplus::Graphics& graphics, int resultIndex, int rowIndex,
                         float panelTop, const Gdiplus::Font& tickerFont,
                         const Gdiplus::Font& nameFont,
                         const Gdiplus::Font& metaFont,
                         const Settings& settings) {
    const SearchResult& result = g_searchResults[resultIndex];
    const float x = 20.0f;
    const float y = panelTop + static_cast<float>(kListRowTop +
                                                   rowIndex * kListRowStep);
    const HitTarget target = static_cast<HitTarget>(
        static_cast<int>(HitTarget::Row0) + rowIndex);
    const bool hovered = IsHovered(target);
    const bool pressed = IsPressed(target);
    Gdiplus::Color fill = hovered ? Gdiplus::Color(65, 255, 255, 255)
                                  : Gdiplus::Color(25, 255, 255, 255);
    if (pressed) fill = Gdiplus::Color(100, 110, 145, 195);
    DrawRoundedFill(graphics,
                    Gdiplus::RectF(x, y, 440.0f,
                                   static_cast<float>(kListRowHeight)),
                    10.0f, fill,
                    hovered ? Gdiplus::Color(65, 235, 245, 255)
                            : Gdiplus::Color(20, 255, 255, 255));
    DrawText(graphics, result.ticker, tickerFont, x + 15.0f, y + 3.0f,
             ParseColor(settings.textColor));
    DrawText(graphics, result.name, nameFont, x + 15.0f, y + 19.0f,
             ParseColor(settings.mutedColor));
    std::wstring meta = result.quoteType;
    if (!result.exchange.empty()) meta += L"  ·  " + result.exchange;
    DrawTextRight(graphics, meta, metaFont, x + 270.0f, y + 2.0f,
                  160.0f, 32.0f, ParseColor(settings.mutedColor));
}

void DrawExpandedPanel(Gdiplus::Graphics& graphics, const Settings& settings,
                       const Gdiplus::Font& titleFont,
                       const Gdiplus::Font& subtitleFont,
                       const Gdiplus::Font& categoryFont,
                       const Gdiplus::Font& tickerFont,
                       const Gdiplus::Font& nameFont,
                       const Gdiplus::Font& priceFont,
                       const Gdiplus::Font& changeFont,
                       const Gdiplus::Font& footerFont,
                       float panelTop) {
    const Gdiplus::Color textColor = ParseColor(settings.textColor);
    const Gdiplus::Color mutedColor = ParseColor(settings.mutedColor);
    const Gdiplus::RectF panelRect(0.0f, panelTop,
                                   static_cast<float>(kExpandedWidth),
                                   static_cast<float>(kPanelHeight));
    DrawGlassCard(graphics, panelRect,
                  ShapeRadius(settings, static_cast<float>(kPanelHeight), true),
                  settings);
    // Text-only header keeps the Fluent surface clean and unobstructed.
    DrawText(graphics, L"MARKET", titleFont, 22.0f, panelTop + 13.0f,
             textColor);
    DrawText(graphics, L"Crypto  ·  US stocks & ETFs  ·  Indonesia",
             subtitleFont, 22.0f, panelTop + 34.0f, mutedColor);
    DrawTextRight(graphics, g_isFetching.load() ? L"UPDATING" : L"LIVE",
                  categoryFont, 350.0f, panelTop + 18.0f, 108.0f, 18.0f,
                  g_isFetching.load()
                      ? Gdiplus::Color(255, 230, 190, 105)
                      : Gdiplus::Color(255, 130, 220, 175));

    // Segmented category control, like a native macOS segmented picker.
    DrawRoundedFill(graphics, Gdiplus::RectF(20.0f, panelTop + 59.0f,
                                             440.0f, 37.0f),
                    13.0f, Gdiplus::Color(34, 0, 0, 0),
                    Gdiplus::Color(30, 255, 255, 255));
    DrawCategoryButton(graphics, 23.0f, panelTop + 62.0f, 142.0f, 31.0f,
                       L"CRYPTO", g_activeCategory == 0,
                       HitTarget::CategoryCrypto, categoryFont, settings);
    DrawCategoryButton(graphics, 169.0f, panelTop + 62.0f, 142.0f, 31.0f,
                       L"US / ETF", g_activeCategory == 1,
                       HitTarget::CategoryUs, categoryFont, settings);
    DrawCategoryButton(graphics, 315.0f, panelTop + 62.0f, 142.0f, 31.0f,
                       L"INDONESIA", g_activeCategory == 2,
                       HitTarget::CategoryIdx, categoryFont, settings);

    // Search control.
    const bool searchHot = IsHovered(HitTarget::Search) ||
                           IsPressed(HitTarget::Search);
    DrawRoundedFill(graphics, Gdiplus::RectF(20.0f, panelTop + 104.0f,
                                             440.0f, 38.0f),
                    12.0f,
                    g_searchFocused ? Gdiplus::Color(90, 72, 104, 150)
                                    : searchHot ? Gdiplus::Color(58, 255, 255, 255)
                                                : Gdiplus::Color(34, 255, 255, 255),
                    searchHot || g_searchFocused
                        ? Gdiplus::Color(90, 175, 205, 255)
                        : Gdiplus::Color(35, 255, 255, 255));
    std::wstring searchDisplay = g_searchText.empty()
        ? L"Search coin, stock, ETF or company..."
        : g_searchText + (g_searchFocused ? L"  |" : L"");
    DrawText(graphics, searchDisplay, nameFont, 32.0f, panelTop + 116.0f,
             g_searchText.empty() ? mutedColor : textColor);
    DrawTextRight(graphics, L"ENTER", footerFont, 390.0f,
                  panelTop + 113.0f, 55.0f, 18.0f, mutedColor);

    DrawText(graphics, g_searchSubmitted ? L"SEARCH RESULTS" : L"WATCHLIST",
             categoryFont, 22.0f, panelTop + 151.0f, mutedColor);
    DrawTextRight(graphics,
                  g_searchSubmitted
                      ? (g_searchLoading ? L"SEARCHING" : L"YAHOO FINANCE")
                      : L"5 INSTRUMENTS",
                  footerFont, 300.0f, panelTop + 150.0f, 158.0f, 18.0f,
                  mutedColor);

    if (g_searchSubmitted) {
        if (g_searchLoading) {
            DrawText(graphics, L"Searching Yahoo Finance...",
                     nameFont, 25.0f, panelTop + 190.0f, mutedColor);
        } else if (g_searchResultCount == 0) {
            DrawText(graphics,
                     L"No matching instruments. Try another keyword.",
                     nameFont, 25.0f, panelTop + 190.0f, mutedColor);
        } else {
            for (int i = 0; i < g_searchResultCount; ++i) {
                DrawSearchResultRow(graphics, i, i, panelTop, tickerFont,
                                    nameFont, footerFont, settings);
            }
        }
    } else {
        const int start = CategoryStart(g_activeCategory);
        for (int i = 0; i < kAssetsPerGroup; ++i) {
            DrawAssetRow(graphics, start + i, i, panelTop, tickerFont,
                         nameFont, priceFont, changeFont, settings);
        }
    }

    const std::wstring footer = g_searchSubmitted
        ? L"Yahoo Finance Search  ·  Select a result to watch"
        : L"Select an instrument to pin it to the Dynamic Island";
    DrawText(graphics, footer, footerFont, 22.0f, panelTop + 385.0f,
             Gdiplus::Color(190, 190, 198, 210));

    const bool refreshHot = IsHovered(HitTarget::Refresh) ||
                            IsPressed(HitTarget::Refresh);
    DrawRoundedFill(graphics, Gdiplus::RectF(370.0f, panelTop + 379.0f,
                                             90.0f, 24.0f),
                    10.0f,
                    refreshHot ? Gdiplus::Color(95, 85, 135, 205)
                               : Gdiplus::Color(62, 65, 115, 175),
                    Gdiplus::Color(85, 145, 185, 240));
    DrawText(graphics, L"REFRESH", footerFont, 389.0f,
             panelTop + 384.0f, textColor);
}

void CalculatePosition(const Settings& settings, int pixelWidth, int pixelHeight,
                       LONG* x, LONG* y) {
    POINT origin{0, 0};
    HMONITOR monitor = MonitorFromPoint(origin, MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) {
        *x = std::max<LONG>(0, 18 + std::max(0, settings.offsetX));
        *y = std::max<LONG>(0, settings.offsetY);
        return;
    }

    // The floating owner window is a fixed canvas sized for the expanded
    // panel. The visible island morphs inside it; the HWND never follows the
    // changing pill/panel bounds during animation.
    const RECT& area = info.rcWork;
    const LONG areaWidth = area.right - area.left;

    const bool right = settings.position == L"TopRight" ||
                       settings.position == L"BottomRight";
    const bool center = settings.position == L"TopCenter" ||
                        settings.position == L"BottomCenter";
    const bool bottom = IsBottomPosition(settings);
    const LONG edgeInset = 18 + std::max(0, settings.offsetX);

    if (center) {
        *x = area.left + (areaWidth - pixelWidth) / 2 + settings.offsetX;
    } else if (right) {
        *x = area.right - pixelWidth - edgeInset;
    } else {
        *x = area.left + edgeInset;
    }

    *y = bottom ? area.bottom - pixelHeight - settings.offsetY
                : area.top + settings.offsetY;

    // Large offsets or small work areas must not push the island completely
    // off-screen. Clamp the fixed canvas, not the animated silhouette.
    const LONG maxX = std::max<LONG>(area.left, area.right - pixelWidth);
    const LONG maxY = std::max<LONG>(area.top, area.bottom - pixelHeight);
    *x = std::clamp<LONG>(*x, area.left, maxX);
    *y = std::clamp<LONG>(*y, area.top, maxY);
}

bool IsShellWindow(HWND hwnd) {
    wchar_t className[128]{};
    GetClassNameW(hwnd, className, ARRAYSIZE(className));
    return wcscmp(className, L"Progman") == 0 ||
           wcscmp(className, L"WorkerW") == 0 ||
           wcscmp(className, L"Shell_TrayWnd") == 0;
}

bool IsForegroundFullscreen(HWND ownWindow) {
    HWND foreground = GetForegroundWindow();
    if (!foreground || foreground == ownWindow || IsShellWindow(foreground) || !IsWindowVisible(foreground)) {
        return false;
    }
    RECT windowRect{};
    if (!GetWindowRect(foreground, &windowRect)) return false;
    HMONITOR monitor = MonitorFromWindow(foreground, MONITOR_DEFAULTTONEAREST);
    MONITORINFO info{};
    info.cbSize = sizeof(info);
    if (!GetMonitorInfoW(monitor, &info)) return false;
    const RECT& monitorRect = info.rcMonitor;
    return windowRect.left <= monitorRect.left + 2 &&
           windowRect.top <= monitorRect.top + 2 &&
           windowRect.right >= monitorRect.right - 2 &&
           windowRect.bottom >= monitorRect.bottom - 2;
}

void UpdateWindowGeometry(HWND hwnd) {
    if (!hwnd) return;
    Settings settings = GetSettings();
    const float scale = UiScale(settings);
    const int width = static_cast<int>(std::ceil(
        CurrentLogicalWidth(settings, g_animationProgress) * scale));
    const int height = static_cast<int>(std::ceil(
        CurrentLogicalHeight(settings, g_animationProgress) * scale));
    LONG x = 0, y = 0;
    CalculatePosition(settings, width, height, &x, &y);

    RECT current{};
    const bool hasRect = GetWindowRect(hwnd, &current) != FALSE;
    const bool geometryChanged =
        !hasRect || current.left != x || current.top != y ||
        current.right - current.left != width ||
        current.bottom - current.top != height;

    const bool isTopmost =
        (GetWindowLongPtrW(hwnd, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
    HWND insertAfter = nullptr;
    bool zOrderChanged = false;

    if (settings.alwaysOnTop && !isTopmost) {
        insertAfter = HWND_TOPMOST;
        zOrderChanged = true;
    } else if (!settings.alwaysOnTop && isTopmost) {
        insertAfter = HWND_NOTOPMOST;
        zOrderChanged = true;
    } else if (!settings.alwaysOnTop) {
        HWND foreground = GetForegroundWindow();
        if (foreground && foreground != hwnd && !IsShellWindow(foreground) &&
            GetWindow(hwnd, GW_HWNDPREV) != foreground) {
            insertAfter = foreground;
            zOrderChanged = true;
        }
    }

    if (geometryChanged || zOrderChanged) {
        UINT flags = SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_SHOWWINDOW;
        if (!zOrderChanged) flags |= SWP_NOZORDER;
        SetWindowPos(hwnd, insertAfter, x, y, width, height, flags);
    }
}

struct AccentPolicy {
    int accentState;
    int accentFlags;
    DWORD gradientColor;
    int animationId;
};
struct WindowCompositionAttributeData {
    int attribute;
    void* data;
    SIZE_T sizeOfData;
};
using SetWindowCompositionAttributeFn = BOOL(WINAPI*)(HWND, WindowCompositionAttributeData*);

HRGN CreateClippedRoundRegion(int left, int top, int width, int height,
                              int radius, bool squareTop = false,
                              bool squareLeft = false, bool squareRight = false,
                              bool squareBottom = false) {
    if (width <= 0 || height <= 0) return nullptr;
    radius = std::clamp(radius, 1,
                        std::max(1, std::min(width, height) / 2));
    const int diameter = std::max(2, radius * 2);
    HRGN region = CreateRoundRectRgn(left, top, left + width + 1,
                                     top + height + 1, diameter, diameter);
    if (!region) return nullptr;

    auto unionRect = [&](int l, int t, int r, int b) {
        if (r <= l || b <= t) return;
        HRGN rect = CreateRectRgn(l, t, r, b);
        if (rect) {
            CombineRgn(region, region, rect, RGN_OR);
            DeleteObject(rect);
        }
    };
    if (squareTop) unionRect(left, top, left + width + 1, top + radius + 1);
    if (squareLeft) unionRect(left, top, left + radius + 1, top + height + 1);
    if (squareRight) unionRect(left + width - radius, top,
                               left + width + 1, top + height + 1);
    if (squareBottom) unionRect(left, top + height - radius,
                                left + width + 1, top + height + 1);
    return region;
}

void AddRegion(HRGN& target, HRGN addition) {
    if (!addition) return;
    if (!target) {
        target = addition;
        return;
    }
    CombineRgn(target, target, addition, RGN_OR);
    DeleteObject(addition);
}

bool UpdateWindowRegion(HWND hwnd, const Settings& settings, float scale,
                         float logicalWidth, float logicalHeight) {
    HRGN region = nullptr;
    auto addShape = [&](float x, float y, float w, float h, float radius,
                        bool squareTop = false, bool squareLeft = false,
                        bool squareRight = false, bool squareBottom = false) {
        int left = static_cast<int>(std::lround(x * scale));
        int top = static_cast<int>(std::lround(y * scale));
        int width = std::max(1, static_cast<int>(std::lround(w * scale)));
        int height = std::max(1, static_cast<int>(std::lround(h * scale)));
        int r = std::max(1, static_cast<int>(std::lround(radius * scale)));
        AddRegion(region, CreateClippedRoundRegion(left, top, width, height,
                                                   r, squareTop, squareLeft,
                                                   squareRight, squareBottom));
    };

    if (settings.dockMode == L"TopAttached") {
        addShape(0, 0, logicalWidth, kTopDockHeight, 18, true, false, false);
        const float panelHeight = CurrentPanelVisibleHeight(
            settings, g_animationProgress);
        if (panelHeight > 0.5f) {
            addShape(PanelLeft(settings, logicalWidth),
                     CurrentPanelTop(settings, g_animationProgress, logicalHeight),
                     kExpandedWidth, panelHeight,
                     ShapeRadius(settings, static_cast<float>(kPanelHeight), true));
        }
    } else if (settings.dockMode == L"SideAttached") {
        const float railTop = (logicalHeight - settings.sideDockHeight) / 2.0f;
        const float railLeft = settings.dockSide == L"Right"
            ? logicalWidth - kSideDockWidth : 0.0f;
        addShape(railLeft, railTop, kSideDockWidth, settings.sideDockHeight,
                 18, false, settings.dockSide == L"Left",
                 settings.dockSide == L"Right");
        if (g_animationProgress > 0.001f) {
            addShape(PanelLeft(settings, logicalWidth),
                     CurrentPanelTop(settings, g_animationProgress, logicalHeight),
                     kExpandedWidth, kPanelHeight,
                     ShapeRadius(settings, static_cast<float>(kPanelHeight), true));
        }
    } else {
        const Gdiplus::RectF island = FloatingIslandRect(
            settings, logicalWidth, logicalHeight, g_animationProgress);
        const float radius = FloatingIslandRadius(settings, g_animationProgress);
        addShape(island.X, island.Y, island.Width, island.Height, radius);
    }

    if (!region) {
        g_windowRegionValid = false;
        return false;
    }
    if (!SetWindowRgn(hwnd, region, TRUE)) {
        DeleteObject(region);
        g_windowRegionValid = false;
        return false;
    }
    g_windowRegionValid = true;
    return true;
}


// Real Windows 11-style glass is rendered in a separate, click-through window
// immediately beneath the interactive market UI. The owner window only draws
// text and controls; the glass window supplies the host backdrop, tint, sheen,
// and rim. This avoids the opaque/rectangular artifacts caused by applying an
// accent blur directly to an UpdateLayeredWindow surface.

constexpr wchar_t kGlassWindowClassName[] = L"WindhawkMarketIslandGlassWindow";
constexpr DWORD kDwmUseHostBackdropBrush = 17;  // DWMWA_USE_HOSTBACKDROPBRUSH

}  // namespace

// Declare the interop interface at global scope. The Windhawk compiler uses
// MinGW/Clang, where __declspec(uuid) is not supported for __uuidof<T>.
struct ICompositorDesktopInteropLocal : ::IUnknown {
    virtual HRESULT __stdcall CreateDesktopWindowTarget(HWND window,
                                                         BOOL isTopmost,
                                                         void** target) = 0;
    virtual HRESULT __stdcall EnsureOnThread(DWORD threadId) = 0;
};

// Specialize guid_v inside the namespace that owns the variable template.
// This avoids both the anonymous-namespace specialization error and MinGW's
// undefined __mingw_uuidof<T> implementation.
namespace winrt::impl {
template <>
inline constexpr winrt::guid
    guid_v<::ICompositorDesktopInteropLocal>{
        0x29E691FA, 0x4567, 0x4DCA,
        {0xB3, 0x19, 0xD0, 0xF2, 0x07, 0xEB, 0x68, 0x07}};
}  // namespace winrt::impl

namespace {

namespace Composition = winrt::Windows::UI::Composition;
namespace CompositionDesktop = winrt::Windows::UI::Composition::Desktop;
namespace Foundation = winrt::Windows::Foundation;
namespace System = winrt::Windows::System;

struct GlassCompositionState {
    System::DispatcherQueueController queue{nullptr};
    Composition::Compositor compositor{nullptr};
    Composition::CompositionBackdropBrush backdrop{nullptr};
    bool apartmentInitialized = false;
    bool ready = false;
};

GlassCompositionState g_glassComposition;

bool InitializeGlassComposition() {
    try {
        winrt::init_apartment(winrt::apartment_type::single_threaded);
        g_glassComposition.apartmentInitialized = true;

        using CreateController_t = HRESULT(WINAPI*)(DispatcherQueueOptions, void**);
        HMODULE coreMessaging = LoadLibraryExW(
            L"CoreMessaging.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
        auto createController = coreMessaging
            ? reinterpret_cast<CreateController_t>(
                  GetProcAddress(coreMessaging, "CreateDispatcherQueueController"))
            : nullptr;
        if (!createController) {
            Wh_Log(L"Market glass: DispatcherQueue factory unavailable.");
            return false;
        }

        const DispatcherQueueOptions options{
            sizeof(DispatcherQueueOptions), DQTYPE_THREAD_CURRENT, DQTAT_COM_STA};
        HRESULT hr = createController(options, winrt::put_abi(g_glassComposition.queue));
        if (FAILED(hr)) {
            Wh_Log(L"Market glass: DispatcherQueue creation failed: %08X", (unsigned)hr);
            return false;
        }

        g_glassComposition.compositor = Composition::Compositor();
        g_glassComposition.backdrop =
            g_glassComposition.compositor.CreateHostBackdropBrush();
        g_glassComposition.ready = true;
        return true;
    } catch (winrt::hresult_error const& e) {
        Wh_Log(L"Market glass: Composition initialization failed: %08X",
               (unsigned)e.code());
    } catch (...) {
        Wh_Log(L"Market glass: Composition initialization failed.");
    }
    return false;
}

LRESULT CALLBACK GlassWindowProc(HWND hwnd, UINT message,
                                 WPARAM wParam, LPARAM lParam) {
    if (message == WM_NCHITTEST) return HTTRANSPARENT;
    if (message == WM_MOUSEACTIVATE) return MA_NOACTIVATE;
    if (message == WM_ERASEBKGND) return 1;
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

bool RegisterGlassWindowClass() {
    static bool registered = false;
    if (registered) return true;

    WNDCLASSEXW wc{};
    wc.cbSize = sizeof(wc);
    wc.lpfnWndProc = GlassWindowProc;
    wc.hInstance = GetModuleHandleW(nullptr);
    wc.lpszClassName = kGlassWindowClassName;
    if (!RegisterClassExW(&wc) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        return false;
    }
    registered = true;
    return true;
}

struct GlassShape {
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    float radius = 0;
    BYTE tintAlpha = 132;
    BYTE red = 20;
    BYTE green = 24;
    BYTE blue = 39;
    bool squareTop = false;
    bool squareLeft = false;
    bool squareRight = false;
    bool squareBottom = false;
    float opacity = 1.0f;
    float rimOpacity = 1.0f;
};

winrt::Windows::UI::Color GlassColor(BYTE alpha, BYTE red, BYTE green, BYTE blue) {
    return winrt::Windows::UI::Color{alpha, red, green, blue};
}

class GlassLayer {
public:
    GlassLayer() = default;
    GlassLayer(const GlassLayer&) = delete;
    GlassLayer& operator=(const GlassLayer&) = delete;

    bool Show(HWND owner, POINT position, SIZE size,
              const std::vector<GlassShape>& shapes,
              const Settings& settings) {
        if (!owner || size.cx <= 0 || size.cy <= 0 || shapes.empty() ||
            settings.blurStyle == L"None") {
            Hide();
            return false;
        }

        if (!EnsureWindow()) {
            Hide();
            return false;
        }
        SyncTopmostBand(owner);

        const bool geometryChanged =
            !m_visible || m_owner != owner ||
            m_position.x != position.x || m_position.y != position.y ||
            m_size.cx != size.cx || m_size.cy != size.cy;
        if (geometryChanged) {
            // The accent fallback installs an HRGN, so size the helper first.
            // Floating mode now has a fixed canvas and reaches this only on
            // first show or an actual settings/display geometry change.
            SetWindowPos(m_hwnd, nullptr, position.x, position.y, size.cx, size.cy,
                         SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER |
                             SWP_NOREDRAW);
        }

        bool rendered = false;

        // Choose a rendering backend once. Repeatedly retrying Composition on
        // every animation frame can alternate between host-backdrop tint and
        // accent/GDI fallback, which looks like a sudden opacity jump.
        if (!m_forceAccentFallback && !m_forceOwnerFallback) {
            if (g_glassComposition.ready && EnsureCompositionTarget(owner)) {
                try {
                    RenderComposition(size, shapes);
                    rendered = true;
                } catch (winrt::hresult_error const& e) {
                    Wh_Log(L"Market glass: backdrop rendering failed: %08X",
                           (unsigned)e.code());
                    DestroyCompositionTarget();
                    m_forceAccentFallback = true;
                } catch (...) {
                    Wh_Log(L"Market glass: backdrop rendering failed.");
                    DestroyCompositionTarget();
                    m_forceAccentFallback = true;
                }
            } else {
                m_forceAccentFallback = true;
            }
        }

        if (!rendered && m_forceAccentFallback && !m_forceOwnerFallback) {
            rendered = ApplyAccentFallback(shapes, settings);
            if (!rendered) {
                // Stay on the owner-window fallback for the rest of this
                // GlassLayer lifetime rather than retrying different materials
                // on subsequent frames.
                m_forceOwnerFallback = true;
            }
        }

        if (!rendered) {
            Hide();
            return false;
        }

        // Keep the glass directly behind the owner. Geometry is applied once,
        // after composition has been updated, instead of twice on every frame.
        const bool orderChanged = GetWindow(m_hwnd, GW_HWNDPREV) != owner;
        if (geometryChanged || orderChanged || !IsWindowVisible(m_hwnd)) {
            UINT flags = SWP_NOACTIVATE | SWP_SHOWWINDOW;
            if (!geometryChanged) flags |= SWP_NOMOVE | SWP_NOSIZE;
            SetWindowPos(m_hwnd, owner, position.x, position.y, size.cx, size.cy,
                         flags);
        }
        m_owner = owner;
        m_position = position;
        m_size = size;
        m_visible = true;
        return true;
    }

    void Hide() {
        if (m_hwnd && IsWindowVisible(m_hwnd)) ShowWindow(m_hwnd, SW_HIDE);
        m_owner = nullptr;
        m_visible = false;
    }

    void Destroy() {
        Hide();
        DestroyCompositionTarget();
        if (m_hwnd) {
            DestroyWindow(m_hwnd);
            m_hwnd = nullptr;
        }
        m_hostBackdropAttempted = false;
        m_hostBackdropEnabled = false;
        m_targetTopmost = false;
        m_forceAccentFallback = false;
        m_forceOwnerFallback = false;
    }

    void ShutdownComposition() {
        g_glassComposition.backdrop = nullptr;
        g_glassComposition.compositor = nullptr;
        if (g_glassComposition.queue) {
            try {
                auto shutdown = g_glassComposition.queue.ShutdownQueueAsync();
                for (int i = 0;
                     i < 80 && shutdown.Status() == Foundation::AsyncStatus::Started;
                     ++i) {
                    MsgWaitForMultipleObjects(0, nullptr, FALSE, 10, QS_ALLINPUT);
                    MSG msg{};
                    while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                    }
                }
            } catch (...) {
                Wh_Log(L"Market glass: DispatcherQueue shutdown was incomplete.");
            }
            g_glassComposition.queue = nullptr;
        }
        g_glassComposition.ready = false;
        if (g_glassComposition.apartmentInitialized) {
            winrt::uninit_apartment();
            g_glassComposition.apartmentInitialized = false;
        }
    }

private:
    struct VisualShape {
        Composition::ContainerVisual container{nullptr};
        Composition::SpriteVisual backdrop{nullptr};
        Composition::SpriteVisual tint{nullptr};
        Composition::SpriteVisual sheen{nullptr};
        Composition::ShapeVisual rim{nullptr};
        Composition::CompositionColorBrush tintBrush{nullptr};
        Composition::CompositionLinearGradientBrush sheenBrush{nullptr};
        Composition::CompositionLinearGradientBrush rimBrush{nullptr};
        Composition::CompositionSpriteShape rimShape{nullptr};
        Composition::CompositionRoundedRectangleGeometry geometry{nullptr};
        Composition::CompositionGeometricClip clip{nullptr};
    };

    void SyncTopmostBand(HWND owner) {
        if (!m_hwnd || !owner) return;
        const bool ownerTopmost =
            (GetWindowLongPtrW(owner, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
        LONG_PTR glassExStyle = GetWindowLongPtrW(m_hwnd, GWL_EXSTYLE);
        const bool glassTopmost = (glassExStyle & WS_EX_TOPMOST) != 0;
        if (ownerTopmost == glassTopmost) return;

        if (ownerTopmost) {
            glassExStyle |= WS_EX_TOPMOST;
        } else {
            glassExStyle &= ~static_cast<LONG_PTR>(WS_EX_TOPMOST);
        }
        SetWindowLongPtrW(m_hwnd, GWL_EXSTYLE, glassExStyle);
        SetWindowPos(m_hwnd, ownerTopmost ? HWND_TOPMOST : HWND_NOTOPMOST,
                     0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE |
                         SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }

    bool EnsureWindow() {
        if (m_hwnd && IsWindow(m_hwnd)) return true;
        if (!RegisterGlassWindowClass()) return false;

        const DWORD exStyle = WS_EX_NOREDIRECTIONBITMAP | WS_EX_LAYERED |
                              WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW |
                              WS_EX_NOACTIVATE;
        m_hwnd = CreateWindowExW(exStyle, kGlassWindowClassName, L"",
                                 WS_POPUP, 0, 0, 1, 1, nullptr, nullptr,
                                 GetModuleHandleW(nullptr), nullptr);
        if (!m_hwnd) return false;

        SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);
        m_hostBackdropAttempted = false;
        m_hostBackdropEnabled = false;
        m_targetTopmost = false;
        return true;
    }

    bool EnsureCompositionTarget(HWND owner) {
        if (!m_hwnd || !owner || !g_glassComposition.ready) return false;

        const bool ownerTopmost =
            (GetWindowLongPtrW(owner, GWL_EXSTYLE) & WS_EX_TOPMOST) != 0;
        if (m_desktopTarget && m_targetTopmost != ownerTopmost) {
            DestroyCompositionTarget();
        }

        if (!m_hostBackdropAttempted) {
            m_hostBackdropAttempted = true;
            const BOOL enabled = TRUE;
            const HRESULT hr = DwmSetWindowAttribute(
                m_hwnd, kDwmUseHostBackdropBrush, &enabled, sizeof(enabled));
            if (FAILED(hr)) {
                Wh_Log(L"Market glass: host backdrop is unavailable: %08X",
                       (unsigned)hr);
                return false;
            }
            m_hostBackdropEnabled = true;
        }
        if (!m_hostBackdropEnabled) return false;

        if (!m_desktopTarget) {
            // Clear a region previously installed by the accent fallback;
            // Composition visuals clip each glass shape independently.
            SetWindowRgn(m_hwnd, nullptr, TRUE);

            SyncTopmostBand(owner);

            auto interop =
                g_glassComposition.compositor.as<ICompositorDesktopInteropLocal>();
            winrt::check_hresult(interop->CreateDesktopWindowTarget(
                m_hwnd, ownerTopmost ? TRUE : FALSE,
                winrt::put_abi(m_desktopTarget)));
            m_root = g_glassComposition.compositor.CreateContainerVisual();
            m_desktopTarget.Root(m_root);
            m_targetTopmost = ownerTopmost;
        }
        return true;
    }

    void RenderComposition(SIZE size, const std::vector<GlassShape>& shapes) {
        const auto& compositor = g_glassComposition.compositor;
        const winrt::Windows::Foundation::Numerics::float2 whole{
            static_cast<float>(size.cx), static_cast<float>(size.cy)};

        while (m_visuals.size() > shapes.size()) {
            m_root.Children().Remove(m_visuals.back().container);
            m_visuals.pop_back();
        }
        m_visuals.resize(shapes.size());

        for (size_t i = 0; i < shapes.size(); ++i) {
            const GlassShape& shape = shapes[i];
            VisualShape& visual = m_visuals[i];

            if (!visual.container) {
                visual.container = compositor.CreateContainerVisual();
                visual.backdrop = compositor.CreateSpriteVisual();
                visual.tintBrush = compositor.CreateColorBrush(
                    GlassColor(shape.tintAlpha, shape.red, shape.green, shape.blue));
                visual.tint = compositor.CreateSpriteVisual();
                visual.tint.Brush(visual.tintBrush);

                visual.sheenBrush = compositor.CreateLinearGradientBrush();
                visual.sheenBrush.MappingMode(Composition::CompositionMappingMode::Absolute);
                visual.sheenBrush.ColorStops().Append(
                    compositor.CreateColorGradientStop(
                        0.0f, GlassColor(0x20, 255, 255, 255)));
                visual.sheenBrush.ColorStops().Append(
                    compositor.CreateColorGradientStop(
                        0.48f, GlassColor(0x09, 255, 255, 255)));
                visual.sheenBrush.ColorStops().Append(
                    compositor.CreateColorGradientStop(
                        1.0f, GlassColor(0x00, 255, 255, 255)));
                visual.sheen = compositor.CreateSpriteVisual();
                visual.sheen.Brush(visual.sheenBrush);

                visual.rimBrush = compositor.CreateLinearGradientBrush();
                visual.rimBrush.MappingMode(Composition::CompositionMappingMode::Absolute);
                visual.rimBrush.ColorStops().Append(
                    compositor.CreateColorGradientStop(
                        0.0f, GlassColor(0x32, 255, 255, 255)));
                visual.rimBrush.ColorStops().Append(
                    compositor.CreateColorGradientStop(
                        0.36f, GlassColor(0x16, 255, 255, 255)));
                visual.rimBrush.ColorStops().Append(
                    compositor.CreateColorGradientStop(
                        0.72f, GlassColor(0x0A, 255, 255, 255)));
                visual.rimBrush.ColorStops().Append(
                    compositor.CreateColorGradientStop(
                        1.0f, GlassColor(0x20, 255, 255, 255)));
                visual.rimShape = compositor.CreateSpriteShape();
                visual.rimShape.StrokeBrush(visual.rimBrush);
                visual.rimShape.StrokeThickness(1.0f);
                visual.rim = compositor.CreateShapeVisual();
                visual.rim.Shapes().Append(visual.rimShape);

                visual.container.Children().InsertAtTop(visual.backdrop);
                visual.container.Children().InsertAtTop(visual.tint);
                visual.container.Children().InsertAtTop(visual.sheen);
                visual.container.Children().InsertAtTop(visual.rim);
                m_root.Children().InsertAtTop(visual.container);
            }

            visual.backdrop.Brush(g_glassComposition.backdrop);
            visual.backdrop.Size(whole);
            visual.tint.Size(whole);
            visual.sheen.Size(whole);
            visual.rim.Size(whole);
            visual.container.Opacity(std::clamp(shape.opacity, 0.0f, 1.0f));
            visual.rim.Opacity(std::clamp(shape.rimOpacity, 0.0f, 1.0f));
            visual.tintBrush.Color(
                GlassColor(shape.tintAlpha, shape.red, shape.green, shape.blue));

            // The top and side attached modes extend the geometry beyond the
            // window edge. The window clips the overflow, leaving that edge
            // square while the opposite corners remain rounded.
            float x = shape.x;
            float y = shape.y;
            float w = shape.width;
            float h = shape.height;
            if (shape.squareTop) {
                y -= shape.radius;
                h += shape.radius;
            }
            if (shape.squareLeft) {
                x -= shape.radius;
                w += shape.radius;
            }
            if (shape.squareRight) {
                w += shape.radius;
            }
            if (shape.squareBottom) {
                h += shape.radius;
            }

            const float cornerRadius = std::min(
                shape.radius, std::min(w, h) * 0.5f);
            if (!visual.geometry) {
                visual.geometry = compositor.CreateRoundedRectangleGeometry();
                visual.clip = compositor.CreateGeometricClip(visual.geometry);
                visual.backdrop.Clip(visual.clip);
                visual.tint.Clip(visual.clip);
                visual.sheen.Clip(visual.clip);
            }
            visual.geometry.Offset({x, y});
            visual.geometry.Size({std::max(1.0f, w), std::max(1.0f, h)});
            visual.geometry.CornerRadius({cornerRadius, cornerRadius});
            visual.rimShape.Geometry(visual.geometry);

            visual.sheenBrush.StartPoint({0.0f, shape.y});
            visual.sheenBrush.EndPoint({0.0f, shape.y + shape.height * 0.65f});
            visual.rimBrush.StartPoint({0.0f, shape.y});
            visual.rimBrush.EndPoint({0.0f, shape.y + shape.height});
        }
    }

    bool ApplyAccentFallback(const std::vector<GlassShape>& shapes,
                             const Settings& settings) {
        if (!m_hwnd) return false;

        HRGN region = CreateRectRgn(0, 0, 0, 0);
        if (!region) return false;

        for (const GlassShape& shape : shapes) {
            HRGN part = CreateClippedRoundRegion(
                static_cast<int>(std::lround(shape.x)),
                static_cast<int>(std::lround(shape.y)),
                std::max(1, static_cast<int>(std::lround(shape.width))),
                std::max(1, static_cast<int>(std::lround(shape.height))),
                std::max(1, static_cast<int>(std::lround(shape.radius))),
                shape.squareTop, shape.squareLeft, shape.squareRight,
                shape.squareBottom);
            if (part) {
                CombineRgn(region, region, part, RGN_OR);
                DeleteObject(part);
            }
        }

        if (SetWindowRgn(m_hwnd, region, TRUE) == 0) {
            DeleteObject(region);
            return false;
        }

        HMODULE user32 = GetModuleHandleW(L"user32.dll");
        auto setComposition = user32
            ? reinterpret_cast<SetWindowCompositionAttributeFn>(
                  GetProcAddress(user32, "SetWindowCompositionAttribute"))
            : nullptr;
        if (!setComposition) return false;

        const ThemePalette palette = GetThemePalette(settings);
        const DWORD color =
            (static_cast<DWORD>(settings.glassTintOpacity) << 24) |
            (static_cast<DWORD>(palette.bottomR) << 16) |
            (static_cast<DWORD>(palette.bottomG) << 8) |
            static_cast<DWORD>(palette.bottomB);
        const int state = settings.blurStyle == L"Glass" ? 3 : 4;
        AccentPolicy policy{state, 2, color, 0};
        WindowCompositionAttributeData data{19, &policy, sizeof(policy)};
        if (!setComposition(m_hwnd, &data)) return false;

        // Accent-policy blur is a window-level effect rather than a Composition
        // visual. Fade the helper window itself so the whole shape (blur and
        // tint together) follows ShapeOpacity.
        const BYTE surfaceAlpha = static_cast<BYTE>(
            std::lround(255.0f * ShapeOpacityFactor(settings)));
        return SetLayeredWindowAttributes(
            m_hwnd, 0, surfaceAlpha, LWA_ALPHA) != FALSE;
    }

    void DestroyCompositionTarget() {
        m_visuals.clear();
        m_root = nullptr;
        m_desktopTarget = nullptr;
        m_targetTopmost = false;
    }

    HWND m_hwnd = nullptr;
    HWND m_owner = nullptr;
    POINT m_position{};
    SIZE m_size{};
    bool m_visible = false;
    bool m_hostBackdropAttempted = false;
    bool m_hostBackdropEnabled = false;
    bool m_targetTopmost = false;
    bool m_forceAccentFallback = false;
    bool m_forceOwnerFallback = false;
    CompositionDesktop::DesktopWindowTarget m_desktopTarget{nullptr};
    Composition::ContainerVisual m_root{nullptr};
    std::vector<VisualShape> m_visuals;
};

GlassLayer g_glassLayer;

std::vector<GlassShape> BuildGlassShapes(const Settings& settings,
                                         float scale,
                                         float logicalWidth,
                                         float logicalHeight) {
    std::vector<GlassShape> shapes;
    shapes.reserve(1);
    const ThemePalette palette = GetThemePalette(settings);
    BYTE tintAlpha = static_cast<BYTE>(std::clamp(settings.glassTintOpacity, 0, 255));
    if (settings.blurStyle == L"Glass") {
        tintAlpha = static_cast<BYTE>(std::lround(tintAlpha * 0.82f));
    }

    auto add = [&](float x, float y, float width, float height, float radius,
                   bool squareTop = false, bool squareLeft = false,
                   bool squareRight = false, bool squareBottom = false,
                   float opacity = 1.0f, float rimOpacity = 1.0f) {
        GlassShape shape;
        shape.x = x * scale;
        shape.y = y * scale;
        shape.width = width * scale;
        shape.height = height * scale;
        shape.radius = radius * scale;
        shape.tintAlpha = tintAlpha;
        shape.red = palette.bottomR;
        shape.green = palette.bottomG;
        shape.blue = palette.bottomB;
        shape.squareTop = squareTop;
        shape.squareLeft = squareLeft;
        shape.squareRight = squareRight;
        shape.squareBottom = squareBottom;
        shape.opacity = std::clamp(
            opacity * ShapeOpacityFactor(settings), 0.0f, 1.0f);
        shape.rimOpacity = std::clamp(rimOpacity, 0.0f, 1.0f);
        shapes.push_back(shape);
    };

    if (settings.dockMode == L"TopAttached") {
        add(0.0f, 0.0f, logicalWidth, static_cast<float>(kTopDockHeight),
            18.0f, true, false, false);
        const float panelHeight = CurrentPanelVisibleHeight(
            settings, g_animationProgress);
        if (panelHeight > 0.5f) {
            add(PanelLeft(settings, logicalWidth),
                CurrentPanelTop(settings, g_animationProgress, logicalHeight),
                static_cast<float>(kExpandedWidth), panelHeight,
                ShapeRadius(settings, static_cast<float>(kPanelHeight), true),
                false, false, false, false,
                std::clamp(g_animationProgress, 0.0f, 1.0f), 0.0f);
        }
    } else if (settings.dockMode == L"SideAttached") {
        const float railTop =
            (logicalHeight - static_cast<float>(settings.sideDockHeight)) / 2.0f;
        const float railLeft = settings.dockSide == L"Right"
            ? logicalWidth - static_cast<float>(kSideDockWidth) : 0.0f;
        add(railLeft, railTop, static_cast<float>(kSideDockWidth),
            static_cast<float>(settings.sideDockHeight), 18.0f, false,
            settings.dockSide == L"Left",
            settings.dockSide == L"Right");
        if (g_animationProgress > 0.001f) {
            add(PanelLeft(settings, logicalWidth),
                CurrentPanelTop(settings, g_animationProgress, logicalHeight),
                static_cast<float>(kExpandedWidth),
                static_cast<float>(kPanelHeight),
                ShapeRadius(settings, static_cast<float>(kPanelHeight), true),
                false, false, false, false,
                std::clamp(g_animationProgress, 0.0f, 1.0f), 0.0f);
        }
    } else {
        const Gdiplus::RectF island = FloatingIslandRect(
            settings, logicalWidth, logicalHeight, g_animationProgress);
        const float radius = FloatingIslandRadius(settings, g_animationProgress);
        // One backdrop/tint/rim follows the visible pill-to-panel silhouette.
        add(island.X, island.Y, island.Width, island.Height, radius,
            false, false, false, false, 1.0f, 1.0f);
    }
    return shapes;
}

void ApplyPanelRevealMask(void* pixels, int bitmapWidth, int bitmapHeight,
                          int left, int top, int width, int fullHeight,
                          int visibleHeight, float opacity) {
    if (!pixels || bitmapWidth <= 0 || bitmapHeight <= 0 ||
        width <= 0 || fullHeight <= 0) {
        return;
    }

    const int x0 = std::clamp(left, 0, bitmapWidth);
    const int y0 = std::clamp(top, 0, bitmapHeight);
    const int x1 = std::clamp(left + width, 0, bitmapWidth);
    const int y1 = std::clamp(top + fullHeight, 0, bitmapHeight);
    const int revealBottom = std::clamp(
        top + std::clamp(visibleHeight, 0, fullHeight), y0, y1);
    const float factor = std::clamp(opacity, 0.0f, 1.0f);
    auto* data = static_cast<std::uint32_t*>(pixels);
    const int rowWidth = x1 - x0;
    if (rowWidth <= 0 || y1 <= y0) return;

    // Fixed-point packed-channel multiplication avoids four floating-point
    // conversions and lround calls per pixel. Entire hidden rows are cleared
    // with memset-equivalent fill_n instead of a per-pixel branch.
    const std::uint32_t alphaScale = static_cast<std::uint32_t>(
        std::clamp<long>(std::lround(factor * 256.0f), 0L, 256L));
    constexpr std::uint32_t kByteLanes = 0x00FF00FFu;
    constexpr std::uint32_t kRoundLanes = 0x00800080u;

    for (int y = y0; y < y1; ++y) {
        std::uint32_t* row = data + static_cast<size_t>(y) * bitmapWidth + x0;
        if (y >= revealBottom || alphaScale == 0) {
            std::fill_n(row, rowWidth, 0u);
            continue;
        }
        if (alphaScale >= 256) continue;

        for (int x = 0; x < rowWidth; ++x) {
            const std::uint32_t pixel = row[x];
            const std::uint32_t blueRed =
                (pixel & kByteLanes) * alphaScale + kRoundLanes;
            const std::uint32_t greenAlpha =
                ((pixel >> 8) & kByteLanes) * alphaScale + kRoundLanes;
            row[x] = ((blueRed >> 8) & kByteLanes) |
                     (((greenAlpha >> 8) & kByteLanes) << 8);
        }
    }
}

void RenderWidget(HWND hwnd) {
    if (!hwnd || g_hiddenForFullscreen) return;
    Settings settings = GetSettings();
    const float scale = UiScale(settings);
    const float logicalWidth = CurrentLogicalWidth(settings, g_animationProgress);
    const float logicalHeight = CurrentLogicalHeight(settings, g_animationProgress);
    const int width = std::max(1, static_cast<int>(std::ceil(logicalWidth * scale)));
    const int height = std::max(1, static_cast<int>(std::ceil(logicalHeight * scale)));
    UpdateWindowGeometry(hwnd);

    // Floating mode uses a per-pixel alpha hit mask and a fixed-size HWND.
    // This avoids recalculating an HRGN and moving the window on every frame.
    if (settings.dockMode == L"Floating") {
        if (g_windowRegionValid) {
            SetWindowRgn(hwnd, nullptr, TRUE);
            g_windowRegionValid = false;
        }
    } else {
        UpdateWindowRegion(hwnd, settings, scale, logicalWidth, logicalHeight);
    }
    RECT ownerRect{};
    if (!GetWindowRect(hwnd, &ownerRect)) return;
    const POINT glassPosition{ownerRect.left, ownerRect.top};
    const SIZE glassSize{width, height};
    const std::vector<GlassShape> glassShapes =
        BuildGlassShapes(settings, scale, logicalWidth, logicalHeight);
    g_compositionGlassActive =
        g_glassLayer.Show(hwnd, glassPosition, glassSize, glassShapes, settings);

    if (!g_renderSurface.Ensure(width, height) ||
        !g_renderFonts.Ensure(settings)) {
        return;
    }
    HDC screenDc = GetDC(nullptr);
    if (!screenDc) return;
    HDC memoryDc = g_renderSurface.dc;
    void* pixels = g_renderSurface.pixels;
    {
        Gdiplus::Graphics graphics(memoryDc);
        graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        graphics.SetTextRenderingHint(Gdiplus::TextRenderingHintAntiAliasGridFit);
        graphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
        graphics.Clear(Gdiplus::Color(0, 0, 0, 0));
        graphics.ScaleTransform(scale, scale);

        Gdiplus::GraphicsPath floatingClipPath;
        Gdiplus::RectF floatingIslandRect;
        const float floatingIslandRadius =
            FloatingIslandRadius(settings, g_animationProgress);
        if (settings.dockMode == L"Floating") {
            floatingIslandRect = FloatingIslandRect(
                settings, logicalWidth, logicalHeight, g_animationProgress);
            AddRoundedRectPath(floatingClipPath, floatingIslandRect,
                               floatingIslandRadius);
            // Keep the text and controls clipped to the same silhouette as
            // the glass, especially during the narrow early reveal frames.
            graphics.SetClip(&floatingClipPath, Gdiplus::CombineModeIntersect);
        }

        if (settings.dockMode == L"Floating" && !g_compositionGlassActive) {
            const Gdiplus::RectF islandRect = floatingIslandRect;
            const float islandRadius = floatingIslandRadius;
            const ThemePalette palette = GetThemePalette(settings);
            const BYTE fallbackAlpha = ApplyShapeOpacity(
                static_cast<BYTE>(
                    std::clamp(settings.glassTintOpacity, 0, 255)),
                settings);
            const BYTE lowerAlpha = ApplyShapeOpacity(
                static_cast<BYTE>(std::max(
                    0, static_cast<int>(settings.glassTintOpacity) - 12)),
                settings);
            Gdiplus::LinearGradientBrush islandFill(
                islandRect,
                Gdiplus::Color(fallbackAlpha, palette.topR, palette.topG, palette.topB),
                Gdiplus::Color(lowerAlpha,
                               palette.bottomR, palette.bottomG, palette.bottomB),
                Gdiplus::LinearGradientModeVertical);
            graphics.FillPath(&islandFill, &floatingClipPath);
            Gdiplus::Pen islandRim(
                Gdiplus::Color(ApplyShapeOpacity(42, settings),
                               palette.borderR, palette.borderG, palette.borderB),
                0.8f);
            graphics.DrawPath(&islandRim, &floatingClipPath);
        }

        const Gdiplus::Font& pillSmall = *g_renderFonts.pillSmall;
        const Gdiplus::Font& pillMain = *g_renderFonts.pillMain;
        const Gdiplus::Font& pillPrice = *g_renderFonts.pillPrice;
        const Gdiplus::Font& pillChange = *g_renderFonts.pillChange;
        const Gdiplus::Font& titleFont = *g_renderFonts.title;
        const Gdiplus::Font& subtitleFont = *g_renderFonts.subtitle;
        const Gdiplus::Font& categoryFont = *g_renderFonts.category;
        const Gdiplus::Font& tickerFont = *g_renderFonts.ticker;
        const Gdiplus::Font& nameFont = *g_renderFonts.name;
        const Gdiplus::Font& rowPriceFont = *g_renderFonts.rowPrice;
        const Gdiplus::Font& changeFont = *g_renderFonts.change;
        const Gdiplus::Font& footerFont = *g_renderFonts.footer;

        if (settings.dockMode == L"Floating") {
            const float pillLeft = FloatingPillLeft(settings, logicalWidth);
            const float pillTop = FloatingPillTop(settings, logicalHeight);
            if (g_animationProgress > 0.001f) {
                const float panelLeft = PanelLeft(settings, logicalWidth);
                const float panelTop = CurrentPanelTop(settings, g_animationProgress, logicalHeight);
                const float visiblePanelHeight = CurrentPanelVisibleHeight(
                    settings, g_animationProgress);
                Gdiplus::GraphicsState state = graphics.Save();
                graphics.SetClip(
                    Gdiplus::RectF(panelLeft, panelTop,
                                   static_cast<float>(kExpandedWidth),
                                   visiblePanelHeight),
                    Gdiplus::CombineModeIntersect);
                graphics.TranslateTransform(panelLeft, 0.0f, Gdiplus::MatrixOrderPrepend);
                DrawExpandedPanel(graphics, settings, titleFont, subtitleFont,
                                  categoryFont, tickerFont, nameFont,
                                  rowPriceFont, changeFont, footerFont, panelTop);
                graphics.Restore(state);
            }

            // GDI+ clipping alone was not sufficient when the panel translated
            // during bottom-attached reveals. Flush the panel drawing, apply a
            // pixel mask using the exact same morph progress as the glass,
            // then paint the pill header last so it can never be erased.
            graphics.Flush(Gdiplus::FlushIntentionSync);
            const float revealLeft = PanelLeft(settings, logicalWidth);
            const float revealTop = CurrentPanelTop(
                settings, g_animationProgress, logicalHeight);
            const float revealHeight = CurrentPanelVisibleHeight(
                settings, g_animationProgress);
            if (g_compositionGlassActive) {
                ApplyPanelRevealMask(
                    pixels, width, height,
                    static_cast<int>(std::lround(revealLeft * scale)),
                    static_cast<int>(std::lround(revealTop * scale)),
                    static_cast<int>(std::ceil(kExpandedWidth * scale)),
                    static_cast<int>(std::ceil(kPanelHeight * scale)),
                    static_cast<int>(std::floor(revealHeight * scale)),
                    g_animationProgress);
            }

            DrawPill(graphics, settings, pillSmall, pillMain, pillPrice,
                     pillChange, pillLeft, pillTop,
                     static_cast<float>(settings.pillWidth),
                     static_cast<float>(settings.pillHeight));
        } else if (settings.dockMode == L"TopAttached") {
            if (g_animationProgress > 0.001f) {
                const float panelLeft = PanelLeft(settings, logicalWidth);
                const float panelTop = CurrentPanelTop(settings, g_animationProgress, logicalHeight);
                const float visiblePanelHeight = CurrentPanelVisibleHeight(
                    settings, g_animationProgress);
                Gdiplus::GraphicsState state = graphics.Save();
                graphics.SetClip(
                    Gdiplus::RectF(panelLeft, panelTop,
                                   static_cast<float>(kExpandedWidth),
                                   visiblePanelHeight),
                    Gdiplus::CombineModeIntersect);
                graphics.TranslateTransform(panelLeft, 0.0f, Gdiplus::MatrixOrderPrepend);
                DrawExpandedPanel(graphics, settings, titleFont, subtitleFont,
                                  categoryFont, tickerFont, nameFont,
                                  rowPriceFont, changeFont, footerFont, panelTop);
                graphics.Restore(state);
            }
            DrawTopDock(graphics, settings, pillSmall, pillMain, pillPrice,
                        pillChange, logicalWidth);
        } else {
            if (g_animationProgress > 0.001f) {
                const float panelLeft = PanelLeft(settings, logicalWidth);
                const float panelTop = CurrentPanelTop(settings, g_animationProgress, logicalHeight);
                const float visiblePanelHeight = CurrentPanelVisibleHeight(
                    settings, g_animationProgress);
                Gdiplus::GraphicsState state = graphics.Save();
                graphics.SetClip(
                    Gdiplus::RectF(panelLeft, panelTop,
                                   static_cast<float>(kExpandedWidth),
                                   visiblePanelHeight),
                    Gdiplus::CombineModeIntersect);
                graphics.TranslateTransform(panelLeft, 0.0f, Gdiplus::MatrixOrderPrepend);
                DrawExpandedPanel(graphics, settings, titleFont, subtitleFont,
                                  categoryFont, tickerFont, nameFont,
                                  rowPriceFont, changeFont, footerFont, panelTop);
                graphics.Restore(state);
            }
            DrawSideDock(graphics, settings, pillSmall, pillMain,
                         logicalWidth, logicalHeight);
        }
    }

    // Reapply the alpha=1 input mask after animation fading. Otherwise the
    // panel's hit-test pixels can round back to alpha zero at low progress.
    {
        Gdiplus::Graphics maskGraphics(memoryDc);
        maskGraphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
        maskGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
        maskGraphics.ScaleTransform(scale, scale);
        Gdiplus::SolidBrush hitTestMask(Gdiplus::Color(1, 0, 0, 0));
        if (settings.dockMode == L"Floating") {
            const Gdiplus::RectF hitRect = FloatingIslandRect(
                settings, logicalWidth, logicalHeight, g_animationProgress);
            Gdiplus::GraphicsPath hitPath;
            AddRoundedRectPath(hitPath, hitRect,
                               FloatingIslandRadius(settings, g_animationProgress));
            maskGraphics.FillPath(&hitTestMask, &hitPath);
        } else {
            maskGraphics.FillRectangle(&hitTestMask, 0.0f, 0.0f,
                                       logicalWidth, logicalHeight);
        }
    }

    POINT destination{};
    CalculatePosition(settings, width, height, &destination.x, &destination.y);
    POINT source{0, 0};
    SIZE size{width, height};
    BLENDFUNCTION blend{AC_SRC_OVER, 0, 255, AC_SRC_ALPHA};
    UpdateLayeredWindow(hwnd, screenDc, &destination, &size,
                        memoryDc, &source, 0, &blend, ULW_ALPHA);


    ReleaseDC(nullptr, screenDc);
}
bool PointInRect(int x, int y, int left, int top, int right, int bottom) {
    return x >= left && x < right && y >= top && y < bottom;
}

bool PointInRoundedRect(float x, float y, const Gdiplus::RectF& rect,
                        float radius) {
    if (x < rect.X || y < rect.Y ||
        x >= rect.X + rect.Width || y >= rect.Y + rect.Height) {
        return false;
    }
    radius = std::clamp(radius, 0.0f,
                        std::min(rect.Width, rect.Height) * 0.5f);
    if (radius <= 0.5f) return true;
    const float cx = std::clamp(x, rect.X + radius,
                                rect.X + rect.Width - radius);
    const float cy = std::clamp(y, rect.Y + radius,
                                rect.Y + rect.Height - radius);
    const float dx = x - cx;
    const float dy = y - cy;
    return dx * dx + dy * dy <= radius * radius;
}

bool GetCursorClientPoint(HWND hwnd, POINT* clientPoint) {
    if (!hwnd || !clientPoint || !GetCursorPos(clientPoint)) return false;
    return ScreenToClient(hwnd, clientPoint) != FALSE;
}

HitTarget HitTestUi(HWND hwnd, int pixelX, int pixelY) {
    if (!hwnd) return HitTarget::None;
    const Settings settings = GetSettings();
    const float scale = UiScale(settings);
    const int x = static_cast<int>(std::floor(pixelX / scale));
    const int y = static_cast<int>(std::floor(pixelY / scale));
    const float logicalWidth = CurrentLogicalWidth(settings, g_animationProgress);
    const float logicalHeight = CurrentLogicalHeight(settings, g_animationProgress);

    // The compact pill / attached rail is always the first interactive layer.
    if (settings.dockMode == L"Floating") {
        const int left = static_cast<int>(std::lround(
            FloatingPillLeft(settings, logicalWidth)));
        const int top = static_cast<int>(std::lround(
            FloatingPillTop(settings, logicalHeight)));
        if (PointInRect(x, y, left, top,
                        left + settings.pillWidth, top + settings.pillHeight)) {
            return HitTarget::Pill;
        }
    } else if (settings.dockMode == L"TopAttached") {
        if (PointInRect(x, y, 0, 0,
                        static_cast<int>(logicalWidth), kTopDockHeight)) {
            return HitTarget::Pill;
        }
    } else {
        const int railLeft = settings.dockSide == L"Right"
            ? static_cast<int>(logicalWidth) - kSideDockWidth : 0;
        const int railTop = static_cast<int>(
            (logicalHeight - settings.sideDockHeight) / 2.0f);
        if (PointInRect(x, y, railLeft, railTop,
                        railLeft + kSideDockWidth,
                        railTop + settings.sideDockHeight)) {
            return HitTarget::Pill;
        }
    }

    if (g_animationProgress <= 0.001f) return HitTarget::None;

    const int panelLeft = static_cast<int>(std::lround(
        PanelLeft(settings, logicalWidth)));
    const int panelTop = static_cast<int>(std::lround(
        CurrentPanelTop(settings, g_animationProgress, logicalHeight)));
    const int panelX = x - panelLeft;
    const int panelY = y - panelTop;

    const int visiblePanelHeight = static_cast<int>(std::ceil(
        CurrentPanelVisibleHeight(settings, g_animationProgress)));
    const bool insidePanel = PointInRect(
        panelX, panelY, 0, 0, kExpandedWidth, visiblePanelHeight);
    if (!insidePanel) {
        if (settings.dockMode == L"Floating") {
            const Gdiplus::RectF island = FloatingIslandRect(
                settings, logicalWidth, logicalHeight, g_animationProgress);
            if (PointInRoundedRect(static_cast<float>(x) + 0.5f,
                                   static_cast<float>(y) + 0.5f,
                                   island,
                                   FloatingIslandRadius(settings, g_animationProgress))) {
                return HitTarget::PanelBackground;
            }
        }
        return HitTarget::None;
    }

    for (int category = 0; category < 3; ++category) {
        const UiHitRect& rect = kCategoryHitRects[category];
        if (PointInRect(panelX, panelY, rect.left, rect.top,
                        rect.right, rect.bottom)) {
            return category == 0 ? HitTarget::CategoryCrypto
                 : category == 1 ? HitTarget::CategoryUs
                                 : HitTarget::CategoryIdx;
        }
    }

    if (PointInRect(panelX, panelY,
                    kSearchHitRect.left, kSearchHitRect.top,
                    kSearchHitRect.right, kSearchHitRect.bottom)) {
        return HitTarget::Search;
    }

    if (PointInRect(panelX, panelY,
                    kRefreshHitRect.left, kRefreshHitRect.top,
                    kRefreshHitRect.right, kRefreshHitRect.bottom)) {
        return HitTarget::Refresh;
    }

    const int visibleRows = g_searchSubmitted
        ? (g_searchLoading ? 0 : g_searchResultCount)
        : kAssetsPerGroup;
    for (int row = 0; row < visibleRows; ++row) {
        const int rowTop = kListRowTop + row * kListRowStep;
        if (PointInRect(panelX, panelY, 20, rowTop,
                        460, rowTop + kListRowHeight)) {
            return static_cast<HitTarget>(
                static_cast<int>(HitTarget::Row0) + row);
        }
    }

    // The panel background remains part of the hover area, but is not a
    // button. The island collapses only after the pointer leaves the whole UI.
    return HitTarget::PanelBackground;
}

bool CursorInsideIsland(HWND hwnd, POINT* clientPoint = nullptr) {
    if (!hwnd) return false;
    POINT screen{};
    RECT windowRect{};
    if (!GetCursorPos(&screen) || !GetWindowRect(hwnd, &windowRect) ||
        !PtInRect(&windowRect, screen)) {
        return false;
    }
    POINT local = screen;
    if (!ScreenToClient(hwnd, &local)) return false;

    const Settings settings = GetSettings();
    bool inside = true;
    if (settings.dockMode == L"Floating") {
        const float scale = UiScale(settings);
        const float logicalWidth = CurrentLogicalWidth(settings, g_animationProgress);
        const float logicalHeight = CurrentLogicalHeight(settings, g_animationProgress);
        const Gdiplus::RectF island = FloatingIslandRect(
            settings, logicalWidth, logicalHeight, g_animationProgress);
        inside = PointInRoundedRect(
            static_cast<float>(local.x) / scale,
            static_cast<float>(local.y) / scale,
            island, FloatingIslandRadius(settings, g_animationProgress));
    } else {
        HRGN region = CreateRectRgn(0, 0, 0, 0);
        if (region) {
            const int regionResult = GetWindowRgn(hwnd, region);
            if (regionResult != ERROR) {
                inside = PtInRegion(region, local.x, local.y) != FALSE;
            }
            DeleteObject(region);
        }
    }

    if (clientPoint) *clientPoint = local;
    return inside;
}

void ReconcileHoverAfterMouseLeave(HWND hwnd) {
    g_trackingMouse = false;
    POINT client{};
    if (CursorInsideIsland(hwnd, &client)) {
        TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd, 0};
        g_trackingMouse = TrackMouseEvent(&track) != FALSE;
        const HitTarget target = HitTestUi(hwnd, client.x, client.y);
        g_hoverTarget = target;
        if (target != HitTarget::None &&
            (!g_expanded || (g_animating && g_animationTo < 0.5f))) {
            StartAnimation(hwnd, true);
        } else if (!g_animating) {
            RenderWidget(hwnd);
        }
        return;
    }

    g_hoverTarget = HitTarget::None;
    if (!g_searchFocused &&
        (g_expanded || g_animationProgress > 0.001f)) {
        StartAnimation(hwnd, false);
    } else if (!g_animating) {
        RenderWidget(hwnd);
    }
}

void CloseIfPointerOutside(HWND hwnd) {
    if (!CursorInsideIsland(hwnd) && !g_searchFocused &&
        (g_expanded || g_animationProgress > 0.001f)) {
        g_hoverTarget = HitTarget::None;
        StartAnimation(hwnd, false);
    }
}

void HandleClick(HWND hwnd, HitTarget target) {
    Settings settings = GetSettings();

    // Expansion is hover-driven. Clicking the pill or empty glass does not
    // toggle it; leaving the island closes it automatically.
    if (target == HitTarget::Pill || target == HitTarget::PanelBackground) {
        return;
    }

    if (target == HitTarget::CategoryCrypto ||
        target == HitTarget::CategoryUs ||
        target == HitTarget::CategoryIdx) {
        g_activeCategory = target == HitTarget::CategoryCrypto ? 0
                         : target == HitTarget::CategoryUs ? 1 : 2;
        g_selectedAsset = CategoryStart(g_activeCategory);
        g_selectedCustom = false;
        KillTimer(hwnd, kCustomQuoteTimer);
        g_searchFocused = false;
        g_searchText.clear();
        g_searchSubmitted = false;
        g_searchLoading = false;
        g_searchResultCount = 0;
        RenderWidget(hwnd);
        return;
    }

    if (target == HitTarget::Search) {
        g_searchFocused = true;
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);
        RenderWidget(hwnd);
        return;
    }

    if (target >= HitTarget::Row0 && target <= HitTarget::Row4) {
        const int row = static_cast<int>(target) -
                        static_cast<int>(HitTarget::Row0);
        if (g_searchSubmitted && !g_searchLoading &&
            row >= 0 && row < g_searchResultCount) {
            g_customAsset = g_searchResults[row];
            g_customQuote = Quote{};
            g_selectedCustom = true;
            g_searchFocused = false;
            SetTimer(hwnd, kCustomQuoteTimer,
                     static_cast<UINT>(settings.refreshSeconds) * 1000, nullptr);
            StartCustomQuoteFetch(g_customAsset);
            RenderWidget(hwnd);
            return;
        }

        if (!g_searchSubmitted && row >= 0 && row < kAssetsPerGroup) {
            g_selectedAsset = CategoryStart(g_activeCategory) + row;
            g_selectedCustom = false;
            KillTimer(hwnd, kCustomQuoteTimer);
            g_searchFocused = false;
            RenderWidget(hwnd);
            return;
        }
    }

    if (target == HitTarget::Refresh) {
        if (g_refreshEvent) SetEvent(g_refreshEvent);
        if (g_selectedCustom) StartCustomQuoteFetch(g_customAsset);
        RenderWidget(hwnd);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case kMessageAnimationFrame:
            AdvanceAnimationFrame(hwnd);
            g_animationFramePending.store(false, std::memory_order_release);
            return 0;
        case kMessageMarketData: {
            auto* snapshot = reinterpret_cast<MarketSnapshot*>(lParam);
            if (snapshot) {
                g_snapshot = *snapshot;
                delete snapshot;
                RenderWidget(hwnd);
            }
            return 0;
        }
        case kMessageSearchResults: {
            auto* results = reinterpret_cast<SearchResultsMessage*>(lParam);
            if (results) {
                if (results->requestId == g_searchRequestId) {
                    g_searchResultCount = std::clamp(results->count, 0, 5);
                    for (int i = 0; i < g_searchResultCount; ++i)
                        g_searchResults[i] = results->items[i];
                    g_searchLoading = false;
                    RenderWidget(hwnd);
                }
                delete results;
            }
            return 0;
        }
        case kMessageCustomQuote: {
            auto* result = reinterpret_cast<CustomQuoteMessage*>(lParam);
            if (result) {
                if (g_selectedCustom && result->symbol == g_customAsset.symbol)
                    g_customQuote = result->quote;
                delete result;
                RenderWidget(hwnd);
            }
            return 0;
        }
        case kMessageRepaint: {
            Settings settings = GetSettings();
            if (g_selectedCustom)
                SetTimer(hwnd, kCustomQuoteTimer,
                         static_cast<UINT>(settings.refreshSeconds) * 1000, nullptr);
            RenderWidget(hwnd);
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (!g_trackingMouse) {
                TRACKMOUSEEVENT track{sizeof(track), TME_LEAVE, hwnd, 0};
                g_trackingMouse = TrackMouseEvent(&track) != FALSE;
            }
            POINT cursorClient{};
            if (!GetCursorClientPoint(hwnd, &cursorClient)) {
                cursorClient.x = GET_X_LPARAM(lParam);
                cursorClient.y = GET_Y_LPARAM(lParam);
            }
            const HitTarget target = HitTestUi(
                hwnd, cursorClient.x, cursorClient.y);
            const bool controlHot =
                target != HitTarget::None &&
                target != HitTarget::PanelBackground &&
                target != HitTarget::Pill;
            SetCursor(LoadCursorW(nullptr,
                                  controlHot ? IDC_HAND : IDC_ARROW));

            const bool hoverChanged = target != g_hoverTarget;
            g_hoverTarget = target;

            // Hovering any visible part of the island opens it. If the pointer
            // returns while it is closing, reverse from the current position.
            if (target != HitTarget::None &&
                (!g_expanded || (g_animating && g_animationTo < 0.5f))) {
                StartAnimation(hwnd, true);
                return 0;
            }
            // The next animation frame will paint the latest hover target.
            // Avoid a second full surface/composition render in the middle of
            // an animation when the pointer crosses panel controls.
            if (hoverChanged && !g_animating) {
                RenderWidget(hwnd);
            }
            return 0;
        }
        case WM_MOUSELEAVE:
            ReconcileHoverAfterMouseLeave(hwnd);
            return 0;
        case WM_LBUTTONDOWN: {
            POINT cursorClient{};
            if (!GetCursorClientPoint(hwnd, &cursorClient)) {
                cursorClient.x = GET_X_LPARAM(lParam);
                cursorClient.y = GET_Y_LPARAM(lParam);
            }
            g_pressedTarget = HitTestUi(hwnd, cursorClient.x, cursorClient.y);
            if (g_pressedTarget == HitTarget::Pill) {
                // A click is also a fallback activation path if a mouse-move
                // event was swallowed during a compositor/window transition.
                StartAnimation(hwnd, true);
                g_pressedTarget = HitTarget::None;
                return 0;
            }
            if (g_pressedTarget == HitTarget::PanelBackground) {
                g_pressedTarget = HitTarget::None;
            }
            if (g_pressedTarget != HitTarget::None) {
                SetCapture(hwnd);
                RenderWidget(hwnd);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            const HitTarget pressed = g_pressedTarget;
            if (GetCapture() == hwnd) ReleaseCapture();
            g_pressedTarget = HitTarget::None;
            POINT cursorClient{};
            if (!GetCursorClientPoint(hwnd, &cursorClient)) {
                cursorClient.x = GET_X_LPARAM(lParam);
                cursorClient.y = GET_Y_LPARAM(lParam);
            }
            const HitTarget released = HitTestUi(
                hwnd, cursorClient.x, cursorClient.y);
            if (pressed != HitTarget::None && pressed == released) {
                HandleClick(hwnd, released);
            }
            RenderWidget(hwnd);
            return 0;
        }
        case WM_CAPTURECHANGED:
            g_pressedTarget = HitTarget::None;
            RenderWidget(hwnd);
            return 0;
        case WM_CHAR:
            if (g_searchFocused) {
                wchar_t ch = static_cast<wchar_t>(wParam);
                if (ch == L'\r') {
                    StartSearch();
                } else if (ch == L'\b') {
                    if (!g_searchText.empty()) g_searchText.pop_back();
                    g_searchSubmitted = false;
                    g_searchResultCount = 0;
                    g_searchLoading = false;
                    RenderWidget(hwnd);
                } else if (ch >= 32 && ch != 127 && g_searchText.size() < 48) {
                    g_searchText.push_back(ch);
                    g_searchSubmitted = false;
                    g_searchResultCount = 0;
                    g_searchLoading = false;
                    RenderWidget(hwnd);
                }
            }
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) {
                if (g_searchFocused) {
                    g_searchFocused = false;
                    g_searchText.clear();
                    g_searchSubmitted = false;
                    g_searchLoading = false;
                    g_searchResultCount = 0;
                    CloseIfPointerOutside(hwnd);
                    RenderWidget(hwnd);
                } else if (g_expanded) {
                    StartAnimation(hwnd, false);
                }
                return 0;
            }
            return 0;
        case WM_MOUSEACTIVATE:
            return MA_ACTIVATE;
        case WM_NCHITTEST:
            return HTCLIENT;
        case WM_TIMER: {
            if (wParam == kAnimationTimer) {
                AdvanceAnimationFrame(hwnd);
                return 0;
            }
            if (wParam == kCustomQuoteTimer) {
                if (g_selectedCustom) StartCustomQuoteFetch(g_customAsset);
                return 0;
            }
            if (wParam == kVisibilityTimer) {
                Settings settings = GetSettings();
                bool shouldHide = settings.hideWhenFullscreen && IsForegroundFullscreen(hwnd);
                if (shouldHide && !g_hiddenForFullscreen) {
                    g_hiddenForFullscreen = true;
                    g_trackingMouse = false;
                    g_hoverTarget = HitTarget::None;
                    g_pressedTarget = HitTarget::None;
                    g_glassLayer.Hide();
                    g_compositionGlassActive = false;
                    ShowWindow(hwnd, SW_HIDE);
                } else if (!shouldHide && g_hiddenForFullscreen) {
                    g_hiddenForFullscreen = false;
                    g_trackingMouse = false;
                    UpdateWindowGeometry(hwnd);
                    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
                    RenderWidget(hwnd);
                    CloseIfPointerOutside(hwnd);
                } else if (!settings.alwaysOnTop && !g_hiddenForFullscreen) {
                    RenderWidget(hwnd);
                }
                return 0;
            }
            return 0;
        }
        case WM_KILLFOCUS:
            g_searchFocused = false;
            g_pressedTarget = HitTarget::None;
            CloseIfPointerOutside(hwnd);
            RenderWidget(hwnd);
            return 0;
        case WM_DISPLAYCHANGE:
        case WM_SETTINGCHANGE:
            if (g_animating && !RestartAnimationFrameTimer(hwnd)) {
                g_animationProgress = g_animationTo;
                StopAnimationTimer(hwnd);
            }
            RenderWidget(hwnd);
            return 0;
        case WM_ERASEBKGND:
            return 1;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        case WM_DESTROY:
            KillTimer(hwnd, kVisibilityTimer);
            StopAnimationTimer(hwnd);
            KillTimer(hwnd, kCustomQuoteTimer);
            g_glassLayer.Destroy();
            g_compositionGlassActive = false;
            g_renderFonts.Reset();
            g_renderSurface.Reset();
            g_window.store(nullptr);
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

DWORD WINAPI UiThreadProc(void*) {
    InitializeGlassComposition();

    Gdiplus::GdiplusStartupInput startupInput;
    if (Gdiplus::GdiplusStartup(&g_gdiplusToken, &startupInput, nullptr) != Gdiplus::Ok) {
        g_uiFailed.store(true);
        SetEvent(g_windowReadyEvent);
        g_glassLayer.ShutdownComposition();
        return 1;
    }

    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_DBLCLKS;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.lpszClassName = kWindowClassName;
    windowClass.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassExW(&windowClass) && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) {
        g_uiFailed.store(true);
        SetEvent(g_windowReadyEvent);
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_glassLayer.ShutdownComposition();
        return 1;
    }

    Settings settings = GetSettings();
    DWORD exStyle = WS_EX_TOOLWINDOW | WS_EX_LAYERED;
    if (settings.alwaysOnTop) exStyle |= WS_EX_TOPMOST;
    HWND hwnd = CreateWindowExW(exStyle, kWindowClassName,
                                L"Fluent Market Island", WS_POPUP,
                                0, 0, kPillWidth, kPillHeight,
                                nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
    if (!hwnd) {
        g_uiFailed.store(true);
        SetEvent(g_windowReadyEvent);
        UnregisterClassW(kWindowClassName, GetModuleHandleW(nullptr));
        Gdiplus::GdiplusShutdown(g_gdiplusToken);
        g_glassLayer.ShutdownComposition();
        return 1;
    }

    g_window.store(hwnd);
    SetTimer(hwnd, kVisibilityTimer, 1000, nullptr);
    RenderWidget(hwnd);
    ShowWindow(hwnd, SW_SHOWNOACTIVATE);
    POINT initialCursor{};
    if (CursorInsideIsland(hwnd, &initialCursor)) {
        const HitTarget initialTarget = HitTestUi(
            hwnd, initialCursor.x, initialCursor.y);
        if (initialTarget != HitTarget::None) {
            g_hoverTarget = initialTarget;
            StartAnimation(hwnd, true);
        }
    }
    SetEvent(g_windowReadyEvent);

    MSG message{};
    while (GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }

    if (IsWindow(hwnd)) DestroyWindow(hwnd);
    g_glassLayer.Destroy();
    g_renderFonts.Reset();
    g_renderSurface.Reset();
    g_glassLayer.ShutdownComposition();
    UnregisterClassW(kWindowClassName, GetModuleHandleW(nullptr));
    Gdiplus::GdiplusShutdown(g_gdiplusToken);
    return 0;
}

}  // namespace

// Windhawk tool-process callbacks.
BOOL WhTool_ModInit() {
    LoadSettings();
    g_stopEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    g_refreshEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    g_windowReadyEvent = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!g_stopEvent || !g_refreshEvent || !g_windowReadyEvent) return FALSE;

    g_uiThread = CreateThread(nullptr, 0, UiThreadProc, nullptr, 0, nullptr);
    if (!g_uiThread) return FALSE;
    if (WaitForSingleObject(g_windowReadyEvent, 10000) != WAIT_OBJECT_0 || g_uiFailed.load()) {
        return FALSE;
    }
    g_dataThread = CreateThread(nullptr, 0, DataThreadProc, nullptr, 0, nullptr);
    if (!g_dataThread) return FALSE;
    return TRUE;
}

void WhTool_ModSettingsChanged() {
    LoadSettings();
    HWND hwnd = g_window.load();
    if (hwnd) PostMessageW(hwnd, kMessageRepaint, 0, 0);
    if (g_refreshEvent) SetEvent(g_refreshEvent);
}

void WhTool_ModUninit() {
    if (g_stopEvent) SetEvent(g_stopEvent);
    if (g_dataThread) {
        WaitForSingleObject(g_dataThread, INFINITE);
        CloseHandle(g_dataThread);
        g_dataThread = nullptr;
    }
    HWND hwnd = g_window.load();
    if (hwnd) PostMessageW(hwnd, WM_CLOSE, 0, 0);
    if (g_uiThread) {
        WaitForSingleObject(g_uiThread, INFINITE);
        CloseHandle(g_uiThread);
        g_uiThread = nullptr;
    }
    if (g_stopEvent) { CloseHandle(g_stopEvent); g_stopEvent = nullptr; }
    if (g_refreshEvent) { CloseHandle(g_refreshEvent); g_refreshEvent = nullptr; }
    if (g_windowReadyEvent) { CloseHandle(g_windowReadyEvent); g_windowReadyEvent = nullptr; }
}

// Windhawk tool-mod launcher: spawn a dedicated windhawk.exe process.
bool g_isToolModProcessLauncher = false;
HANDLE g_toolModProcessMutex = nullptr;

void WINAPI EntryPoint_Hook() {
    Wh_Log(L"> ");
    ExitThread(0);
}

BOOL Wh_ModInit() {
    bool isExcluded = false;
    bool isToolModProcess = false;
    bool isCurrentToolModProcess = false;
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv) {
        Wh_Log(L"CommandLineToArgvW failed");
        return FALSE;
    }

    for (int i = 1; i < argc; i++) {
        if (wcscmp(argv[i], L"-service") == 0 ||
            wcscmp(argv[i], L"-service-start") == 0 ||
            wcscmp(argv[i], L"-service-stop") == 0) {
            isExcluded = true;
            break;
        }
    }
    for (int i = 1; i < argc - 1; i++) {
        if (wcscmp(argv[i], L"-tool-mod") == 0) {
            isToolModProcess = true;
            if (wcscmp(argv[i + 1], WH_MOD_ID) == 0) isCurrentToolModProcess = true;
            break;
        }
    }
    LocalFree(argv);
    if (isExcluded) return FALSE;

    if (isCurrentToolModProcess) {
        g_toolModProcessMutex = CreateMutexW(nullptr, TRUE, L"windhawk-tool-mod_" WH_MOD_ID);
        if (!g_toolModProcessMutex) {
            Wh_Log(L"CreateMutex failed");
            ExitProcess(1);
        }
        if (GetLastError() == ERROR_ALREADY_EXISTS) {
            Wh_Log(L"Tool mod already running (%s)", WH_MOD_ID);
            ExitProcess(1);
        }
        if (!WhTool_ModInit()) ExitProcess(1);

        IMAGE_DOS_HEADER* dosHeader = reinterpret_cast<IMAGE_DOS_HEADER*>(GetModuleHandleW(nullptr));
        IMAGE_NT_HEADERS* ntHeaders = reinterpret_cast<IMAGE_NT_HEADERS*>(
            reinterpret_cast<BYTE*>(dosHeader) + dosHeader->e_lfanew);
        DWORD entryPointRVA = ntHeaders->OptionalHeader.AddressOfEntryPoint;
        void* entryPoint = reinterpret_cast<BYTE*>(dosHeader) + entryPointRVA;
        Wh_SetFunctionHook(entryPoint, reinterpret_cast<void*>(EntryPoint_Hook), nullptr);
        return TRUE;
    }
    if (isToolModProcess) return FALSE;
    g_isToolModProcessLauncher = true;
    return TRUE;
}

void Wh_ModAfterInit() {
    if (!g_isToolModProcessLauncher) return;
    WCHAR currentProcessPath[MAX_PATH];
    DWORD pathLength = GetModuleFileNameW(nullptr, currentProcessPath, ARRAYSIZE(currentProcessPath));
    if (pathLength == 0 || pathLength >= ARRAYSIZE(currentProcessPath)) {
        Wh_Log(L"GetModuleFileNameW failed");
        return;
    }
    WCHAR commandLine[MAX_PATH + 128];
    swprintf_s(commandLine, ARRAYSIZE(commandLine), L"\"%s\" -tool-mod \"%s\"",
               currentProcessPath, WH_MOD_ID);

    HMODULE kernelModule = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelModule) kernelModule = GetModuleHandleW(L"kernel32.dll");
    if (!kernelModule) {
        Wh_Log(L"No kernelbase.dll/kernel32.dll");
        return;
    }
    using CreateProcessInternalW_t = BOOL(WINAPI*)(
        HANDLE, LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
        WINBOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW,
        LPPROCESS_INFORMATION, PHANDLE);
    auto createProcessInternalW = reinterpret_cast<CreateProcessInternalW_t>(
        GetProcAddress(kernelModule, "CreateProcessInternalW"));
    if (!createProcessInternalW) {
        Wh_Log(L"CreateProcessInternalW not found");
        return;
    }
    STARTUPINFOW startupInfo{};
    startupInfo.cb = sizeof(startupInfo);
    startupInfo.dwFlags = STARTF_FORCEOFFFEEDBACK;
    PROCESS_INFORMATION processInfo{};
    if (!createProcessInternalW(nullptr, currentProcessPath, commandLine,
                                nullptr, nullptr, FALSE, NORMAL_PRIORITY_CLASS,
                                nullptr, nullptr, &startupInfo, &processInfo, nullptr)) {
        Wh_Log(L"CreateProcessInternalW failed");
        return;
    }
    CloseHandle(processInfo.hProcess);
    CloseHandle(processInfo.hThread);
}

void Wh_ModSettingsChanged() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModSettingsChanged();
}

void Wh_ModUninit() {
    if (g_isToolModProcessLauncher) return;
    WhTool_ModUninit();
    ExitProcess(0);
}