#include "ws.h"

#include "WL_DEF.H"

#include <chrono>

static const float LUM_TARGET = .25f;
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

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);

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

        ws_matrix2D = ws_Matrix::CreateOrthographicOffCenter(0, (float)ws_3d_w, (float)ws_3d_h, 0, -999, 999);
        {
            glUseProgram(ws_resources.programPTC);
            auto uniform = glGetUniformLocation(ws_resources.programPTC, "ProjMtx");
            glUniformMatrix4fv(uniform, 1, GL_FALSE, &ws_matrix2D._11);
        }

        ws_prepare_for_ptc(GL_QUADS);

        // Ambient
        glBindTexture(GL_TEXTURE_2D, ws_gbuffer.albeoHandle);
        ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count, 0, 0, (float)ws_3d_w, (float)ws_3d_h, 0, 1, 1, 0, ws_ambient_color);
        ws_flush();

        // Lights
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE); // We accumulate lights with ADD
            glDisable(GL_TEXTURE_2D);

            glUseProgram(ws_resources.programPointlightP);
            glBindBuffer(GL_ARRAY_BUFFER, ws_resources.sphereVB);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (float *)(uintptr_t)(0));
            glEnable(GL_DEPTH_TEST);
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            glDepthFunc(GL_GREATER);
            glDepthMask(GL_FALSE);
            glEnableVertexAttribArray(0); // pos
            glDisableVertexAttribArray(1);
            glDisableVertexAttribArray(2);
            glDisableVertexAttribArray(3);

            auto InvProjMtx = ws_matrix3D.Invert().Transpose();
            {
                static auto uniform = glGetUniformLocation(ws_resources.programPointlightP, "InvProjMtx");
                glUniformMatrix4fv(uniform, 1, GL_FALSE, &InvProjMtx._11);
            }
            {
                static auto uniform = glGetUniformLocation(ws_resources.programPointlightP, "AlbeoTexture");
                glUniform1i(uniform, 0);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, ws_gbuffer.albeoHandle);
            }
            {
                static auto uniform = glGetUniformLocation(ws_resources.programPointlightP, "NormalTexture");
                glUniform1i(uniform, 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, ws_gbuffer.normalHandle);
            }
            {
                static auto uniform = glGetUniformLocation(ws_resources.programPointlightP, "DepthTexture");
                glUniform1i(uniform, 2);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, ws_gbuffer.depthHandle);
            }

            glUniformMatrix4fv(glGetUniformLocation(ws_resources.programPointlightP, "ProjMtx"), 1, GL_FALSE, &ws_matrix3D._11);

            // Player light
            ws_player_light.position = { (float)player->x / 65536.f, 64.0f - (float)player->y / 65536.f, 0.5f };
            // ws_draw_pointlight(ws_player_light);

            // Other lights
            for (auto& light : ws_active_lights)
            {
                ws_draw_pointlight(light);
            }
        }

        // Draw HDR image
        {
            glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.mainRT.frameBuffer);
            glDisable(GL_DEPTH_TEST);
            glDepthMask(GL_TRUE);
            glDepthFunc(GL_LESS);
            glDisable(GL_BLEND);
            glDisable(GL_CULL_FACE);
            glCullFace(GL_BACK);

            ws_draw_mode = -1;
            ws_prepare_for_ptc(GL_QUADS);
            glUseProgram(ws_resources.programHDRPTC);

            static auto projUniform = glGetUniformLocation(ws_resources.programHDRPTC, "ProjMtx");
            static auto lumMultiplierUniform = glGetUniformLocation(ws_resources.programHDRPTC, "LumMultiplier");
            glUniformMatrix4fv(projUniform, 1, GL_FALSE, &ws_matrix2D._11);
            glUniform1f(lumMultiplierUniform, ws_hdr_multiplier);

            glEnable(GL_TEXTURE_2D);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ws_resources.hdrRT.handle);
            ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count, 0, 0, (float)ws_3d_w, (float)ws_3d_h, 0, 1, 1, 0, { 1, 1, 1, 1 });
            ws_flush();
        }

        // Draw into last frame so we can adjust HDR on next frame
        {
            glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.lastFrameRT.frameBuffer);
            glBindTexture(GL_TEXTURE_2D, ws_resources.mainRT.handle);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
            glViewport(0, 0, 4, 4);
            ws_draw_mode = -1;
            ws_prepare_for_ptc(GL_QUADS);
            ws_ptc_count += ws_draw_rect(ws_resources.pPTCVertices + ws_ptc_count, 0, 0, (float)ws_3d_w, (float)ws_3d_h, 0, 1, 1, 0, { 1, 1, 1, 1 });
            ws_flush();
            glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.uiRT.frameBuffer);

            // Calculate lum multiplier for HDR based on last rendered frame
            {
                glBindTexture(GL_TEXTURE_2D, ws_resources.lastFrameRT.handle);
                byte lumData[4 * 16];
                glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, lumData);

                float lumDataf[3] = { 0 };
                for (int i = 0; i < 16; ++i)
                {
                    float w = .666667f;
                    if (i % 4 >= 1 &&
                        i % 4 <= 2 &&
                        i / 4 >= 1 &&
                        i / 4 <= 2) w = 2;
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
    }

    // Go back to our UI frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, ws_resources.uiRT.frameBuffer);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glViewport(0, 0, MaxX, MaxY);

    ws_matrix2D = ws_Matrix::CreateOrthographicOffCenter(0, (float)MaxX, (float)MaxY, 0, -999, 999);
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
