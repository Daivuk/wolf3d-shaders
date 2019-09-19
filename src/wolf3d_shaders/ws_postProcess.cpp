#include "ws.h"

#include "WL_DEF.H"

#include <chrono>

static const float LUM_TARGET = .15f;
static const float LUM_ADAPT_SPEED_LIGHT = 20.0f;
static const float LUM_ADAPT_SPEED_DARK = 20.0f;
static const float LUM_MIN = .01f;
static const float LUM_MAX = 10.0f;
float ws_hdr_multiplier = LUM_MIN;

// sRGB "gamma" function (approx 2.2)
static double gam_sRGB(double v) {
    if (v <= 0.0031308)
        v *= 12.92;
    else
        v = 1.055*pow(v, 1.0 / 2.4) - 0.055;
    return v;
}

// Inverse of sRGB "gamma" function. (approx 2.2)
static double inv_gam_sRGB(double ic)
{
    double c = ic;
    if (c <= 0.04045)
        return c / 12.92;
    else
        return pow(((c + 0.055) / (1.055)), 2.4);
}

// sRGB luminance(Y) values
const double rY = 0.212655;// * 2;
const double gY = 0.715158;// - 0.072187 - 0.212655;
const double bY = 0.072187;// * 2;

// GRAY VALUE ("brightness")
static double gray(double r, double g, double b)
{
    return gam_sRGB(
        rY * inv_gam_sRGB(r) +
        gY * inv_gam_sRGB(g) +
        bY * inv_gam_sRGB(b)
    );
}

