
namespace Offset{
enum {

_position = 0x17E7640, // Transform -- get_position_injected
_transform = 0x17D78B8, // Component -- get_transform
_main = 0x17A9248, // Camera -- get_main
_worldtoscreenpoint = 0x17A8C60, // Camera -- worldtoscreenpoint_injected
_dead = 0x14F1F14, //get_IsAlive

};
}


//Class.Component
//get_transform
void *getTransform(void *player) {
    if (!player) return NULL;
    static const auto get_transform_injected = reinterpret_cast<uint64_t(__fastcall *)(void *)>(getAbsoluteAddress("libil2cpp.so", Offset::_transform));
    return (void *) get_transform_injected(player);
}

//Class.Transform
//get_position_Injected
Vector3 get_position(void *transform) {
    if (!transform)return Vector3();
    Vector3 position;
    static const auto get_position_injected = reinterpret_cast<uint64_t(__fastcall *)(void *,Vector3 &)>(getAbsoluteAddress("libil2cpp.so", Offset::_position));
    get_position_injected(transform, position);
    return position;
}

//Class.Camera
//WorldToScreenPoint_Injected
Vector3 WorldToScreenPoint(void *transform, Vector3 test) {
    if (!transform)return Vector3();
    Vector3 position;
    static const auto WorldToScreenPoint_Injected = reinterpret_cast<uint64_t(__fastcall *)(void *,Vector3, int, Vector3 &)>(getAbsoluteAddress("libil2cpp.so", Offset::_worldtoscreenpoint));
      WorldToScreenPoint_Injected(transform, test, 4, position);
      return position;
}

//Class.Camera
//get_main
void *get_camera() {
    static const auto get_camera_injected = reinterpret_cast<uint64_t(__fastcall *)()>(getAbsoluteAddress("libil2cpp.so", Offset::_main));
    return (void *) get_camera_injected();
}

Vector3 GetPlayerLocation(void *player) {
    return get_position(getTransform(player));
}

// health >> float or int depend on offset

/*
float currentHealth(void *player) {
    return *(float *) ((uintptr_t) player + 0x00); 
}
float macHealth(void *player) {
    return *(float *) ((uintptr_t) player + 0x00); 
}
*/

/*
int GetPlayerHealth(void *player) {
    if (!player) return NULL;
    static const auto get_Health = reinterpret_cast<uint64_t(__fastcall *)(void *)>(getAbsoluteAddress("libil2cpp.so", 0x000000));
    return (int) get_Health(player);
}
*/

bool GetPlayerDead(void *player) {
    if (!player) return NULL;
    static const auto get_Dead = reinterpret_cast<uint64_t(__fastcall *)(void *)>(getAbsoluteAddress("libil2cpp.so", Offset::_dead));
    return (bool) get_Dead(player);
}

bool PlayerAlive(void *player) {
    return player != NULL && GetPlayerDead(player) > 0;
}

bool IsPlayerDead(void *player) {
    return player == NULL && GetPlayerDead(player) < 1;
}

