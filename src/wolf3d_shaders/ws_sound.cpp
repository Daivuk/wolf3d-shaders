#include "ws.h"


struct PlayingSound
{
    float *data;
    int len;
    float x, y;
    bool _3d;
};

bool ws_audio_on = false;

static std::vector<PlayingSound> playingSounds;

void ws_play_sound(float *data, int len, float x, float y, bool _3d)
{
    if (!ws_audio_on) return;
    playingSounds.push_back({ data, len, x, 64.0f - y, _3d });
}

void ws_audio_callback(void *userdata, Uint8 *stream, int len)
{
    memset(stream, 0, len);

    len /= 8; // float * 2 channels
    float *pOut = (float*)stream;
    float volL, volR;

    for (auto it = playingSounds.begin(); it != playingSounds.end();)
    {
        auto& playingSound = *it;

        if (playingSound._3d)
        {
            auto sndPos = ws_Vector3(playingSound.x, playingSound.y, 0.5f);
            auto distance = ws_Vector3::DistanceSquared(ws_cam_eye, sndPos);
            auto vol = 1.0f / std::max(1.0f, distance / 15.0f);
            auto dir = sndPos - ws_cam_eye;
            dir.Normalize();
            auto dot = dir.Dot(ws_cam_right) + 1.0f; // 0 = left, 2 = ws_cam_right
            auto dotf = dir.Dot(ws_cam_front); // We attenuate stuff behind us
            dotf = std::max(0.75f, std::min(dotf, 0.0f) + 1.0f); // [0.75 to 1] 1 = perpenticular to camera and forward, 0.75 completely in back
            volL = vol * dotf;
            volR = vol * dotf;
            volL *= std::min(1.0f, (2.0f - dot));
            volR *= std::min(1.0f, dot);
        }
        else
        {
            volL = 1.0f;
            volR = 1.0f;
        }

        auto len1 = std::min(len, playingSound.len);
        for (int i = 0; i < len1; ++i)
        {
            auto sample = playingSound.data[i];
            pOut[i * 2 + 0] += sample * volL;
            pOut[i * 2 + 1] += sample * volR;
        }
        playingSound.data += len1;
        playingSound.len -= len1;
        if (playingSound.len <= 0)
        {
            it = playingSounds.erase(it);
            continue;
        }
        ++it;
    }
}
