#include "ws.h"

#include "WL_DEF.h"

#define DRAW_MODE_PC 0
#define DRAW_MODE_PTC 1
#define DRAW_MODE_PNTC 2

GLenum ws_draw_mode_prim = 0;
int ws_pc_count = 0;
int ws_ptc_count = 0;
int ws_pntc_count = 0;
int ws_draw_mode = -1;
float ws_ui_scale = 1.0f;
GLuint ws_current_3d_texture = 0;

void ws_draw_pc(const ws_VertexPC *pVertices, int count, GLenum mode)
{
    glBindBuffer(GL_ARRAY_BUFFER, ws_resources.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ws_VertexPC) * count, pVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ws_VertexPC), (float *)(uintptr_t)(0));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ws_VertexPC), (float *)(uintptr_t)(8));
    glDrawArrays(mode, 0, count);
}

void ws_draw_ptc(const ws_VertexPTC *pVertices, int count, GLenum mode)
{
    glBindBuffer(GL_ARRAY_BUFFER, ws_resources.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ws_VertexPTC) * count, pVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ws_VertexPTC), (float *)(uintptr_t)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ws_VertexPTC), (float *)(uintptr_t)(8));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ws_VertexPTC), (float *)(uintptr_t)(16));
    glDrawArrays(mode, 0, count);
}

void ws_draw_pntc(const ws_VertexPNTC *pVertices, int count, GLenum mode)
{
    glBindBuffer(GL_ARRAY_BUFFER, ws_resources.vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ws_VertexPNTC) * count, pVertices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(ws_VertexPNTC), (float *)(uintptr_t)(0));
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(ws_VertexPNTC), (float *)(uintptr_t)(12));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(ws_VertexPNTC), (float *)(uintptr_t)(24));
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ws_VertexPNTC), (float *)(uintptr_t)(32));
    glDrawArrays(mode, 0, count);
}

void ws_draw_lines(const ws_VertexPC *pVertices, int count)
{
    ws_draw_pc(pVertices, count, GL_LINES);
}

void ws_draw_points(const ws_VertexPC *pVertices, int count)
{
    ws_draw_pc(pVertices, count, GL_POINTS);
}

void ws_draw_quads(const ws_VertexPC *pVertices, int count)
{
    ws_draw_pc(pVertices, count, GL_QUADS);
}

void ws_prepare_for_pc(int prim)
{
    if (ws_draw_mode == DRAW_MODE_PC && prim == ws_draw_mode_prim)
        return;
    ws_flush();
    ws_draw_mode_prim = prim;
    if (ws_draw_mode != DRAW_MODE_PC)
    {
        ws_draw_mode = DRAW_MODE_PC;
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glUseProgram(ws_resources.programPC);
        glEnableVertexAttribArray(0); // pos
        glEnableVertexAttribArray(1); // color
        glDisableVertexAttribArray(2);
        glDisableVertexAttribArray(3);
    }
}

void ws_prepare_for_ptc(int prim)
{
    if (ws_draw_mode == DRAW_MODE_PTC && prim == ws_draw_mode_prim)
        return;
    ws_flush();
    ws_draw_mode_prim = prim;
    if (ws_draw_mode != DRAW_MODE_PTC)
    {
        ws_draw_mode = DRAW_MODE_PTC;
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_DEPTH_TEST);
        glActiveTexture(GL_TEXTURE0);
        glUseProgram(ws_resources.programPTC);
        glEnableVertexAttribArray(0); // pos
        glEnableVertexAttribArray(1); // texcoord
        glEnableVertexAttribArray(2); // color
        glDisableVertexAttribArray(3);
    }
}

void ws_prepare_for_pntc(int prim, GLuint texture)
{
    if (ws_pntc_count < 90000 && ws_draw_mode == DRAW_MODE_PNTC && prim == ws_draw_mode_prim && ws_current_3d_texture == texture)
        return;
    ws_flush();
    ws_draw_mode_prim = prim;
    ws_current_3d_texture = texture;
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ws_current_3d_texture);
    if (ws_draw_mode != DRAW_MODE_PNTC)
    {
        ws_draw_mode = DRAW_MODE_PNTC;
        glUseProgram(ws_deferred_enabled ? ws_resources.programGBufferPNTC : ws_resources.programPNTC);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        glEnableVertexAttribArray(0); // pos
        glEnableVertexAttribArray(1); // normal
        glEnableVertexAttribArray(2); // texcoord
        glEnableVertexAttribArray(3); // color
    }
}

