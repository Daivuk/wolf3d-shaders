/*
    Program entry and main loop.
    This is where we initialize SDL and poll events.
*/

#if defined(WIN32)
#include <windows.h>
#endif

#include <GL/gl3w.h>
#include <SDL.h>

#include <assert.h>
#include <chrono>
#include <stdio.h>

void wolf3d_init();
void wolf3d_update();
void wolf3d_shutdown();
void audioCallback(void *userdata, Uint8 *stream, int len);

int _argc; // global arguments. this is referenced a bit everywhere
char** _argv;

#if defined(WIN32)
int CALLBACK WinMain(HINSTANCE appInstance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdCount)
{
    int argc = 0;
    auto cmdLineA = GetCommandLineA();
    int cmdLineL = strlen(cmdLineA);
    wchar_t *cmdLineW = new wchar_t[cmdLineL + 1];
    for (int i = 0; i < cmdLineL + 1; ++i)
    {
        cmdLineW[i] = (wchar_t)cmdLineA[i];
    }
    auto argvW = CommandLineToArgvW(cmdLineW, &argc);
    auto argv = new char*[argc];
    for (int j = 0; j < argc; ++j)
    {
        auto argW = argvW[j];
        auto len = (int)std::wstring(argW).size();
        auto arg = new char[len + 1];
        for (int i = 0; i < len + 1; ++i)
        {
            arg[i] = (char)argW[i];
        }
        argv[j] = arg;
    }
    delete[] cmdLineW;
#else
int main(int argc, char** argv)
{
#endif
    _argc = argc;
    _argv = argv;

    // Init SDL
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    auto sdlWindow = SDL_CreateWindow(
        "Wolf3D Shaders",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1024, 768,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    // Init OpenGL
    auto glContext = SDL_GL_CreateContext(sdlWindow);
    SDL_GL_SetSwapInterval(1);
    gl3wInit();

    // Init audio
    SDL_AudioSpec audioSpec;
    memset(&audioSpec, 0, sizeof(SDL_AudioSpec));
    audioSpec.freq = 44100;
    audioSpec.format = AUDIO_S16LSB;
    audioSpec.callback = audioCallback;
    audioSpec.channels = 2;
    audioSpec.samples = 4096 / 4;
    audioSpec.userdata = nullptr;
    if (SDL_OpenAudio(&audioSpec, NULL) < 0)
    {
        assert(false);
    }
    SDL_PauseAudio(0);

    // Init wolf
    wolf3d_init();

    // Main loop
    bool done = false;
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!done)
    {
        // Calculate delta time
        auto now = std::chrono::high_resolution_clock::now();
        auto diffTime = now - lastTime;
        lastTime = now;
        auto dt = (float)((double)std::chrono::duration_cast<std::chrono::microseconds>(diffTime).count() / 1000000.0);

        // Poll events
        SDL_LockAudio();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                done = true;
                break;
            case SDL_KEYDOWN:
                // Input::onKeyDown(registry, event.key.keysym.sym);
                break;
            case SDL_KEYUP:
                // Input::onKeyUp(registry, event.key.keysym.sym);
                break;
            case SDL_MOUSEBUTTONDOWN:
                // Input::onMouseButtonDown(registry, event.button.button);
                break;
            case SDL_MOUSEBUTTONUP:
                // Input::onMouseButtonUp(registry, event.button.button);
                break;
            case SDL_MOUSEMOTION:
                // Input::onMouseMotion(registry, event.motion.x, event.motion.y);
                break;
            }
        }

        // Update game simulation

        // Update UI stuff, independent from simulation

        // Draw game

        SDL_UnlockAudio();

        // Swap buffers
        SDL_GL_SwapWindow(sdlWindow);
    }

    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
}

void audioCallback(void *userdata, Uint8 *stream, int len)
{
    memset(stream, 0, len);
}
