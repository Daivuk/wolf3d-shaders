/*
    Program entry and main loop.
    This is where we initialize SDL and poll events.
*/

#if defined(WIN32)
#include <windows.h>
#endif

#include "WL_DEF.H"
#include "ws.h"
#include "ws_shaders.h"

#include "ws_Vector3.h"

#include <algorithm>
#include <assert.h>
#include <chrono>
#include <stdio.h>
#include <vector>
#include <map>
#include <string>

#include <imgui.h>

// That's for 2D
static const int MAX_VERTICES = 100000;

void Quit(char *error);

void wolf3d_init();
void wolf3d_update();
void wolf3d_shutdown();

int16_t ws_argc; // global arguments. this is referenced a bit everywhere
char **ws_argv;

int ws_screen_w = 1024, ws_screen_h = 768;
int ws_3d_w = 0, ws_3d_h = 0;
SDL_Window *sdlWindow;
int lastMouse[2] = { 0 };
int curMouse[2] = { 0 };
bool mouseInitialized = false;
int16_t mouseButtons = 0;
float ws_dt = 0.0f;
float ws_rdt = 0.0f;
ws_Matrix ws_ui_matrix;
ws_Matrix ws_matrix2D;
ws_Matrix ws_matrix3D;
ws_Vector3 ws_cam_eye;
ws_Vector3 ws_cam_right;
ws_Vector3 ws_cam_front;
ws_Vector3 ws_cam_front_flat;
ws_Vector3 ws_cam_target;

bool ws_debug_view_enabled = false;
bool camControl = false;
ws_Vector3 freecamPos(32.0f, 32.0f, 4.0f);
float freecamAngleX = -30.0f;
float freecamAngleZ = 45.0f;

bool wasMouseRel = false;

#define KeyInt 9 // The keyboard ISR number

static int scancode = 0;
static Interrupt KeyInt_in = nullptr;

static float controller_axis[16] = { 0.f };

