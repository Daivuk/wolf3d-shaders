#include <imgui.h>
#include <imgui_internal.h>

#include "ws.h"

static const ws_Vector2 TOOL_GBUFFER_SCALES[] = {
    {(float)ws_screen_w / 8.0f, (float)ws_screen_h / 8.0f},
    {(float)ws_screen_w / 6.0f, (float)ws_screen_h / 6.0f},
    {(float)ws_screen_w / 4.0f, (float)ws_screen_h / 4.0f},
    {(float)ws_screen_w / 3.0f, (float)ws_screen_h / 3.0f},
    {(float)ws_screen_w / 2.0f, (float)ws_screen_h / 2.0f}
};
static const int TOOL_GBUFFER_SCALES_COUNT = sizeof(TOOL_GBUFFER_SCALES) / sizeof(ws_Vector2);

static ws_SpriteSettings ws_sprite_settings_clipboard = WS_DEFAULT_SPRITE_SETTINGS;

int ws_selected_sprite = -1;
int ws_gbuffer_tool_scale = 0;

bool ws_tool_fullscreen_textures_pinned = false;
bool ws_tool_fonts_pinned = false;
bool ws_tool_ui_textures_pinned = false;
bool ws_tool_walls_pinned = false;
bool ws_tool_rendering_pinned = false;
bool ws_tool_buffers_pinned = false;
bool ws_tool_sprites_pinned = false;
bool ws_tool_stats_pinned = false;

static void tool_pointlight(const char* name, ws_PointLight& pointLight)
{
    if (ImGui::CollapsingHeader(name))
    {
        ImGui::DragFloat3("Offset", &pointLight.position.x, 0.01f, -2.0f, 2.0f, "%.2f");
        ImGui::ColorEdit3("Color", &pointLight.color.r);
        ImGui::DragFloat("Radius", &pointLight.radius, 0.1f, 0.0f, 1000.0f, "%.1f");
        ImGui::DragFloat("Intensity", &pointLight.intensity, 0.01f, 0.0f, 100.0f, "%.2f");
        ImGui::Checkbox("Cast Shadow", &pointLight.cast_shadow);
    }
}

static void tool_fullScreenTextures()
{
    ImGui::Begin("Fullscreen Textures");
    ImGui::Checkbox("Pinned", &ws_tool_fullscreen_textures_pinned);
    ImGui::Separator();
    auto w = ImGui::GetWindowWidth();
    for (auto &kv : ws_screen_textures)
    {
        if (ImGui::GetCursorPosX() + 320.0f < w - 320.0f) ImGui::SameLine();
        ImGui::Image(&kv.second, { 320.0f, 200.0f });
    }
    ImGui::End();
}

static void tool_fonts()
{
    ImGui::Begin("Fonts");
    ImGui::Checkbox("Pinned", &ws_tool_fonts_pinned);
    ImGui::Separator();
    for (auto &kv : ws_font_textures)
    {
        ImGui::Image(&kv.second.tex, { 128.0f, 10.0f }, { 0.0f, 0.0f }, {0.15f, 1.0f});
    }
    ImGui::End();
}

static void tool_uiTextures()
{
    ImGui::Begin("UI Textures");
    ImGui::Checkbox("Pinned", &ws_tool_ui_textures_pinned);
    ImGui::Separator();
    auto w = ImGui::GetWindowWidth();
    float x = w;
    for (auto &kv : ws_ui_textures)
    {
        auto uiw = (float)kv.second.w;
        if (x + uiw < w - uiw) ImGui::SameLine();
        x = ImGui::GetCursorPosX();
        ImGui::Image(&kv.second.tex, { uiw, (float)kv.second.h });
    }
    ImGui::End();
}

static void tool_walls()
{
    ImGui::Begin("Walls");
    ImGui::Checkbox("Pinned", &ws_tool_walls_pinned);
    ImGui::Separator();
    auto w = ImGui::GetWindowWidth();
    float x = w;
    for (auto &kv : ws_wall_textures)
    {
        auto ww = (float)kv.second.w;
        if (x + ww < w - ww) ImGui::SameLine();
        x = ImGui::GetCursorPosX();
        ImGui::Image(&kv.second.tex, { ww, (float)kv.second.h });
    }
    ImGui::End();
}

static void tool_rendering()
{
    ImGui::Begin("Rendering");
    ImGui::Checkbox("Pinned", &ws_tool_rendering_pinned);
    ImGui::Separator();
    ImGui::Checkbox("Wireframe", &ws_wireframe_enabled);
    ImGui::Checkbox("Textures", &ws_texture_enabled);
    ImGui::Checkbox("Sprite textures", &ws_sprite_texture_enabled);
    ImGui::Checkbox("Sprites", &ws_sprite_enabled);
    if (ImGui::CollapsingHeader("Ambient Occlusion"))
    {
        ImGui::Checkbox("Enabled##AO", &ws_ao_enabled);
        ImGui::SliderFloat("Amount##AO", &ws_ao_amount, 0.0f, 1.0f);
        ImGui::SliderFloat("Size##AO", &ws_ao_size, 0.0f, 0.5f);
    }
    if (ImGui::CollapsingHeader("Deferred Shading"))
    {
        ImGui::Checkbox("Enabled##DS", &ws_deferred_enabled);
        ImGui::ColorEdit3("Ambient Color", &ws_ambient_color.r);
        tool_pointlight("Player Light", ws_player_light);
    }
    ImGui::End();
}