void ws_draw_lines(const ws_VertexPTC *pVertices, int count)
{
    ws_draw_ptc(pVertices, count, GL_LINES);
}

void ws_draw_points(const ws_VertexPTC *pVertices, int count)
{
    ws_draw_ptc(pVertices, count, GL_POINTS);
}

void ws_draw_quads(const ws_VertexPTC *pVertices, int count)
{
    ws_draw_ptc(pVertices, count, GL_QUADS);
}

int ws_draw_rect(ws_VertexPTC *pVertices, float x, float y, float w, float h, float u1, float v1, float u2, float v2, const ws_Color &color)
{
    pVertices[0].position.x = x;
    pVertices[0].position.y = y;
    pVertices[0].texCoord = { u1, v1 };
    pVertices[0].color = color;

    pVertices[1].position.x = x;
    pVertices[1].position.y = y + h;
    pVertices[1].texCoord = { u1, v2 };
    pVertices[1].color = color;

    pVertices[2].position.x = x + w;
    pVertices[2].position.y = y + h;
    pVertices[2].texCoord = { u2, v2 };
    pVertices[2].color = color;

    pVertices[3].position.x = x + w;
    pVertices[3].position.y = y;
    pVertices[3].texCoord = { u2, v1 };
    pVertices[3].color = color;
    pVertices += 4;

    return 4;
}

int ws_draw_rect(ws_VertexPC *pVertices, float x, float y, float w, float h, const ws_Color &color)
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

int ws_draw_line(ws_VertexPC *pVertices, const ws_Vector2 &from, const ws_Vector2 &to, const ws_Color &color)
{
    pVertices[0].position.x = from.x;
    pVertices[0].position.y = from.y;
    pVertices[0].color = color;

    pVertices[1].position.x = to.x;
    pVertices[1].position.y = to.y;
    pVertices[1].color = color;

    return 2;
}

void VL_Bar(int16_t x, int16_t y, int16_t width, int16_t height, int16_t color)
{
    ws_prepare_for_pc(GL_QUADS);
    ws_pc_count += ws_draw_rect(ws_resources.pPCVertices + ws_pc_count,
        (float)x * ws_ui_scale,
        (float)y * ws_ui_scale,
        (float)width * ws_ui_scale,
        (float)height * ws_ui_scale,
        ws_palette[color]);
}

void VL_Hlin(uint16_t x, uint16_t y, uint16_t width, uint16_t color)
{
    ws_prepare_for_pc(GL_QUADS);
    ws_pc_count += ws_draw_rect(ws_resources.pPCVertices + ws_pc_count,
        (float)x * ws_ui_scale,
        (float)y * ws_ui_scale,
        (float)width * ws_ui_scale,
        1 * ws_ui_scale,
        ws_palette[color]);
}

void VL_Vlin(int16_t x, int16_t y, int16_t height, int16_t color)
{
    ws_prepare_for_pc(GL_QUADS);
    ws_pc_count += ws_draw_rect(ws_resources.pPCVertices + ws_pc_count,
        (float)x * ws_ui_scale,
        (float)y * ws_ui_scale,
        1 * ws_ui_scale,
        (float)height * ws_ui_scale,
        ws_palette[color]);
}

void VWB_DrawPic(int16_t x, int16_t y, int16_t chunknum, int16_t w, int16_t h)
{
    // Load them into textures instead of doing them lazy otherwise it crashes (?)
    ws_preload_ui_textures();

    int16_t picnum = chunknum - STARTPICS;
    uint16_t width, height;
    width = pictable[picnum].width;
    height = pictable[picnum].height;
    if (w != -1)
        width = w;
    if (h != -1)
        height = h;
    x &= ~7;

    ws_Texture pic;
    auto it = ws_ui_textures.find(chunknum);
    auto lw = 40;
    if (it == ws_ui_textures.end())
    {
        pic = ws_load_ui_texture(chunknum);
    }
    else
    {
        pic = it->second;
    }

    ws_flush();
    ws_prepare_for_ptc(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, pic.tex);
    ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count,
        (float)x * ws_ui_scale, (float)y * ws_ui_scale,
        (float)pic.w * ws_ui_scale, (float)pic.h * ws_ui_scale,
        0, 0, 1, 1, { 1, 1, 1, 1 });
    ws_flush();
}