void ws_finish_draw_3d()
{
    ws_flush();
    glScissor(0, 0, ws_screen_w, ws_screen_h);
    glViewport(0, 0, ws_screen_w, ws_screen_h);
    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT, GL_FILL);

    // Update render ticks
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto curTime = std::chrono::high_resolution_clock::now();
    auto elapsed = curTime - lastTime;
    lastTime = curTime;
    ws_rdt = (float)((double)std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() / 1000000.0);

    // Post process into mainRT
    if (ws_deferred_enabled)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.hdrRT.frameBuffer);

        auto statusLineH = (int)((float)STATUSLINES * ((float)ws_screen_h / 200.0f));
        float v = (float)(ws_screen_h - statusLineH) / (float)ws_screen_h;

        ws_matrix2D = ws_Matrix::CreateOrthographicOffCenter(0, (float)ws_screen_w, (float)ws_screen_h - (float)statusLineH, 0, -999, 999);
        {
            glUseProgram(ws_resources.programPTC);
            auto uniform = glGetUniformLocation(ws_resources.programPTC, "ProjMtx");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &ws_matrix2D._11);
        }
        glViewport(0, statusLineH, ws_screen_w, ws_screen_h - statusLineH);

        ws_prepare_for_ptc(GL_QUADS);
        glEnable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glDisable(GL_DEPTH_TEST);

        // Ambient
        glBindTexture(GL_TEXTURE_2D, ws_gbuffer.albeoHandle);
        ws_draw_rect(ws_resources.pPTCVertices, 0, 0, (float)ws_screen_w, (float)ws_screen_h - (float)statusLineH, 0, 1, 1, 1 - v, ws_ambient_color);
        ws_draw_ptc(ws_resources.pPTCVertices, 4, GL_QUADS);

        // Player light
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDisable(GL_TEXTURE_2D);
        glUseProgram(ws_resources.programPointlightPTC);
        auto InvProjMtx = ws_matrix3D.Invert().Transpose();
        {
            static auto uniform = glGetUniformLocation(ws_resources.programPointlightPTC, "InvProjMtx");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &InvProjMtx._11);
        }
        {
            static auto uniform = glGetUniformLocation(ws_resources.programPointlightPTC, "AlbeoTexture");
            glUniform1i(uniform, 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ws_gbuffer.albeoHandle);
        }
        {
            static auto uniform = glGetUniformLocation(ws_resources.programPointlightPTC, "NormalTexture");
            glUniform1i(uniform, 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, ws_gbuffer.normalHandle);
        }
        {
            static auto uniform = glGetUniformLocation(ws_resources.programPointlightPTC, "DepthTexture");
            glUniform1i(uniform, 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, ws_gbuffer.depthHandle);
        }

        glUniformMatrix4fv(glGetUniformLocation(ws_resources.programPointlightPTC, "ProjMtx"), 1, GL_FALSE, &ws_matrix2D._11);
        ws_player_light.position = { (float)player->x / 65536.f, 64.0f - (float)player->y / 65536.f, 0.5f };
        ws_draw_pointlight(ws_player_light);

        // Other lights
        for (auto& light : ws_active_lights)
        {
            ws_draw_pointlight(light);
        }

        // Draw HDR image
        {
            glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.mainRT.frameBuffer);
            glEnable(GL_TEXTURE_2D);
            glDisable(GL_DEPTH_TEST);
            glDisable(GL_BLEND);
            glActiveTexture(GL_TEXTURE0);
            glUseProgram(ws_resources.programPTC);
            glEnableVertexAttribArray(0); // pos
            glEnableVertexAttribArray(1); // texcoord
            glEnableVertexAttribArray(2); // color
            glDisableVertexAttribArray(3);
            glViewport(0, statusLineH, ws_screen_w, ws_screen_h - statusLineH);

            glBindTexture(GL_TEXTURE_2D, ws_resources.hdrRT.handle);
            glUseProgram(ws_resources.programHDRPTC);

            static auto projUniform = glGetUniformLocation(ws_resources.programHDRPTC, "ProjMtx");
            static auto lumMultiplierUniform = glGetUniformLocation(ws_resources.programHDRPTC, "LumMultiplier");
            glUniformMatrix4fv(projUniform, 1, GL_FALSE, &ws_matrix2D._11);
            glUniform1f(lumMultiplierUniform, ws_hdr_multiplier);
            
            ws_ptc_count = ws_draw_rect(ws_resources.pPTCVertices, 0, 0, (float)ws_screen_w, (float)ws_screen_h, 0, 1, 1, 0, { 1, 1, 1, 1 });
            ws_draw_ptc(ws_resources.pPTCVertices, ws_ptc_count, GL_QUADS);
            ws_ptc_count = 0;

            // Gun and more UI will be draw after, revert some states
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_BLEND);
            glUseProgram(ws_resources.programPTC);

            // Draw into last frame so we can adjust HDR on next frame
            glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.lastFrameRT.frameBuffer);
            glBindTexture(GL_TEXTURE_2D, ws_resources.mainRT.handle);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
            glViewport(0, 0, 4, 4);
            ws_ptc_count = ws_draw_rect(ws_resources.pPTCVertices, 0, 0, (float)ws_screen_w, (float)ws_screen_h, 0, 1, 1, 0, { 1, 1, 1, 1 });
            ws_draw_ptc(ws_resources.pPTCVertices, ws_ptc_count, GL_QUADS);
            ws_ptc_count = 0;

            glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.mainRT.frameBuffer);
            glViewport(0, statusLineH, ws_screen_w, ws_screen_h - statusLineH);

            // Calculate lum multiplier for HDR based on last rendered frame
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, ws_resources.lastFrameRT.handle);
                byte lumData[4 * 16];
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, lumData);

                float lumDataf[3] = { 0 };
                for (int i = 0; i < 16; ++i)
                {
                    float w = .3333f;
                    if (i % 4 >= 1 &&
                        i % 4 <= 2 &&
                        i / 4 >= 1 &&
                        i / 4 <= 2) w = 3;
                    lumDataf[0] += (float)lumData[i * 4 + 0] / 255.0f * w;
                    lumDataf[1] += (float)lumData[i * 4 + 1] / 255.0f * w;
                    lumDataf[2] += (float)lumData[i * 4 + 2] / 255.0f * w;
                }
                float lumLast = (float)gray(
                    (double)lumDataf[0] / 16.0,
                    (double)lumDataf[1] / 16.0,
                    (double)lumDataf[2] / 16.0);
                if (lumLast < LUM_TARGET)
                {
                    ws_hdr_multiplier += (LUM_TARGET - lumLast) * ws_rdt * LUM_ADAPT_SPEED_LIGHT;
                    if (ws_hdr_multiplier > LUM_MAX) ws_hdr_multiplier = LUM_MAX;
                }
                else if (lumLast > LUM_TARGET)
                {
                    ws_hdr_multiplier -= (lumLast - LUM_TARGET) * ws_rdt * LUM_ADAPT_SPEED_DARK;
                    if (ws_hdr_multiplier < LUM_MIN) ws_hdr_multiplier = LUM_MIN;
                }
            }
        }

        // Go back to our 2D frame buffer
        glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.mainRT.frameBuffer);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glViewport(0, 0, ws_screen_w, ws_screen_h);

        ws_matrix2D = ws_Matrix::CreateOrthographicOffCenter(0, (float)1024, (float)640, 0, -999, 999);
        {
            glUseProgram(ws_resources.programPC);
            auto uniform = glGetUniformLocation(ws_resources.programPC, "ProjMtx");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &ws_matrix2D._11);
        }
        {
            glUseProgram(ws_resources.programPTC);
            auto uniform = glGetUniformLocation(ws_resources.programPTC, "ProjMtx");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &ws_matrix2D._11);
        }
    }
    else
    {
        glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.mainRT.frameBuffer);
    }

    // Draw crosshair
    //ws_prepare_for_pc(GL_LINES);
    //ws_pc_count += ws_draw_line(ws_resources.pPCVertices + ws_pc_count, { 512, 260 - 10 }, { 512, 260 + 1000 }, { 1, 1, 1, 1 });
    //ws_pc_count += ws_draw_line(ws_resources.pPCVertices + ws_pc_count, { 512 - 10, 260 }, { 512 + 10, 260 }, { 1, 1, 1, 1 });
    //ws_flush();
}
