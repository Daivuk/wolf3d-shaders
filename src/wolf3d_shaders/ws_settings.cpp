#include "ws.h"

#include "WL_DEF.H"

#include <json/json.h>
#include <fstream>
#include <tinyfiledialogs.h>

bool ws_ao_enabled = true;
bool ws_texture_enabled = true;
bool ws_sprite_texture_enabled = true;
bool ws_sprite_enabled = true;
bool ws_wireframe_enabled = false;
bool ws_deferred_enabled = true;
float ws_ao_amount = 0.65f;
float ws_ao_size = 0.25f;
ws_Color ws_ambient_color = { .1f, .1f, .1f, 1.0f };
ws_PointLight ws_player_light = { {0,0,0}, { 1.0f, 1.0f, 1.0f, 1.0f }, 5.0f, 1.3f };
std::map<int, ws_SpriteSettings> ws_sprite_settings;

float ws_save_flash_anim = 0.0f;
float ws_bonus_flash = 0.0f;
float ws_damage_flash = 0.0f;

float ws_sound_volume = 0.5f;
float ws_music_volume = 0.5f;

static const std::string GLOBAL_SETTINGS_FILENAME = "global_settings.json";
static const std::string CONFIGS_FILENAME = "configs.json";

static int json_to_int(const Json::Value& json, int _default = 0)
{
    return json.isInt() ? json.asInt() : _default;
}

static float json_to_float(const Json::Value& json, float _default = 0.0f)
{
    return json.isNumeric() ? json.asFloat() : _default;
}

static bool json_to_bool(const Json::Value& json, bool _default = false)
{
    return json.isBool() ? json.asBool() : _default;
}

static Json::Value color_to_json(const ws_Color& col)
{
    Json::Value ret;
    ret["r"] = col.r;
    ret["g"] = col.g;
    ret["b"] = col.b;
    ret["a"] = col.a;
    return ret;
}

static ws_Color json_to_color(const Json::Value& json)
{
    ws_Color ret;
    ret.r = json_to_float(json["r"], 1.0f);
    ret.g = json_to_float(json["g"], 1.0f);
    ret.b = json_to_float(json["b"], 1.0f);
    ret.a = json_to_float(json["a"], 1.0f);
    return ret;
}

static Json::Value vec3_to_json(const ws_Vector3& v)
{
    Json::Value ret;
    ret["x"] = v.x;
    ret["y"] = v.y;
    ret["z"] = v.z;
    return ret;
}

static ws_Vector3 json_to_vec3(const Json::Value& json)
{
    ws_Vector3 ret;
    ret.x = json_to_float(json["x"], 0.0f);
    ret.y = json_to_float(json["y"], 0.0f);
    ret.z = json_to_float(json["z"], 0.0f);
    return ret;
}

static Json::Value clip_to_json(const int v[])
{
    Json::Value ret;
    ret["x1"] = v[0];
    ret["y1"] = v[1];
    ret["x2"] = v[2];
    ret["y2"] = v[3];
    return ret;
}

static void json_to_clip(const Json::Value& json, int v[])
{
    v[0] = json_to_int(json["x1"], 0);
    v[1] = json_to_int(json["y1"], 0);
    v[2] = json_to_int(json["x2"], 64);
    v[3] = json_to_int(json["y2"], 64);
}

static ws_PointLight json_to_pointLight(const Json::Value& json, const ws_PointLight& _default = WS_DEFAULT_SPRITE_SETTINGS.light)
{
    if (!json) return _default;
    ws_PointLight ret;
    ret.position = json_to_vec3(json["position"]);
    ret.color = json_to_color(json["color"]);
    ret.radius = json_to_float(json["radius"], 5.0f);
    ret.intensity = json_to_float(json["intensity"], 1.3f);
    ret.cast_shadow = json_to_bool(json["cast_shadow"], false);
    return ret;
}

static Json::Value pointLight_to_json(const ws_PointLight& pointLight)
{
    Json::Value ret;
    ret["position"] = vec3_to_json(pointLight.position);
    ret["color"] = color_to_json(pointLight.color);
    ret["radius"] = pointLight.radius;
    ret["intensity"] = pointLight.intensity;
    ret["cast_shadow"] = pointLight.cast_shadow;
    return ret;
}

static ws_SpriteSettings json_to_sprite(const Json::Value& json, const ws_SpriteSettings& _default = WS_DEFAULT_SPRITE_SETTINGS)
{
    if (!json) return _default;
    ws_SpriteSettings ret;
    ret.id = json_to_int(json["id"], -1);
    ret.emit_light = json_to_bool(json["emit_light"], false);
    ret.self_illumination = json_to_float(json["self_illumination"], 0.0f);
    ret.light = json_to_pointLight(json["light"]);
    json_to_clip(json["clip"], ret.clip);
    return ret;
}

static Json::Value sprite_to_json(const ws_SpriteSettings& sprite)
{
    Json::Value ret;
    ret["id"] = sprite.id;
    ret["emit_light"] = sprite.emit_light;
    ret["self_illumination"] = sprite.self_illumination;
    ret["light"] = pointLight_to_json(sprite.light);
    ret["clip"] = clip_to_json(sprite.clip);
    return ret;
}