std::map<int, int> SDL2DosKeymap = {
    std::make_pair<int, int>(SDL_SCANCODE_UNKNOWN, sc_None),
    std::make_pair<int, int>(0xff, sc_Bad),
    std::make_pair<int, int>(SDL_SCANCODE_RETURN, sc_Return),
    std::make_pair<int, int>(SDL_SCANCODE_ESCAPE, sc_Escape),
    std::make_pair<int, int>(SDL_SCANCODE_SPACE, sc_Space),
    std::make_pair<int, int>(SDL_SCANCODE_BACKSPACE, sc_BackSpace),
    std::make_pair<int, int>(SDL_SCANCODE_TAB, sc_Tab),
    std::make_pair<int, int>(SDL_SCANCODE_LALT, sc_Alt),
    std::make_pair<int, int>(SDL_SCANCODE_LCTRL, sc_Control),
    std::make_pair<int, int>(SDL_SCANCODE_CAPSLOCK, sc_CapsLock),
    std::make_pair<int, int>(SDL_SCANCODE_LSHIFT, sc_LShift),
    std::make_pair<int, int>(SDL_SCANCODE_RSHIFT, sc_RShift),
    std::make_pair<int, int>(SDL_SCANCODE_UP, sc_UpArrow),
    std::make_pair<int, int>(SDL_SCANCODE_DOWN, sc_DownArrow),
    std::make_pair<int, int>(SDL_SCANCODE_LEFT, sc_LeftArrow),
    std::make_pair<int, int>(SDL_SCANCODE_RIGHT, sc_RightArrow),
    std::make_pair<int, int>(SDL_SCANCODE_INSERT, sc_Insert),
    std::make_pair<int, int>(SDL_SCANCODE_DELETE, sc_Delete),
    std::make_pair<int, int>(SDL_SCANCODE_HOME, sc_Home),
    std::make_pair<int, int>(SDL_SCANCODE_END, sc_End),
    std::make_pair<int, int>(SDL_SCANCODE_PAGEUP, sc_PgUp),
    std::make_pair<int, int>(SDL_SCANCODE_PAGEDOWN, sc_PgDn),
    std::make_pair<int, int>(SDL_SCANCODE_F1, sc_F1),
    std::make_pair<int, int>(SDL_SCANCODE_F2, sc_F2),
    std::make_pair<int, int>(SDL_SCANCODE_F3, sc_F3),
    std::make_pair<int, int>(SDL_SCANCODE_F4, sc_F4),
    std::make_pair<int, int>(SDL_SCANCODE_F5, sc_F5),
    std::make_pair<int, int>(SDL_SCANCODE_F6, sc_F6),
    std::make_pair<int, int>(SDL_SCANCODE_F7, sc_F7),
    std::make_pair<int, int>(SDL_SCANCODE_F8, sc_F8),
    std::make_pair<int, int>(SDL_SCANCODE_F9, sc_F9),
    std::make_pair<int, int>(SDL_SCANCODE_F10, sc_F10),
    std::make_pair<int, int>(SDL_SCANCODE_F11, sc_F11),
    std::make_pair<int, int>(SDL_SCANCODE_F12, sc_F12),

    std::make_pair<int, int>(SDL_SCANCODE_1, sc_1),
    std::make_pair<int, int>(SDL_SCANCODE_2, sc_2),
    std::make_pair<int, int>(SDL_SCANCODE_3, sc_3),
    std::make_pair<int, int>(SDL_SCANCODE_4, sc_4),
    std::make_pair<int, int>(SDL_SCANCODE_5, sc_5),
    std::make_pair<int, int>(SDL_SCANCODE_6, sc_6),
    std::make_pair<int, int>(SDL_SCANCODE_7, sc_7),
    std::make_pair<int, int>(SDL_SCANCODE_8, sc_8),
    std::make_pair<int, int>(SDL_SCANCODE_9, sc_9),
    std::make_pair<int, int>(SDL_SCANCODE_0, sc_0),

    std::make_pair<int, int>(SDL_SCANCODE_A, sc_A),
    std::make_pair<int, int>(SDL_SCANCODE_B, sc_B),
    std::make_pair<int, int>(SDL_SCANCODE_C, sc_C),
    std::make_pair<int, int>(SDL_SCANCODE_D, sc_D),
    std::make_pair<int, int>(SDL_SCANCODE_E, sc_E),
    std::make_pair<int, int>(SDL_SCANCODE_F, sc_F),
    std::make_pair<int, int>(SDL_SCANCODE_G, sc_G),
    std::make_pair<int, int>(SDL_SCANCODE_H, sc_H),
    std::make_pair<int, int>(SDL_SCANCODE_I, sc_I),
    std::make_pair<int, int>(SDL_SCANCODE_J, sc_J),
    std::make_pair<int, int>(SDL_SCANCODE_K, sc_K),
    std::make_pair<int, int>(SDL_SCANCODE_L, sc_L),
    std::make_pair<int, int>(SDL_SCANCODE_M, sc_M),
    std::make_pair<int, int>(SDL_SCANCODE_N, sc_N),
    std::make_pair<int, int>(SDL_SCANCODE_O, sc_O),
    std::make_pair<int, int>(SDL_SCANCODE_P, sc_P),
    std::make_pair<int, int>(SDL_SCANCODE_Q, sc_Q),
    std::make_pair<int, int>(SDL_SCANCODE_R, sc_R),
    std::make_pair<int, int>(SDL_SCANCODE_S, sc_S),
    std::make_pair<int, int>(SDL_SCANCODE_T, sc_T),
    std::make_pair<int, int>(SDL_SCANCODE_U, sc_U),
    std::make_pair<int, int>(SDL_SCANCODE_V, sc_V),
    std::make_pair<int, int>(SDL_SCANCODE_W, sc_W),
    std::make_pair<int, int>(SDL_SCANCODE_X, sc_X),
    std::make_pair<int, int>(SDL_SCANCODE_Y, sc_Y),
    std::make_pair<int, int>(SDL_SCANCODE_Z, sc_Z),
};

float fade_val = 1.0f;
float debugOffset = 0.0f;

ws_Resources ws_resources;

GLuint createVertexBuffer()
{
    GLuint handle;
    glGenBuffers(1, &handle);
    return handle;
}