static void tool_sprites()
{
    auto spriteScale = 4.0f;
    ImGui::Begin("Sprites");
    ImGui::Checkbox("Pinned", &ws_tool_sprites_pinned);
    ImGui::Separator();
    if (ws_selected_sprite != -1)
    {
        if (ws_sprite_settings.find(ws_selected_sprite) == ws_sprite_settings.end())
        {
            ws_SpriteSettings ss = WS_DEFAULT_SPRITE_SETTINGS;
            ss.id = ws_selected_sprite;
            ws_sprite_settings[ws_selected_sprite] = ss;
        }
        auto& sprite = ws_sprite_settings[ws_selected_sprite];
        auto& pic = ws_sprite_textures[(int16_t)ws_selected_sprite];

        ImGui::LabelText("ID", "%i", sprite.id);
        if (ImGui::Button("Copy"))
        {
            ws_sprite_settings_clipboard = sprite;
        }
        ImGui::SameLine();
        if (ImGui::Button("Paste"))
        {
            auto id = sprite.id;
            sprite = ws_sprite_settings_clipboard;
            sprite.id = id;
        }
        ImGui::Separator();

        {
            auto window = ImGui::GetCurrentWindow();
            auto cur = window->DC.CursorPos;
            ImGui::Image(&pic.tex, { (float)pic.w * spriteScale, (float)pic.h * spriteScale });
            //if (ImGui::IsItemClicked())
            //{
            //    auto mouse = ImGui::GetMousePos();
            //    auto picX = (int)((mouse.x - cur.x) / spriteScale);
            //    auto picY = (int)((mouse.y - cur.y) / spriteScale);
            //    if (picX >= 0 && picX < 64 && picY >= 0 && picY < 64)
            //    {
            //        auto colId = pic.originalData[picY * 64 + picX];
            //        bool alreadyPicked = false;
            //        for (auto otherCol : sprite.transparents) alreadyPicked |= otherCol == colId;
            //        if (!alreadyPicked)
            //        {
            //            sprite.transparents.push_back(colId);
            //            ws_refresh_sprite_texture(pic, sprite);
            //        }
            //    }
            //}
            window->DrawList->AddRect(
                { cur.x + (float)sprite.clip[0] * spriteScale, cur.y + (float)sprite.clip[1] * spriteScale },
                { cur.x + (float)sprite.clip[2] * spriteScale, cur.y + (float)sprite.clip[3] * spriteScale },
                0xFF0000FF);
        }

        ImGui::SliderInt("Left", &sprite.clip[0], 0, 64);
        ImGui::SliderInt("Top", &sprite.clip[1], 0, 64);
        ImGui::SliderInt("Right", &sprite.clip[2], 0, 64);
        ImGui::SliderInt("Bottom", &sprite.clip[3], 0, 64);

        // Transparents
        ImGui::Text("Transparent colors:");
        for (auto it = sprite.transparents.begin(); it != sprite.transparents.end();)
        {
            auto col = *it;
            if (ImGui::Button(("Remove##" + std::to_string(col)).c_str()))
            {
                it = sprite.transparents.erase(it);
                ws_refresh_sprite_texture(pic, sprite);
            }
            else ++it;
            ImGui::SameLine();
            ImGui::Text(std::to_string(col).c_str());
        }

        ImGui::Separator();
        ImGui::DragFloat("Self Illum", &sprite.self_illumination, 0.01f, 0.0f, 1.0f, "%.2f");
        ImGui::Checkbox("Emit light", &sprite.emit_light);
        if (sprite.emit_light)
        {
            tool_pointlight("Light", sprite.light);
        }
    }
    // Sprite list
    {
        auto w = ImGui::GetWindowWidth();
        float x = w;
        for (auto &kv : ws_sprite_textures)
        {
            auto sw = (float)kv.second.w;
            if (x + sw < w - sw) ImGui::SameLine();
            x = ImGui::GetCursorPosX();
            if (ImGui::ImageButton(&kv.second.tex, { sw, (float)kv.second.h }))
            {
                ws_selected_sprite = (int)kv.first;
            }
        }
    }
    ImGui::End();
}

