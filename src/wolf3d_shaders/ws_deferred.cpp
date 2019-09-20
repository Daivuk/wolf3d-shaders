#include "ws.h"

#include "WL_DEF.H"

ws_GBuffer ws_gbuffer;
std::vector<ws_PointLight> ws_active_lights;

GLuint ws_create_sphere()
{
    GLuint handle;
    glGenBuffers(1, &handle);
    glBindBuffer(GL_ARRAY_BUFFER, handle);

    ws_Vector3 *pVertices = new ws_Vector3[WS_SPHERE_VERT_COUNT];
    int hseg = 8;
    int vseg = 8;

    auto pVerts = pVertices;
    {
        auto cos_h = cosf(1.0f / (float)hseg * (float)M_PI);
        auto sin_h = sinf(1.0f / (float)hseg * (float)M_PI);
        for (int j = 1; j < hseg - 1; ++j)
        {
            auto cos_h_next = cosf((float)(j + 1) / (float)hseg * (float)M_PI);
            auto sin_h_next = sinf((float)(j + 1) / (float)hseg * (float)M_PI);
            auto cos_v = cosf(0.0f);
            auto sin_v = sinf(0.0f);
            for (int i = 0; i < vseg; ++i)
            {
                auto cos_v_next = cosf((float)(i + 1) / (float)vseg * 2.0f * (float)M_PI);
                auto sin_v_next = sinf((float)(i + 1) / (float)vseg * 2.0f * (float)M_PI);

                pVerts->x = cos_v * sin_h;
                pVerts->y = sin_v * sin_h;
                pVerts->z = cos_h;
                ++pVerts;

                pVerts->x = cos_v * sin_h_next;
                pVerts->y = sin_v * sin_h_next;
                pVerts->z = cos_h_next;
                ++pVerts;

                pVerts->x = cos_v_next * sin_h_next;
                pVerts->y = sin_v_next * sin_h_next;
                pVerts->z = cos_h_next;
                ++pVerts;

                pVerts->x = cos_v * sin_h;
                pVerts->y = sin_v * sin_h;
                pVerts->z = cos_h;
                ++pVerts;

                pVerts->x = cos_v_next * sin_h_next;
                pVerts->y = sin_v_next * sin_h_next;
                pVerts->z = cos_h_next;
                ++pVerts;

                pVerts->x = cos_v_next * sin_h;
                pVerts->y = sin_v_next * sin_h;
                pVerts->z = cos_h;
                ++pVerts;

                cos_v = cos_v_next;
                sin_v = sin_v_next;
            }
            cos_h = cos_h_next;
            sin_h = sin_h_next;
        }
    }

    // Caps
    {
        auto cos_h_next = cosf(1.0f / (float)hseg * (float)M_PI);
        auto sin_h_next = sinf(1.0f / (float)hseg * (float)M_PI);
        auto cos_v = cosf(0.0f);
        auto sin_v = sinf(0.0f);
        for (int i = 0; i < vseg; ++i)
        {
            auto cos_v_next = cosf((float)(i + 1) / (float)vseg * 2.0f * (float)M_PI);
            auto sin_v_next = sinf((float)(i + 1) / (float)vseg * 2.0f * (float)M_PI);

            pVerts->x = 0.0f;
            pVerts->y = 0.0f;
            pVerts->z = 1.0f;
            ++pVerts;

            pVerts->x = cos_v * sin_h_next;
            pVerts->y = sin_v * sin_h_next;
            pVerts->z = cos_h_next;
            ++pVerts;

            pVerts->x = cos_v_next * sin_h_next;
            pVerts->y = sin_v_next * sin_h_next;
            pVerts->z = cos_h_next;
            ++pVerts;

            pVerts->x = 0.0f;
            pVerts->y = 0.0f;
            pVerts->z = -1.0f;
            ++pVerts;

            pVerts->x = cos_v_next * sin_h_next;
            pVerts->y = sin_v_next * sin_h_next;
            pVerts->z = -cos_h_next;
            ++pVerts;

            pVerts->x = cos_v * sin_h_next;
            pVerts->y = sin_v * sin_h_next;
            pVerts->z = -cos_h_next;
            ++pVerts;

            cos_v = cos_v_next;
            sin_v = sin_v_next;
        }
    }

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * WS_SPHERE_VERT_COUNT, pVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (float *)(uintptr_t)(0));

    delete[] pVertices;

    return handle;
}

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

void ws_draw_pointlight(const ws_PointLight& pointLight)
{
    //auto statusLineH = (int)((float)STATUSLINES * ((float)ws_screen_h / 200.0f));
    //float v = (float)(ws_screen_h - statusLineH) / (float)ws_screen_h;

    static auto LightPosition_uniform = glGetUniformLocation(ws_resources.programPointlightP, "LightPosition");
    static auto LightRadius_uniform = glGetUniformLocation(ws_resources.programPointlightP, "LightRadius");
    static auto LightIntensity_uniform = glGetUniformLocation(ws_resources.programPointlightP, "LightIntensity");
    ws_Vector3 lpos = {pointLight.position.x, pointLight.position.y, pointLight.position.z * 1.2f};
    glUniform3fv(LightPosition_uniform, 1, &lpos.x);
    glUniform1f(LightRadius_uniform, pointLight.radius);
    glUniform1f(LightIntensity_uniform, pointLight.intensity);

    glDrawArrays(GL_TRIANGLES, 0, WS_SPHERE_VERT_COUNT);

    //ws_draw_rect(ws_resources.pPTCVertices, 0, 0, (float)ws_screen_w, (float)ws_screen_h - (float)statusLineH, 0, 1, 1, 1 - v, pointLight.color);
    //ws_draw_ptc(ws_resources.pPTCVertices, 4, GL_QUADS);
}
