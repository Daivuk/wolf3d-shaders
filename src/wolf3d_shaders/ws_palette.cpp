#include "ws.h"

#define wolf_RGB(r, g, b) { (float)((r)*255 / 63) / 255.0f, (float)((g)*255 / 63) / 255.0f, (float)((b)*255 / 63) / 255.0f, 1.0f }

ws_Color ws_palette[] = {
#ifdef SPEAR
#include "sodpal.inc"
#else
#include "wolfpal.inc"
#endif
};

ws_Color ws_dynamic_palette[] = {
#ifdef SPEAR
#include "sodpal.inc"
#else
#include "wolfpal.inc"
#endif
};

void VL_SetPalette(byte *pal)
{
    for (int i = 0; i < 256; ++i)
    {
        auto &col = ws_dynamic_palette[i];
        col.r = (float)pal[i * 3 + 0] * 255.0f;
        col.g = (float)pal[i * 3 + 1] * 255.0f;
        col.b = (float)pal[i * 3 + 2] * 255.0f;
        col.a = 1.0f;
    }
    ws_dynamic_palette[255].a = 0.0f;
}

void VL_GetPalette(byte  *ws_palette)
{
    for (int i = 0; i < 256; ++i)
    {
        auto &col = ws_dynamic_palette[i];
        ws_palette[i * 3 + 0] = (byte)(col.r * 255.0f);
        ws_palette[i * 3 + 1] = (byte)(col.g * 255.0f);
        ws_palette[i * 3 + 2] = (byte)(col.b * 255.0f);
    }
}
