/*
    Program entry and main loop.
    This is where we initialize SDL and poll events.
*/

#if defined(WIN32)
#include <windows.h>
#endif

#include "WL_DEF.H"
#include "ws_main.h"
#include "ws_shaders.h"

#include <algorithm>
#include <assert.h>
#include <chrono>
#include <stdio.h>
#include <vector>
#include <map>

// That's for 2D
static const int MAX_VERTICES = 100000;

void	Quit (char *error);

void wolf3d_init();
void wolf3d_update();
void wolf3d_shutdown();
void audioCallback(void *userdata, Uint8 *stream, int len);
static void flush();
void drawDebugString(char* string, float x, float y);

int16_t _argc; // global arguments. this is referenced a bit everywhere
char** _argv;
int pcCount = 0;
int ptcCount = 0;
ws_Matrix matrix2D;
ws_Matrix matrix3D;
int screen_w = 1024, screen_h = 640;
SDL_Window* sdlWindow;

#define	KeyInt		9	// The keyboard ISR number

static byte scancode = 0;
static Interrupt KeyInt_in = nullptr;

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

struct RenderTarget
{
    GLuint handle;
    GLuint frameBuffer;
};

struct Position
{
    float x, y;
};

struct Color
{
    float r, g, b, a;
};

struct TexCoord
{
    float u, v;
};

struct VertexPC
{
    Position position;
    Color color;
};

struct VertexPTC
{
    Position position;
    TexCoord texCoord;
    Color color;
};

struct Resources
{
    GLuint      programPC;      /* Position, Color */
    GLuint      programPTC;     /* Position, TexCoord, Color */
    GLuint      vertexBuffer;   /* Dynamic version buffer used by batches */
    VertexPC   *pPCVertices;    /* Used by dynamic rendering of Position/Color */
    VertexPTC  *pPTCVertices;   /* Used by dynamic rendering of Position/TexCoord/Color */
    GLuint      checkerTexture;
    RenderTarget mainRT;
} resources;

struct Pic
{
    int w, h;
    GLuint tex;
};

struct BakedFont
{
    GLuint tex;
    float ws[256];
    float os[256];
};

std::map<int16_t, Pic> pics;
std::map<int16_t, GLuint> screenRaws;
std::map<int, BakedFont> fontTextures;
std::map<int16_t, Pic> sprites;

#define DRAW_MODE_PC 0
#define DRAW_MODE_PTC 1
int drawMode = -1;
GLenum drawModePrim = 0;
float fade_val = 1.0f;
bool showDebug = false;
float debugOffset = 0.0f;

#define wolf_RGB(r, g, b) {(float)((r)*255/63)/255.0f, (float)((g)*255/63)/255.0f, (float)((b)*255/63)/255.0f, 1.0f}
Color palette[] = {
#ifdef SPEAR
    #include "sodpal.inc"
#else
    #include "wolfpal.inc"
#endif
};

static void checkShader(GLuint handle)
{
    GLint bResult;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &bResult);
    if (bResult == GL_FALSE)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(handle, 1023, NULL, infoLog);
        Quit((char*)"shader compile failed: %s\n");
    }
}

GLuint createProgram(const GLchar *vs, const GLchar *ps, const std::vector<const char *> &attribs)
{
    const GLchar *vertex_shader_with_version[2]     = {"#version 120\n", vs};
    const GLchar *fragment_shader_with_version[2]   = {"#version 120\n", ps};

    auto vertHandle = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertHandle, 2, vertex_shader_with_version, NULL);
    glCompileShader(vertHandle);
    checkShader(vertHandle);

    auto fragHandle = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragHandle, 2, fragment_shader_with_version, NULL);
    glCompileShader(fragHandle);
    checkShader(fragHandle);

    GLuint program = glCreateProgram();
    glAttachShader(program, vertHandle);
    glAttachShader(program, fragHandle);
    int i = 0;
    for (auto attrib : attribs)
    {
        glBindAttribLocation(program, i, attrib);
        ++i;
    }
    glLinkProgram(program);

    return program;
}

// GLuint createTexture(const char *filename)
// {
//     int x, y, n;
//     unsigned char *data = stbi_load(filename, &x, &y, &n, 4);

//     GLuint handle;
//     glGenTextures(1, &handle);
//     glEnable(GL_TEXTURE_2D);
//     glActiveTexture(GL_TEXTURE0);
//     glBindTexture(GL_TEXTURE_2D, handle);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//     stbi_image_free(data);

