/* Native Windows version. No .NET or third-party runtime required. */
#include <stdlib.h>
#include <string.h>

static int deck[16], found[16], first = -1, second = -1, moves, pairs;

static void new_game(void) {
    first = second = -1;
    moves = pairs = 0;
    memset(found, 0, sizeof found);
    for (int i = 0; i < 16; ++i) deck[i] = i % 8;
    for (int i = 15; i > 0; --i) {
        int j = rand() % (i + 1), tmp = deck[i];
        deck[i] = deck[j];
        deck[j] = tmp;
    }
}

/* 0: ignored, 1: first card, 2: match, 3: wait then hide. */
static int open_card(int i) {
    if (i < 0 || i >= 16 || found[i] || i == first || second >= 0) return 0;
    if (first < 0) { first = i; return 1; }
    ++moves;
    if (deck[first] == deck[i]) {
        found[first] = found[i] = 1;
        first = -1;
        ++pairs;
        return 2;
    }
    second = i;
    return 3;
}

static void close_pair(void) { first = second = -1; }

#ifndef MEMORY_GAME_TEST
#ifndef UNICODE
#define UNICODE
#endif
#ifndef _UNICODE
#define _UNICODE
#endif
#include <windows.h>
#include <wchar.h>
#include <time.h>

static HWND cards[16], score, restart;
static HFONT emoji_font, text_font;
static int dpi = 96;
static const wchar_t *emoji[] = {L"🍎", L"🍌", L"🍓", L"🍇", L"🍉", L"🍒", L"🥝", L"🍑"};
static int px(int n) { return MulDiv(n, dpi, 96); }

static void render(void) {
    wchar_t text[100];
    if (pairs == 8)
        swprintf(text, 100, L"Победа! Ходов: %d", moves);
    else
        swprintf(text, 100, L"Пары: %d/8     •     Ходы: %d", pairs, moves);
    SetWindowTextW(score, text);
    for (int i = 0; i < 16; ++i) {
        SetWindowTextW(cards[i], found[i] || i == first || i == second ? emoji[deck[i]] : L"❓");
        EnableWindow(cards[i], !found[i]);
    }
}

static void layout(HWND window) {
    RECT r;
    GetClientRect(window, &r);
    int pad = px(12), gap = px(8), top = px(68);
    int w = (r.right - 2 * pad - 3 * gap) / 4;
    int h = (r.bottom - top - px(68) - 3 * gap) / 4;
    MoveWindow(score, pad, px(8), r.right - 2 * pad, px(48), TRUE);
    MoveWindow(restart, pad, r.bottom - px(56), r.right - 2 * pad, px(44), TRUE);
    for (int i = 0; i < 16; ++i)
        MoveWindow(cards[i], pad + i % 4 * (w + gap), top + i / 4 * (h + gap), w, h, TRUE);
}

static LRESULT CALLBACK window_proc(HWND window, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE: {
        HINSTANCE instance = ((CREATESTRUCTW *)lp)->hInstance;
        emoji_font = CreateFontW(-px(36), 0, 0, 0, FW_NORMAL, 0, 0, 0,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI Emoji");
        text_font = CreateFontW(-px(19), 0, 0, 0, FW_NORMAL, 0, 0, 0,
            DEFAULT_CHARSET, 0, 0, CLEARTYPE_QUALITY, 0, L"Segoe UI");
        score = CreateWindowW(L"STATIC", L"", WS_CHILD | WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE,
            0, 0, 0, 0, window, NULL, instance, NULL);
        restart = CreateWindowW(L"BUTTON", L"Новая игра", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
            0, 0, 0, 0, window, (HMENU)(INT_PTR)1, instance, NULL);
        SendMessageW(score, WM_SETFONT, (WPARAM)text_font, TRUE);
        SendMessageW(restart, WM_SETFONT, (WPARAM)text_font, TRUE);
        for (int i = 0; i < 16; ++i) {
            cards[i] = CreateWindowW(L"BUTTON", L"", WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                0, 0, 0, 0, window, (HMENU)(INT_PTR)(100 + i), instance, NULL);
            SendMessageW(cards[i], WM_SETFONT, (WPARAM)emoji_font, TRUE);
        }
        new_game();
        render();
        return 0;
    }
    case WM_GETMINMAXINFO:
        ((MINMAXINFO *)lp)->ptMinTrackSize = (POINT){px(360), px(480)};
        return 0;
    case WM_SIZE:
        if (wp != SIZE_MINIMIZED) layout(window);
        return 0;
    case WM_COMMAND:
        if (HIWORD(wp) != BN_CLICKED) break;
        if (LOWORD(wp) == 1) {
            KillTimer(window, 1);
            new_game();
            render();
        } else if (LOWORD(wp) >= 100 && LOWORD(wp) < 116) {
            int result = open_card(LOWORD(wp) - 100);
            if (result == 3 && !SetTimer(window, 1, 750, NULL)) close_pair();
            render();
        }
        return 0;
    case WM_TIMER:
        if (wp == 1) {
            KillTimer(window, 1);
            close_pair();
            render();
        }
        return 0;
    case WM_DESTROY:
        KillTimer(window, 1);
        DeleteObject(emoji_font);
        DeleteObject(text_font);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(window, msg, wp, lp);
}

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prev, PWSTR cmd, int show) {
    (void)prev; (void)cmd;
    SetProcessDPIAware();
    HDC screen = GetDC(NULL);
    dpi = GetDeviceCaps(screen, LOGPIXELSY);
    ReleaseDC(NULL, screen);
    srand((unsigned)time(NULL) ^ GetTickCount());
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = instance;
    wc.hCursor = LoadCursorW(NULL, IDC_ARROW);
    wc.hIcon = LoadIconW(NULL, IDI_APPLICATION);
    wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
    wc.lpszClassName = L"EmojiMemoryGame";
    if (!RegisterClassW(&wc)) return 1;
    HWND window = CreateWindowW(wc.lpszClassName, L"Найди пару • Эмодзи", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, px(456), px(590), NULL, NULL, instance, NULL);
    if (!window) return 1;
    ShowWindow(window, show);
    MSG msg;
    int result;
    while ((result = GetMessageW(&msg, NULL, 0, 0)) > 0) {
        if (!IsDialogMessageW(window, &msg)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }
    return result < 0 ? 1 : (int)msg.wParam;
}
#endif