GLuint createVertexBuffer(GLsizeiptr size, const void *data)
{
    GLuint handle;
    glGenBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    return handle;
}

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
    auto argv = new char *[argc];
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
int main(int argc, char **argv)
{
#endif
    ws_argc = argc;
    ws_argv = argv;

    ws_palette[255].a = 0.0f; // Last color is transparent
    extern byte gamepal[256][3];
    VL_GetPalette((byte*)gamepal);

    // Init SDL
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    sdlWindow = SDL_CreateWindow(
        "Wolf3D Shaders",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        ws_screen_w, ws_screen_h,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE /*| SDL_WINDOW_MAXIMIZED*/);
    SDL_Init(SDL_INIT_GAMECONTROLLER);

    // Init OpenGL
    auto glContext = SDL_GL_CreateContext(sdlWindow);
    SDL_GL_SetSwapInterval(1);
    gl3wInit();

    // Init audio
    SDL_AudioSpec audioSpec;
    memset(&audioSpec, 0, sizeof(SDL_AudioSpec));
    audioSpec.freq = 44100;//6896;
    audioSpec.format = AUDIO_F32;
    audioSpec.callback = ws_audio_callback;
    audioSpec.channels = 2;
    audioSpec.samples = 512;
    audioSpec.userdata = nullptr;
    ws_audio_on = SDL_OpenAudio(&audioSpec, NULL) >= 0;
    SDL_PauseAudio(0);

    // Init main ws_resources
    ws_3d_w = ws_screen_w;
    ws_3d_h = ws_screen_h - ws_get_statusline_height();

    ws_resources.programPC = ws_create_program(PC_VERT, PC_FRAG, {"Position", "Color"});
    ws_resources.programPTC = ws_create_program(PTC_VERT, PTC_FRAG, {"Position", "TexCoord", "Color"});
    ws_resources.programPNTC = ws_create_program(PNTC_VERT, PNTC_FRAG, {"Position", "Normal", "TexCoord", "Color"});
    ws_resources.programGBufferPNTC = ws_create_program(PNTC_GBUFFER_VERT, PNTC_GBUFFER_FRAG, {"Position", "Normal", "TexCoord", "Color"});
    ws_resources.programPointlightP = ws_create_program(PTC_POINTLIGHT_VERT, PTC_POINTLIGHT_FRAG, {"Position", "TexCoord", "Color"});
    ws_resources.programHDRPTC = ws_create_program(PTC_HDR_VERT, PTC_HDR_FRAG, {"Position", "TexCoord", "Color"});
    ws_resources.vertexBuffer = createVertexBuffer();
    ws_resources.pPCVertices = new ws_VertexPC[MAX_VERTICES];
    ws_resources.pPTCVertices = new ws_VertexPTC[MAX_VERTICES];
    ws_resources.pPNTCVertices = new ws_VertexPNTC[MAX_VERTICES];
    ws_resources.uiRT = ws_create_rt(MaxX, MaxY);
    ws_resources.mainRT = ws_create_main_rt(ws_3d_w, ws_3d_h);
    ws_resources.hdrRT = ws_create_hdr_rt(ws_3d_w, ws_3d_h);
    ws_resources.lastFrameRT = ws_create_rt(4, 4);
    ws_resources.sphereVB = ws_create_sphere();
    ws_gbuffer = ws_create_gbuffer(ws_3d_w, ws_3d_h);

    srand(0);
    uint32_t checkerBytes[] = {0xFF880088, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF880088};
    ws_resources.checkerTexture = ws_create_texture((uint8_t *)checkerBytes, 2, 2);
    ws_resources.whiteTexture = ws_create_texture((uint8_t*)&checkerBytes[1], 1, 1);

    // imgui
    ImGui::CreateContext();
    auto& io = ImGui::GetIO();
    uint8_t *pPixelData;
    int w, h;
    io.Fonts->GetTexDataAsRGBA32(&pPixelData, &w, &h);
    ws_resources.imguiFontTexture = ws_create_texture(pPixelData, w, h);
    io.Fonts->SetTexID(&ws_resources.imguiFontTexture);
    io.KeyMap[ImGuiKey_Tab] = (int)SDL_SCANCODE_TAB; // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
    io.KeyMap[ImGuiKey_LeftArrow] = (int)SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = (int)SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = (int)SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = (int)SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = (int)SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = (int)SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = (int)SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = (int)SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Delete] = (int)SDL_SCANCODE_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = (int)SDL_SCANCODE_BACKSPACE;
    io.KeyMap[ImGuiKey_Enter] = (int)SDL_SCANCODE_RETURN;
    io.KeyMap[ImGuiKey_Escape] = (int)SDL_SCANCODE_ESCAPE;
    io.KeyMap[ImGuiKey_A] = (int)SDL_SCANCODE_A;
    io.KeyMap[ImGuiKey_C] = (int)SDL_SCANCODE_C;
    io.KeyMap[ImGuiKey_V] = (int)SDL_SCANCODE_V;
    io.KeyMap[ImGuiKey_X] = (int)SDL_SCANCODE_X;
    io.KeyMap[ImGuiKey_Y] = (int)SDL_SCANCODE_Y;
    io.KeyMap[ImGuiKey_Z] = (int)SDL_SCANCODE_Z;

    ws_load_configs();
    ws_load_settings();

    ws_update_sdl();
    VW_UpdateScreen();

    // Init wolf
    wolf3d_init(); // This also pretty much runs the main loop

    // Cleanup
    //SDL_GL_DeleteContext(glContext); // We will never get there
    //SDL_DestroyWindow(sdlWindow);
    //SDL_Quit();
}

int getDosScanCode(int sdlScanCode)
{
    auto it = SDL2DosKeymap.find(sdlScanCode);
    if (it == SDL2DosKeymap.end())
        return 0;
    return it->second;
}

