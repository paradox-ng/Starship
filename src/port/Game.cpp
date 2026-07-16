#include <libultraship.h>

#include <Fast3D/interpreter.h>
#include "Engine.h"

// libultragx's GX backend (gfx_gx_api.cpp / gfx_gx_tev.cpp) references these render
// bisect flags as externs and expects the game to define them (see Ghostship's
// Game.cpp). Full pipeline, no skips.
extern "C" {
int g_gx_skip_draw = 0;
int g_gx_skip_geom = 0;
int g_gx_stop_at = 5; // 1=TEV 2=vtxdesc 3=mtx 4=chan 5=full
}

extern "C" {
#include <sf64mesg.h>
    void Main_SetVIMode(void);
    void Main_Initialize(void);
    void Main_ThreadEntry(void* arg);
    void Lib_FillScreen(u8 setFill);
    void Graphics_ThreadUpdate();
    void AudioThread_CreateTask();
}

extern "C"
void Graphics_PushFrame(Gfx* data) {
    GameEngine::ProcessGfxCommands(data);
}

extern "C" void Timer_Update();

void push_frame() {
    Graphics_ThreadUpdate();
    GameEngine::StartAudioFrame();
    GameEngine::Instance->StartFrame();
    Timer_Update();
    // thread5_iteration();
    GameEngine::EndAudioFrame();
}

#ifdef _WIN32
int SDL_main(int argc, char **argv) {
#else
#if defined(__cplusplus) && defined(PLATFORM_IOS)
extern "C"
#endif
int main(int argc, char *argv[]) {
#endif
    GameEngine::Create();
    Main_SetVIMode();
    Lib_FillScreen(1);
    Main_Initialize();
    Main_ThreadEntry(NULL);
    while (WindowIsRunning()) {
        push_frame();
    }
    GameEngine::Instance->Destroy();
    return 0;
}