void LatchDrawPic(uint16_t x, uint16_t y, uint16_t picnum)
{
    uint16_t wide, height, source;

    wide = pictable[picnum - STARTPICS].width;
    height = pictable[picnum - STARTPICS].height;
    source = latchpics[2 + picnum - LATCHPICS_LUMP_START];

    //VL_LatchToScreen (source,wide/4,height,x*8,y);

    VWB_DrawPic(x * 8, y, (int16_t)picnum, (int16_t)wide / 4, (int16_t)height);
}

void ws_draw_screen_from_raw(byte *_data, int16_t chunk)
{
    GLuint texture = 0;
    auto it = ws_screen_textures.find(chunk);
    if (it == ws_screen_textures.end())
    {
        auto data = new uint8_t[MaxX * MaxY * 4];

        int k = 0;
        for (int y = 0; y < MaxY; ++y)
        {
            for (int x = 0; x < MaxX; ++x)
            {
                auto col = ws_palette[_data[(y * 80 + (x >> 2)) + (x & 3) * 80 * 200]];
                data[k + 0] = (byte)(col.r * 255.0f);
                data[k + 1] = (byte)(col.g * 255.0f);
                data[k + 2] = (byte)(col.b * 255.0f);
                data[k + 3] = (byte)(col.a * 255.0f);
                k += 4;
            }
        }
        texture = ws_create_texture(data, MaxX, MaxY);
        delete[] data;
        ws_screen_textures[chunk] = texture;
    }
    else
    {
        texture = it->second;
    }

    ws_flush();
    ws_prepare_for_ptc(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, texture);
    ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count, (float)0, (float)0, (float)ws_screen_w, (float)ws_screen_h, 0, 0, 1, 1, { 1, 1, 1, 1 });
    ws_flush();
}

void VW_DrawPropString(char *string)
{
    fontstruct *font;
    int16_t width, height, i;
    byte *source, *dest, *origdest;
    byte ch, mask;

    font = (fontstruct *)grsegs[STARTFONT + fontnumber];
    auto &bakedFont = ws_get_font(STARTFONT + fontnumber);
    height = font->height;

    ws_flush();
    ws_prepare_for_ptc(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, bakedFont.tex);

    while ((ch = *string++) != 0)
    {
        width = font->width[ch];
        auto os = bakedFont.os[ch];
        ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count,
            (float)px * ws_ui_scale, (float)py * ws_ui_scale,
            (float)width * ws_ui_scale, (float)height * ws_ui_scale,
            os, 0, os + bakedFont.ws[ch], 1,
            { 1, 1, 1, 1 });
        px += width;
    }

    ws_flush();
}

void SimpleScaleShape(int16_t xcenter, int16_t shapenum, uint16_t height)
{
    if (ws_debug_view_enabled) return; // Don't show the gun in debug view

    //ws_preload_sprites();
    //auto offset = xcenter - viewwidth / 2;

    auto it = ws_sprite_textures.find(shapenum);
    ws_Texture pic;
    if (it == ws_sprite_textures.end())
        pic = ws_load_sprite_texture(shapenum);
    else
        pic = it->second;

    static const int SCALE = 2;

    ws_flush();
    ws_prepare_for_ptc(GL_QUADS);
    glBindTexture(GL_TEXTURE_2D, pic.tex);
    ws_ptc_count += ws_draw_rect(
        ws_resources.pPTCVertices + ws_ptc_count,
        (float)(MaxX / 2 - pic.w / 2 * SCALE) * ws_ui_scale,
        (float)(MaxY - SCALE - STATUSLINES - pic.h * SCALE + SCALE) * ws_ui_scale,
        (float)(pic.w * SCALE) * ws_ui_scale,
        (float)(pic.h * SCALE) * ws_ui_scale,
        0, 0, 1, 1, { 1, 1, 1, 1 });
    ws_flush();
}

void ws_flush()
{
    switch (ws_draw_mode)
    {
    case DRAW_MODE_PC:
        if (ws_pc_count)
        {
            ws_draw_pc(ws_resources.pPCVertices, ws_pc_count, ws_draw_mode_prim);
            ws_pc_count = 0;
        }
        break;
    case DRAW_MODE_PTC:
        if (ws_ptc_count)
        {
            ws_draw_ptc(ws_resources.pPTCVertices, ws_ptc_count, ws_draw_mode_prim);
            ws_ptc_count = 0;
        }
        break;
    case DRAW_MODE_PNTC:
        if (ws_pntc_count)
        {
            ws_draw_pntc(ws_resources.pPNTCVertices, ws_pntc_count, ws_draw_mode_prim);
            ws_pntc_count = 0;
        }
        break;
    }
}
