#pragma once

// Minimal SDL2 stand-in for the console build. The game uses SDL only for one
// desktop affordance: a "trust this mod author?" message box. libultragx has no
// SDL; this header provides just enough of the SDL_MessageBox* surface to compile,
// and SDL_ShowMessageBox auto-selects the default ("Yes") so the console boot is
// not blocked by a dialog that cannot be shown.

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SDL_arraysize(array) (sizeof(array) / sizeof((array)[0]))

typedef enum {
    SDL_MESSAGEBOX_ERROR = 0x00000010,
    SDL_MESSAGEBOX_WARNING = 0x00000020,
    SDL_MESSAGEBOX_INFORMATION = 0x00000040,
} SDL_MessageBoxFlags;

enum {
    SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT = 0x00000001,
    SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT = 0x00000002,
};

enum {
    SDL_MESSAGEBOX_COLOR_BACKGROUND = 0,
    SDL_MESSAGEBOX_COLOR_TEXT,
    SDL_MESSAGEBOX_COLOR_BUTTON_BORDER,
    SDL_MESSAGEBOX_COLOR_BUTTON_BACKGROUND,
    SDL_MESSAGEBOX_COLOR_BUTTON_SELECTED,
    SDL_MESSAGEBOX_COLOR_MAX,
};

typedef struct {
    uint32_t flags;
    int buttonid;
    const char* text;
} SDL_MessageBoxButtonData;

typedef struct {
    uint8_t r, g, b;
} SDL_MessageBoxColor;

typedef struct {
    SDL_MessageBoxColor colors[SDL_MESSAGEBOX_COLOR_MAX];
} SDL_MessageBoxColorScheme;

typedef struct {
    uint32_t flags;
    void* window;
    const char* title;
    const char* message;
    int numbuttons;
    const SDL_MessageBoxButtonData* buttons;
    const SDL_MessageBoxColorScheme* colorScheme;
} SDL_MessageBoxData;

// No console UI; pick the return-key default button (the "Yes"/trust choice) and
// report success so the caller proceeds.
static inline int SDL_ShowMessageBox(const SDL_MessageBoxData* messageboxdata, int* buttonid) {
    int picked = 1;
    if (messageboxdata != NULL) {
        for (int i = 0; i < messageboxdata->numbuttons; i++) {
            if (messageboxdata->buttons[i].flags & SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT) {
                picked = messageboxdata->buttons[i].buttonid;
                break;
            }
        }
    }
    if (buttonid != NULL) {
        *buttonid = picked;
    }
    return 0;
}

// No console UI - log-only; report success. Matches the desktop signature so callers
// that ignore the result compile unchanged.
static inline int SDL_ShowSimpleMessageBox(uint32_t flags, const char* title, const char* message, void* window) {
    (void) flags;
    (void) title;
    (void) message;
    (void) window;
    return 0;
}

// Milliseconds since boot, read straight from the PPC time base register. Done with
// inline asm rather than libogc's gettime()/ticks_to_millisecs so this widely-included
// stub pulls in no libogc headers: <ogc/lwp_watchdog.h> drags in gccore.h, whose GX
// gx.h/gu.h define Mtx/Vtx/guOrtho and would clash with libultragx's N64 SDK types.
// The time base runs at the bus clock / 4: 60.75 MHz on Wii, 40.5 MHz on GameCube.
static inline uint32_t SDL_GetTicks(void) {
    uint32_t hi, lo, tmp;
    do {
        __asm__ volatile("mftbu %0" : "=r"(hi));
        __asm__ volatile("mftb  %0" : "=r"(lo));
        __asm__ volatile("mftbu %0" : "=r"(tmp));
    } while (tmp != hi);
    uint64_t tb = ((uint64_t) hi << 32) | lo;
#ifdef HW_RVL
    return (uint32_t) (tb / 60750ULL); // 60.75 MHz / 1000
#else
    return (uint32_t) (tb / 40500ULL); // 40.5 MHz / 1000
#endif
}

#ifdef __cplusplus
}
#endif