static void tool_buffers()
{
    ImGui::Begin("Buffers");
    ImGui::Checkbox("Pinned", &ws_tool_buffers_pinned);
    ImGui::Separator();
    ImGui::SliderInt("Scale", &ws_gbuffer_tool_scale, 0, TOOL_GBUFFER_SCALES_COUNT - 1);
    auto scale = TOOL_GBUFFER_SCALES[ws_gbuffer_tool_scale];
    auto w = ImGui::GetWindowWidth();
    auto x = ImGui::GetCursorPosX();
    ImGui::Image(&ws_gbuffer.albeoHandle, { scale.x, scale.y }, { 0, 1 }, { 1, 0 });
    if (x + scale.x < w - scale.x) ImGui::SameLine();
    x = ImGui::GetCursorPosX();
    ImGui::Image(&ws_gbuffer.normalHandle, { scale.x, scale.y }, { 0, 1 }, { 1, 0 });
    if (x + scale.x < w - scale.x) ImGui::SameLine();
    x = ImGui::GetCursorPosX();
    ImGui::Image(&ws_gbuffer.depthHandle, { scale.x, scale.y }, { 0, 1 }, { 1, 0 });
    if (x + scale.x < w - scale.x) ImGui::SameLine();
    x = ImGui::GetCursorPosX();
    ImGui::Image(&ws_resources.hdrRT.handle, { scale.x, scale.y }, { 0, 1 }, { 1, 0 });
    if (x + scale.x < w - scale.x) ImGui::SameLine();
    ImGui::Image(&ws_resources.lastFrameRT.handle, { scale.x, scale.y }, { 0, 1 }, { 1, 0 });
    ImGui::End();
}

static void tool_stats()
{
    ImGui::Begin("Stats");
    ImGui::Checkbox("Pinned", &ws_tool_stats_pinned);
    ImGui::Separator();
    ImGui::Text("Delta Time: %.3f", ws_rdt);
    ImGui::Text("HDR Multiplier: %.3f", ws_hdr_multiplier);
    ImGui::Text("Active lights: %i", (int)ws_active_lights.size());
    ImGui::End();
}

void ws_do_tools()
{
    auto& io = ImGui::GetIO();
    io.DeltaTime = ws_rdt;
    io.DisplaySize = { (float)ws_screen_w, (float)ws_screen_h };
    ImGui::NewFrame();

    if (ws_debug_view_enabled || ws_tool_fullscreen_textures_pinned) tool_fullScreenTextures();
    if (ws_debug_view_enabled || ws_tool_fonts_pinned) tool_fonts();
    if (ws_debug_view_enabled || ws_tool_ui_textures_pinned) tool_uiTextures();
    if (ws_debug_view_enabled || ws_tool_walls_pinned) tool_walls();
    if (ws_debug_view_enabled || ws_tool_rendering_pinned) tool_rendering();
    if (ws_debug_view_enabled || ws_tool_buffers_pinned) tool_buffers();
    if (ws_debug_view_enabled || ws_tool_sprites_pinned) tool_sprites();
    if (ws_debug_view_enabled || ws_tool_stats_pinned) tool_stats();

    ImGui::Render();
    auto pDrawData = ImGui::GetDrawData();
    auto cmdListCount = pDrawData->CmdListsCount;
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, ws_screen_w, ws_screen_h);
    for (int i = 0; i < cmdListCount; ++i)
    {
        auto pCmdList = pDrawData->CmdLists[i];
        auto pIndexBuffer = pCmdList->IdxBuffer.Data;
        auto pVertexBuffer = pCmdList->VtxBuffer.Data;

        auto vertCount = pCmdList->VtxBuffer.size();
        auto indexCount = pCmdList->IdxBuffer.size();
        int drawOffset = 0;

        // Loop sub meshes
        auto cmdBufferCount = pCmdList->CmdBuffer.size();
        for (int k = 0; k < cmdBufferCount; ++k)
        {
            auto pCmd = pCmdList->CmdBuffer.Data + k;

            auto scissorW = (int)(pCmd->ClipRect.z - pCmd->ClipRect.x);
            auto scissorH = (int)(pCmd->ClipRect.w - pCmd->ClipRect.y);
            glScissor((int)pCmd->ClipRect.x, ws_screen_h - (int)pCmd->ClipRect.y - scissorH, scissorW, scissorH);

            if (pCmd->UserCallback)
            {
                pCmd->UserCallback(pCmdList, pCmd);
            }
            else
            {
                GLuint texture = *(GLuint*)pCmd->TextureId;
                ws_prepare_for_ptc(GL_TRIANGLES);
                glBindTexture(GL_TEXTURE_2D, texture);
                auto pVertices = ws_resources.pPTCVertices;
                for (int j = drawOffset; j < drawOffset + pCmd->ElemCount; ++j)
                {
                    auto pVert = pVertexBuffer + pIndexBuffer[j];
                    pVertices->position.x = pVert->pos.x;
                    pVertices->position.y = pVert->pos.y;
                    pVertices->texCoord.u = pVert->uv.x;
                    pVertices->texCoord.v = pVert->uv.y;
                    pVertices->color.r = (float)((pVert->col) & 0xff) / 255.0f;
                    pVertices->color.g = (float)((pVert->col >> 8) & 0xff) / 255.0f;
                    pVertices->color.b = (float)((pVert->col >> 16) & 0xff) / 255.0f;
                    pVertices->color.a = (float)((pVert->col >> 24) & 0xff) / 255.0f;
                    ++pVertices;
                }
                ws_ptc_count = pCmd->ElemCount;
                ws_flush();
            }
            drawOffset += pCmd->ElemCount;
        }
    }
    glDisable(GL_SCISSOR_TEST);

    ws_ptc_count = 0;
}