//     return handle;
// }

GLuint ws_create_texture(uint8_t *data, int w, int h)
{
    GLuint handle;
    glGenTextures(1, &handle);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return handle;
}

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

// void setTransform(Registry &registry, const float *matrix)
// {
//     Resources &resources = registry.get<Resources>(registry.attachee<Tag::Resources>());
//     {
//         glUseProgram(resources.programPC);
//         auto uniform = glGetUniformLocation(resources.programPC, "ProjMtx");
//         glUniformMatrix4fv(uniform, 1, GL_FALSE, matrix);
//     }
//     {
//         glUseProgram(resources.programPTC);
//         auto uniform = glGetUniformLocation(resources.programPTC, "ProjMtx");
//         glUniformMatrix4fv(uniform, 1, GL_FALSE, matrix);
//     }
// }

static void drawPC(const VertexPC *pVertices, int count, GLenum mode)
{
    glBindBuffer(GL_ARRAY_BUFFER, resources.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPC) * count, pVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPC), (float*)(uintptr_t)(0));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPC), (float*)(uintptr_t)(8));
    glDrawArrays(mode, 0, count);
}

void drawLines(const VertexPC *pVertices, int count)
{
    drawPC(pVertices, count, GL_LINES);
}

void drawPoints(const VertexPC *pVertices, int count)
{
    drawPC(pVertices, count, GL_POINTS);
}

void drawQuads(const VertexPC *pVertices, int count)
{
    drawPC(pVertices, count, GL_QUADS);
}

void prepareForPC(int prim)
{
    if (drawMode == DRAW_MODE_PC && prim == drawModePrim) return;
    flush();
    drawModePrim = prim;
    if (drawMode != DRAW_MODE_PC)
    {
        drawMode = DRAW_MODE_PC;
        glDisable(GL_TEXTURE_2D);
        glUseProgram(resources.programPC);
        glEnableVertexAttribArray(0); // pos
        glEnableVertexAttribArray(1); // color
        glDisableVertexAttribArray(2);
    }
}

void prepareForPTC(int prim)
{
    if (drawMode == DRAW_MODE_PTC && prim == drawModePrim) return;
    flush();
    drawModePrim = prim;
    if (drawMode != DRAW_MODE_PTC)
    {
        drawMode = DRAW_MODE_PTC;
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glUseProgram(resources.programPTC);
        glEnableVertexAttribArray(0); // pos
        glEnableVertexAttribArray(1); // color
        glEnableVertexAttribArray(2);
    }
}

static void drawPTC(const VertexPTC *pVertices, int count, GLenum mode)
{
    glBindBuffer(GL_ARRAY_BUFFER, resources.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(VertexPTC) * count, pVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTC), (float*)(uintptr_t)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPTC), (float*)(uintptr_t)(8));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPTC), (float*)(uintptr_t)(16));
    glDrawArrays(mode, 0, count);
}

void drawLines(const VertexPTC *pVertices, int count)
{
    drawPTC(pVertices, count, GL_LINES);
}

void drawPoints(const VertexPTC *pVertices, int count)
{
    drawPTC(pVertices, count, GL_POINTS);
}

void drawQuads(const VertexPTC *pVertices, int count)
{
    drawPTC(pVertices, count, GL_QUADS);
}

int drawRect(VertexPTC *pVertices, float x, float y, float w, float h, float u1, float v1, float u2, float v2, const Color &color)
{
    pVertices[0].position.x = x;
    pVertices[0].position.y = y;
    pVertices[0].texCoord = {u1, v1};
    pVertices[0].color = color;

    pVertices[1].position.x = x;
    pVertices[1].position.y = y + h;
    pVertices[1].texCoord = {u1, v2};
    pVertices[1].color = color;

    pVertices[2].position.x = x + w;
    pVertices[2].position.y = y + h;
    pVertices[2].texCoord = {u2, v2};
    pVertices[2].color = color;

    pVertices[3].position.x = x + w;
    pVertices[3].position.y = y;
    pVertices[3].texCoord = {u2, v1};
    pVertices[3].color = color;
    pVertices += 4;

    return 4;
}

