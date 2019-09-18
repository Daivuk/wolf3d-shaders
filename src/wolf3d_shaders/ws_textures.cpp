#include "ws.h"

std::map<int16_t, ws_Texture> ws_ui_textures;
std::map<int16_t, GLuint> ws_screen_textures;
std::map<int, ws_Font> ws_font_textures;
std::map<int16_t, ws_Texture> ws_sprite_textures;
std::map<int16_t, ws_Texture> ws_wall_textures;

GLuint ws_create_texture(uint8_t *data, int w, int h)
{
    GLuint handle;
    glGenTextures(1, &handle);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    return handle;
}

ws_RenderTarget ws_create_main_rt(int w, int h)
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

    GLuint rboDepthStencil;
    glGenRenderbuffers(1, &rboDepthStencil);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepthStencil);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboDepthStencil);

    ws_RenderTarget ret;
    ret.handle = handle;
    ret.frameBuffer = frameBuffer;
    ret.depth = rboDepthStencil;
    return ret;
}

ws_RenderTarget ws_create_rt(int w, int h)
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

    ws_RenderTarget ret;
    ret.handle = handle;
    ret.frameBuffer = frameBuffer;
    ret.depth = 0;
    return ret;
}

ws_RenderTarget ws_create_hdr_rt(int w, int h)
{
    ws_RenderTarget ret;

    glGenFramebuffers(1, &ret.frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, ret.frameBuffer);

    glGenTextures(1, &ret.handle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ret.handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ret.handle, 0);

    // Attach the main depth buffer
    {
        glBindRenderbuffer(GL_RENDERBUFFER, ws_resources.mainRT.depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ws_resources.mainRT.depth);
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    return ret;
}

void ws_resize_rt(ws_RenderTarget &rt, int w, int h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, rt.frameBuffer);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rt.handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void ws_resize_main_rt(ws_RenderTarget &rt, int w, int h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, rt.frameBuffer);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rt.handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, rt.depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rt.depth);
}

void ws_resize_hdr_rt(ws_RenderTarget &rt, int w, int h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, rt.frameBuffer);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, rt.handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

ws_Texture ws_load_ui_texture(int16_t chunknum)
{
    int16_t picnum = chunknum - STARTPICS;

    uint16_t width, height;
    ws_Texture pic;

    width = pictable[picnum].width;
    height = pictable[picnum].height;
    auto _data = (byte *)grsegs[chunknum];

    auto data = new uint8_t[width * height * 4];

    int k = 0;
    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            auto col = ws_palette[_data[(y * (width >> 2) + (x >> 2)) + (x & 3) * (width >> 2) * height]];
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
    ws_ui_textures[chunknum] = pic;

    return pic;
}

void ws_preload_ui_textures()
{
    static bool loaded = false;
    if (!loaded)
    {
        loaded = true;
        for (int i = 0; i < NUMPICS; ++i)
        {
            CA_CacheGrChunk(i + STARTPICS);
            ws_load_ui_texture(i + STARTPICS);
        }
    }
}

ws_Texture ws_load_sprite_texture(int16_t shapenum)
{
    uint16_t width, height;
    ws_Texture pic;

    width = 64;
    height = 64;

    auto _data = (byte *)PM_GetSpritePage(shapenum);

    auto first_column = (word)_data[0] | (word)(_data[1]) << 8;
    auto last_column = (word)_data[2] | (word)(_data[3]) << 8;
    word *column_offsets = new word[last_column - first_column + 1];

    for (int i = 0; i <= last_column - first_column; ++i)
    {
        column_offsets[i] = (word)_data[4 + 2 * i] | (word)(_data[4 + 2 * i + 1]) << 8;
    }

    auto sprdata = new byte[64 * 64];
    memset(sprdata, 255, 64 * 64);

    byte* trexel = _data + (4 + 2 * (last_column - first_column + 1));
    word *column_offset_reader = column_offsets; // read-head that will traverse the column offsets
    for (word column = first_column; column <= last_column; ++column)
    {
        word *drawing_instructions = (word *)(_data + *column_offset_reader);
        uint32_t idx = 0;
        while (drawing_instructions[idx] != 0)
        {
            for (int row = drawing_instructions[idx + 2] / 2; row < drawing_instructions[idx] / 2; ++row)
            {
                sprdata[column - 1 + row * 64 - 1] = *trexel++;
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
            auto col = ws_palette[sprdata[k / 4]];
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
    ws_sprite_textures[shapenum] = pic;

    return pic;
}

ws_Texture ws_load_wall_texture(int wallpic)
{
    uint16_t width, height;
    ws_Texture pic;

    width = 64;
    height = 64;

    auto _data = (byte *)PM_GetPage(wallpic);

    auto data = new byte[64 * 64 * 4];

    for (int x = 0; x < 64; ++x)
    {
        for (int y = 0; y < 64; ++y)
        {
            auto& col = ws_palette[*_data++];
            auto k = (y * 64 + x) * 4;
            data[k + 0] = (byte)(col.r * 255.0f);
            data[k + 1] = (byte)(col.g * 255.0f);
            data[k + 2] = (byte)(col.b * 255.0f);
            data[k + 3] = (byte)(col.a * 255.0f);
        }
    }

    auto texture = ws_create_texture(data, width, height);
    delete[] data;
    pic.w = width;
    pic.h = height;
    pic.tex = texture;
    ws_wall_textures[wallpic] = pic;

    return pic;
}

void ws_preload_sprites()
{
    extern word PMSpriteStart, PMSoundStart;

    static bool loaded = false;
    if (!loaded)
    {
        loaded = true;
        for (int i = 0; i < PMSoundStart - PMSpriteStart; ++i)
        {
            ws_load_sprite_texture(i);
        }
    }
}

ws_Font &ws_get_font(int id)
{
    //fontcolor = f; backcolor
    auto kid = id | (backcolor << 16) | (fontcolor << 24); // Generate a unique id by font color. A bit overkill, but meh...
    auto it = ws_font_textures.find(kid);
    if (it == ws_font_textures.end())
    {
        auto &bakedFont = ws_font_textures[kid];

        // Load it
        auto font = (fontstruct *)grsegs[id];
        int w, h;
        w = 0;
        h = font->height;
        for (int i = 0; i < 256; ++i)
        {
            w += font->width[i];
        }
        auto data = new uint8_t[w * h * 4];

        auto lx = 0;
        auto fg = ws_palette[fontcolor];
        auto bg = ws_palette[backcolor];
        bg.a = 0.0f;
        for (int i = 0; i < 256; ++i)
        {
            auto cw = font->width[i];
            bakedFont.os[i] = (float)lx / (float)w;
            bakedFont.ws[i] = (float)cw / (float)w;
            if (cw == 0)
                continue;
            auto src = ((byte *)font) + font->location[i];
            for (int y = 0; y < h; ++y)
            {
                for (int x = lx; x < lx + cw; ++x)
                {
                    auto k = (y * w + x) * 4;
                    ws_Color col = bg;
                    if (*src++)
                        col = fg;
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
