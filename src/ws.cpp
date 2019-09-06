/*
    Program entry and main loop.
    This is where we initialize SDL and poll events.
*/

#if defined(WIN32)
#include <windows.h>
#endif

#include "ws.h"
#include "shaders.h"
#include "Matrix.h"

#include <assert.h>
#include <chrono>
#include <stdio.h>
#include <vector>

// That's for 2D
static const int MAX_VERTICES = 100000;

void	Quit (char *error);

void wolf3d_init();
void wolf3d_update();
void wolf3d_shutdown();
void audioCallback(void *userdata, Uint8 *stream, int len);

int _argc; // global arguments. this is referenced a bit everywhere
char** _argv;
int pcCount = 0;
int ptcCount = 0;
Matrix matrix2D;
Matrix matrix3D;

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
} resources;

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

void prepareForPC()
{
    glDisable(GL_TEXTURE_2D);
    glUseProgram(resources.programPC);
    glEnableVertexAttribArray(0); // pos
    glEnableVertexAttribArray(1); // color
    glDisableVertexAttribArray(2);
}

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

void prepareForPTC()
{
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(resources.programPTC);
    glEnableVertexAttribArray(0); // pos
    glEnableVertexAttribArray(1); // color
    glEnableVertexAttribArray(2);
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

    // Init main resources
    resources.programPC = createProgram(PC_VERT, PC_FRAG, {"Position", "Color"});
    resources.programPTC = createProgram(PTC_VERT, PTC_FRAG, {"Position", "TexCoord", "Color"});
    resources.vertexBuffer = createVertexBuffer();
    resources.pPCVertices = new VertexPC[MAX_VERTICES];
    resources.pPTCVertices = new VertexPTC[MAX_VERTICES];

    uint32_t checkerBytes[] = { 0xFF880088, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF880088 };
    resources.checkerTexture = ws_create_texture((uint8_t*)&checkerBytes, 2, 2);

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
        SDL_UnlockAudio();

        // Draw game
        pcCount = 0;
        ptcCount = 0;
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // matrix2D = Matrix::CreateOrthographic(1024, 768, -999, 999);
        matrix2D = Matrix::CreateOrthographicOffCenter(0, 1024, 768, 0, -999, 999);
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

        // prepareForPC();
        // // pcCount += drawLine(resources.pPCVertices, {0, 0}, {100, 100}, {1, 0, 0, 1});
        // pcCount += drawRect(resources.pPCVertices, 0, 0, 100, 100, {1, 0, 0, 1});
        // drawPC(resources.pPCVertices, pcCount, GL_QUADS);

        prepareForPTC();
        ptcCount += drawRect(resources.pPTCVertices, 0, 0, 100, 100, 0, 0, 4, 4, {1, 1, 1, 1});
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, resources.checkerTexture);
        drawPTC(resources.pPTCVertices, ptcCount, GL_QUADS);

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