int drawRect(VertexPC *pVertices, float x, float y, float w, float h, const Color &color)
{
    pVertices[0].position.x = x;
    pVertices[0].position.y = y;
    pVertices[0].color = color;

    pVertices[1].position.x = x;
    pVertices[1].position.y = y + h;
    pVertices[1].color = color;

    pVertices[2].position.x = x + w;
    pVertices[2].position.y = y + h;
    pVertices[2].color = color;

    pVertices[3].position.x = x + w;
    pVertices[3].position.y = y;
    pVertices[3].color = color;
    pVertices += 4;

    return 4;
}

int drawLine(VertexPC *pVertices, const Position &from, const Position &to, const Color &color)
{
    pVertices[0].position.x = from.x;
    pVertices[0].position.y = from.y;
    pVertices[0].color = color;

    pVertices[1].position.x = to.x;
    pVertices[1].position.y = to.y;
    pVertices[1].color = color;

    return 2;
}

RenderTarget createRT(int w, int h)
{
    GLuint frameBuffer;
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

    GLuint handle;
    glGenTextures(1, &handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, handle, 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    //TODO
    //GLuint rboDepthStencil;
    //glGenRenderbuffers(1, &rboDepthStencil);
    //glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    //glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.x, size.y);
    //glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    RenderTarget ret;
    ret.handle = handle;
    ret.frameBuffer = frameBuffer;
    return ret;
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
    palette[255].a = 0.0f; // Last color is transparent

    // Init SDL
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    sdlWindow = SDL_CreateWindow(
        "Wolf3D Shaders",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screen_w, screen_h,
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

    // Init main resources
    resources.programPC = createProgram(PC_VERT, PC_FRAG, {"Position", "Color"});
    resources.programPTC = createProgram(PTC_VERT, PTC_FRAG, {"Position", "TexCoord", "Color"});
    resources.vertexBuffer = createVertexBuffer();
    resources.pPCVertices = new VertexPC[MAX_VERTICES];
    resources.pPTCVertices = new VertexPTC[MAX_VERTICES];
    resources.mainRT = createRT(MaxX, MaxY);

    srand(0);
    uint32_t checkerBytes[] = { 0xFF880088, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF880088 };
    resources.checkerTexture = ws_create_texture((uint8_t*)&checkerBytes, 2, 2);

    ws_update_sdl();
    VW_UpdateScreen();

    // Init wolf
    wolf3d_init(); // This also pretty much runs the main loop

    // Cleanup
    //SDL_GL_DeleteContext(glContext); // We will never get there
    //SDL_DestroyWindow(sdlWindow);
    //SDL_Quit();
}

void audioCallback(void *userdata, Uint8 *stream, int len)
{
    memset(stream, 0, len);
}

int getDosScanCode(int sdlScanCode)
{
    auto it = SDL2DosKeymap.find(sdlScanCode);
    if (it == SDL2DosKeymap.end()) return 0;
    return it->second;
}

void ws_update_sdl()
{
    // Poll events
    SDL_LockAudio();
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            exit(0);
            break;
        case SDL_KEYDOWN:
            if (showDebug)
            {
                if (event.key.keysym.scancode == SDL_SCANCODE_F1)
                    showDebug = false;
                else if (event.key.keysym.scancode == SDL_SCANCODE_DOWN)
                    debugOffset += 120;
                else if (event.key.keysym.scancode == SDL_SCANCODE_UP)
                    debugOffset -= 120;
                VW_UpdateScreen();
            }
            else if (event.key.keysym.scancode == SDL_SCANCODE_F1)
            {
                showDebug = true;
                VW_UpdateScreen();
            }
            else
            {
                scancode = getDosScanCode(event.key.keysym.scancode);
                if (KeyInt_in) KeyInt_in();
            }
            break;
        case SDL_KEYUP:
            if (!showDebug)
            {
                scancode = getDosScanCode(event.key.keysym.scancode) | 0x80;
                if (KeyInt_in) KeyInt_in();
            }
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

    SDL_UnlockAudio();

    // Update ticks
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto curTime = std::chrono::high_resolution_clock::now();
    auto elapsed = curTime - lastTime;
    static const long long TARGET_FPS = 1000000 / 70;
    static long long curStep = 0;
    curStep += std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    lastTime = curTime;

    while (curStep >= TARGET_FPS)
    {
        curStep -= TARGET_FPS;
        ++TimeCount;
    }
}

void VW_UpdateScreen()
{
    flush();

    // Draw game
    pcCount = 0;
    ptcCount = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glViewport(0, 0, screen_w, screen_h);

    matrix2D = ws_Matrix::CreateOrthographicOffCenter(0, (float)screen_w, (float)screen_h, 0, -999, 999);
    // matrix2D = matrix2D.Transpose();
    {
        glUseProgram(resources.programPC);
        auto uniform = glGetUniformLocation(resources.programPC, "ProjMtx");
        glUniformMatrix4fv(uniform, 1, GL_FALSE, &matrix2D._11);
    }
    {
        glUseProgram(resources.programPTC);
        auto uniform = glGetUniformLocation(resources.programPTC, "ProjMtx");
        glUniformMatrix4fv(uniform, 1, GL_FALSE, &matrix2D._11);
    }

    prepareForPTC(GL_QUADS);
    if (showDebug)
    {
        float y = -debugOffset;
        prepareForPC(GL_QUADS);
        for (int i = 0; i < 256; ++i)
        {
            pcCount += drawRect(resources.pPCVertices + pcCount, (float)i * 4, y, 4, 4, palette[i]);
        }
        prepareForPTC(GL_QUADS);
        y += 5;
        for (auto& kv : fontTextures)
        {
            glBindTexture(GL_TEXTURE_2D, kv.second.tex);
            ptcCount += drawRect(resources.pPTCVertices + ptcCount, 0, y, (float)screen_w, 10, 0, 0, 1, 1, { 1, 1, 1, 1 });
            flush();
            y += 12.0f;
        }
        float maxy = 0;
        float x = 0;
        for (auto& kv : screenRaws)
        {
            if (x + 320 + 2 > (float)screen_w)
            {
                x = 0.0f;
                y += maxy + 2;
                maxy = 0.0f;
            }
            glBindTexture(GL_TEXTURE_2D, kv.second);
            ptcCount += drawRect(resources.pPTCVertices + ptcCount, x, y, (float)320, (float)200, 0, 0, 1, 1, { 1, 1, 1, 1 });
            flush();
            drawDebugString((char*)std::to_string(kv.first).c_str(), x, y);
            x += (float)320 + 2;
            maxy = std::max(maxy, (float)200);
        }
        for (auto& kv : pics)
        {
            if (x + (float)kv.second.w + 2 > (float)screen_w)
            {
                x = 0.0f;
                y += maxy + 2;
                maxy = 0.0f;
            }
            glBindTexture(GL_TEXTURE_2D, kv.second.tex);
            ptcCount += drawRect(resources.pPTCVertices + ptcCount, x, y, (float)kv.second.w, (float)kv.second.h, 0, 0, 1, 1, { 1, 1, 1, 1 });
            flush();
            drawDebugString((char*)std::to_string(kv.first).c_str(), x, y);
            x += (float)kv.second.w + 2;
            maxy = std::max(maxy, (float)kv.second.h);
        }
        for (auto& kv : sprites)
        {
            if (x + (float)kv.second.w + 2 > (float)screen_w)
            {
                x = 0.0f;
                y += maxy + 2;
                maxy = 0.0f;
            }
            glBindTexture(GL_TEXTURE_2D, kv.second.tex);
            ptcCount += drawRect(resources.pPTCVertices + ptcCount, x, y, (float)kv.second.w, (float)kv.second.h, 0, 0, 1, 1, { 1, 1, 1, 1 });
            flush();
            drawDebugString((char*)std::to_string(kv.first).c_str(), x, y);
            x += (float)kv.second.w + 2;
            maxy = std::max(maxy, (float)kv.second.h);
        }
    }
    else
    {
        ptcCount += drawRect(resources.pPTCVertices + ptcCount, 0, 0, (float)screen_w, (float)screen_h, 0, 1, 1, 0, { 1, 1, 1, fade_val });
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, resources.mainRT.handle);
        drawPTC(resources.pPTCVertices, ptcCount, GL_QUADS);
    }

    // Swap buffers
    SDL_GL_SwapWindow(sdlWindow);

    // Go back to our 2D frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, resources.mainRT.frameBuffer);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, MaxX, MaxY);

    matrix2D = ws_Matrix::CreateOrthographicOffCenter(0, (float)MaxX, (float)MaxY, 0, -999, 999);
    //matrix2D = matrix2D.Transpose();
    {
        glUseProgram(resources.programPC);
        auto uniform = glGetUniformLocation(resources.programPC, "ProjMtx");
        glUniformMatrix4fv(uniform, 1, GL_FALSE, &matrix2D._11);
    }
    {
        glUseProgram(resources.programPTC);
        auto uniform = glGetUniformLocation(resources.programPTC, "ProjMtx");
        glUniformMatrix4fv(uniform, 1, GL_FALSE, &matrix2D._11);
    }

    pcCount = 0;
    ptcCount = 0;
    drawMode = -1;
    drawModePrim = 0;
}

void VGAClearScreen(void)
{
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, STATUSLINES, MaxX, MaxY - STATUSLINES);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glScissor(0, 0, MaxX, MaxY);
    glDisable(GL_SCISSOR_TEST);
}