void ws_update_sdl()
{
    // Poll events
    // SDL_LockAudio();
    SDL_Event event;
    static bool freecamInputs[7] = { false };

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN:
            if (ws_debug_view_enabled)
            {
                auto& io = ImGui::GetIO();
                //io.KeyCtrl = (event.key.keysym.mod & KMOD_LCTRL) && !camControl ? true : false;
                //io.KeyShift = (event.key.keysym.mod & KMOD_LSHIFT) && !camControl ? true : false;
                //io.KeyAlt = (event.key.keysym.mod & KMOD_LALT) && !camControl ? true : false;
                //io.KeySuper = (event.key.keysym.mod & KMOD_LGUI) && !camControl ? true : false;
                io.KeysDown[event.key.keysym.scancode] = !camControl ? true : false;

                if (event.key.keysym.scancode == SDL_SCANCODE_F1)
                {
                    memset(freecamInputs, 0, sizeof(freecamInputs));
                    camControl = false;
                    SDL_SetRelativeMouseMode(wasMouseRel ? SDL_TRUE : SDL_FALSE);
                    ws_debug_view_enabled = false;
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_GRAVE)
                {
                    camControl = !camControl;
                    SDL_SetRelativeMouseMode(camControl ? SDL_TRUE : SDL_FALSE);
                }
                if (camControl)
                {
                    if (event.key.keysym.scancode == SDL_SCANCODE_W)
                        freecamInputs[0] = true;
                    if (event.key.keysym.scancode == SDL_SCANCODE_A)
                        freecamInputs[1] = true;
                    if (event.key.keysym.scancode == SDL_SCANCODE_S)
                        freecamInputs[2] = true;
                    if (event.key.keysym.scancode == SDL_SCANCODE_D)
                        freecamInputs[3] = true;
                    if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
                        freecamInputs[4] = true;
                    if (event.key.keysym.scancode == SDL_SCANCODE_LCTRL)
                        freecamInputs[5] = true;
                    if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
                        freecamInputs[6] = true;
                }
                else
                {
                    if (event.key.keysym.scancode == SDL_SCANCODE_S && event.key.keysym.mod & KMOD_LCTRL)
                    {
                        ws_save_settings();
                    }
                }
            }
            else
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_F1)
                {
                    wasMouseRel = SDL_GetRelativeMouseMode() == SDL_TRUE;
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                    ws_debug_view_enabled = true;
                    //VW_UpdateScreen(); // Why?
                    freecamPos = ws_cam_eye;
                }
                else
                {
                    scancode = getDosScanCode(event.key.keysym.scancode);
                    if (KeyInt_in)
                        KeyInt_in();
                }
            }
            break;
        case SDL_KEYUP:
            if (ws_debug_view_enabled)
            {
                auto& io = ImGui::GetIO();
                //io.KeyCtrl = (event.key.keysym.mod & KMOD_LCTRL) && !camControl ? true : false;
                //io.KeyShift = (event.key.keysym.mod & KMOD_LSHIFT) && !camControl ? true : false;
                //io.KeyAlt = (event.key.keysym.mod & KMOD_LALT) && !camControl ? true : false;
                //io.KeySuper = (event.key.keysym.mod & KMOD_LGUI) && !camControl ? true : false;
                io.KeysDown[event.key.keysym.scancode] = false;

                if (camControl)
                {
                    if (event.key.keysym.scancode == SDL_SCANCODE_W)
                        freecamInputs[0] = false;
                    if (event.key.keysym.scancode == SDL_SCANCODE_A)
                        freecamInputs[1] = false;
                    if (event.key.keysym.scancode == SDL_SCANCODE_S)
                        freecamInputs[2] = false;
                    if (event.key.keysym.scancode == SDL_SCANCODE_D)
                        freecamInputs[3] = false;
                    if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)
                        freecamInputs[4] = false;
                    if (event.key.keysym.scancode == SDL_SCANCODE_LCTRL)
                        freecamInputs[5] = false;
                    if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
                        freecamInputs[6] = false;
                }
            }
            else
            {
                scancode = getDosScanCode(event.key.keysym.scancode) | 0x80;
                if (KeyInt_in)
                    KeyInt_in();
            }
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (ws_debug_view_enabled)
            {
                auto& io = ImGui::GetIO();
                //io.KeyCtrl = (event.key.keysym.mod & KMOD_LCTRL) && !camControl ? true : false;
                //io.KeyShift = (event.key.keysym.mod & KMOD_LSHIFT) && !camControl ? true : false;
                //io.KeyAlt = (event.key.keysym.mod & KMOD_LALT) && !camControl ? true : false;
                //io.KeySuper = (event.key.keysym.mod & KMOD_LGUI) && !camControl ? true : false;
                if (event.button.button == SDL_BUTTON_LEFT && !camControl)
                {
                    io.MouseDown[0] = true;
                    // Select items in the world
                    if (!io.WantCaptureMouse)
                    {
                        // Mouse pick sprites
                        auto statusLineH = (int)((float)STATUSLINES * ((float)ws_screen_h / 200.0f));

                        auto invTransform = ws_matrix3D.Invert();
                        ws_Vector3 from(io.MousePos.x / (float)ws_screen_w * 2.0f - 1.0f, 
                                        -(io.MousePos.y / (float)(ws_screen_h - statusLineH) * 2.0f - 1.0f), 0.0f);
                        ws_Vector3 to(from.x, from.y, 1.0f);
                        from = ws_Vector3::Transform(from, invTransform);
                        to = ws_Vector3::Transform(to, invTransform);
                        auto dir = to - from;
                        dir.Normalize();

                        int16_t closest = -1;
                        float closestDist = 10000;

                        for (auto statptr = &statobjlist[0]; statptr != laststatobj; statptr++)
                        {
                            if (statptr->shapenum == -1)
                                continue;						// object has been deleted

                            ws_Vector3 center((float)statptr->tilex + 0.5f, 64.0f - ((float)statptr->tiley + 0.5f), 0.5f);
                            auto dist = ws_Vector3::DistanceSquared(center, from);
                            if (dist > closestDist) continue;

                            // Ray distance to sphere
                            auto rayDist = dir.Cross(center - from).LengthSquared();
                            if (rayDist < 0.5f * 0.5f)
                            {
                                closestDist = dist;
                                closest = statptr->shapenum;
                            }
                        }

                        for (obj = player->next;obj;obj = obj->next)
                        {
                            if (!obj->state->shapenum) continue;
                            
                            ws_Vector3 center((float)obj->x / 65536.0f, 64.0f - (float)obj->y / 65536.0f, 0.5f);
                            auto dist = ws_Vector3::DistanceSquared(center, from);
                            if (dist > closestDist) continue;

                            // Ray distance to sphere
                            auto rayDist = dir.Cross(center - from).LengthSquared();
                            if (rayDist < 0.5f * 0.5f)
                            {
                                closestDist = dist;
                                closest = obj->state->shapenum;
                            }
                        }

                        if (closest != -1)
                        {
                            ws_selected_sprite = closest;
                        }
                    }
                }
                else if (event.button.button == SDL_BUTTON_RIGHT && !camControl)
                    io.MouseDown[1] = true;
                else if (event.button.button == SDL_BUTTON_MIDDLE && !camControl)
                {
                    if (!camControl) io.MouseDown[2] = true;
                    camControl = true;
                    SDL_SetRelativeMouseMode(SDL_TRUE);
                }
            }
            else
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouseButtons |= 1;
                    scancode = sc_Mouse0;
                    if (KeyInt_in)
                        KeyInt_in();
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    mouseButtons |= 2;
                    scancode = sc_Mouse1;
                    if (KeyInt_in)
                        KeyInt_in();
                }
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    mouseButtons |= 4;
                    scancode = sc_Mouse2;
                    if (KeyInt_in)
                        KeyInt_in();
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (ws_debug_view_enabled)
            {
                auto& io = ImGui::GetIO();
                //io.KeyCtrl = (event.key.keysym.mod & KMOD_LCTRL) && !camControl ? true : false;
                //io.KeyShift = (event.key.keysym.mod & KMOD_LSHIFT) && !camControl ? true : false;
                //io.KeyAlt = (event.key.keysym.mod & KMOD_LALT) && !camControl ? true : false;
                //io.KeySuper = (event.key.keysym.mod & KMOD_LGUI) && !camControl ? true : false;
                if (event.button.button == SDL_BUTTON_LEFT && !camControl)
                    io.MouseDown[0] = false;
                else if (event.button.button == SDL_BUTTON_RIGHT && !camControl)
                    io.MouseDown[1] = false;
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    memset(freecamInputs, 0, sizeof(freecamInputs));
                    io.MouseDown[2] = false;
                    camControl = false;
                    SDL_SetRelativeMouseMode(SDL_FALSE);
                }
            }
            else
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    mouseButtons &= 0xFFFE;
                    scancode = sc_Mouse0 | 0x80;
                    if (KeyInt_in)
                        KeyInt_in();
                }
                else if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    mouseButtons &= 0xFFFD;
                    scancode = sc_Mouse1 | 0x80;
                    if (KeyInt_in)
                        KeyInt_in();
                }
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                {
                    mouseButtons &= 0xFFFB;
                    scancode = sc_Mouse2 | 0x80;
                    if (KeyInt_in)
                        KeyInt_in();
                }
            }
            break;
        case SDL_MOUSEMOTION:
            if (ws_debug_view_enabled)
            {
                auto& io = ImGui::GetIO();
                if (camControl)
                {
                    freecamAngleZ += (float)event.motion.xrel * 0.3f;
                    freecamAngleX -= (float)event.motion.yrel * 0.3f;

                    while (freecamAngleZ < 0.0f) freecamAngleZ += 360.0f;
                    while (freecamAngleZ > 360.0f) freecamAngleZ -= 360.0f;
                    if (freecamAngleX < -89.0f) freecamAngleX = -89.0f;
                    if (freecamAngleX > 89.0f) freecamAngleX = 89.0f;
                }
                else
                {
                    io.MousePos.x = (float)event.motion.x;
                    io.MousePos.y = (float)event.motion.y;
                }
            }
            else
            {
                curMouse[0] += (int)event.motion.xrel;
                curMouse[1] += (int)event.motion.yrel;
                if (!mouseInitialized)
                {
                    mouseInitialized = true;
                    lastMouse[0] = curMouse[0];
                    lastMouse[1] = curMouse[1];
                }
            }
            break;
        case SDL_MOUSEWHEEL:
            if (ws_debug_view_enabled)
            {
                if (!camControl)
                {
                    auto& io = ImGui::GetIO();
                    io.MouseWheel = (float)event.wheel.y;
                    io.MouseWheelH = (float)event.wheel.x;
                }
            }
            break;
        case SDL_TEXTINPUT:
            if (ws_debug_view_enabled && !camControl)
            {
                auto& io = ImGui::GetIO();
                auto len = strlen(event.text.text);
                for (decltype(len) i = 0; i < len; ++i)
                {
                    auto c = event.text.text[i];
                    io.AddInputCharacter(c);
                }
                break;
            }
        case SDL_WINDOWEVENT:
            switch (event.window.event)
            {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                {
                    ws_screen_w = event.window.data1;
                    ws_screen_h = event.window.data2;
                    ws_3d_w = ws_screen_w;
                    ws_3d_h = ws_screen_h - ws_get_statusline_height();
                    auto lineh = ws_get_statusline_height();
                    ws_resize_main_rt(ws_resources.mainRT, ws_3d_w, ws_3d_h);
                    ws_resize_hdr_rt(ws_resources.hdrRT, ws_3d_w, ws_3d_h);
                    ws_resize_gbuffer(ws_gbuffer, ws_3d_w, ws_3d_h);
                    break;
                }
                default: break;
            }
            break;
        case SDL_CONTROLLERBUTTONDOWN:
        {
            if (event.cbutton.button > 15) break; // Sorry only 16 buttons supported for now
            scancode = sc_JoyBtnBase + (int)event.cbutton.button;
            if (KeyInt_in)
                KeyInt_in();
            break;
        }
        case SDL_CONTROLLERBUTTONUP:
        {
            if (event.cbutton.button > 15) break; // Sorry only 16 buttons supported for now
            scancode = (sc_JoyBtnBase + (int)event.cbutton.button) | 0x80;
            if (KeyInt_in)
                KeyInt_in();
            break;
        }
        case SDL_CONTROLLERAXISMOTION:
        {
            if (event.caxis.axis * 2 > 15) break; // Sorry only 16 buttons supported for now
            auto normalized = (float)((double)event.caxis.value / 32768.0);
            auto deadzone = (float)(7849.0 / 32768.0);
            auto prev = controller_axis[event.caxis.axis];
            controller_axis[event.caxis.axis] = normalized;
            if (normalized >= deadzone)
            {
                if (prev < deadzone)
                {
                    scancode = sc_JoyAxisBtnBase + (int)event.caxis.axis * 2 + ();
                    if (KeyInt_in)
                        KeyInt_in();
                }
            }
            else
            {

            }
            break;
        }
        case SDL_CONTROLLERDEVICEADDED:
            {
                SDL_GameControllerOpen(event.cdevice.which);
                break;
            }
        }
    }
    // SDL_UnlockAudio();

    // Update ticks
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto curTime = std::chrono::high_resolution_clock::now();
    auto elapsed = curTime - lastTime;
    ws_dt = (float)((double)std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / 1000000.0);
    static const long long TARGET_FPS = 1000000 / 70;
    static long long curStep = 0;
    curStep += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    lastTime = curTime;
    while (curStep >= TARGET_FPS)
    {
        curStep -= TARGET_FPS;
        ++TimeCount;
    }

    if (ws_debug_view_enabled && camControl)
    {
        // Update free camera movement
        ws_cam_front.x = sinf(freecamAngleZ * (float)M_PI / 180.0f) * cosf(freecamAngleX * (float)M_PI / 180.0f);
        ws_cam_front.y = cosf(freecamAngleZ * (float)M_PI / 180.0f) * cosf(freecamAngleX * (float)M_PI / 180.0f);
        ws_cam_front.z = sinf(freecamAngleX * (float)M_PI / 180.0f);
        ws_cam_front.Normalize();
        ws_cam_right = { ws_cam_front.y, -ws_cam_front.x, 0.0f };
        ws_cam_right.Normalize();
        ws_cam_eye = freecamPos;
        ws_cam_target = ws_cam_eye + ws_cam_front;
        ws_cam_front_flat = ws_cam_front;
        ws_cam_front_flat.z = 0;
        ws_cam_front_flat.Normalize();

        float moveSpeed = (freecamInputs[6] ? 15.0f : 5.0f) * ws_dt;

        if (freecamInputs[0])
            freecamPos += ws_cam_front * moveSpeed;
        if (freecamInputs[1])
            freecamPos -= ws_cam_right * moveSpeed;
        if (freecamInputs[2])
            freecamPos -= ws_cam_front * moveSpeed;
        if (freecamInputs[3])
            freecamPos += ws_cam_right * moveSpeed;
        if (freecamInputs[4])
            freecamPos += ws_Vector3(0, 0, 1.0f) * moveSpeed;
        if (freecamInputs[5])
            freecamPos -= ws_Vector3(0, 0, 1.0f) * moveSpeed;
    }

    // Screen flashes
    ws_save_flash_anim = std::max(0.0f, ws_save_flash_anim - ws_dt * 2.0f);
    ws_bonus_flash = std::max(0.0f, ws_bonus_flash - ws_dt * 2.0f);
    ws_damage_flash = std::max(0.0f, ws_damage_flash - ws_dt * 2.0f);
}