void ws_save_settings()
{
    std::ofstream file(GLOBAL_SETTINGS_FILENAME);
    if (!file.is_open())
    {
        tinyfd_messageBox("Open", ("Failed to open/create file:\n" + GLOBAL_SETTINGS_FILENAME).c_str(), "ok", "error", 0);
        return;
    }

    Json::Value jsonDocument;

    //-----------------------
    jsonDocument["ao_amount"] = ws_ao_amount;
    jsonDocument["ao_size"] = ws_ao_size;
    jsonDocument["ambient_color"] = color_to_json(ws_ambient_color);
    jsonDocument["player_light"] = pointLight_to_json(ws_player_light);
    Json::Value sprites(Json::arrayValue);
    for (auto& kv : ws_sprite_settings)
    {
        sprites.append(sprite_to_json(kv.second));
    }
    jsonDocument["sprites"] = sprites;
    jsonDocument["gbuffer_tool_scale"] = ws_gbuffer_tool_scale;
    //-----------------------

    file << jsonDocument;
    ws_save_flash_anim = 1.0f;
}

void ws_load_settings()
{
    std::ifstream file(GLOBAL_SETTINGS_FILENAME);
    if (!file.is_open())
    {
        tinyfd_messageBox("Open", ("Failed to open file:\n" + GLOBAL_SETTINGS_FILENAME).c_str(), "ok", "error", 0);
        return;
    }
    Json::Value jsonDocument;
    try
    {
        file >> jsonDocument;
    }
    catch (...)
    {
        tinyfd_messageBox("Open", ("Failed to open file:\n" + GLOBAL_SETTINGS_FILENAME + "\nIt is corrupted.").c_str(), "ok", "error", 0);
        return;
    }

    //-----------------------
    ws_ao_amount = json_to_float(jsonDocument["ao_amount"], 0.65f);
    ws_ao_size = json_to_float(jsonDocument["ao_size"], 0.25f);
    ws_ambient_color = json_to_color(jsonDocument["ambient_color"]);
    ws_player_light = json_to_pointLight(jsonDocument["player_light"], ws_player_light);
    const auto& sprites = jsonDocument["sprites"];
    if (sprites.isArray())
    {
        for (int i = 0; i < (int)sprites.size(); ++i)
        {
            auto sprite = json_to_sprite(sprites[i]);
            ws_sprite_settings[sprite.id] = sprite;
        }
    }
    ws_gbuffer_tool_scale = json_to_int(jsonDocument["gbuffer_tool_scale"], 0);
    //-----------------------
}

void ws_save_configs()
{
    std::ofstream file(CONFIGS_FILENAME);
    if (!file.is_open())
    {
        tinyfd_messageBox("Open", ("Failed to open/create file:\n" + CONFIGS_FILENAME).c_str(), "ok", "error", 0);
        return;
    }

    Json::Value jsonDocument;

    //-----------------------
    jsonDocument["sound_volume"] = ws_sound_volume;
    jsonDocument["music_volume"] = ws_music_volume;
    jsonDocument["mouse_adjustment"] = (int)mouseadjustment;
    Json::Value buttons(Json::arrayValue);
    for (int i = 0; i < NUMBUTTONS; ++i)
    {
        Json::Value button;
        button["id"] = i;
        button["config1"] = (int)buttonscan[0][i];
        button["config2"] = (int)buttonscan[1][i];
        buttons.append(button);
    }
    jsonDocument["buttons"] = buttons;
    //-----------------------

    file << jsonDocument;
    file.close();
}

void ws_load_configs()
{
    std::ifstream file(CONFIGS_FILENAME);
    if (!file.is_open())
    {
        tinyfd_messageBox("Open", ("Failed to open file:\n" + CONFIGS_FILENAME).c_str(), "ok", "error", 0);
        return;
    }
    Json::Value jsonDocument;
    try
    {
        file >> jsonDocument;
    }
    catch (...)
    {
        tinyfd_messageBox("Open", ("Failed to open file:\n" + CONFIGS_FILENAME + "\nIt is corrupted.").c_str(), "ok", "error", 0);
        return;
    }

    //-----------------------
    ws_sound_volume = json_to_float(jsonDocument["sound_volume"], 0.5f);
    ws_music_volume = json_to_float(jsonDocument["music_volume"], 0.5f);
    mouseadjustment = (int16_t)json_to_int(jsonDocument["mouse_adjustment"], 3);
    const auto& buttons = jsonDocument["buttons"];
    if (buttons.isArray())
    {
        for (int i = 0; i < (int)buttons.size(); ++i)
        {
            auto& button = buttons[i];
            auto id = json_to_int(button["id"], -1);
            if (id != -1)
            {
                buttonscan[0][id] = json_to_int(button["config1"], sc_None);
                buttonscan[1][id] = json_to_int(button["config2"], sc_None);
            }
        }
    }
    //-----------------------

    file.close();
}