int palidx = 0;

int16_t inportb(int16_t addr)
{
    switch (addr)
    {
        case 0x60:
        {
            return scancode;
        }
        case PEL_DATA:
        {
            auto &col = palette[palidx / 3];
            int16_t ret = 0;
            switch (palidx % 3)
            {
                case 0: ret = (int16_t)(byte)(col.r * 255.0f); break;
                case 1: ret = (int16_t)(byte)(col.g * 255.0f); break;
                case 2: ret = (int16_t)(byte)(col.b * 255.0f); break;
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

static void flush()
{
    switch (drawMode)
    {
        case DRAW_MODE_PC:
            if (pcCount)
            {
                drawPC(resources.pPCVertices, pcCount, drawModePrim);
                pcCount = 0;
            }
            break;
        case DRAW_MODE_PTC:
            if (ptcCount)
            {
                drawPTC(resources.pPTCVertices, ptcCount, drawModePrim);
                ptcCount = 0;
            }
            break;
    }
}

void VL_Bar(int16_t x, int16_t y, int16_t width, int16_t height, int16_t color)
{
    prepareForPC(GL_QUADS);
    pcCount += drawRect(resources.pPCVertices + pcCount, (float)x, (float)y, (float)width, (float)height, palette[color]);
}

void VL_Hlin(uint16_t x, uint16_t y, uint16_t width, uint16_t color)
{
    prepareForPC(GL_QUADS);
    pcCount += drawRect(resources.pPCVertices + pcCount, (float)x, (float)y, (float)width, 1, palette[color]);
}

void VL_Vlin(int16_t x, int16_t y, int16_t height, int16_t color)
{
    prepareForPC(GL_QUADS);
    pcCount += drawRect(resources.pPCVertices + pcCount, (float)x, (float)y, 1, (float)height, palette[color]);
}

BakedFont& getBakedFont(int id)
{
    //fontcolor = f; backcolor
    auto kid = id | (backcolor << 16) | (fontcolor << 24); // Generate a unique id by font color. A bit overkill, but meh...
    auto it = fontTextures.find(kid);
    if (it == fontTextures.end())
    {
        auto& bakedFont = fontTextures[kid];

        // Load it
        auto font = (fontstruct*)grsegs[id];
        int w, h;
        w = 0;
        h = font->height;
        for (int i = 0; i < 256; ++i)
        {
            w += font->width[i];
        }
        auto data = new uint8_t[w * h * 4];

        auto lx = 0;
        auto fg = palette[fontcolor];
        auto bg = palette[backcolor];
        bg.a = 0.0f;
        for (int i = 0; i < 256; ++i)
        {
            auto cw = font->width[i];
            bakedFont.os[i] = (float)lx / (float)w;
            bakedFont.ws[i] = (float)cw / (float)w;
            if (cw == 0) continue;
            auto src = ((byte*)font) + font->location[i];
            for (int y = 0; y < h; ++y)
            {
                for (int x = lx; x < lx + cw; ++x)
                {
                    auto k = (y * w + x) * 4;
                    Color col = bg;
                    if (*src++) col = fg;
                    data[k + 0] = (byte)(col.r * 255.0f);
                    data[k + 1] = (byte)(col.g * 255.0f);
                    data[k + 2] = (byte)(col.b * 255.0f);
                    data[k + 3] = (byte)(col.a * 255.0f);
                }
            }
            lx += cw;
        }

        bakedFont.tex = ws_create_texture(data, w, h);
        delete[] data;
        return bakedFont;
    }
    return it->second;
}

void drawDebugString(char* string, float x, float y)
{
    // backcolor = 0;
    // fontcolor = 128;
    
    fontstruct		*font;
    int16_t		width, height, i;
    byte	 *source, *dest, *origdest;
    byte	ch, mask;

    font = (fontstruct  *)grsegs[STARTFONT + 0];
    auto& bakedFont = getBakedFont(STARTFONT + 0);
    height = font->height;

    flush();
    prepareForPTC(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, bakedFont.tex);

    while ((ch = *string++) != 0)
    {
        width = font->width[ch];
        auto os = bakedFont.os[ch];
        ptcCount += drawRect(resources.pPTCVertices + ptcCount, x, y, (float)width, (float)height,
            os, 0, os + bakedFont.ws[ch], 1,
            { 1, 1, 1, 1 });
        x += (float)width;
    }

    flush();
}

void VW_DrawPropString(char  *string)
{
    fontstruct		*font;
    int16_t		width, height, i;
    byte	 *source, *dest, *origdest;
    byte	ch, mask;

    font = (fontstruct  *)grsegs[STARTFONT + fontnumber];
    auto& bakedFont = getBakedFont(STARTFONT + fontnumber);
    height = font->height;

    flush();
    prepareForPTC(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, bakedFont.tex);

    while ((ch = *string++) != 0)
    {
        width = font->width[ch];
        auto os = bakedFont.os[ch];
        ptcCount += drawRect(resources.pPTCVertices + ptcCount, (float)px, (float)py, (float)width, (float)height,
            os, 0, os + bakedFont.ws[ch], 1,
            { 1, 1, 1, 1 });
        px += width;
    }

    flush();
}

void ws_draw_screen_from_raw(byte* _data, int16_t chunk)
{
    GLuint texture = 0;
    auto it = screenRaws.find(chunk);
    if (it == screenRaws.end())
    {
        auto data = new uint8_t[MaxX * MaxY * 4];
        
        int k = 0;
        for (int y = 0; y < MaxY; ++y)
        {
            for (int x = 0; x < MaxX; ++x)
            {
                auto col = palette[_data[(y * 80 + (x >> 2)) + (x & 3) * 80 * 200]];
                data[k + 0] = (byte)(col.r * 255.0f);
                data[k + 1] = (byte)(col.g * 255.0f);
                data[k + 2] = (byte)(col.b * 255.0f);
                data[k + 3] = (byte)(col.a * 255.0f);
                k += 4;
            }
        }
        texture = ws_create_texture(data, MaxX, MaxY);
        delete[] data;
        screenRaws[chunk] = texture;
    }
    else
    {
        texture = it->second;
    }

    flush();
    prepareForPTC(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, texture);
    ptcCount += drawRect(resources.pPTCVertices + ptcCount, (float)0, (float)0, (float)MaxX, (float)MaxY, 0, 0, 1, 1, { 1, 1, 1, 1 });
    flush();
}

Pic load_pic(int16_t chunknum)
{
    int16_t	picnum = chunknum - STARTPICS;

    uint16_t width, height;
    Pic pic;

    width = pictable[picnum].width;
    height = pictable[picnum].height;
    auto _data = (byte*)grsegs[chunknum];

    auto data = new uint8_t[width * height * 4];

    int k = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            auto col = palette[_data[(y*(width >> 2) + (x >> 2)) + (x & 3)*(width >> 2)*height]];
            data[k + 0] = (byte)(col.r * 255.0f);
            data[k + 1] = (byte)(col.g * 255.0f);
            data[k + 2] = (byte)(col.b * 255.0f);
            data[k + 3] = (byte)(col.a * 255.0f);
            k += 4;
        }
    }
    auto texture = ws_create_texture(data, width, height);
    delete[] data;
    pic.w = width;
    pic.h = height;
    pic.tex = texture;
    pics[chunknum] = pic;

    return pic;
}

void ws_preload_pics()
{
    static bool loaded = false;
    if (!loaded)
    {
        loaded = true;
        for (int i = 0; i < NUMPICS; ++i)
        {
            CA_CacheGrChunk(i + STARTPICS);
            load_pic(i + STARTPICS);
        }
    }
}

void LatchDrawPic(uint16_t x, uint16_t y, uint16_t picnum)
{
    uint16_t wide, height, source;

    wide = pictable[picnum-STARTPICS].width;
    height = pictable[picnum-STARTPICS].height;
    source = latchpics[2+picnum-LATCHPICS_LUMP_START];

    //VL_LatchToScreen (source,wide/4,height,x*8,y);

    VWB_DrawPic(x * 8, y, (int16_t)picnum, (int16_t)wide / 4, (int16_t)height);
}

void VWB_DrawPic (int16_t x, int16_t y, int16_t chunknum, int16_t w, int16_t h)
{
    // Load them into textures instead of doing them lazy
    ws_preload_pics();

    int16_t	picnum = chunknum - STARTPICS;
    uint16_t width, height;
    width = pictable[picnum].width;
    height = pictable[picnum].height;
    if (w != -1) width = w;
    if (h != -1) height = h;
    x &= ~7;

    Pic pic;
    auto it = pics.find(chunknum);
    auto lw = 40;
    if (it == pics.end())
    {
        pic = load_pic(chunknum);
    }
    else
    {
        pic = it->second;
    }

    flush();
    prepareForPTC(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, pic.tex);
    ptcCount += drawRect(resources.pPTCVertices + ptcCount, (float)x, (float)y, (float)pic.w, (float)pic.h, 0, 0, 1, 1, { 1, 1, 1, 1 });
    flush();
}

void VL_SetPalette(byte  *pal)
{
    for (int i = 0; i < 256; ++i)
    {
        auto& col = palette[i];
        col.r = (float)pal[i * 3 + 0] * 255.0f;
        col.g = (float)pal[i * 3 + 1] * 255.0f;
        col.b = (float)pal[i * 3 + 2] * 255.0f;
        col.a = 1.0f;
    }
    palette[255].a = 0.0f;
}

Pic load_sprite(int16_t shapenum)
{
    uint16_t width, height;
    Pic pic;

    width = 64;
    height = 64;

    auto _data = (byte*)PM_GetSpritePage(shapenum);

    auto first_column = (word)_data[0] | (word)(_data[1]) << 8;
    auto last_column = (word)_data[2] | (word)(_data[3]) << 8;
    word *column_offsets = new word[last_column - first_column + 1];

    for (int i = 0; i <= last_column - first_column; ++i)
    {
        column_offsets[i] = (word)_data[4 + 2 * i] | (word)(_data[4 + 2 * i + 1]) << 8;
    }

    auto sprdata = new byte[64 * 64];
    memset(sprdata, 255, 64 * 64);

    word *column_offset_reader = column_offsets; // read-head that will traverse the column offsets
    for (word column = first_column; column <= last_column; ++column)
    {
        word *drawing_instructions = (word *)(_data + *column_offset_reader);
        uint32_t idx = 0;
        while (drawing_instructions[idx] != 0)
        {
            for (int row = drawing_instructions[idx + 2] / 2; row < drawing_instructions[idx] / 2; ++row)
            {
                sprdata[column + row * 64] = _data[drawing_instructions[idx + 1] + row];
            }
            idx += 3;
        }
        ++column_offset_reader; // advance the read-head
    }
    delete[] column_offsets;

    auto data = new uint8_t[width * height * 4];

    int k = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            auto col = palette[sprdata[k/4]];
            data[k + 0] = (byte)(col.r * 255.0f);
            data[k + 1] = (byte)(col.g * 255.0f);
            data[k + 2] = (byte)(col.b * 255.0f);
            data[k + 3] = (byte)(col.a * 255.0f);
            k += 4;
        }
    }

    delete[] sprdata;
    auto texture = ws_create_texture(data, width, height);
    delete[] data;
    pic.w = width;
    pic.h = height;
    pic.tex = texture;
    sprites[shapenum] = pic;

    return pic;
}

extern word PMSpriteStart, PMSoundStart;

void ws_preload_sprites()
{
    static bool loaded = false;
    if (!loaded)
    {
        loaded = true;
        for (int i = 0; i < PMSoundStart - PMSpriteStart; ++i)
        {
            load_sprite(i);
        }
    }
}

void SimpleScaleShape(int16_t xcenter, int16_t shapenum, uint16_t height)
{
    //ws_preload_sprites();

    auto it = sprites.find(shapenum);
    Pic pic;
    if (it == sprites.end()) pic = load_sprite(shapenum);
    else pic = it->second;

    static const int SCALE = 2;

    flush();
    prepareForPTC(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, pic.tex);
    ptcCount += drawRect(resources.pPTCVertices + ptcCount, (float)(MaxX / 2 + (xcenter - pic.w / 2) * SCALE), (float)(MaxY - height * SCALE + 1 - pic.h * SCALE - STATUSLINES), (float)(pic.w * SCALE), (float)(pic.h * SCALE), 0, 0, 1, 1, { 1, 1, 1, 1 });
    flush();
}