void VGAClearScreen()
{
    // This punches a whole inside the UI render target so we can render 3D
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, STATUSLINES, MaxX, MaxY - STATUSLINES);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
}

void VW_UpdateScreen()
{
    ws_flush();

    // Calculate our overlay flash color (When picking up powerups or taking damage)
    auto bonusFlash = fabsf(sinf(ws_bonus_flash * 5.0f)) / (3.0f - ws_bonus_flash * 3.0f + 1.0f);
    ws_Color flashColor = {
        (1 + ws_save_flash_anim + bonusFlash * 0.75f) * (1.0f - ws_damage_flash) + ws_damage_flash,
        (1 + ws_save_flash_anim + bonusFlash * 1.0f) * (1.0f - ws_damage_flash),
        (1 + ws_save_flash_anim + bonusFlash * 1.25f) * (1.0f - ws_damage_flash),
        1
    };

    // Prepare the main OpenGL framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glViewport(0, 0, ws_screen_w, ws_screen_h);

    // Render the final 3D image
    {
        auto transform = ws_Matrix::CreateOrthographicOffCenter(0, (float)ws_screen_w, (float)ws_screen_h, 0, -999, 999);
        glUseProgram(ws_resources.programPTC);
        static auto uniform = glGetUniformLocation(ws_resources.programPTC, "ProjMtx");
        glUniformMatrix4fv(uniform, 1, GL_FALSE, &transform._11);

        ws_prepare_for_ptc(GL_QUADS);
        glBindTexture(GL_TEXTURE_2D, ws_resources.mainRT.handle);
        ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count, 0, 0, (float)ws_3d_w, (float)ws_3d_h, 0, 1, 1, 0, flashColor);
        ws_flush();
    }

    // Draw UI
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBindTexture(GL_TEXTURE_2D, ws_resources.uiRT.handle);
        auto ratio = (float)ws_screen_h / (float)240; // We don't use MaxY here because it's 40 Pixels more
        auto w = (float)MaxX * ratio;
        auto left = ((float)ws_screen_w - w) / 2.0f;
        auto right = left + w;
        ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count, 0, 0, left, (float)ws_screen_h, 0, 1, 0, 0, flashColor);
        ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count, left, 0, w, (float)ws_screen_h, 0, 1, 1, 0, flashColor);
        ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count, right, 0, (float)ws_screen_w - right, (float)ws_screen_h, 1, 1, 1, 0, flashColor);
        //ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count, 0, 0, (float)MaxX, (float)MaxY, 0, 1, 1, 0, flashColor);
        ws_flush();
    }

    // Draw imgui tools on top of everything
    {
        ws_do_tools();
    }

    // Draw the fade on top of everything
    ws_prepare_for_pc(GL_QUADS);
    ws_pc_count += ws_draw_rect(ws_resources.pPCVertices + ws_pc_count, 0, 0, (float)ws_screen_w, (float)ws_screen_h, { 0, 0, 0, 1 - fade_val });
    ws_flush();

    // Swap buffers
    SDL_GL_SwapWindow(sdlWindow);

    // Go back to our UI frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.uiRT.frameBuffer);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, MaxX, MaxY);

    ws_ui_matrix = ws_Matrix::CreateOrthographicOffCenter(0, (float)MaxX, (float)MaxY, 0, -999, 999);
    {
        glUseProgram(ws_resources.programPC);
        auto uniform = glGetUniformLocation(ws_resources.programPC, "ProjMtx");
        glUniformMatrix4fv(uniform, 1, GL_FALSE, &ws_ui_matrix._11);
    }
    {
        glUseProgram(ws_resources.programPTC);
        auto uniform = glGetUniformLocation(ws_resources.programPTC, "ProjMtx");
        glUniformMatrix4fv(uniform, 1, GL_FALSE, &ws_ui_matrix._11);
    }

    ws_pc_count = 0;
    ws_ptc_count = 0;
    ws_pntc_count = 0;
    ws_draw_mode = -1;
    ws_draw_mode_prim = 0;
    ws_current_3d_texture = 0;
}

