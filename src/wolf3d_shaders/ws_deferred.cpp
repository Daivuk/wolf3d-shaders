#include "ws.h"

ws_GBuffer ws_gbuffer;

ws_GBuffer ws_create_gbuffer(int w, int h)
{
    ws_GBuffer gbuffer;

    glGenFramebuffers(1, &gbuffer.frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.frameBuffer);
    glActiveTexture(GL_TEXTURE0);

    // Albeo
    {
        glGenTextures(1, &gbuffer.albeoHandle);
        glBindTexture(GL_TEXTURE_2D, gbuffer.albeoHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gbuffer.albeoHandle, 0);
    }

    // Normal
    {
        glGenTextures(1, &gbuffer.normalHandle);
        glBindTexture(GL_TEXTURE_2D, gbuffer.normalHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gbuffer.normalHandle, 0);
    }

    // Depth
    {
        glGenTextures(1, &gbuffer.depthHandle);
        glBindTexture(GL_TEXTURE_2D, gbuffer.depthHandle);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gbuffer.depthHandle, 0);
    }

    // Attach the main depth buffer
    {
        glBindRenderbuffer(GL_RENDERBUFFER, ws_resources.mainRT.depth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, ws_resources.mainRT.depth);
    }

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    return gbuffer;
}

void ws_resize_gbuffer(ws_GBuffer &gbuffer, int w, int h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer.frameBuffer);

    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, gbuffer.albeoHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, gbuffer.normalHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glBindTexture(GL_TEXTURE_2D, gbuffer.depthHandle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
}

void ws_draw_pointlight(const ws_Vector3& pos, const ws_Color& col, float radius, float intensity)
{
    static auto LightPosition_uniform = glGetUniformLocation(ws_resources.programPointlightPTC, "LightPosition");
    static auto LightRadius_uniform = glGetUniformLocation(ws_resources.programPointlightPTC, "LightRadius");
    static auto LightIntensity_uniform = glGetUniformLocation(ws_resources.programPointlightPTC, "LightIntensity");
    glUniform3fv(LightPosition_uniform, 1, &pos.x);
    glUniform1f(LightRadius_uniform, radius);
    glUniform1f(LightIntensity_uniform, intensity);

    ws_draw_rect(ws_resources.pPTCVertices, 0, 0, (float)ws_screen_w, (float)ws_screen_h, 0, 1, 1, 0, col);
    ws_draw_ptc(ws_resources.pPTCVertices, 4, GL_QUADS);
}
