/*
    Program entry and main loop.
    This is where we initialize SDL and poll events.
*/

#if defined(WIN32)
#include <windows.h>
#endif

#include "ws_main.h"
#include "ws_shaders.h"

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
void flush();

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
    {SDL_SCANCODE_UNKNOWN, sc_None},
    {0xff, sc_Bad},
    {SDL_SCANCODE_RETURN, sc_Return},
    {sc_Return, sc_Enter},
    {SDL_SCANCODE_ESCAPE, sc_Escape},
    {SDL_SCANCODE_SPACE, sc_Space},
    {SDL_SCANCODE_BACKSPACE, sc_BackSpace},
    {SDL_SCANCODE_TAB, sc_Tab},
    {SDL_SCANCODE_LALT, sc_Alt},
    {SDL_SCANCODE_LCTRL, sc_Control},
    {SDL_SCANCODE_CAPSLOCK, sc_CapsLock},
    {SDL_SCANCODE_LSHIFT, sc_LShift},
    {SDL_SCANCODE_RSHIFT, sc_RShift},
    {SDL_SCANCODE_UP, sc_UpArrow},
    {SDL_SCANCODE_DOWN, sc_DownArrow},
    {SDL_SCANCODE_LEFT, sc_LeftArrow},
    {SDL_SCANCODE_RIGHT, sc_RightArrow},
    {SDL_SCANCODE_INSERT, sc_Insert},
    {SDL_SCANCODE_DELETE, sc_Delete},
    {SDL_SCANCODE_HOME, sc_Home},
    {SDL_SCANCODE_END, sc_End},
    {SDL_SCANCODE_PAGEUP, sc_PgUp},
    {SDL_SCANCODE_PAGEDOWN, sc_PgDn},
    {SDL_SCANCODE_F1, sc_F1},
    {SDL_SCANCODE_F2, sc_F2},
    {SDL_SCANCODE_F3, sc_F3},
    {SDL_SCANCODE_F4, sc_F4},
    {SDL_SCANCODE_F5, sc_F5},
    {SDL_SCANCODE_F6, sc_F6},
    {SDL_SCANCODE_F7, sc_F7},
    {SDL_SCANCODE_F8, sc_F8},
    {SDL_SCANCODE_F9, sc_F9},
    {SDL_SCANCODE_F10, sc_F10},
    {SDL_SCANCODE_F11, sc_F11},
    {SDL_SCANCODE_F12, sc_F12},

    {SDL_SCANCODE_1, sc_1},
    {SDL_SCANCODE_2, sc_2},
    {SDL_SCANCODE_3, sc_3},
    {SDL_SCANCODE_4, sc_4},
    {SDL_SCANCODE_5, sc_5},
    {SDL_SCANCODE_6, sc_6},
    {SDL_SCANCODE_7, sc_7},
    {SDL_SCANCODE_8, sc_8},
    {SDL_SCANCODE_9, sc_9},
    {SDL_SCANCODE_0, sc_0},

    {SDL_SCANCODE_A, sc_A},
    {SDL_SCANCODE_B, sc_B},
    {SDL_SCANCODE_C, sc_C},
    {SDL_SCANCODE_D, sc_D},
    {SDL_SCANCODE_E, sc_E},
    {SDL_SCANCODE_F, sc_F},
    {SDL_SCANCODE_G, sc_G},
    {SDL_SCANCODE_H, sc_H},
    {SDL_SCANCODE_I, sc_I},
    {SDL_SCANCODE_J, sc_J},
    {SDL_SCANCODE_K, sc_K},
    {SDL_SCANCODE_L, sc_L},
    {SDL_SCANCODE_M, sc_M},
    {SDL_SCANCODE_N, sc_N},
    {SDL_SCANCODE_O, sc_O},
    {SDL_SCANCODE_P, sc_P},
    {SDL_SCANCODE_Q, sc_Q},
    {SDL_SCANCODE_R, sc_R},
    {SDL_SCANCODE_S, sc_S},
    {SDL_SCANCODE_T, sc_T},
    {SDL_SCANCODE_U, sc_U},
    {SDL_SCANCODE_V, sc_V},
    {SDL_SCANCODE_W, sc_W},
    {SDL_SCANCODE_X, sc_X},
    {SDL_SCANCODE_Y, sc_Y},
    {SDL_SCANCODE_Z, sc_Z},
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

#define DRAW_MODE_PC 0
#define DRAW_MODE_PTC 1
int drawMode = -1;
GLenum drawModePrim = 0;
Color palette[256];

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
    drawMode = DRAW_MODE_PC;
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

    // Start with a random colot palette
    srand(0);
    for (int i = 0; i < 256; ++i)
    {
        palette[i].r = (float)(rand() % 256) / 255.0f;
        palette[i].g = (float)(rand() % 256) / 255.0f;
        palette[i].b = (float)(rand() % 256) / 255.0f;
        palette[i].a = 1.0f;
    }

    uint32_t checkerBytes[] = { 0xFF880088, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF880088 };
    resources.checkerTexture = ws_create_texture((uint8_t*)&checkerBytes, 2, 2);

    // Draw test checker
    {
        ws_update_sdl();

        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        //glBindTexture(GL_TEXTURE_2D, resources.checkerTexture);

        //prepareForPTC(GL_QUADS);
        ////ptcCount += drawRect(resources.pPTCVertices, (float)MaxX / 2 - 50, (float)MaxY / 2 - 50, 100, 100, 0, 0, 4, 4, { 1, 1, 1, 1 });
        //ptcCount += drawRect(resources.pPTCVertices, MaxX/2-32, MaxY/2-32, 64, 64, 0, 0, 4, 4, { 1, 1, 1, 1 });
        //drawPTC(resources.pPTCVertices, ptcCount, GL_QUADS);
        //ptcCount = 0;
        //flush();
    }

    // Init wolf
    wolf3d_init(); // This also pretty much runs the main loop

    // Cleanup
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
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
    flush();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
            scancode = getDosScanCode(event.key.keysym.scancode);
            if (KeyInt_in) KeyInt_in();
            break;
        case SDL_KEYUP:
            scancode = getDosScanCode(event.key.keysym.scancode);
            if (KeyInt_in) KeyInt_in();
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

    // Draw game
    pcCount = 0;
    ptcCount = 0;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
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
    ptcCount += drawRect(resources.pPTCVertices, 0, 0, (float)screen_w, (float)screen_h, 0, 1, 1, 0, { 1, 1, 1, 1 });
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, resources.mainRT.handle);
    drawPTC(resources.pPTCVertices, ptcCount, GL_QUADS);

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

int16_t inportb(int16_t addr)
{
    if (addr == 0x60)
    {
        return scancode;
    }
    return 0;
}

void outportb(int16_t addr, char val)
{
    if (addr == 0x61)
    {

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

struct BakedFont
{
    GLuint tex;
    float ws[256];
    float os[256];
};
std::map<int, BakedFont> fontTextures;

BakedFont& getBakedFont(int id)
{
    auto it = fontTextures.find(id);
    if (it == fontTextures.end())
    {
        auto& bakedFont = fontTextures[id];

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
                    auto col = palette[*src++];
                    data[k + 0] = (byte)(col.r * 255.0f);
                    data[k + 1] = (byte)(col.g * 255.0f);
                    data[k + 2] = (byte)(col.b * 255.0f);
                    data[k + 3] = 255;
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