int palidx = 0;

int16_t inportb(int16_t addr)
{
    switch (addr)
    {
    case 0x60:
    {
        return (int16_t)scancode;
    }
    case PEL_DATA:
    {
        auto &col = ws_palette[palidx / 3];
        int16_t ret = 0;
        switch (palidx % 3)
        {
        case 0:
            ret = (int16_t)(byte)(col.r * 255.0f);
            break;
        case 1:
            ret = (int16_t)(byte)(col.g * 255.0f);
            break;
        case 2:
            ret = (int16_t)(byte)(col.b * 255.0f);
            break;
        }
        palidx++;
        return ret;
    }
    }
    return 0;
}

void outportb(int16_t addr, char val)
{
    switch (addr)
    {
        case PEL_READ_ADR:
        {
            palidx = 0;
            break;
        }
    }
}

Interrupt getvect(int16_t r_num)
{
    switch (r_num)
    {
        case KeyInt:
            return KeyInt_in;
    }
    return nullptr;
}

void setvect(int16_t r_num, Interrupt interrupt)
{
    switch (r_num)
    {
        case KeyInt:
            KeyInt_in = interrupt;
            break;
    }
}

void ws_begin_draw_3d()
{
    ws_flush();

    if (ws_deferred_enabled)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ws_gbuffer.frameBuffer);
        GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, buffers);
        glUseProgram(ws_resources.programGBufferPNTC);

        auto uniform = glGetUniformLocation(ws_resources.programGBufferPNTC, "ProjMtx");
        glUniformMatrix4fv(uniform, 1, GL_FALSE, &ws_matrix3D._11);
        ws_active_lights.clear();
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.mainRT.frameBuffer);
        glUseProgram(ws_resources.programPNTC);
        auto uniform = glGetUniformLocation(ws_resources.programPNTC, "ProjMtx");
        glUniformMatrix4fv(uniform, 1, GL_FALSE, &ws_matrix3D._11);
    }

    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, ws_wireframe_enabled ? GL_LINE : GL_FILL);
    glViewport(0, 0, ws_3d_w, ws_3d_h);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ws_update_camera()
{
    auto proj = ws_Matrix::CreatePerspectiveFieldOfView(56.0f * (float)M_PI / 180.0f, (float)ws_3d_w / (float)ws_3d_h, 0.01f, 1000.0f);

    if (ws_debug_view_enabled)
    {
        ws_cam_front.x = sinf(freecamAngleZ * (float)M_PI / 180.0f) * cosf(freecamAngleX * (float)M_PI / 180.0f);
        ws_cam_front.y = cosf(freecamAngleZ * (float)M_PI / 180.0f) * cosf(freecamAngleX * (float)M_PI / 180.0f);
        ws_cam_front.z = sinf(freecamAngleX * (float)M_PI / 180.0f);
        ws_cam_front.Normalize();
        ws_cam_right = { ws_cam_front.y, -ws_cam_front.x, 0.0f };
        ws_cam_right.Normalize();
        ws_cam_eye = freecamPos;
        ws_cam_target = ws_cam_eye + ws_cam_front;
    }
    else
    {
        auto vsin = sinf((float)player->angle * (float)M_PI / 180.0f);
        auto vcos = cosf((float)player->angle * (float)M_PI / 180.0f);
        float px = (float)player->x / 65536.0f;
        float py = (float)player->y / 65536.0f;

        ws_cam_front = ws_Vector3(vcos, vsin, 0.0f);
        ws_cam_front.Normalize();
        ws_cam_right = { ws_cam_front.y, -ws_cam_front.x, 0.0f };
        ws_cam_right.Normalize();
        ws_cam_eye = ws_Vector3(px, 64.0f - py, WS_WALL_HEIGHT * 0.55f);
        ws_cam_target = ws_Vector3(px + vcos, 64.0f - (py - vsin), WS_WALL_HEIGHT * 0.55f);

        // Update freecam so if we switch we're at the player's position
        freecamPos = ws_cam_eye;
        freecamAngleX = 0.0f;
        freecamAngleZ = -(float)player->angle + 90.0f;
    }

    ws_cam_front_flat = ws_cam_front;
    ws_cam_front_flat.z = 0;
    ws_cam_front_flat.Normalize();

    auto view = ws_Matrix::CreateLookAt(
        ws_cam_eye,
        ws_cam_target,
        ws_Vector3(0.0f, 0.0f, 1.0f));
    ws_matrix3D = view * proj;
}

void Mouse(int16_t x)
{
    if (x == MDelta)
    {
        _CX = curMouse[0] - lastMouse[0];
        _DX = curMouse[1] - lastMouse[1];
        lastMouse[0] = curMouse[0];
        lastMouse[1] = curMouse[1];
    }
    else if (x == MButtons)
    {
        _BX = mouseButtons;
    }
}

int ws_get_statusline_height()
{
    return (int)((float)STATUSLINES * ((float)ws_screen_h / 200.0f));
}
